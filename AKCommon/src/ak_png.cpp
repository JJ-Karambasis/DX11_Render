#define AK_PNG_SIGNATURE 0x0A1A0A0D474E5089

enum 
{    
    AK_PNG_CHUNK_TYPE_IHDR = AK_RiffCode('I', 'H', 'D', 'R'),
    AK_PNG_CHUNK_TYPE_IDAT = AK_RiffCode('I', 'D', 'A', 'T'),    
    AK_PNG_CHUNK_TYPE_IEND = AK_RiffCode('I', 'E', 'N', 'D')    
};

enum ak_png_filter_type
{
    AK_PNG_FILTER_NONE,
    AK_PNG_FILTER_SUB,
    AK_PNG_FILTER_UP,
    AK_PNG_FILTER_AVG,
    AK_PNG_FILTER_PAETH
};

#pragma pack(push, 1)
struct ak_internal__ihdr
{
    ak_u32 Width;
    ak_u32 Height;
    ak_u8 BitDepth;
    ak_u8 ColorType;
    ak_u8 CompressionMethod;
    ak_u8 FilterMethod;    
    ak_u8 InterlaceMethod;
};
#pragma pack(pop)

struct ak_internal__png_data_chunk
{
    ak_u8* Data;
    ak_u32 Length;
    
    ak_internal__png_data_chunk* Next;
};

struct ak_internal__png_context
{
    ak_u32 StreamLength;
    ak_u32 StreamAt;
    ak_u8* Stream;
    
    ak_u32 CompressedStreamLength;
    ak_u8* CompressedStream;
    
    ak_u32 ComponentCount;
    ak_u32 BytesPerComponent;
    ak_u32 Width;
    ak_u32 Height;        
};

struct ak_internal__png_chunk
{
    ak_u32 Length;
    ak_u32 Type;
};

inline ak_bool 
AK_Internal__PNGValidateSignature(ak_buffer* Buffer)
{
    ak_u64 Signature = *(ak_u64*)Buffer->Data;
    ak_bool Result = Signature == AK_PNG_SIGNATURE;
    return Result;    
}

ak_char* AK_Internal__PNGValidateIHDR(ak_internal__ihdr* IHDR, ak_u32* ComponentCount)
{
    if(IHDR->BitDepth == 8 || IHDR->BitDepth == 16)
    {
        ak_bool ValidIHDR = true;             
        if(IHDR->ColorType == 0) *ComponentCount = 1;
        else if(IHDR->ColorType == 2) *ComponentCount = 3;
        else if(IHDR->ColorType == 6) *ComponentCount = 4;
        else ValidIHDR = false;            
        
        if(!ValidIHDR)    
            return "PNG file is not supported. Must have 1, 3, or 4 channels";                    
        return NULL;
        
    }
    else
        return "PNG file is not supported. Must have 8 or 16 bits per pixel";                        
}        


inline ak_u8* 
AK_Internal__PNGRead(ak_internal__png_context* Context, ak_u32 Length)
{
    ak_u8* Result = Context->Stream + Context->StreamAt;
    Context->StreamAt += Length;    
    
    if(Context->StreamAt > Context->StreamLength)
        return NULL;
    
    return Result;
}

ak_internal__png_chunk* 
AK_Internal__PNGGetChunk(ak_internal__png_context* Context)
{
    ak_internal__png_chunk* Result = (ak_internal__png_chunk*)AK_Internal__PNGRead(Context, sizeof(ak_internal__png_chunk));
    if(Result) Result->Length = AK_EndianSwap32(Result->Length);                        
    return Result;
}

inline ak_u8 
AK_Internal__PaethFilter(ak_u8 A, ak_u8 B, ak_u8 C)
{
    ak_i32 a = (ak_i32)A;
    ak_i32 b = (ak_i32)B;
    ak_i32 c = (ak_i32)C;
    
    ak_i32 p = a + b - c;
    
    ak_i32 pa = p - a;
    ak_i32 pb = p - b;
    ak_i32 pc = p - c;
    if(pa < 0) pa = -pa;
    if(pb < 0) pb = -pb;
    if(pc < 0) pc = -pc;
    
    if((pa <= pb) && (pa <= pc))
        return A;
    else if(pb <= pc)
        return B;
    else
        return C;
}

