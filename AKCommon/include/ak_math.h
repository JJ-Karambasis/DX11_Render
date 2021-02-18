#ifndef AK_MATH_H
#define AK_MATH_H

#define AK_PI 3.14159265359f
#define AK_INV_PI 0.31830988618f
#define AK_SQRT2_2 0.70710678118f
#define AK_ToDegree(rad) ((rad) * 180.0f * AK_INV_PI)
#define AK_ToRadians(degree) ((degree) * AK_PI / 180.0f)
#define AK_Square(v) ((v)*(v))

#define AK_XAxis(type) AK_V3<type>((type)1, (type)0, (type)0)
#define AK_YAxis(type) AK_V3<type>((type)0, (type)1, (type)0)
#define AK_ZAxis(type) AK_V3<type>((type)0, (type)0, (type)1)

#define AK_XAxisF32() AK_XAxis(ak_f32)
#define AK_YAxisF32() AK_YAxis(ak_f32)
#define AK_ZAxisF32() AK_ZAxis(ak_f32)

template <typename type>
struct ak_v2
{
    union
    {
        struct
        {
            type x;
            type y;
        };
        
        struct
        {
            type w;
            type h;
        };
        
        type Data[2];
    };
    
    type& operator[](ak_u32 Index);    
    ak_u32 LargestComp();        
};
typedef ak_v2<ak_f32> ak_v2f;
typedef ak_v2<ak_i32> ak_v2i;
typedef ak_v2<ak_u32> ak_v2u;

template <typename type>
struct ak_v3
{
    union
    {
        struct
        {
            type x;
            type y;
            type z;
        };
        
        struct
        {
            type w;
            type h;
            type d;
        };
        
        struct
        {
            type roll;
            type pitch;
            type yaw;
        };
        
        struct
        {
            type r;
            type g;
            type b;
        };
        
        struct
        {
            type radius;
            type azimuth;
            type inclination;
        };
        
        struct
        {
            ak_v2<type> xy;
            type __unused_0__;
        };
        
        struct
        {
            type __unused_1__;
            ak_v2<type> yz;
        };
        
        type Data[3];
    };
    
    type& operator[](ak_u32 Index);    
    ak_u32 LargestComp();        
};
typedef ak_v3<ak_f32> ak_v3f;
typedef ak_v3<ak_i32> ak_v3i;
typedef ak_v3f ak_color3f;

template <typename type>
struct ak_v4
{
    union
    {
        struct
        {
            type x;
            type y;
            type z;
            type w;
        };
        
        struct
        {
            ak_v2<type> xy;
            ak_v2<type> zw;
        };
        
        struct
        {
            type __unused_0__;
            ak_v2<type> yz;
            type __unused_1__;
        };
        
        struct
        {
            ak_v3<type> xyz;
            type __unused_2__;            
        };
        
        struct
        {
            ak_v3<type> v;
            type s;
        };
        
        struct
        {
            type __unused_3__;
            ak_v3<type> yzw;
        };
        
        struct
        {
            type r;
            type g;
            type b;
            type a;
        };
        
        type Data[4];
    };
    
    type& operator[](ak_u32 Index);    
    ak_u32 LargestComp();
};
typedef ak_v4<ak_f32> ak_v4f;
typedef ak_v4<ak_i32> ak_v4i;
typedef ak_v4f ak_color4f;

template <typename type>
struct ak_quat
{
    union
    {
        struct
        {
            type x;
            type y;
            type z;
            type w;
        };
        
        struct
        {
            ak_v3<type> v;
            type s;
        };
        
        ak_v4<type> q;
        type Data[4];
    };
    
    type& operator[](ak_u32 Index);
};
typedef ak_quat<ak_f32> ak_quatf;

template <typename type>
struct ak_m3
{
    union
    {
        type Data[9];
        ak_v3<type> Rows[3];
        
        struct
        {
            type m00, m01, m02;
            type m10, m11, m12;
            type m20, m21, m22;
        };
        
        struct
        {
            ak_v3<type> Row0;
            ak_v3<type> Row1;
            ak_v3<type> Row2;
        };
        
        struct
        {
            ak_v3<type> XAxis;
            ak_v3<type> YAxis;
            ak_v3<type> ZAxis;
        };        
    };
    
    type& operator[](ak_u32 Index);
};
typedef ak_m3<ak_f32> ak_m3f;

template <typename type>
struct ak_m4
{
    union
    {
        type Data[16];
        ak_v4<type> Rows[4];
        
        struct
        {
            type m00, m01, m02, m03;
            type m10, m11, m12, m13;
            type m20, m21, m22, m23;
            type m30, m31, m32, m33;
        };
        
        struct
        {
            ak_v4<type> Row0;
            ak_v4<type> Row1;
            ak_v4<type> Row2;
            ak_v4<type> Row3;            
        };
        
        struct
        {
            ak_v4<type> XAxis;
            ak_v4<type> YAxis;
            ak_v4<type> ZAxis;
            ak_v4<type> Translation;
        };
    };
    
    type& operator[](ak_u32 Index);    
};

template <>
struct ak_m4<ak_f32>
{
    union
    {
        ak_f32 Data[16];
        ak_v4f Rows[4];
        
        struct
        {
            ak_f32 m00, m01, m02, m03;
            ak_f32 m10, m11, m12, m13;
            ak_f32 m20, m21, m22, m23;
            ak_f32 m30, m31, m32, m33;
        };
        
