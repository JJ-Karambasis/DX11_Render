enum
{
    AK_WAV_CHUNK_TYPE_RIFF = AK_RiffCode('R', 'I', 'F', 'F'), 
    AK_WAV_CHUNK_TYPE_FMT = AK_RiffCode('f', 'm', 't', ' '), 
    AK_WAV_CHUNK_TYPE_DATA = AK_RiffCode('d', 'a', 't', 'a')
};

struct ak__internal_wav_chunk
{
    ak_u32 Type;
    ak_u32 Length;
};

struct ak__internal_wav_format
{
    ak_u16 Format;
    ak_u16 ChannelCount;
    ak_u32 SamplesPerSecond;
    ak_u32 AvgBytesPerSecond;
    ak_u16 BlockAlign;
    ak_u16 BitsPerSample;
    ak_u16 ExtensionSize;
    ak_u32 ChannelMask;
    ak_u8 SubFormat[16];
};

void AK_Internal__WAVWriteToErrorStream(ak_string_builder* ErrorStream, ak_char* Format, ...)
{
    if(ErrorStream)
    {
        va_list Args;
        va_start(Args, Format);
        ErrorStream->WriteLine(Format, Args);
        va_end(Args);
    }
}

ak_wav* AK_LoadWAV(ak_char* File, ak_string_builder* ErrorStream)
{
    ak_arena* GlobalArena = AK_GetGlobalArena();
    ak_temp_arena TempArena = GlobalArena->BeginTemp();
    
    ak_buffer Buffer = AK_ReadEntireFile(File, GlobalArena);
    if(Buffer.IsValid())
    {
        ak_stream Stream = AK_BeginStream(Buffer);
        ak__internal_wav_chunk* RiffChunk = Stream.PeekConsume<ak__internal_wav_chunk>();
        if(RiffChunk->Type != AK_WAV_CHUNK_TYPE_RIFF)
        {
            GlobalArena->EndTemp(&TempArena);
            AK_Internal__WAVWriteToErrorStream(ErrorStream, "Invalid wav riff chunk header");
            return NULL;
        }
        
        if(Stream.CopyConsume<ak_u32>() != AK_RiffCode('W', 'A', 'V', 'E'))
        {
            GlobalArena->EndTemp(&TempArena);
            AK_Internal__WAVWriteToErrorStream(ErrorStream, "Invalid wav riff chunk header");
            return NULL;
        }
        
        ak__internal_wav_chunk* FormatChunk = Stream.PeekConsume<ak__internal_wav_chunk>();
        if(FormatChunk->Type != AK_WAV_CHUNK_TYPE_FMT)
        {
            GlobalArena->EndTemp(&TempArena);
            AK_Internal__WAVWriteToErrorStream(ErrorStream, "Invalid wav fmt chunk header");
            return NULL;
        }
        
        ak__internal_wav_format* WAVFormat = (ak__internal_wav_format*)Stream.PeekConsume(FormatChunk->Length);
        if((WAVFormat->Format != AK_WAV_FORMAT_PCM) && (WAVFormat->Format != AK_WAV_FORMAT_FLOAT))
        {
            GlobalArena->EndTemp(&TempArena);
            AK_Internal__WAVWriteToErrorStream(ErrorStream, "Unsupported wav format. Only supports PCM samples");
            return NULL;
        }
        
        ak__internal_wav_chunk* DataChunk = NULL;
        for(;;)
        {
            ak__internal_wav_chunk* Chunk = Stream.PeekConsume<ak__internal_wav_chunk>();
            if(Chunk->Type == AK_WAV_CHUNK_TYPE_DATA)
            {
                DataChunk = Chunk;
                break;
            }
            
            if((Stream.At + Chunk->Length) > Stream.Length)
            {
                GlobalArena->EndTemp(&TempArena);
                AK_Internal__WAVWriteToErrorStream(ErrorStream, "Invalid wav file structure. Could not find data chunk");
                return NULL;
            }
            Stream.Consume(Chunk->Length);
        }
        
        ak_u32 SampleCount = DataChunk->Length / (WAVFormat->ChannelCount*(WAVFormat->BitsPerSample/8));
        void* Samples = Stream.PeekConsume(DataChunk->Length);
        
        ak_wav* Result = (ak_wav*)AK_Allocate(sizeof(ak_wav)+DataChunk->Length);
        if(!Result)
        {
            GlobalArena->EndTemp(&TempArena);
            AK_Internal__WAVWriteToErrorStream(ErrorStream, "Invalid memory allocation");
            return NULL;
        }
        
        Result->Format = (ak_wav_format)WAVFormat->Format;
        Result->ChannelCount = WAVFormat->ChannelCount;
        Result->SamplesPerSecond = WAVFormat->SamplesPerSecond;
        Result->BytesPerSample = WAVFormat->BitsPerSample/8;
        Result->SampleCount = SampleCount;
        Result->Samples = (void*)(Result+1);
        AK_MemoryCopy(Result->Samples, Samples, DataChunk->Length);
        return Result;
    }
    else
    {
        GlobalArena->EndTemp(&TempArena);
        AK_Internal__WAVWriteToErrorStream(ErrorStream, "Failed to open the WAV file %s", File);
    }
    return NULL;
}

ak_wav* AK_LoadWAV(ak_string File, ak_string_builder* ErrorStream)
{
    return AK_LoadWAV(File.Data, ErrorStream);
}

void AK_FreeWAV(ak_wav* WAV)
{
    AK_Free(WAV);
}
