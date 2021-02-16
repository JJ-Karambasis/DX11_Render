#ifndef AK_PNG_H
#define AK_PNG_H

struct ak_png
{
    void* Texels;
    ak_u32   Width;
    ak_u32   Height;
    ak_u32   ComponentCount;        
};

ak_png* AK_LoadPNG(ak_char* File, ak_string_builder* ErrorStream=NULL);
ak_png* AK_LoadPNG(ak_string File, ak_string_builder* ErrorStream=NULL);
void AK_FreePNG(ak_png* PNG);

#endif