        struct
        {
            ak_v4f Row0;
            ak_v4f Row1;
            ak_v4f Row2;
            ak_v4f Row3;            
        };
        
        struct
        {
            ak_v4f XAxis;
            ak_v4f YAxis;
            ak_v4f ZAxis;
            ak_v4f Translation;
        };
        
        struct
        {
            __m128 WideRows[4];
        };
    };
    
    ak_f32& operator[](ak_u32 Index);
};

typedef ak_m4<ak_f32> ak_m4f;

template <typename type>
struct ak_sqt
{
    union
    {
        ak_v3<type>   Translation;
        struct { type tx, ty, tz, __pad0__; };
    };
    
    union
    {
        ak_quat<type> Orientation;
        struct { type qx, qy, qz, qw; };
    };
    
    union
    {
        ak_v3<type>   Scale;
        struct { type sx, sy, sz, __pad1__; };
    };
};
typedef ak_sqt<ak_f32> ak_sqtf;

template <typename type>
struct ak_rigid_transform
{
    ak_v3<type> Position;
    ak_m3<type> Orientation;
};
typedef ak_rigid_transform<ak_f32> ak_rigid_transformf;

template <typename type>
struct ak_frustum
{
    ak_v3f Normals[6];
    ak_f32 Distances[6];
};

struct ak_quadratic_equation_result
{
    ak_u32 RootCount;
    ak_f32 Roots[2];
};

struct ak_vertex_p2_uv_c
{
    ak_v2f P;
    ak_v2f UV;
    ak_u32 C;
};

struct ak_vertex_p3
{
    ak_v3f P;
};

struct ak_vertex_p3_n3
{
    ak_v3f P;
    ak_v3f N;
};

struct ak_vertex_p3_c3
{
    ak_v3f P;
    ak_color3f C;
};

struct ak_vertex_p4_c4
{
    ak_v4f P;
    ak_color4f C;
};

struct ak_vertex_p3_n3_w
{
    ak_v3f P;
    ak_v3f N;
    ak_u8  JointI[4];
    ak_f32 JointW[4];
};

struct ak_vertex_p3_n3_uv
{
    ak_v3f P;
    ak_v3f N;
    ak_v2f UV;
};

struct ak_vertex_p3_n3_uv_w
{
    ak_v3f P;
    ak_v3f N;
    ak_v2f UV;
    ak_u8  JointI[4];
    ak_f32 JointW[4];
};

////////////////////////////////////////////////////
//NOTE(EVERYONE): Common f32 math functions
////////////////////////////////////////////////////
template <typename type> type AK_Lerp(type A, type t, type B);

////////////////////////////////////////////////////
//NOTE(EVERYONE): V2 operations
////////////////////////////////////////////////////
template <typename type> ak_v2<type> AK_V2();
template <typename type> ak_v2<type> AK_V2(type x, type y);
template <typename type> ak_v2<type> AK_V2(type* V);
template <typename type> ak_v2<type> operator+(type Left, ak_v2<type> Right);
template <typename type> ak_v2<type> operator+(ak_v2<type> Left, type Right);
template <typename type> ak_v2<type> operator+(ak_v2<type> Left, ak_v2<type> Right);
template <typename type> ak_v2<type>& operator+=(ak_v2<type>& Left, type Right);
template <typename type> ak_v2<type>& operator+=(ak_v2<type>& Left, ak_v2<type> Right);
template <typename type> ak_v2<type> operator-(type Left, ak_v2<type> Right);
template <typename type> ak_v2<type> operator-(ak_v2<type> Left, type Right);
template <typename type> ak_v2<type> operator-(ak_v2<type> Left, ak_v2<type> Right);
template <typename type> ak_v2<type>& operator-=(ak_v2<type>& Left, type Right);
template <typename type> ak_v2<type>& operator-=(ak_v2<type>& Left, ak_v2<type> Right);
template <typename type> ak_v2<type> operator*(type Left, ak_v2<type> Right);
template <typename type> ak_v2<type> operator*(ak_v2<type> Left, type Right);
template <typename type> ak_v2<type> operator*(ak_v2<type> Left, ak_v2<type> Right);
template <typename type> ak_v2<type>& operator*=(ak_v2<type>& Left, type Right);
template <typename type> ak_v2<type>& operator*=(ak_v2<type>& Left, ak_v2<type> Right);
template <typename type> ak_v2<type> operator/(type Left, ak_v2<type> Right);
template <typename type> ak_v2<type> operator/(ak_v2<type> Left, type Right);
template <typename type> ak_v2<type> operator/(ak_v2<type> Left, ak_v2<type> Right);
template <typename type> ak_v2<type>& operator/=(ak_v2<type>& Left, type Right);
template <typename type> ak_v2<type>& operator/=(ak_v2<type>& Left, ak_v2<type> Right);
template <typename type> ak_v2<type> operator-(ak_v2<type> V);
template <typename type> ak_bool operator!=(type Left, ak_v2<type> Right);
template <typename type> ak_bool operator!=(ak_v2<type> Left, type Right);
template <typename type> ak_bool operator!=(ak_v2<type> Left, ak_v2<type> Right);
template <typename type> ak_bool operator==(type Left, ak_v2<type> Right);
template <typename type> ak_bool operator==(ak_v2<type> Left, type Right);
template <typename type> ak_bool operator==(ak_v2<type> Left, ak_v2<type> Right);
template <typename type> ak_bool operator<(type Left, ak_v2<type> Right);
template <typename type> ak_bool operator<(ak_v2<type> Left, type Right);
template <typename type> ak_bool operator<(ak_v2<type> Left, ak_v2<type> Right);
template <typename type> ak_bool operator>(type Left, ak_v2<type> Right);
template <typename type> ak_bool operator>(ak_v2<type> Left, type Right);
template <typename type> ak_bool operator>(ak_v2<type> Left, ak_v2<type> Right);
template <typename type> ak_bool operator>=(type Left, ak_v2<type> Right);
template <typename type> ak_bool operator>=(ak_v2<type> Left, type Right);
template <typename type> ak_bool operator>=(ak_v2<type> Left, ak_v2<type> Right);
template <typename type> ak_bool operator<=(type Left, ak_v2<type> Right);
template <typename type> ak_bool operator<=(ak_v2<type> Left, type Right);
template <typename type> ak_bool operator<=(ak_v2<type> Left, ak_v2<type> Right);
template <typename type> ak_v2<type> AK_AbsV2(ak_v2<type> V);
template <typename type> type AK_Dot(ak_v2<type> Left, ak_v2<type> Right);
template <typename type> type AK_SqrMagnitude(ak_v2<type> V);
template <typename type> type AK_Magnitude(ak_v2<type> V);
template <typename type> ak_v2<type> AK_Normalize(ak_v2<type> V);
template <typename type> ak_v2<type> AK_Lerp(ak_v2<type> A, type t, ak_v2<type> B);