void AK_Internal__PNGWriteToErrorStream(ak_string_builder* ErrorStream, ak_char* Format, ...)
{
    if(ErrorStream)
    {
        va_list Args;
        va_start(Args, Format);
        ErrorStream->WriteLine(Format, Args);
        va_end(Args);
    }
}

ak_png* AK_LoadPNG(ak_char* File, ak_string_builder* ErrorStream)
{    
    ak_arena* GlobalArena = AK_GetGlobalArena();
    ak_temp_arena TempArena = GlobalArena->BeginTemp();
    
    ak_buffer Buffer = AK_ReadEntireFile(File, GlobalArena);
    if(Buffer.IsValid())
    {
        if(!AK_Internal__PNGValidateSignature(&Buffer))    
        {
            GlobalArena->EndTemp(&TempArena);
            AK_Internal__PNGWriteToErrorStream(ErrorStream, "PNG signature is invalid");
            return NULL;
        }
        
        ak_internal__png_context Context = {};        
        Context.Stream = Buffer.Data;
        Context.StreamLength = AK_SafeU32(Buffer.Size);
        Context.StreamAt = 8;
        
        ak_internal__ihdr* IHDR = NULL;
        
        ak_internal__png_data_chunk* DataChunkList = NULL;    
        ak_internal__png_data_chunk* LastAddedChunk = NULL;
        
        ak_bool End = false;
        while(ak_internal__png_chunk* Chunk = AK_Internal__PNGGetChunk(&Context))
        {            
            switch(Chunk->Type)
            {            
                case AK_PNG_CHUNK_TYPE_IHDR:
                {
                    IHDR = (ak_internal__ihdr*)AK_Internal__PNGRead(&Context, Chunk->Length);
                    ak_char* ErrorMessage = AK_Internal__PNGValidateIHDR(IHDR, &Context.ComponentCount);
                    if(ErrorMessage)
                    {
                        GlobalArena->EndTemp(&TempArena);
                        AK_Internal__PNGWriteToErrorStream(ErrorStream, ErrorMessage);
                        return NULL;
                    }
                    
                    Context.Width = AK_EndianSwap32(IHDR->Width);
                    Context.Height = AK_EndianSwap32(IHDR->Height);
                    Context.BytesPerComponent = IHDR->BitDepth/8;
                } break;
                
                case AK_PNG_CHUNK_TYPE_IDAT:
                {
                    ak_internal__png_data_chunk* DataChunk = GlobalArena->Push<ak_internal__png_data_chunk>();
                    DataChunk->Length = Chunk->Length;
                    DataChunk->Data = AK_Internal__PNGRead(&Context, Chunk->Length);
                    
                    if(!DataChunkList)
                    {
                        DataChunkList = DataChunk;
                        LastAddedChunk = DataChunkList;
                    }
                    else
                    {                    
                        LastAddedChunk->Next = DataChunk;
                        LastAddedChunk = DataChunk;                    
                    }                
                } break;
                
                case AK_PNG_CHUNK_TYPE_IEND:
                {                
                    if(!DataChunkList)                
                    {
                        GlobalArena->EndTemp(&TempArena);
                        AK_Internal__PNGWriteToErrorStream(ErrorStream, "Invalid png file. Did not read any data chunks. The file is corrupted");
                        return NULL;
                    }
                    
                    ak_internal__png_data_chunk* At = DataChunkList;
                    while(At)
                    {
                        Context.CompressedStreamLength += At->Length;
                        At = At->Next;
                    }
                    
                    Context.CompressedStream = (ak_u8*)GlobalArena->Push(Context.CompressedStreamLength);
                    
                    At = DataChunkList;    
                    ak_u8* Dst = Context.CompressedStream;
                    while(At)
                    {
                        AK_MemoryCopy(Dst, At->Data, At->Length);
                        Dst += At->Length;
                        At = At->Next;
                    }                    
                    
                    End = true;
                } break;
                
                default:
                {
                    AK_Internal__PNGRead(&Context, Chunk->Length);
                } break;
            }           
            
            if(End) break;
            
            //NOTE(EVERYONE): We skip the cyclic redundacy check.
            //TODO(JJ): Maybe perform the CRC algorithm test to make sure everything has validated correcly?
            AK_Internal__PNGRead(&Context, 4);            
        }    
        
        if(!End)
        {
            GlobalArena->EndTemp(&TempArena);
            
            AK_Internal__PNGWriteToErrorStream(ErrorStream, "Could not find the last IEND chunk for the PNG file. The file is corrupted");
            return NULL;
        }
        
        ak_u32 DecompressedLength = (Context.Width*Context.ComponentCount*Context.BytesPerComponent+1)*Context.Height;
        ak_u8* DecompressedData = (ak_u8*)GlobalArena->Push(DecompressedLength);
        
        if(AK_ZLibDecompress(DecompressedData, Context.CompressedStream, Context.CompressedStreamLength, ErrorStream))
        {   
            void* Texels = GlobalArena->Push(Context.Width*Context.Height*Context.ComponentCount*Context.BytesPerComponent);
            
            ak_u8* Src = (ak_u8*)DecompressedData;
            ak_u8* Dst = (ak_u8*)Texels;
            
            ak_u8* LastRow = NULL;
            for(ak_u32 YIndex = 0; YIndex < Context.Height; YIndex++)
            {
                ak_u8 FilterType = *Src++;
                
                ak_u8* CurrentRow = Dst;
                switch(FilterType)
                {
                    case AK_PNG_FILTER_NONE:
                    {
                        for(ak_u32 XIndex = 0; XIndex < Context.Width; XIndex++)
                        {                    
                            for(ak_u32 ComponentIndex = 0; ComponentIndex < Context.ComponentCount; ComponentIndex++)       
                            {
                                for(ak_u32 ByteIndex = 0; ByteIndex < Context.BytesPerComponent; ByteIndex++)
                                    *Dst++ = *Src++;                        
                            }                    
                        }
                    } break;
                    
                    case AK_PNG_FILTER_SUB:
                    {
                        ak_u8* PrevPixel = NULL;                
                        for(ak_u32 XIndex = 0; XIndex < Context.Width; XIndex++)
                        {                    
                            ak_u8* Start = Dst;
                            for(ak_u32 ComponentIndex = 0; ComponentIndex < Context.ComponentCount; ComponentIndex++)
                            {
                                for(ak_u32 ByteIndex = 0; ByteIndex < Context.BytesPerComponent; ByteIndex++)
                                {
                                    ak_u8 PrevPixelValue = PrevPixel ? PrevPixel[ComponentIndex*Context.BytesPerComponent + ByteIndex] : 0;
                                    *Dst++ = (*Src++ + PrevPixelValue);                                                                        
                                }
                            }
                            PrevPixel = Start;                                                                                
                        }
                        
                    } break;
                    
                    case AK_PNG_FILTER_UP:
                    {
                        if(!LastRow)       
                        {
                            GlobalArena->EndTemp(&TempArena);
                            AK_Internal__PNGWriteToErrorStream(ErrorStream, "Failed to reconstruct the png file. Unfiltered data is corrupted");
                            return NULL;
                        }
                        
                        for(ak_u32 XIndex = 0; XIndex < Context.Width; XIndex++)
                        {
                            for(ak_u32 ComponentIndex = 0; ComponentIndex < Context.ComponentCount; ComponentIndex++)                    
                            {
                                for(ak_u32 ByteIndex = 0; ByteIndex < Context.BytesPerComponent; ByteIndex++)                        
                                    *Dst++ = *Src++ + *LastRow++;                                            
                            }
                        }
                        
                    } break;
                    
                    case AK_PNG_FILTER_AVG:
                    {
                        if(!LastRow)    
                        {
                            GlobalArena->EndTemp(&TempArena);
                            AK_Internal__PNGWriteToErrorStream(ErrorStream, "Failed to reconstruct the png file. Unfiltered data is corrupted");
                            return NULL;
                        }
                        
                        ak_u8* PrevPixel = NULL;
                        for(ak_u32 XIndex = 0; XIndex < Context.Width; XIndex++)
                        {
                            ak_u8* Start = Dst;
                            for(ak_u32 ComponentIndex = 0; ComponentIndex < Context.ComponentCount; ComponentIndex++)
                            {
                                for(ak_u32 ByteIndex = 0; ByteIndex < Context.BytesPerComponent; ByteIndex++)
                                {
                                    ak_u8 PrevPixelValue = PrevPixel ? PrevPixel[ComponentIndex*Context.BytesPerComponent + ByteIndex] : 0;
                                    ak_u8 LastRowValue = *LastRow++;
                                    
                                    ak_u32 Avg = ((ak_u32)PrevPixelValue + (ak_u32)LastRowValue) / 2;                                                
                                    *Dst++ = *Src++ + (ak_u8)Avg;
                                }
                            }
                            PrevPixel = Start;                    
                        }
                    } break;
                    
                    case AK_PNG_FILTER_PAETH:
                    {
                        if(!LastRow)                
                        {
                            GlobalArena->EndTemp(&TempArena);
                            AK_Internal__PNGWriteToErrorStream(ErrorStream, "Failed to reconstruct the png file. Unfiltered data is corrupted");
                        }
                        
                        ak_u8* LastRowPrevPixel = NULL;
                        ak_u8* PrevPixel = NULL;
                        
                        for(ak_u32 XIndex = 0; XIndex < Context.Width; XIndex++)
                        {
                            ak_u8* Start = Dst;
                            ak_u8* StartLastRow = LastRow;
                            
                            for(ak_u32 ComponentIndex = 0; ComponentIndex < Context.ComponentCount; ComponentIndex++)
                            {
                                for(ak_u32 ByteIndex = 0; ByteIndex < Context.BytesPerComponent; ByteIndex++)
                                {
                                    ak_u8 PrevPixelValue = PrevPixel ? PrevPixel[ComponentIndex*Context.BytesPerComponent + ByteIndex] : 0;
                                    ak_u8 LastRowValue = *LastRow++;
                                    ak_u8 PrevPixelLastRowValue = LastRowPrevPixel ? LastRowPrevPixel[ComponentIndex*Context.BytesPerComponent + ByteIndex] : 0;                        
                                    
                                    ak_u8 Paeth = AK_Internal__PaethFilter(PrevPixelValue, LastRowValue, PrevPixelLastRowValue);                        
                                    *Dst++ = *Src++ + Paeth;                        
                                }
                            }
                            
                            LastRowPrevPixel = StartLastRow;
                            PrevPixel = Start;
                        }
                    } break;
                    
                    default:
                    {
                        GlobalArena->EndTemp(&TempArena);
                        AK_Internal__PNGWriteToErrorStream(ErrorStream, "Failed to reconstruct the png file. Unfiltered data is corrupted. Could not find a valid filter type");                
                        return NULL;
                    } break;
                }
                
                LastRow = CurrentRow;
            }    
            
            GlobalArena->EndTemp(&TempArena);
            
            if(Context.BytesPerComponent == 2)
            {
                ak_u32 TotalPixels = Context.Width*Context.Height*Context.ComponentCount;
                ak_u16* Dst16 = (ak_u16*)Texels;
                
                for(ak_u32 PixelIndex = 0; PixelIndex < TotalPixels; PixelIndex++)
                    Dst16[PixelIndex] = AK_EndianSwap16(Dst16[PixelIndex]);
            }            
            
            ak_u32 ImageSize = Context.Width*Context.Height*Context.ComponentCount;
            
            ak_png* Result = (ak_png*)AK_Allocate(sizeof(ak_png)+ImageSize);
            Result->Texels = (void*)(Result+1);
            if(Context.BytesPerComponent == 1)
            {                                    
                AK_MemoryCopy(Result->Texels, Texels, ImageSize);
            }
            else
            {                    
                ak_u8* DstTexels = (ak_u8*)Result->Texels;
                ak_u16* SrcTexels = (ak_u16*)Texels;
                
                for(ak_u32 TexelIndex = 0; TexelIndex < ImageSize; TexelIndex++)
                    DstTexels[TexelIndex] = (ak_u8)((SrcTexels[TexelIndex] >> 8) & 0xFF);                    
            }            
            
            Result->Width = Context.Width;
            Result->Height = Context.Height;
            Result->ComponentCount = Context.ComponentCount;
            return Result;
        }
        else
        {
            GlobalArena->EndTemp(&TempArena);
            AK_Internal__PNGWriteToErrorStream(ErrorStream, "Failed to decompress the PNG Zlib data. Please see previous message for details");
            return NULL;
        }
    }
    else
    {
        GlobalArena->EndTemp(&TempArena);
        AK_Internal__PNGWriteToErrorStream(ErrorStream, "Failed to open the PNG file %s", File);
    }
    
    return NULL;
}

ak_png* AK_LoadPNG(ak_string File, ak_string_builder* ErrorStream)
{
    return AK_LoadPNG(File.Data, ErrorStream);
}

void AK_FreePNG(ak_png* PNG)
{
    AK_Free(PNG);
}