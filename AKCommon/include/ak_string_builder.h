#ifndef AK_STRING_BUILDER_H
#define AK_STRING_BUILDER_H

struct ak_string_builder
{
    ak_arena* Arena;    
    
    void WriteChar(ak_char Character);
    void Write(ak_char* Format, ...);
    void Write(const ak_char* Format, ...);
    void Write(ak_char* Format, va_list Args);
    void Write(const ak_char* Format, va_list Args);
    
    void WriteLine(ak_char* Format, ...);
    void WriteLine(const ak_char* Format, ...);
    void WriteLine(ak_char* Format, va_list Args);
    void WriteLine(const ak_char* Format,  va_list Args);
    
    void NewLine();
    
    ak_string PushString(ak_arena* DstArena);
    void Clear();
    void ReleaseMemory();
};

#endif