ak_v2f AK_InvalidV2f();
ak_v2f AK_V2f(ak_i32 x, ak_i32 y);
ak_v2f AK_V2f(ak_v2i V);
ak_v2f AK_V2f(ak_f64 x, ak_f64 y);
ak_v2f AK_V2f(ak_f64* Data);
ak_bool AK_EqualApprox(ak_v2f A, ak_v2f B, ak_f32 Epsilon);

////////////////////////////////////////////////////
//NOTE(EVERYONE): V3 operations
////////////////////////////////////////////////////

template <typename type> ak_v3<type> AK_V3();
template <typename type> ak_v3<type> AK_V3(type x, type y, type z);
template <typename type> ak_v3<type> AK_V3(ak_v2<type> xy);
template <typename type> ak_v3<type> AK_V3(ak_v2<type> xy, type z);
template <typename type> ak_v3<type> AK_V3(type x, ak_v2<type> yz);
template <typename type> ak_v3<type> AK_V3(type* V);
template <typename type> ak_v3<type> operator+(type Left, ak_v3<type> Right);
template <typename type> ak_v3<type> operator+(ak_v3<type> Left, type Right);
template <typename type> ak_v3<type> operator+(ak_v3<type> Left, ak_v3<type> Right);
template <typename type> ak_v3<type>& operator+=(ak_v3<type>& Left, type Right);
template <typename type> ak_v3<type>& operator+=(ak_v3<type>& Left, ak_v3<type> Right);
template <typename type> ak_v3<type> operator-(type Left, ak_v3<type> Right);
template <typename type> ak_v3<type> operator-(ak_v3<type> Left, type Right);
template <typename type> ak_v3<type> operator-(ak_v3<type> Left, ak_v3<type> Right);
template <typename type> ak_v3<type>& operator-=(ak_v3<type>& Left, type Right);
template <typename type> ak_v3<type>& operator-=(ak_v3<type>& Left, ak_v3<type> Right);
template <typename type> ak_v3<type> operator*(type Left, ak_v3<type> Right);
template <typename type> ak_v3<type> operator*(ak_v3<type> Left, type Right);
template <typename type> ak_v3<type> operator*(ak_v3<type> Left, ak_v3<type> Right);
template <typename type> ak_v3<type>& operator*=(ak_v3<type>& Left, type Right);
template <typename type> ak_v3<type>& operator*=(ak_v3<type>& Left, ak_v3<type> Right);
template <typename type> ak_v3<type> operator/(type Left, ak_v3<type> Right);
template <typename type> ak_v3<type> operator/(ak_v3<type> Left, type Right);
template <typename type> ak_v3<type> operator/(ak_v3<type> Left, ak_v3<type> Right);
template <typename type> ak_v3<type>& operator/=(ak_v3<type>& Left, type Right);
template <typename type> ak_v3<type>& operator/=(ak_v3<type>& Left, ak_v3<type> Right);
template <typename type> ak_v3<type> operator-(ak_v3<type> V);
template <typename type> ak_bool operator!=(type Left, ak_v3<type> Right);
template <typename type> ak_bool operator!=(ak_v3<type> Left, type Right);
template <typename type> ak_bool operator!=(ak_v3<type> Left, ak_v3<type> Right);
template <typename type> ak_bool operator==(type Left, ak_v3<type> Right);
template <typename type> ak_bool operator==(ak_v3<type> Left, type Right);
template <typename type> ak_bool operator==(ak_v3<type> Left, ak_v3<type> Right);
template <typename type> ak_bool operator<(type Left, ak_v3<type> Right);
template <typename type> ak_bool operator<(ak_v3<type> Left, type Right);
template <typename type> ak_bool operator<(ak_v3<type> Left, ak_v3<type> Right);
template <typename type> ak_bool operator>(type Left, ak_v3<type> Right);
template <typename type> ak_bool operator>(ak_v3<type> Left, type Right);
template <typename type> ak_bool operator>(ak_v3<type> Left, ak_v3<type> Right);
template <typename type> ak_bool operator>=(type Left, ak_v3<type> Right);
template <typename type> ak_bool operator>=(ak_v3<type> Left, type Right);
template <typename type> ak_bool operator>=(ak_v3<type> Left, ak_v3<type> Right);
template <typename type> ak_bool operator<=(type Left, ak_v3<type> Right);
template <typename type> ak_bool operator<=(ak_v3<type> Left, type Right);
template <typename type> ak_bool operator<=(ak_v3<type> Left, ak_v3<type> Right);
template <typename type> ak_v3<type> AK_AbsV3(ak_v3<type> V);
template <typename type> type AK_Dot(ak_v3<type> Left, ak_v3<type> Right);
template <typename type> type AK_SqrMagnitude(ak_v3<type> V);
template <typename type> type AK_Magnitude(ak_v3<type> V);
template <typename type> ak_v3<type> AK_Normalize(ak_v3<type> V);
template <typename type> ak_v3<type> AK_Lerp(ak_v3<type> A, type t, ak_v3<type> B);
template <typename type> ak_v3<type> AK_Cross(ak_v3<type> Left, ak_v3<type> Right);
template <typename type> type AK_Determinant(ak_v3<type> U, ak_v3<type> V, ak_v3<type> W);
template <typename type> ak_v3<type> AK_SphericalToCartesian(ak_v3<type> SphericalCoordinates);
template <typename type> ak_v3<type> AK_CartesianToSpherical(ak_v3<type> Cartesian);
template <typename type> ak_v3<type> AK_ToNormalizedDeviceCoordinates(ak_v2<type> XY, ak_v2<type> Resolution);
template <typename type> ak_v3<type> AK_Orthogonal(ak_v3<type> V);

