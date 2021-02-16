#ifndef AK_ZLIB_H
#define AK_ZLIB_H

ak_bool AK_ZLibDecompress(void* DecompressedData, void* CompressedData, ak_u32 CompressedDataLength, ak_string_builder* ErrorStream=NULL);

#endif