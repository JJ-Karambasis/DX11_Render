#define ak_f32 float
#define ak_i32 int
#define ak_u32 uint
#define ak_v2f float2
#define ak_v3f float3
#define ak_v4f float4
#define ak_v2i int2
#define ak_v3i int3
#define ak_v4i int4
#define ak_v2u uint2
#define ak_v3u uint3
#define ak_v4u uint4
#define ak_color3f float3
#define ak_color4f float4
#define ak_m3f float3x3
#define ak_m4f float4x4

struct vs_object_input_p4_c4
{
    ak_v4f P : Position0;
    ak_v4f C : Color0;
};

struct vs_object_output_p4_c4
{
    ak_v4f P : SV_Position;
    ak_v4f C : Color0;
};

vs_object_output_p4_c4 ObjectVertexShader(vs_object_input_p4_c4 Input)
{
    vs_object_input_p4_c4 Result;
    Result.P = Input.P;
    Result.C = Input.C;
    return Result;
}

ak_color4f ObjectPixelShader(vs_object_output_p4_c4 Input) : SV_Target
{
    return Input.C;
}