ak_v3f AK_InvalidV3f();
ak_v3f AK_V3f(ak_i32 x, ak_i32 y, ak_i32 z);
ak_v3f AK_V3f(ak_f64 x, ak_f64 y, ak_f64 z);
ak_v3f AK_V3f(ak_f64* Data);
ak_v3f AK_V3f(const ak_f64* Data);
ak_bool AK_EqualApprox(ak_v3f A, ak_v3f B, ak_f32 Epsilon);
ak_bool AK_EqualEps(ak_v3f A, ak_v3f B);

ak_color3f AK_RGB(ak_f32 r, ak_f32 g, ak_f32 b);
ak_color3f AK_White3();
ak_color3f AK_Red3();
ak_color3f AK_Green3();
ak_color3f AK_Blue3();
ak_color3f AK_Yellow3();
ak_color3f AK_Black3();
ak_color3f AK_Cyan3();
ak_color3f AK_Magenta3();

////////////////////////////////////////////////////
//NOTE(EVERYONE): V4 operations
////////////////////////////////////////////////////
template <typename type> ak_v4<type> AK_V4();
template <typename type> ak_v4<type> AK_V4(type x, type y, type z, type w);
template <typename type> ak_v4<type> AK_V4(ak_v2<type> xy);
template <typename type> ak_v4<type> AK_V4(ak_v2<type> xy, type z, type w);
template <typename type> ak_v4<type> AK_V4(ak_v3<type> xyz);
template <typename type> ak_v4<type> AK_V4(ak_v2<type> xy, ak_v2<type> yz);
template <typename type> ak_v4<type> AK_V4(ak_v3<type> xyz, type w);
template <typename type> ak_v4<type> AK_V4(type* V);
template <typename type> ak_v4<type> operator+(type Left, ak_v4<type> Right);
template <typename type> ak_v4<type> operator+(ak_v4<type> Left, type Right);
template <typename type> ak_v4<type> operator+(ak_v4<type> Left, ak_v4<type> Right);
template <typename type> ak_v4<type>& operator+=(ak_v4<type>& Left, type Right);
template <typename type> ak_v4<type>& operator+=(ak_v4<type>& Left, ak_v4<type> Right);
template <typename type> ak_v4<type> operator-(type Left, ak_v4<type> Right);
template <typename type> ak_v4<type> operator-(ak_v4<type> Left, type Right);
template <typename type> ak_v4<type> operator-(ak_v4<type> Left, ak_v4<type> Right);
template <typename type> ak_v4<type>& operator-=(ak_v4<type>& Left, type Right);
template <typename type> ak_v4<type>& operator-=(ak_v4<type>& Left, ak_v4<type> Right);
template <typename type> ak_v4<type> operator*(type Left, ak_v4<type> Right);
template <typename type> ak_v4<type> operator*(ak_v4<type> Left, type Right);
template <typename type> ak_v4<type> operator*(ak_v4<type> Left, ak_v4<type> Right);
template <typename type> ak_v4<type>& operator*=(ak_v4<type>& Left, type Right);
template <typename type> ak_v4<type>& operator*=(ak_v4<type>& Left, ak_v4<type> Right);
template <typename type> ak_v4<type> operator/(type Left, ak_v4<type> Right);
template <typename type> ak_v4<type> operator/(ak_v4<type> Left, type Right);
template <typename type> ak_v4<type> operator/(ak_v4<type> Left, ak_v4<type> Right);
template <typename type> ak_v4<type>& operator/=(ak_v4<type>& Left, type Right);
template <typename type> ak_v4<type>& operator/=(ak_v4<type>& Left, ak_v4<type> Right);
template <typename type> ak_v4<type> operator-(ak_v4<type> V);
template <typename type> ak_bool operator!=(type Left, ak_v4<type> Right);
template <typename type> ak_bool operator!=(ak_v4<type> Left, type Right);
template <typename type> ak_bool operator!=(ak_v4<type> Left, ak_v4<type> Right);
template <typename type> ak_bool operator==(type Left, ak_v4<type> Right);
template <typename type> ak_bool operator==(ak_v4<type> Left, type Right);
template <typename type> ak_bool operator==(ak_v4<type> Left, ak_v4<type> Right);
template <typename type> ak_bool operator<(type Left, ak_v4<type> Right);
template <typename type> ak_bool operator<(ak_v4<type> Left, type Right);
template <typename type> ak_bool operator<(ak_v4<type> Left, ak_v4<type> Right);
template <typename type> ak_bool operator>(type Left, ak_v4<type> Right);
template <typename type> ak_bool operator>(ak_v4<type> Left, type Right);
template <typename type> ak_bool operator>(ak_v4<type> Left, ak_v4<type> Right);
template <typename type> ak_bool operator>=(type Left, ak_v4<type> Right);
template <typename type> ak_bool operator>=(ak_v4<type> Left, type Right);
template <typename type> ak_bool operator>=(ak_v4<type> Left, ak_v4<type> Right);
template <typename type> ak_bool operator<=(type Left, ak_v4<type> Right);
template <typename type> ak_bool operator<=(ak_v4<type> Left, type Right);
template <typename type> ak_bool operator<=(ak_v4<type> Left, ak_v4<type> Right);
template <typename type> ak_v4<type> AK_AbsV4(ak_v4<type> V);
template <typename type> type AK_Dot(ak_v4<type> Left, ak_v4<type> Right);
template <typename type> type AK_SqrMagnitude(ak_v4<type> V);
template <typename type> type AK_Magnitude(ak_v4<type> V);
template <typename type> ak_v4<type> AK_Normalize(ak_v4<type> V);

