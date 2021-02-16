#ifndef AK_WAV_H
#define AK_WAV_H

//TODO(JJ): Add more support formats
enum ak_wav_format
{
    AK_WAV_FORMAT_PCM = 1, 
    AK_WAV_FORMAT_FLOAT = 3
};

struct ak_wav
{
    ak_wav_format Format;
    ak_u16 ChannelCount;
    ak_u32 SamplesPerSecond;
    ak_u16 BytesPerSample;
    ak_u32 SampleCount;
    void*  Samples;
};

ak_wav* AK_LoadWAV(ak_char* File, ak_string_builder* ErrorStream=NULL);
ak_wav* AK_LoadWAV(ak_string File, ak_string_builder* ErrorStream=NULL);
void AK_FreeWAV(ak_wav* WAV);

#endif