ak_v4f AK_InvalidV4f();
ak_v4f AK_V4f(ak_i32 x, ak_i32 y, ak_i32 z, ak_i32 w);
ak_v4f AK_V4f(ak_f64 x, ak_f64 y, ak_f64 z, ak_f64 w);
ak_v4f AK_V4f(ak_f64* Data);
ak_bool AK_EqualApprox(ak_v4f A, ak_v4f B, ak_f32 Epsilon);
ak_v4f AK_Lerp(ak_v4f A, ak_f32 t, ak_v4f B);

ak_color4f AK_RGBA(ak_f32 r, ak_f32 g, ak_f32 b, ak_f32 a = 1.0f);
ak_color4f AK_White4(ak_f32 a = 1.0f);
ak_color4f AK_Red4(ak_f32 a = 1.0f);
ak_color4f AK_Green4(ak_f32 a = 1.0f);
ak_color4f AK_Blue4(ak_f32 a = 1.0f);
ak_color4f AK_Yellow4(ak_f32 a = 1.0f);
ak_color4f AK_Black4(ak_f32 a = 1.0f);
ak_color4f AK_Cyan4(ak_f32 a = 1.0f);
ak_color4f AK_Magenta4(ak_f32 a = 1.0f);
ak_color4f AK_Orange4(ak_f32 a = 1.0f);

ak_u32 AK_RGBA_U32(ak_color4f Color);
ak_color4f AK_RGBA(ak_u32 Color);

////////////////////////////////////////////////////
//NOTE(EVERYONE): Quaternion operations
////////////////////////////////////////////////////
template <typename type> ak_quat<type> AK_Quat(type x, type y, type z, type w);
template <typename type> ak_quat<type> AK_Quat(ak_v3<type> v, type s);
template <typename type> ak_quat<type> AK_Quat(ak_v4<type> q);
template <typename type> ak_quat<type> AK_Quat(type* Data);
template <typename type> ak_quat<type> operator+(type Left, ak_quat<type> Right);
template <typename type> ak_quat<type> operator+(ak_quat<type> Left, type Right);
template <typename type> ak_quat<type> operator+(ak_quat<type> Left, ak_quat<type> Right);
template <typename type> ak_quat<type>& operator+=(ak_quat<type>& Left, type Right);
template <typename type> ak_quat<type>& operator+=(ak_quat<type>& Left, ak_quat<type> Right);
template <typename type> ak_quat<type> operator-(type Left, ak_quat<type> Right);
template <typename type> ak_quat<type> operator-(ak_quat<type> Left, type Right);
template <typename type> ak_quat<type> operator-(ak_quat<type> Left, ak_quat<type> Right);
template <typename type> ak_quat<type>& operator-=(ak_quat<type>& Left, type Right);
template <typename type> ak_quat<type>& operator-=(ak_quat<type>& Left, ak_quat<type> Right);
template <typename type> ak_quat<type> operator*(type Left, ak_quat<type> Right);
template <typename type> ak_quat<type> operator*(ak_quat<type> Left, type Right);
template <typename type> ak_quat<type> operator*(ak_quat<type> Left, ak_quat<type> Right);
template <typename type> ak_quat<type>& operator*=(ak_quat<type>& Left, type Right);
template <typename type> ak_quat<type>& operator*=(ak_quat<type>& Left, ak_quat<type> Right);
template <typename type> ak_bool operator!=(type Left, ak_quat<type> Right);
template <typename type> ak_bool operator!=(ak_quat<type> Left, type Right);
template <typename type> ak_bool operator!=(ak_quat<type> Left, ak_quat<type> Right);
template <typename type> ak_bool operator==(type Left, ak_quat<type> Right);
template <typename type> ak_bool operator==(ak_quat<type> Left, type Right);
template <typename type> ak_bool operator==(ak_quat<type> Left, ak_quat<type> Right);
template <typename type> ak_bool operator<(type Left, ak_quat<type> Right);
template <typename type> ak_bool operator<(ak_quat<type> Left, type Right);
template <typename type> ak_bool operator<(ak_quat<type> Left, ak_quat<type> Right);
template <typename type> ak_bool operator>(type Left, ak_quat<type> Right);
template <typename type> ak_bool operator>(ak_quat<type> Left, type Right);
template <typename type> ak_bool operator>(ak_quat<type> Left, ak_quat<type> Right);
template <typename type> ak_bool operator>=(type Left, ak_quat<type> Right);
template <typename type> ak_bool operator>=(ak_quat<type> Left, type Right);
template <typename type> ak_bool operator>=(ak_quat<type> Left, ak_quat<type> Right);
template <typename type> ak_bool operator<=(type Left, ak_quat<type> Right);
template <typename type> ak_bool operator<=(ak_quat<type> Left, type Right);
template <typename type> ak_bool operator<=(ak_quat<type> Left, ak_quat<type> Right);
template <typename type> ak_quat<type> AK_EulerToQuat(type Roll, type Pitch, type Yaw);
template <typename type> ak_quat<type> AK_EulerToQuat(ak_v3<type> Euler);
template <typename type> ak_quat<type> AK_MatrixToQuat(ak_m3<type> Matrix);
template <typename type> ak_v3<type>   AK_QuatToEuler(ak_quat<type> Quat);
template <typename type> ak_m3<type>   AK_QuatToMatrix(ak_quat<type> Quat);
template <typename type> ak_v3<type> AK_QuatToXAxis(ak_quat<type> Quat);
template <typename type> ak_v3<type> AK_QuatToYAxis(ak_quat<type> Quat);
template <typename type> ak_v3<type> AK_QuatToZAxis(ak_quat<type> Quat);
template <typename type> ak_quat<type> AK_IdentityQuat();
template <typename type> ak_quat<type> AK_Conjugate(ak_quat<type> Quat);
template <typename type> type          AK_SqrMagnitude(ak_quat<type> Quat);
template <typename type> type          AK_Magnitude(ak_quat<type> Quat);
template <typename type> ak_quat<type> AK_Normalize(ak_quat<type> Quat);
template <typename type> ak_quat<type> AK_RotQuat(ak_v3<type> Axis, type Angle);
template <typename type> void AK_QuatToAxisAngle(ak_v3<type>* Axis, type* Angle, ak_quat<type> Orientation);
template <typename type> ak_v3<type>   AK_Rotate(ak_v3<type> V, ak_quat<type> Quat);
template <typename type> ak_quat<type> AK_Inverse(ak_quat<type> Quat);
template <typename type> ak_quat<type> AK_QuatDiff(ak_quat<type> Q1, ak_quat<type> Q2);
template <typename type> ak_quat<type> AK_GetRotationFromVecToVec(ak_v3<type> V1, ak_v3<type> V2);

ak_quatf AK_Lerp(ak_quatf A, ak_f32 t, ak_quatf B);
ak_quatf AK_SLerp(ak_quatf A, ak_f32 t, ak_quatf B);

////////////////////////////////////////////////////
//NOTE(EVERYONE): Matrix3x3 operations
////////////////////////////////////////////////////
template <typename type> ak_m3<type> AK_M3();
template <typename type> ak_m3<type> AK_M3(type Diagonal);
template <typename type> ak_m3<type> AK_M3(ak_v3<type> Row0, ak_v3<type> Row1, ak_v3<type> Row2);
template <typename type> ak_m3<type> AK_M3(type* Data);
template <typename type> ak_m3<type> AK_M3(ak_v3<type>* Rows);
template <typename type> ak_m3<type> AK_M3(ak_m4<type> M);
template <typename type> ak_m3<type> AK_IdentityM3();
template <typename type> void        AK_Basis(ak_v3<type> Z, ak_v3<type>* X, ak_v3<type>* Y);
template <typename type> ak_m3<type> AK_Basis(ak_v3<type> Z);
template <typename type> type        AK_Determinant(ak_m3<type> M);
template <typename type> ak_m3<type> AK_Transpose(ak_m3<type> M);
template <typename type> ak_m3<type> AK_Inverse(ak_m3<type> M);
template <typename type> ak_m3<type> AK_TransformM3(ak_m3<type> Orient, ak_v3<type> Scale);
template <typename type> ak_m3<type> AK_TransformM3(ak_quat<type> Orient, ak_v3<type> Scale);
template <typename type> ak_m3<type> AK_InvTransformM3(ak_m3<type> Orient, ak_v3<type> Scale);
template <typename type> ak_m3<type> AK_InvTransformM3(ak_quat<type> Orient, ak_v3<type> Scale);
template <typename type> ak_m3<type> AK_InvTransformM3(ak_m3<type> M);
template <typename type> ak_m3<type> AK_NormalizeM3(ak_m3<type> M);
template <typename type> ak_m3<type> operator+(type Left, ak_m3<type> Right);
template <typename type> ak_m3<type> operator+(ak_m3<type> Left, type Right);
template <typename type> ak_m3<type> operator+(ak_m3<type> Left, ak_m3<type> Right);
template <typename type> ak_m3<type>& operator+=(ak_m3<type>& Left, type Right);
template <typename type> ak_m3<type>& operator+=(ak_m3<type>& Left, ak_m3<type> Right);
template <typename type> ak_m3<type> operator-(type Left, ak_m3<type> Right);
template <typename type> ak_m3<type> operator-(ak_m3<type> Left, type Right);
template <typename type> ak_m3<type> operator-(ak_m3<type> Left, ak_m3<type> Right);
template <typename type> ak_m3<type>& operator-=(ak_m3<type>& Left, type Right);
template <typename type> ak_m3<type>& operator-=(ak_m3<type>& Left, ak_m3<type> Right);
template <typename type> ak_m3<type> operator*(type Left, ak_m3<type> Right);
template <typename type> ak_m3<type> operator*(ak_m3<type> Left, type Right);
template <typename type> ak_m3<type> operator*(ak_m3<type> Left, ak_m3<type> Right);
template <typename type> ak_m3<type>& operator*=(ak_m3<type>& Left, type Right);
template <typename type> ak_m3<type>& operator*=(ak_m3<type>& Left, ak_m3<type> Right);
template <typename type> ak_v3<type> operator*(ak_v3<type> Left, ak_m3<type> Right);
template <typename type> ak_v3<type>& operator*=(ak_v3<type>& Left, ak_m3<type> Right);
ak_m3f AK_OrientAt(ak_v3f Position, ak_v3f Target, ak_v3f Up = AK_ZAxisF32());
ak_m3f AK_M3f(ak_f64* Data);

////////////////////////////////////////////////////
//NOTE(EVERYONE): Matrix4x4 operations
////////////////////////////////////////////////////
template <typename type> ak_m4<type> AK_M4();
template <typename type> ak_m4<type> AK_M4(type Diagonal);
template <typename type> ak_m4<type> AK_M4(ak_v4<type> Row0, ak_v4<type> Row1, ak_v4<type> Row2, ak_v4<type> Row3);
template <typename type> ak_m4<type> AK_M4(ak_v4<type>* Rows);
template <typename type> ak_m4<type> AK_M4(ak_v3<type> Row0, ak_v3<type> Row1, ak_v3<type> Row2, ak_v3<type> Row3);
template <typename type> ak_m4<type> AK_M4(ak_v3<type>* Rows);
template <typename type> ak_m4<type> AK_M4(type* Data);
template <typename type> ak_m4<type> AK_IdentityM4();
template <typename type> type        AK_Determinant(ak_m4<type> M);
template <typename type> ak_m4<type> AK_Transpose(ak_m4<type> M);
template <typename type> ak_m4<type> AK_Inverse(ak_m4<type> M);
template <typename type> ak_m4<type> AK_TransformM4(ak_v3<type> Translation);
template <typename type> ak_m4<type> AK_TransformM4(ak_v3<type> Translation, ak_v3<type> XAxis, ak_v3<type> YAxis, ak_v3<type> ZAxis);
template <typename type> ak_m4<type> AK_TransformM4(ak_v3<type> Translation, ak_m3<type>  Orientation);
template <typename type> ak_m4<type> AK_TransformM4(ak_v3<type> Translation, ak_quat<type> Orientation);
template <typename type> ak_m4<type> AK_TransformM4(ak_v3<type> Translation, ak_quat<type> Orientation, type Scale);
template <typename type> ak_m4<type> AK_TransformM4(ak_v3<type> Translation, ak_v3<type> Scale);
template <typename type> ak_m4<type> AK_TransformM4(ak_v3<type> Translation, ak_m3<type>  Orientation,  ak_v3<type> Scale);
template <typename type> ak_m4<type> AK_TransformM4(ak_v3<type> Translation, ak_m3<type>  Orientation,  ak_f32 Scale);
template <typename type> ak_m4<type> AK_TransformM4(ak_v3<type> Translation, ak_quat<type> Orientation, ak_v3<type> Scale);
template <typename type> ak_m4<type> AK_TransformM4(ak_sqt<type> Transform);
template <typename type> ak_m4<type> AK_ScaleM4(ak_v3<type> Scale);
template <typename type> ak_m4<type> AK_InvScale(ak_v3<type> Scale);
template <typename type> ak_m4<type> AK_XRotM4(type Radians);
template <typename type> ak_m4<type> AK_YRotM4(type Radians);
template <typename type> ak_m4<type> AK_ZRotM4(type Radians);
template <typename type> ak_m4<type> AK_XYZRotM4(ak_v3<type> Radians);
template <typename type> ak_m4<type> AK_InvTransformM4(ak_v3<type> Translation, ak_v3<type> XAxis, ak_v3<type> YAxis, ak_v3<type> ZAxis);
template <typename type> ak_m4<type> AK_InvTransformM4(ak_v3<type> Translation, ak_m3<type>  Orientation);
template <typename type> ak_m4<type> AK_InvTransformM4(ak_v3<type> Translation, ak_quat<type> Orientation);
template <typename type> ak_m4<type> AK_InvTransformM4(ak_m4<type> M);
template <typename type> ak_m4<type> operator+(type Left, ak_m4<type> Right);
template <typename type> ak_m4<type> operator+(ak_m4<type> Left, type Right);
template <typename type> ak_m4<type> operator+(ak_m4<type> Left, ak_m4<type> Right);
template <typename type> ak_m4<type>& operator+=(ak_m4<type>& Left, type Right);
template <typename type> ak_m4<type>& operator+=(ak_m4<type>& Left, ak_m4<type>& Right);
template <typename type> ak_m4<type> operator-(type Left, ak_m4<type> Right);
template <typename type> ak_m4<type> operator-(ak_m4<type> Left, type Right);
template <typename type> ak_m4<type> operator-(ak_m4<type> Left, ak_m4<type> Right);
template <typename type> ak_m4<type>& operator-=(ak_m4<type>& Left, type Right);
template <typename type> ak_m4<type>& operator-=(ak_m4<type>& Left, ak_m4<type> Right);
template <typename type> ak_m4<type> operator*(type Left, ak_m4<type> Right);
template <typename type> ak_m4<type> operator*(ak_m4<type> Left, type Right);
template <typename type> ak_m4<type> operator*(ak_m4<type> Left, ak_m4<type> Right);
template <typename type> ak_m4<type>& operator*=(ak_m4<type>& Left, type Right);
template <typename type> ak_m4<type>& operator*=(ak_m4<type>& Left, ak_m4<type> Right);
template <typename type> ak_v4<type> operator*(ak_v4<type> Left, ak_m4<type> Right);
template <typename type> ak_v4<type>& operator*=(ak_v4<type>& Left, ak_m4<type> Right);
template <typename type> ak_m4<type> AK_Perspective(type FOV, type AspectRatio, type Near, type Far);
template <typename type> ak_m4<type> AK_Orthographic(type Left, type Right, type Top, type Bottom, type Near, type Far);
template <typename type> ak_v3<type> AK_TransformPoint(ak_v3<type> Point, ak_m4<type> Transform);
template <typename type> void AK_TransformPoints(ak_v3<type>* Points, ak_u32 PointCount, ak_m4<type> Transform);
ak_m4f AK_LookAt(ak_v3f Position, ak_v3f Target, ak_v3f Up = AK_ZAxisF32());
ak_m4f AK_M4f(ak_f64* Data);

////////////////////////////////////////////////////
//NOTE(EVERYONE): SQT operations
////////////////////////////////////////////////////
template <typename type> ak_sqt<type> AK_SQT();
template <typename type> ak_sqt<type> AK_SQT(ak_v3<type> Translation);
template <typename type> ak_sqt<type> AK_SQT(ak_v3<type> Translation, ak_quat<type> Orientation);
template <typename type> ak_sqt<type> AK_SQT(ak_v3<type> Translation, ak_v3<type> Scale);
template <typename type> ak_sqt<type> AK_SQT(ak_v3<type> Translation, type Scale);
template <typename type> ak_sqt<type> AK_SQT(ak_v3<type> Translation, ak_quat<type> Orientation, ak_v3<type> Scale);
template <typename type> ak_sqt<type> AK_SQT(ak_v3<type> Translation, ak_quat<type> Orientation, type Scale);
template <typename type> ak_sqt<type> AK_SQT(ak_v3<type> Translation, ak_m3<type> Orientation);
template <typename type> ak_sqt<type> AK_SQT(ak_m4<type> M);
template <typename type> ak_sqt<type> operator*(ak_sqt<type> A, ak_sqt<type> B);
template <typename type> ak_sqt<type>& operator*=(ak_sqt<type>& A, ak_sqt<type> B);
template <typename type> ak_bool operator==(ak_sqt<type> A, ak_sqt<type> B);
template <typename type> ak_v3<type> AK_Transform(ak_v3<type> Point, ak_v3<type> Translation, ak_quat<type> Orientation, ak_v3<type> Scale);
template <typename type> ak_v3<type> AK_Transform(ak_v3<type> Point, ak_sqt<type> Transform);
template <typename type> ak_v3<type> AK_InverseTransform(ak_v3<type> Point, ak_v3<type> Translation, ak_quat<type> Orientation, ak_v3<type> Scale);
template <typename type> ak_v3<type> AK_InverseTransform(ak_v3<type> Point, ak_sqt<type> Transform);

/////////////////////////////////////////////////
//NOTE(EVERYONE): Quadratic equation
/////////////////////////////////////////////////
ak_quadratic_equation_result AK_SolveQuadraticEquation(ak_f32 a, ak_f32 b, ak_f32 c);

/////////////////////////////////////////////////
//NOTE(EVERYONE): Frustum operations
/////////////////////////////////////////////////
template <typename type> ak_frustum<type> AK_CreateFrustum(ak_m4<type> Matrix);
template <typename type> void AK_GetFrustumCorners(ak_v3<type>* Corners, ak_m4<type> Perspective);

#endif