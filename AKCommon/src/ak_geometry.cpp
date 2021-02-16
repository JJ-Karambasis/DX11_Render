#define GJK_INTERNAL__EPSILON 1.19209290E-07F
#define EPA_INTERNAL__MAX_VERTICES 128
#define EPA_INTERNAL__MAX_FACES 256
#define EPA_INTERNAL__EPSILON 1e-4f
#define EPA_INTERNAL__PLANE_EPSILON 1e-5f
#define GJK_INTERNAL__SUPPORT_BRUTE_FORCE 0

#ifndef EPA_MARGIN
#define EPA_MARGIN 0.01f
#endif

struct internal__point_support
{
    ak_v3f P;
    inline ak_v3f Support(ak_v3f D) { return P; }
};

struct internal__moving_point_support
{
    ak_v3f P;
    ak_v3f Delta;
    inline ak_v3f Support(ak_v3f D) { return AK_Dot(D, Delta) > 0 ? P+Delta : P; }
};

struct internal__line_segment_support
{
    ak_v3f P[2];
    ak_v3f Support(ak_v3f D);
};

struct internal__moving_line_segment_support
{
    ak_v3f P[2];
    ak_v3f Delta;
    ak_v3f Support(ak_v3f D);
};

struct internal__sphere_support
{
    ak_v3f CenterP;
    ak_f32 Radius;
    ak_v3f Support(ak_v3f D);
};

struct internal__capsule_support
{
    ak_v3f P0;
    ak_v3f P1;
    ak_f32 Radius;
    ak_v3f Support(ak_v3f D);
};

struct internal__obb_support
{
    ak_obb* OBB;
    ak_v3f Support(ak_v3f D);
};

struct internal__moving_obb_support
{
    ak_obb* OBB;
    ak_v3f Delta;
    ak_v3f Support(ak_v3f D);
};

struct internal__margin_obb_support
{
    ak_obb* OBB;
    ak_f32 Margin;
    ak_v3f Support(ak_v3f D);
};

struct internal__convex_hull_support
{
    ak_convex_hull* Hull;
    ak_m4f T;
    ak_m3f BTransposed;
    ak_v3f Support(ak_v3f D);
};

struct internal__moving_convex_hull_support
{
    ak_convex_hull* Hull;
    ak_m4f T;
    ak_m3f BTransposed;
    ak_v3f Delta;
    ak_v3f Support(ak_v3f D);
};

struct internal__margin_convex_hull_support
{
    ak_convex_hull* Hull;
    ak_m4f T;
    ak_m3f BTransposed;
    ak_f32 Margin;
    ak_v3f Support(ak_v3f D);
};

struct gjk_internal__vertex
{
    ak_v3f V;
    ak_v3f W;
    ak_v3f A;
    ak_v3f B;
    ak_i32 AID;
    ak_i32 BID;
};

struct gjk_internal__simplex
{
    ak_u32 Count;
    gjk_internal__vertex Vertex[4];
    ak_f32 Barycentric[4];
};

struct gjk_internal__distance
{
    gjk_internal__simplex Simplex;
    ak_f32 SqrDistance;
    ak_closest_points ClosestPoints;
    ak_v3f CacheV;
};

struct epa_internal__face
{
    ak_v3f Normal;
    ak_f32 Distance;
    gjk_internal__vertex Vertices[3];
    epa_internal__face* AdjFaces[3];
    epa_internal__face* ChildFaces[2];        
    ak_u8 Edges[3];
    ak_u8 Pass;
};

struct epa_internal__list
{
    ak_u32 Count;
    epa_internal__face* Root;
    void Add(epa_internal__face* Face);
    void Remove(epa_internal__face* Face);
};

struct epa_internal__silhouette
{
    epa_internal__face* Face0;
    epa_internal__face* Face1;
    ak_u32    NextFace;
};

enum epa_internal__status
{
    EPA_STATUS_NONE,
    EPA_STATUS_INVALID_TETRAHEDRON,    
    EPA_STATUS_OUT_OF_FACES,
    EPA_STATUS_DEGENERATE, 
    EPA_STATUS_NOT_CONVEX,
    EPA_STATUS_INVALID_HULL,
    EPA_STATUS_ACCURACY_REACHED, 
    EPA_STATUS_VALID
};

struct epa_internal__result
{
    ak_bool IsValid;
    epa_internal__status Status;
    ak_v3f Witness[2];    
    ak_v3f Normal;
    ak_f32 Penetration;
};


struct epa_internal__context
{
    epa_internal__status Status;    
    epa_internal__face FaceStore[EPA_INTERNAL__MAX_FACES];    
    epa_internal__list Hull;
    epa_internal__list Free;
    
    epa_internal__face* CreateFace(gjk_internal__vertex P0, gjk_internal__vertex P1, gjk_internal__vertex P2, ak_bool Forced);
    epa_internal__face* GetBestFace();
    ak_bool Expand(ak_u32 Pass, gjk_internal__vertex Vertex, epa_internal__face* Face, ak_u32 Edge, epa_internal__silhouette* Silhouette);
    template <typename typeA, typename typeB> epa_internal__result Evaluate(typeA* ObjectA, typeB* ObjectB, gjk_internal__vertex* Vertices);
};


template <typename typeA, typename typeB> gjk_internal__vertex GJK_Internal__GetSupport(typeA* ObjectA, typeB* ObjectB, ak_v3f V);
template <typename typeA, typename typeB> gjk_internal__distance GJK_Internal__Distance(typeA* ObjectA, typeB* ObjectB, ak_v3f InitialV);
template <typename typeA, typename typeB> gjk_internal__distance GJK_Internal__Distance(typeA* ObjectA, typeB* ObjectB);
template <typename typeA, typename typeB> ak_bool GJK_Internal__Intersected(typeA* ObjectA, typeB* ObjectB);
template <typename typeA, typename typeB> ak_toi GJK_Internal__TOI(typeA* ObjectA, typeB* ObjectB, ak_v3f R);
ak_bool GJK_Internal__QuadraticIntersected(gjk_internal__distance Distance, ak_f32 RadiusA, ak_f32 RadiusB);

template <typename typeA, typename typeB> epa_internal__result EPA_Internal__Evaluate(typeA* ObjectA, typeB* ObjectB);

ak_sphere AK_CreateSphere(ak_v3f CenterP, ak_f32 Radius)
{
    ak_sphere Sphere = {CenterP, Radius};
    return Sphere;
}

ak_capsule AK_CreateCapsule(ak_v3f P0, ak_v3f P1, ak_f32 Radius)
{
    ak_capsule Capsule = {P0, P1, Radius};
    return Capsule;
}

ak_capsule AK_CreateCapsule(ak_v3f Bottom, ak_f32 Radius, ak_f32 Height)
{
    ak_v3f P0 = Bottom + AK_ZAxisF32()*Radius;
    return AK_CreateCapsule(P0, P0+AK_ZAxisF32()*Height, Radius);
}

ak_aabb AK_CreateAABB(ak_f32 MinX, ak_f32 MinY, ak_f32 MinZ, ak_f32 MaxX, ak_f32 MaxY, ak_f32 MaxZ)
{
    ak_aabb Result;
    Result.MinX = MinX;
    Result.MinY = MinY;
    Result.MinZ = MinZ;
    Result.MaxX = MaxX;
    Result.MaxY = MaxY;
    Result.MaxZ = MaxZ;
    return Result;
}

ak_aabb AK_CreateAABB(ak_v3f Min, ak_v3f Max)
{
    return AK_CreateAABB(Min.x, Min.y, Min.z, Max.x, Max.y, Max.z);
}

ak_obb AK_CreateOBB(ak_v3f CenterP, ak_m3f Orientation, ak_v3f Scale)
{
    ak_obb Result;
    Result.CenterP = CenterP;
    Result.Orientation = Orientation;
    Result.Dimensions = Scale;
    return Result;
}

ak_plane AK_CreatePlane(ak_v3f Origin, ak_v3f Normal)
{
    ak_plane Result = {};
    Result.Normal = Normal;
    Result.Origin = Origin;
    Result.Equation[0] = Normal.x;
    Result.Equation[1] = Normal.y;
    Result.Equation[2] = Normal.z;
    Result.Equation[3] = -(Normal.x*Origin.x + Normal.y*Origin.y + Normal.z*Origin.z);
    return Result;
}

ak_capsule AK_TransformCapsule(ak_v3f P0, ak_v3f P1, ak_f32 Radius, ak_sqtf Transform)
{
    ak_capsule Result;
    
    ak_v3f ZScale = AK_V3(1.0f, 1.0f, Transform.Scale.z);
    Result.P0 = AK_Transform(P0, Transform.Translation, Transform.Orientation, ZScale);
    Result.P1 = AK_Transform(P1, Transform.Translation, Transform.Orientation, ZScale);
    
    ak_u32 Component = Transform.Scale.xy.LargestComp();
    Result.Radius = Radius*Transform.Scale[Component];
    return Result;
}

ak_capsule AK_TransformCapsule(ak_capsule* Capsule, ak_sqtf Transform)
{
    return AK_TransformCapsule(Capsule->P0, Capsule->P1, Capsule->Radius, Transform);
}

ak_capsule AK_TransformCapsule(ak_v3f P0, ak_v3f P1, ak_f32 Radius, ak_v3f Translation, ak_m3f Orientation, ak_v3f Scale)
{
    ak_capsule Result;
    ak_v3f ZScale = AK_V3(1.0f, 1.0f, Scale.z);
    ak_m4f Transform = AK_TransformM4(Translation, Orientation, ZScale);
    Result.P0 = AK_TransformPoint(P0, Transform);
    Result.P1 = AK_TransformPoint(P1, Transform);
    Result.Radius = Radius*Scale[Scale.xy.LargestComp()];
    return Result;
}

ak_capsule AK_TransformCapsule(ak_capsule* Capsule, ak_v3f Translation, ak_m3f Orientation, ak_v3f Scale)
{
    return AK_TransformCapsule(Capsule->P0, Capsule->P1, Capsule->Radius, Translation, Orientation, Scale);
}

ak_sphere AK_TransformSphere(ak_v3f CenterP, ak_f32 Radius, ak_sqtf Transform)
{
    ak_sphere Result = {};
    ak_u32 Component = Transform.Scale.LargestComp();    
    Result.Radius = Radius*Transform.Scale[Component];        
    Result.CenterP = AK_Transform(CenterP, Transform);
    return Result;
}

ak_sphere AK_TransformSphere(ak_sphere* Sphere, ak_sqtf Transform)
{
    return AK_TransformSphere(Sphere->CenterP, Sphere->Radius, Transform);
}

ak_obb AK_TransformOBB(ak_obb* OBB, ak_sqtf Transform)
{
    ak_obb Result;
    Result.CenterP = AK_Transform(OBB->CenterP, Transform);
    Result.Dimensions = OBB->Dimensions*Transform.Scale;
    Result.Orientation = OBB->Orientation*AK_QuatToMatrix(Transform.Orientation);
    return Result;
}

void AK_TranslateCapsule(ak_capsule* Capsule, ak_v3f Delta)
{
    Capsule->P0 += Delta;
    Capsule->P1 += Delta;
}

void AK_TranslateSphere(ak_sphere* Sphere, ak_v3f Delta)
{
    Sphere->CenterP += Delta;
}

ak_f32 AK_SignDistance(ak_plane Plane, ak_v3f Point)
{
    return AK_Dot(Point, Plane.Normal) + Plane.Equation[3];
}

ak_v3f AK_RayPixelToView(ak_v2i PixelCoords, ak_v2i PixelDim, ak_m4f Perspective)
{
    ak_v3f NDC = AK_ToNormalizedDeviceCoordinates(AK_V2f(PixelCoords), AK_V2f(PixelDim));
    ak_v4f Clip = AK_V4(NDC.xy, -1.0f, 1.0f);
    
    ak_m4f InvPerspective = AK_Inverse(Perspective);
    ak_v4f RayView = Clip*InvPerspective;
    
    return RayView.xyz;
}

ak_v3f AK_RayPixelToWorld(ak_v2i PixelCoords, ak_v2i PixelDim, ak_m4f Perspective, ak_m4f View)
{
    ak_m4f InvView = AK_InvTransformM4(View);
    ak_v3f RayView = AK_RayPixelToView(PixelCoords, PixelDim, Perspective);
    ak_v3f RayWorld = AK_Normalize(RayView*AK_M3(InvView));
    return RayWorld;
}

ak_toi AK_RaySphereCast(ak_v3f Origin, ak_v3f Direction, ak_v3f CenterP, ak_f32 Radius)
{
    ak_toi Result = {};
    
    ak_v3f CO = Origin-CenterP;
    ak_f32 A = AK_Dot(CO, Direction);
    ak_f32 B = AK_SqrMagnitude(CO) - AK_Square(Radius);
    
    if(A > 0 && B > 0) return Result;
    ak_f32 Discr = AK_Square(A) - B;
    if(Discr < 0) return Result;
    
    Result.Intersected = true;
    
    Result.t = -A - AK_Sqrt(Discr);
    if(Result.t < 0) Result.t = 0;
    
    Result.Normal = AK_Normalize(CenterP - (Origin + Direction*Result.t));
    
    return Result;
}

ak_toi AK_RaySphereCast(ak_ray Ray, ak_sphere Sphere)
{
    return AK_RaySphereCast(Ray.Origin, Ray.Direction, Sphere.CenterP, Sphere.Radius);
}

ak_toi AK_RayCapsuleCast(ak_v3f Origin, ak_v3f Direction, ak_v3f P0, ak_v3f P1, ak_f32 Radius)
{
    ak_toi Result = {};
    
    ak_v3f AB = P1-P0;
    ak_v3f AO = Origin - P0;
    
    ak_f32 RayCapsuleProjection = AK_Dot(AB, Direction);
    ak_f32 LineSegmentCapsuleProjection = AK_Dot(AB, AO);
    ak_f32 ABSqr = AK_SqrMagnitude(AB);
    
    ak_f32 m = RayCapsuleProjection / ABSqr;
    ak_f32 n = LineSegmentCapsuleProjection / ABSqr;
    
    ak_v3f Q = Direction - (AB*m);
    ak_v3f R = AO - (AB*n);
    
    ak_f32 a = AK_SqrMagnitude(Q);
    ak_f32 b = 2.0f * AK_Dot(Q, R);
    ak_f32 c = AK_SqrMagnitude(R) - AK_Square(Radius);
    
    if(AK_EqualZeroEps(a))
    {        
        ak_toi tA = AK_RaySphereCast(Origin, Direction, P0, Radius);
        ak_toi tB = AK_RaySphereCast(Origin, Direction, P1, Radius);
        
        if(!tA.Intersected || !tB.Intersected)
            return Result;
        
        Result.Intersected = true;
        Result.t = AK_Min(tA.t, tB.t);
        Result.Normal = (Result.t == tA.t) ? tA.Normal : tB.Normal;
        
        return Result;
    }
    
    ak_quadratic_equation_result QEqu = AK_SolveQuadraticEquation(a, b, c);
    if(QEqu.RootCount < 2)
        return Result;
    
    if(QEqu.Roots[0] < 0 && QEqu.Roots[1] < 0)
        return Result;
    
    ak_f32 tMin = AK_Min(QEqu.Roots[0], QEqu.Roots[1]);
    tMin = AK_Max(tMin, 0.0f);
    
    ak_f32 t0 = tMin*m + n;
    if(t0 < 0)
    {        
        return AK_RaySphereCast(Origin, Direction, P0, Radius);
    }
    else if(t0 > 1)
    {        
        return AK_RaySphereCast(Origin, Direction, P1, Radius);
    }
    else
    {
        ak_v3f P = Origin + Direction*tMin;
        ak_v3f K = P0 + AB*t0;
        
        Result.Intersected = true;
        Result.t = tMin;
        Result.Normal = AK_Normalize(K-P);
        
        return Result;
    }        
}

ak_toi AK_RayCapsuleCast(ak_ray Ray, ak_capsule Capsule)
{
    return AK_RayCapsuleCast(Ray.Origin, Ray.Direction, Capsule.P0, Capsule.P1, Capsule.Radius);
}

ak_ray_barycentric_cast AK_RayTriangleCast(ak_v3f Origin, ak_v3f Direction, ak_v3f P0, ak_v3f P1, ak_v3f P2)
{
    ak_ray_barycentric_cast Result = {};
    
    ak_v3f Edge1 = P1 - P0;
    ak_v3f Edge2 = P2 - P0;
    
    ak_v3f PVec = AK_Cross(Direction, Edge2);
    
    ak_f32 Det = AK_Dot(Edge1, PVec);
    
    if(AK_EqualZeroEps(Det))
        return Result;
    
    ak_v3f TVec = Origin - P0;
    
    ak_f32 u = AK_Dot(TVec, PVec);
    if(u < 0.0f || u > Det)
        return Result;
    
    ak_v3f QVec = AK_Cross(TVec, Edge1);
    
    ak_f32 v = AK_Dot(Direction, QVec);
    if(v < 0.0f || u + v > Det)
        return Result;
    
    ak_f32 t = AK_Dot(Edge2, QVec);
    
    ak_f32 InvDet = 1.0f / Det;
    
    Result.t = t*InvDet;
    Result.u = u*InvDet;
    Result.v = v*InvDet;
    Result.Intersected = true;
    
    return Result;
}

ak_ray_barycentric_cast AK_RayTriangleCast(ak_ray Ray, ak_triangle Triangle)
{
    return AK_RayTriangleCastNoCull(Ray.Origin, Ray.Direction, Triangle.P0, Triangle.P1, Triangle.P2);
}

ak_ray_barycentric_cast AK_RayTriangleCastNoCull(ak_v3f Origin, ak_v3f Direction, ak_v3f P0, ak_v3f P1, ak_v3f P2)
{
    ak_ray_barycentric_cast Result = {};
    
    ak_v3f Edge1 = P1 - P0;
    ak_v3f Edge2 = P2 - P0;
    
    ak_v3f PVec = AK_Cross(Direction, Edge2);
    
    ak_f32 Det = AK_Dot(Edge1, PVec);
    
    if(AK_EqualZeroEps(Det))    
        return Result;    
    
    ak_f32 InverseDeterminant = 1.0f / Det;
    ak_v3f TVec = Origin - P0;
    
    ak_f32 u = AK_Dot(TVec, PVec) * InverseDeterminant;
    if(u < 0 || u > 1)
    {
        return Result;
    }
    
    ak_v3f QVec = AK_Cross(TVec, Edge1);
    
    ak_f32 v = AK_Dot(Direction, QVec) * InverseDeterminant;
    if(v < 0 || u + v > 1)
    {
        return Result;
    }
    
    Result.Intersected = true;
    Result.t = AK_Dot(Edge2, QVec) * InverseDeterminant;
    Result.u = u;
    Result.v = v;
    
    return Result;
}

ak_ray_barycentric_cast AK_RayTriangleCastNoCull(ak_ray Ray, ak_triangle Triangle)
{
    return AK_RayTriangleCastNoCull(Ray.Origin, Ray.Direction, Triangle.P0, Triangle.P1, Triangle.P2);
}

ak_ray_barycentric_cast AK_RayTriangleMeshCast(ak_v3f RayOrigin, ak_v3f RayDirection, 
                                               ak_v3f* Vertices, ak_u16* Indices, ak_u32 IndexCount, 
                                               ak_m4f MeshTransform)
{
    ak_ray_barycentric_cast Result = {};
    
    ak_u32 TriangleCount = IndexCount/3;
    for(ak_u32 TriangleIndex = 0; TriangleIndex < TriangleCount; TriangleIndex++)
    {
        ak_u32 Index = TriangleIndex*3;
        
        ak_v3f P0 = AK_TransformPoint(Vertices[Indices[Index]], MeshTransform);
        ak_v3f P1 = AK_TransformPoint(Vertices[Indices[Index+1]], MeshTransform);
        ak_v3f P2 = AK_TransformPoint(Vertices[Indices[Index+2]], MeshTransform);
        
        ak_ray_barycentric_cast TriangleCast = AK_RayTriangleCast(RayOrigin, RayDirection, P0, P1, P2);
        
        if(TriangleCast.Intersected)
        {
            if(!Result.Intersected || (TriangleCast.t < Result.t))
                Result = TriangleCast;
        }
    }
    
    return Result;
}

ak_ray_barycentric_cast AK_RayTriangleMeshCast(ak_ray Ray, ak_v3f* Vertices, ak_u16* Indices, ak_u32 IndexCount, 
                                               ak_m4f MeshTransform)
{
    return AK_RayTriangleMeshCastNoCull(Ray.Origin, Ray.Direction, Vertices, Indices, IndexCount, MeshTransform);
}

ak_ray_barycentric_cast AK_RayTriangleMeshCastNoCull(ak_v3f RayOrigin, ak_v3f RayDirection, 
                                                     ak_v3f* Vertices, ak_u16* Indices, ak_u32 IndexCount, 
                                                     ak_m4f MeshTransform)
{
    ak_ray_barycentric_cast Result = {};
    
    ak_u32 TriangleCount = IndexCount/3;
    for(ak_u32 TriangleIndex = 0; TriangleIndex < TriangleCount; TriangleIndex++)
    {
        ak_u32 Index = TriangleIndex*3;
        
        ak_v3f P0 = AK_TransformPoint(Vertices[Indices[Index]], MeshTransform);
        ak_v3f P1 = AK_TransformPoint(Vertices[Indices[Index+1]], MeshTransform);
        ak_v3f P2 = AK_TransformPoint(Vertices[Indices[Index+2]], MeshTransform);
        
        ak_ray_barycentric_cast TriangleCast = AK_RayTriangleCastNoCull(RayOrigin, RayDirection, P0, P1, P2);
        
        if(TriangleCast.Intersected)
        {
            if(!Result.Intersected || (TriangleCast.t < Result.t))
                Result = TriangleCast;
        }
    }
    
    return Result;
}

ak_ray_barycentric_cast AK_RayTriangleMeshCastNoCull(ak_ray Ray, ak_v3f* Vertices, ak_u16* Indices, ak_u32 IndexCount, 
                                                     ak_m4f MeshTransform)
{
    return AK_RayTriangleMeshCast(Ray.Origin, Ray.Direction, Vertices, Indices, IndexCount, MeshTransform);
}

ak_ray_barycentric_cast AK_RayTriangleMeshCast(ak_v3f RayOrigin, ak_v3f RayDirection, 
                                               ak_v3f* Vertices, ak_u32* Indices, ak_u32 IndexCount, 
                                               ak_m4f MeshTransform, ak_f32* uOut=NULL, ak_f32* vOut=NULL)
{
    ak_ray_barycentric_cast Result = {};
    
    ak_u32 TriangleCount = IndexCount/3;
    for(ak_u32 TriangleIndex = 0; TriangleIndex < TriangleCount; TriangleIndex++)
    {
        ak_u32 Index = TriangleIndex*3;
        
        ak_v3f P0 = AK_TransformPoint(Vertices[Indices[Index]], MeshTransform);
        ak_v3f P1 = AK_TransformPoint(Vertices[Indices[Index+1]], MeshTransform);
        ak_v3f P2 = AK_TransformPoint(Vertices[Indices[Index+2]], MeshTransform);
        
        ak_ray_barycentric_cast TriangleCast = AK_RayTriangleCast(RayOrigin, RayDirection, P0, P1, P2);
        
        if(TriangleCast.Intersected)
        {
            if(!Result.Intersected || (TriangleCast.t < Result.t))
                Result = TriangleCast;
        }
    }
    
    return Result;
}

ak_ray_barycentric_cast AK_RayTriangleMeshCast(ak_ray Ray, ak_v3f* Vertices, ak_u32* Indices, ak_u32 IndexCount, 
                                               ak_m4f MeshTransform)
{
    return AK_RayTriangleMeshCastNoCull(Ray.Origin, Ray.Direction, Vertices, Indices, IndexCount, 
                                        MeshTransform);
}

ak_ray_barycentric_cast AK_RayTriangleMeshCastNoCull(ak_v3f RayOrigin, ak_v3f RayDirection, 
                                                     ak_v3f* Vertices, ak_u32* Indices, ak_u32 IndexCount, 
                                                     ak_m4f MeshTransform)
{
    ak_ray_barycentric_cast Result = {};
    
    ak_u32 TriangleCount = IndexCount/3;
    for(ak_u32 TriangleIndex = 0; TriangleIndex < TriangleCount; TriangleIndex++)
    {
        ak_u32 Index = TriangleIndex*3;
        
        ak_v3f P0 = AK_TransformPoint(Vertices[Indices[Index]], MeshTransform);
        ak_v3f P1 = AK_TransformPoint(Vertices[Indices[Index+1]], MeshTransform);
        ak_v3f P2 = AK_TransformPoint(Vertices[Indices[Index+2]], MeshTransform);
        
        ak_ray_barycentric_cast TriangleCast = AK_RayTriangleCastNoCull(RayOrigin, RayDirection, P0, P1, P2);
        
        if(TriangleCast.Intersected)
        {
            if(!Result.Intersected || (TriangleCast.t < Result.t))
                Result = TriangleCast;
        }
    }
    
    return Result;
}

ak_ray_barycentric_cast AK_RayTriangleMeshCastNoCull(ak_ray Ray, ak_v3f* Vertices, ak_u32* Indices, ak_u32 IndexCount, 
                                                     ak_m4f MeshTransform)
{
    return AK_RayTriangleMeshCastNoCull(Ray.Origin, Ray.Direction, Vertices, Indices, IndexCount, MeshTransform);
}

ak_ray_barycentric_cast AK_RayQuadCast(ak_v3f Origin, ak_v3f Direction, ak_v3f P0, ak_v3f P1, ak_v3f P2, ak_v3f P3)
{
    ak_ray_barycentric_cast Result = AK_RayTriangleCastNoCull(Origin, Direction, P0, P1, P2);
    if(!Result.Intersected)
        Result = AK_RayTriangleCastNoCull(Origin, Direction, P3, P0, P2);
    return Result;
}

ak_ray_barycentric_cast AK_RayQuadCast(ak_ray Ray, ak_v3f* Quad)
{
    return AK_RayQuadCast(Ray.Origin, Ray.Direction, Quad[0], Quad[1], Quad[2], Quad[3]);
}

ak_ray_cast AK_RayPlaneCast(ak_v3f Origin, ak_v3f Direction, ak_v3f PlaneNormal, ak_v3f PlanePoint)
{
    ak_ray_cast Result = {};
    
    ak_f32 Denom = AK_Dot(PlaneNormal, Direction);
    if(AK_EqualZeroEps(Denom)) return Result;    
    
    ak_f32 t = AK_Dot(PlanePoint - Origin, PlaneNormal) / Denom;
    if(t <= 0) return Result;    
    
    Result.Intersected = true;
    Result.t = t;
    
    return Result;
}

ak_ray_cast AK_RayPlaneCast(ak_ray Ray, ak_plane Plane)
{
    return AK_RayPlaneCast(Ray.Origin, Ray.Direction, Plane.Normal, Plane.Origin);
}

ak_ray_cast AK_RayAABBCast(ak_v3f Origin, ak_v3f Direction, ak_v3f Min, ak_v3f Max)
{
    ak_f32 tmin = (Min.x - Origin.x) / Direction.x;
    ak_f32 tmax = (Max.x - Origin.x) / Direction.x;
    
    ak_ray_cast Result = {};
    
    if(tmin > tmax)
    {
        ak_f32 temp = tmin;
        tmin = tmax;
        tmax = temp;
    }
    
    ak_f32 tymin = (Min.y - Origin.y) / Direction.y;
    ak_f32 tymax = (Max.y - Origin.y) / Direction.y;
    
    if(tymin > tymax)
    {
        ak_f32 temp = tymin;
        tymin = tymax;
        tymax = temp;
    }
    
    if(tmin > tymax || tymin > tmax)
        return Result;
    
    if(tymin > tmin)
        tmin = tymin;
    
    if(tymax < tmax)
        tmax = tymax;
    
    ak_f32 tzmin = (Min.z - Origin.z) / Direction.z;
    ak_f32 tzmax = (Max.z - Origin.z) / Direction.z;
    
    if(tzmin > tzmax)
    {
        ak_f32 temp = tzmin;
        tzmin = tzmax;
        tzmax = temp;
    }
    
    if(tmin > tzmax || tzmin > tmax)
        return Result;
    
    if(tzmin > tmin)
        tmin = tzmin;
    
    if(tzmax < tmax)
        tmax = tzmax;
    
    Result.Intersected = true;
    Result.t = tmin;
    return Result;
}

ak_ray_cast AK_RayAABBCast(ak_ray Ray, ak_aabb AABB)
{
    return AK_RayAABBCast(Ray.Origin, Ray.Direction, AABB.Min, AABB.Max);
}

ak_ray_cast AK_RayOBBCast(ak_v3f Origin, ak_v3f Direction, ak_v3f CenterP, ak_m3f Orientation, ak_v3f Dimensions)
{
    ak_v3f P0 = Origin;
    ak_v3f P1 = Origin+Direction;
    
    ak_v3f LocalOrigin = AK_V3(AK_Dot(P0-CenterP, Orientation.XAxis), 
                               AK_Dot(P0-CenterP, Orientation.YAxis), 
                               AK_Dot(P0-CenterP, Orientation.ZAxis));
    ak_v3f LocalP1 = AK_V3(AK_Dot(P1-CenterP, Orientation.XAxis), 
                           AK_Dot(P1-CenterP, Orientation.YAxis), 
                           AK_Dot(P1-CenterP, Orientation.ZAxis));
    ak_v3f LocalDirection = AK_Normalize(LocalP1-LocalOrigin);
    ak_v3f HalfDimensions = Dimensions*0.5f;
    return AK_RayAABBCast(LocalOrigin, LocalDirection, -HalfDimensions, HalfDimensions);
}

ak_ray_cast AK_RayOBBCast(ak_ray Ray, ak_obb OBB)
{
    return AK_RayOBBCast(Ray.Origin, Ray.Direction, OBB.CenterP, OBB.Orientation, OBB.Dimensions); 
}

ak_toi AK_LineSegmentSphereCast(ak_v3f* LineSegment, ak_v3f CenterP, ak_f32 Radius)
{
    ak_toi Result = {};
    
    ak_v3f D = LineSegment[1]-LineSegment[0];
    ak_f32 SegmentLength = AK_Magnitude(D);
    if(AK_EqualZeroEps(SegmentLength))
        return Result;
    
    D /= SegmentLength;
    
    Result = AK_RaySphereCast(LineSegment[0], D, CenterP, Radius);
    if(Result.Intersected)
    {
        if(Result.t > SegmentLength)
        {
            Result.Normal = {};
            Result.Intersected = false;
            return Result;
        }
        
        Result.t /= SegmentLength;        
    }    
    
    return Result;
}

ak_toi AK_LineSegmentSphereCast(ak_v3f* LineSegment, ak_sphere Sphere)
{
    return AK_LineSegmentSphereCast(LineSegment, Sphere.CenterP, Sphere.Radius);
}

ak_toi AK_LineSegmentCapsuleCast(ak_v3f* LineSegment, ak_v3f P0, ak_v3f P1, ak_f32 Radius)
{
    ak_toi Result = {};
    
    ak_v3f D = LineSegment[1]-LineSegment[0];
    ak_f32 SegmentLength = AK_Magnitude(D);
    if(AK_EqualZeroEps(SegmentLength))
        return Result;
    
    D /= SegmentLength;
    
    Result = AK_RayCapsuleCast(LineSegment[0], D, P0, P1, Radius);
    if(Result.Intersected)
    {
        if(Result.t > SegmentLength)
        {
            Result.Normal = {};
            Result.Intersected = false;
            return Result;
        }
        
        Result.t /= SegmentLength;
    }
    
    return Result;
}

ak_toi AK_LineSegmentCapsuleCast(ak_v3f* LineSegment, ak_capsule Capsule)
{
    return AK_LineSegmentCapsuleCast(LineSegment, Capsule.P0, Capsule.P1, Capsule.Radius);
}

ak_bool AK_SphereSphereOverlap(ak_sphere* SphereA, ak_sphere* SphereB)
{
    ak_f32 Radius = SphereA->Radius+SphereB->Radius;
    return AK_SqrMagnitude(SphereB->CenterP-SphereA->CenterP) <= AK_Square(Radius);
}

ak_bool AK_SphereCapsuleOverlap(ak_sphere* Sphere, ak_capsule* Capsule)
{
    ak_closest_points ClosestPoints = AK_ClosestPointsPointLineSegment(Sphere->CenterP, Capsule->P);
    ak_f32 Radius = Sphere->Radius+Capsule->Radius;
    return AK_SqrMagnitude(ClosestPoints.PointA-ClosestPoints.PointB) <= AK_Square(Radius);
}

ak_bool AK_SphereOBBOverlap(ak_sphere* Sphere, ak_obb* OBB)
{
    internal__point_support APoint = {Sphere->CenterP};
    internal__obb_support BOBB = {OBB};
    gjk_internal__distance Distance = GJK_Internal__Distance(&APoint, &BOBB);
    return GJK_Internal__QuadraticIntersected(Distance, Sphere->Radius, 0.0f);
}

ak_bool AK_SphereHullOverlap(ak_sphere* Sphere, ak_convex_hull* ConvexHull, ak_m4f ConvexHullTransform)
{
    internal__point_support APoint = {Sphere->CenterP};
    internal__convex_hull_support BHull = {ConvexHull, ConvexHullTransform, AK_Transpose(AK_M3(ConvexHullTransform))};
    gjk_internal__distance Distance = GJK_Internal__Distance(&APoint, &BHull);    
    return GJK_Internal__QuadraticIntersected(Distance, Sphere->Radius, 0.0f);
}

ak_bool AK_CapsuleSphereOverlap(ak_capsule* Capsule, ak_sphere* Sphere)
{
    return AK_SphereCapsuleOverlap(Sphere, Capsule);
}

ak_bool AK_CapsuleCapsuleOverlap(ak_capsule* CapsuleA, ak_capsule* CapsuleB)
{
    ak_closest_points ClosestPoints = AK_ClosestPointsLineSegments(CapsuleA->P, CapsuleB->P);
    ak_f32 Radius = CapsuleA->Radius+CapsuleB->Radius;
    return AK_SqrMagnitude(ClosestPoints.PointA-ClosestPoints.PointB) <= AK_Square(Radius);
}

ak_bool AK_CapsuleOBBOverlap(ak_capsule* Capsule, ak_obb* OBB)
{
    internal__line_segment_support ASupport = {Capsule->P[0], Capsule->P[1]};
    internal__obb_support BSupport = {OBB};
    gjk_internal__distance Distance = GJK_Internal__Distance(&ASupport, &BSupport);
    return GJK_Internal__QuadraticIntersected(Distance, Capsule->Radius, 0.0f);
}

ak_bool AK_CapsuleHullOverlap(ak_capsule* Capsule, ak_convex_hull* ConvexHull, ak_m4f ConvexHullTransform)
{
    internal__line_segment_support ASupport = {Capsule->P[0], Capsule->P[1]};    
    internal__convex_hull_support BSupport = {ConvexHull, ConvexHullTransform, AK_Transpose(AK_M3(ConvexHullTransform))};    
    gjk_internal__distance Distance = GJK_Internal__Distance(&ASupport, &BSupport);
    return GJK_Internal__QuadraticIntersected(Distance, Capsule->Radius, 0.0f);
}

ak_bool AK_OBBSphereOverlap(ak_obb* OBB, ak_sphere* Sphere)
{
    return AK_SphereOBBOverlap(Sphere, OBB);
}

ak_bool AK_OBBCapsuleOverlap(ak_obb* OBB, ak_capsule* Capsule)
{
    return AK_CapsuleOBBOverlap(Capsule, OBB);
}

ak_bool AK_OBBOBBOverlap(ak_obb* AOBB, ak_obb* BOBB)
{
    //TODO(JJ): The proper way to implement this is to use SAT on the obb's to 
    //potentially exit out early, improving performance. At some point we should 
    //convert from using GJK to SAT
    internal__obb_support ASupport = {AOBB};
    internal__obb_support BSupport = {BOBB};
    return GJK_Internal__Intersected(&ASupport, &BSupport);
}

ak_bool AK_OBBHullOverlap(ak_obb* OBB, ak_convex_hull* ConvexHull, ak_m4f ConvexHullTransform)
{
    internal__obb_support ASupport = {OBB};
    internal__convex_hull_support BSupport = {ConvexHull, ConvexHullTransform, AK_Transpose(AK_M3(ConvexHullTransform))};
    return GJK_Internal__Intersected(&ASupport, &BSupport);
}

ak_bool AK_HullSphereOverlap(ak_convex_hull* ConvexHull, ak_m4f ConvexHullTransform, ak_sphere* Sphere)
{
    return AK_SphereHullOverlap(Sphere, ConvexHull, ConvexHullTransform);
}

ak_bool AK_HullCapsuleOverlap(ak_convex_hull* ConvexHull, ak_m4f ConvexHullTransform, ak_capsule* Capsule)
{
    return AK_CapsuleHullOverlap(Capsule, ConvexHull, ConvexHullTransform);
}

ak_bool AK_HullOBBOverlap(ak_convex_hull* ConvexHull, ak_m4f ConvexHullTransform, ak_obb* OBB)
{
    return AK_OBBHullOverlap(OBB, ConvexHull, ConvexHullTransform);
}

ak_bool AK_HullHullOverlap(ak_convex_hull* ConvexHullA, ak_m4f ConvexHullTransformA, ak_convex_hull* ConvexHullB, ak_m4f ConvexHullTransformB)
{
    internal__convex_hull_support ASupport = {ConvexHullA, ConvexHullTransformA, AK_Transpose(AK_M3(ConvexHullTransformA))};
    internal__convex_hull_support BSupport = {ConvexHullB, ConvexHullTransformB, AK_Transpose(AK_M3(ConvexHullTransformB))};
    return GJK_Internal__Intersected(&ASupport, &BSupport);
}

ak_toi AK_SphereSphereTOI(ak_sphere* SphereA, ak_v3f DeltaA, ak_sphere* SphereB, ak_v3f DeltaB)
{
    ak_sphere LargeSphere = AK_CreateSphere(SphereB->CenterP, SphereA->Radius+SphereB->Radius);
    ak_v3f Delta = DeltaA-DeltaB;
    ak_v3f LineSegment[2] = {SphereA->CenterP, SphereA->CenterP+Delta};
    return AK_LineSegmentSphereCast(LineSegment, LargeSphere.CenterP, LargeSphere.Radius);
}

ak_toi AK_SphereCapsuleTOI(ak_sphere* SphereA, ak_v3f DeltaA, ak_capsule* CapsuleB, ak_v3f DeltaB)
{
    ak_capsule LargeCapsule = AK_CreateCapsule(CapsuleB->P0, CapsuleB->P1, SphereA->Radius+CapsuleB->Radius); 
    ak_v3f Delta = DeltaA-DeltaB;
    ak_v3f LineSegment[2] = {SphereA->CenterP, SphereA->CenterP+Delta};
    return AK_LineSegmentCapsuleCast(LineSegment, LargeCapsule.P0, LargeCapsule.P1, LargeCapsule.Radius);
}

ak_toi AK_SphereOBBTOI(ak_sphere* Sphere, ak_v3f DeltaA, ak_obb* OBB, ak_v3f DeltaB)
{
    internal__sphere_support ASupport = {Sphere->CenterP, Sphere->Radius};
    internal__obb_support BSupport = {OBB};
    return GJK_Internal__TOI(&ASupport, &BSupport, DeltaB-DeltaA);
}

ak_toi AK_SphereHullTOI(ak_sphere* SphereA, ak_v3f DeltaA, ak_convex_hull* ConvexHullB, ak_m4f TransformB, ak_v3f DeltaB)
{
    internal__sphere_support ASupport = {SphereA->CenterP, SphereA->Radius};
    internal__convex_hull_support BSupport = {ConvexHullB, TransformB, AK_Transpose(AK_M3(TransformB))};
    return GJK_Internal__TOI(&ASupport, &BSupport, DeltaB-DeltaA);
}

ak_toi AK_CapsuleSphereTOI(ak_capsule* CapsuleA, ak_v3f DeltaA, ak_sphere* SphereB, ak_v3f DeltaB)
{
    ak_toi Result = AK_SphereCapsuleTOI(SphereB, DeltaB, CapsuleA, DeltaA);
    if(Result.Intersected) Result.Normal = -Result.Normal;
    return Result;
}

ak_toi AK_CapsuleCapsuleTOI(ak_capsule* CapsuleA, ak_v3f DeltaA, ak_capsule* CapsuleB, ak_v3f DeltaB)
{
    internal__capsule_support ASupport = {CapsuleA->P0, CapsuleA->P1, CapsuleA->Radius};
    internal__capsule_support BSupport = {CapsuleB->P0, CapsuleB->P1, CapsuleB->Radius};
    return GJK_Internal__TOI(&ASupport, &BSupport, DeltaB-DeltaA);
}

ak_toi AK_CapsuleOBBTOI(ak_capsule* Capsule, ak_v3f DeltaA, ak_obb* OBB, ak_v3f DeltaB)
{
    internal__moving_line_segment_support ASupport = {Capsule->P0, Capsule->P1, DeltaA};
    internal__moving_obb_support BSupport = {OBB, DeltaB};
    gjk_internal__distance Distance = GJK_Internal__Distance(&ASupport, &BSupport);
    if(GJK_Internal__QuadraticIntersected(Distance, Capsule->Radius, 0.0f))
    {
        internal__capsule_support ASupport2 = {Capsule->P0, Capsule->P1, Capsule->Radius};
        internal__obb_support BSupport2 = {OBB};
        return GJK_Internal__TOI(&ASupport2, &BSupport2, DeltaB-DeltaA);
    }
    return {};
}

ak_toi AK_CapsuleHullTOI(ak_capsule* CapsuleA, ak_v3f DeltaA, ak_convex_hull* ConvexHullB, ak_m4f TransformB, ak_v3f DeltaB)
{
    internal__moving_line_segment_support ASupport = {CapsuleA->P0, CapsuleA->P1, DeltaA};
    internal__moving_convex_hull_support BSupport = {ConvexHullB, TransformB, AK_Transpose(AK_M3(TransformB)), DeltaB};
    gjk_internal__distance Distance = GJK_Internal__Distance(&ASupport, &BSupport);
    if(GJK_Internal__QuadraticIntersected(Distance, CapsuleA->Radius, 0.0f))
    {
        internal__capsule_support ASupport2 = {CapsuleA->P0, CapsuleA->P1, CapsuleA->Radius};
        internal__convex_hull_support BSupport2 = {ConvexHullB, TransformB, BSupport.BTransposed};
        return GJK_Internal__TOI(&ASupport2, &BSupport2, DeltaB-DeltaA);
    }
    return {};
}

ak_toi AK_OBBSphereTOI(ak_obb* OBB, ak_v3f DeltaA, ak_sphere* Sphere, ak_v3f DeltaB)
{
    ak_toi Result = AK_SphereOBBTOI(Sphere, DeltaB, OBB, DeltaA);
    if(Result.Intersected) Result.Normal = -Result.Normal;
    return Result;
}

ak_toi AK_OBBCapsuleTOI(ak_obb* OBB, ak_v3f DeltaA, ak_capsule* Capsule, ak_v3f DeltaB)
{
    ak_toi Result = AK_CapsuleOBBTOI(Capsule, DeltaB, OBB, DeltaA);
    if(Result.Intersected) Result.Normal = -Result.Normal;
    return Result;
}

ak_toi AK_OBBOBBTOI(ak_obb* AOBB, ak_v3f DeltaA, ak_obb* BOBB, ak_v3f DeltaB)
{
    internal__obb_support ASupport = {AOBB};
    internal__obb_support BSupport = {BOBB};
    return GJK_Internal__TOI(&ASupport, &BSupport, DeltaB-DeltaA);
}

ak_toi AK_OBBHullTOI(ak_obb* OBB, ak_v3f DeltaA, ak_convex_hull* ConvexHull, ak_m4f Transform, ak_v3f DeltaB)
{
    internal__obb_support ASupport = {OBB};
    internal__convex_hull_support BSupport = {ConvexHull, Transform, AK_Transpose(AK_M3(Transform))};
    return GJK_Internal__TOI(&ASupport, &BSupport, DeltaB-DeltaA);
}

ak_toi AK_HullSphereTOI(ak_convex_hull* ConvexHullA, ak_m4f TransformA, ak_v3f DeltaA, ak_sphere* SphereB, ak_v3f DeltaB)
{
    ak_toi Result = AK_SphereHullTOI(SphereB, DeltaB, ConvexHullA, TransformA, DeltaA);
    if(Result.Intersected) Result.Normal = -Result.Normal;
    return Result;
}

ak_toi AK_HullCapsuleTOI(ak_convex_hull* ConvexHullA, ak_m4f TransformA, ak_v3f DeltaA, ak_capsule* CapsuleB, ak_v3f DeltaB)
{
    ak_toi Result = AK_CapsuleHullTOI(CapsuleB, DeltaB, ConvexHullA, TransformA, DeltaA);
    if(Result.Intersected) Result.Normal = -Result.Normal;
    return Result;
}

ak_toi AK_HullOBBTOI(ak_convex_hull* ConvexHull, ak_m4f Transform, ak_v3f DeltaA, ak_obb* OBB, ak_v3f DeltaB)
{
    ak_toi Result = AK_OBBHullTOI(OBB, DeltaB, ConvexHull, Transform, DeltaA);
    if(Result.Intersected) Result.Normal = -Result.Normal;
    return Result;
}

ak_toi AK_HullHullTOI(ak_convex_hull* ConvexHullA, ak_m4f TransformA, ak_v3f DeltaA, 
                      ak_convex_hull* ConvexHullB, ak_m4f TransformB, ak_v3f DeltaB)
{
    internal__convex_hull_support ASupport = {ConvexHullA, TransformA, AK_Transpose(AK_M3(TransformA))};
    internal__convex_hull_support BSupport = {ConvexHullB, TransformB, AK_Transpose(AK_M3(TransformB))};
    return GJK_Internal__TOI(&ASupport, &BSupport, DeltaB-DeltaA);
}

ak_contact_vector AK_Internal__GetQuadraticDeepestContact(ak_v3f P0, ak_v3f P1, ak_f32 RadiusA, ak_f32 RadiusB)
{
    ak_f32 Radius = RadiusA+RadiusB;
    
    ak_v3f Normal = P1-P0;
    ak_f32 NormalLength = AK_Magnitude(Normal);
    AK_Assert(!AK_EqualZeroEps(NormalLength), "Normal is not defined");
    
    Normal /= NormalLength;
    
    ak_contact_vector Result;
    Result.Direction = Normal;
    Result.Penetration = AK_Max(Radius-NormalLength, 0.0f);
    return Result;
}

ak_contact_vector AK_SphereSphereCV(ak_sphere* SphereA, ak_sphere* SphereB)
{
    ak_contact_vector Result = AK_Internal__GetQuadraticDeepestContact(SphereA->CenterP, SphereB->CenterP, SphereA->Radius, SphereB->Radius);
    return Result;
}

ak_contact_vector AK_SphereCapsuleCV(ak_sphere* SphereA, ak_capsule* CapsuleB)
{
    ak_closest_points ClosestPoints = AK_ClosestPointsPointLineSegment(SphereA->CenterP, CapsuleB->P);
    ak_contact_vector Result = AK_Internal__GetQuadraticDeepestContact(ClosestPoints.PointA, ClosestPoints.PointB, SphereA->Radius, CapsuleB->Radius);
    return Result;
}

ak_contact_vector AK_SphereOBBCV(ak_sphere* Sphere, ak_obb* OBB)
{
    internal__point_support ASupport = {Sphere->CenterP};
    internal__obb_support BSupport = {OBB};
    
    gjk_internal__distance Distance = GJK_Internal__Distance(&ASupport, &BSupport);
    ak_closest_points ClosestPoints = Distance.ClosestPoints;
    
    ak_contact_vector Result = AK_Internal__GetQuadraticDeepestContact(ClosestPoints.PointA, ClosestPoints.PointB, Sphere->Radius, 0.0f);
    return Result;
}

ak_contact_vector AK_SphereHullCV(ak_sphere* SphereA, ak_convex_hull* ConvexHullB, ak_m4f TransformB)
{
    internal__point_support PointGJK = {SphereA->CenterP};
    internal__convex_hull_support ConvexHullGJK = {ConvexHullB, TransformB, AK_Transpose(AK_M3(TransformB))};
    
    gjk_internal__distance Distance = GJK_Internal__Distance(&PointGJK, &ConvexHullGJK);
    ak_closest_points ClosestPoints = Distance.ClosestPoints;
    
    ak_contact_vector Result = AK_Internal__GetQuadraticDeepestContact(ClosestPoints.PointA, ClosestPoints.PointB, SphereA->Radius, 0.0f);
    return Result;
}

ak_contact_vector AK_CapsuleSphereCV(ak_capsule* CapsuleA, ak_sphere* SphereB)
{
    ak_contact_vector Result = AK_SphereCapsuleCV(SphereB, CapsuleA);
    Result.Direction = -Result.Direction;
    return Result;
}

ak_contact_vector AK_CapsuleCapsuleCV(ak_capsule* CapsuleA, ak_capsule* CapsuleB)
{
    ak_closest_points ClosestPoints = AK_ClosestPointsLineSegments(CapsuleA->P, CapsuleB->P);
    ak_contact_vector Result = AK_Internal__GetQuadraticDeepestContact(ClosestPoints.PointA, ClosestPoints.PointB, CapsuleA->Radius, CapsuleB->Radius);
    return Result;
}

ak_contact_vector AK_CapsuleOBBCV(ak_capsule* Capsule, ak_obb* OBB)
{
    internal__line_segment_support ASupport = {Capsule->P0, Capsule->P1}; 
    internal__obb_support BSupport = {OBB};
    
    gjk_internal__distance Distance = GJK_Internal__Distance(&ASupport, &BSupport);
    ak_closest_points ClosestPoints = Distance.ClosestPoints;
    ak_contact_vector Result = AK_Internal__GetQuadraticDeepestContact(ClosestPoints.PointA, ClosestPoints.PointB, Capsule->Radius, 0.0f);
    return Result;
}

ak_contact_vector AK_CapsuleHullCV(ak_capsule* CapsuleA, ak_convex_hull* ConvexHullB, ak_m4f TransformB)
{
    internal__line_segment_support AGJK = {CapsuleA->P0, CapsuleA->P1};
    internal__convex_hull_support  BGJK = {ConvexHullB, TransformB, AK_Transpose(AK_M3(TransformB))};
    
    gjk_internal__distance Distance = GJK_Internal__Distance(&AGJK, &BGJK);
    ak_closest_points ClosesetPoints = Distance.ClosestPoints;
    ak_contact_vector Result = AK_Internal__GetQuadraticDeepestContact(ClosesetPoints.PointA, ClosesetPoints.PointB, CapsuleA->Radius, 0.0f);
    return Result;
}


ak_contact_vector AK_OBBSphereCV(ak_obb* OBB, ak_sphere* Sphere)
{
    ak_contact_vector Result = AK_SphereOBBCV(Sphere, OBB);
    Result.Direction = -Result.Direction;
    return Result;
}

ak_contact_vector AK_OBBCapsuleCV(ak_obb* OBB, ak_capsule* Capsule)
{
    ak_contact_vector Result = AK_CapsuleOBBCV(Capsule, OBB);
    Result.Direction = -Result.Direction;
    return Result;
}

ak_contact_vector AK_OBBOBBCV(ak_obb* AOBB, ak_obb* BOBB)
{
    internal__margin_obb_support AEPA = {AOBB, EPA_MARGIN};
    internal__margin_obb_support BEPA = {BOBB, EPA_MARGIN};
    
    epa_internal__result EPAResult = EPA_Internal__Evaluate(&AEPA, &BEPA);
    if(!EPAResult.IsValid)
    {
        internal__obb_support AGJK = {AOBB};
        internal__obb_support BGJK = {BOBB};
        
        gjk_internal__distance GJKDistance = GJK_Internal__Distance(&AGJK, &BGJK);
        ak_contact_vector Result = {};
        Result.Direction = AK_Normalize(GJKDistance.ClosestPoints.PointB-GJKDistance.ClosestPoints.PointA);
        return Result;
    }
    
    ak_contact_vector Result;
    Result.Direction = EPAResult.Normal;
    Result.Penetration = AK_Max((AEPA.Margin+BEPA.Margin)-EPAResult.Penetration, 0.0f);
    return Result;
}

ak_contact_vector AK_OBBHullCV(ak_obb* OBB, ak_convex_hull* ConvexHull, ak_m4f Transform)
{
    ak_m3f Transpose = AK_Transpose(AK_M3(Transform));
    
    internal__margin_obb_support AEPA = {OBB, EPA_MARGIN};
    internal__margin_convex_hull_support BEPA = {ConvexHull, Transform, Transpose, EPA_MARGIN};
    
    epa_internal__result EPAResult = EPA_Internal__Evaluate(&AEPA, &BEPA);
    if(!EPAResult.IsValid)
    {
        internal__obb_support AGJK = {OBB};
        internal__convex_hull_support BGJK = {ConvexHull, Transform, Transpose};
        
        gjk_internal__distance GJKDistance = GJK_Internal__Distance(&AGJK, &BGJK);
        
        ak_contact_vector Result = {};
        Result.Direction = AK_Normalize(GJKDistance.ClosestPoints.PointB-GJKDistance.ClosestPoints.PointA);
        return Result;
    }
    
    ak_contact_vector Result;
    Result.Direction = EPAResult.Normal;
    Result.Penetration = AK_Max((AEPA.Margin+BEPA.Margin)-EPAResult.Penetration, 0.0f);
    return Result;
}

ak_contact_vector AK_HullSphereCV(ak_convex_hull* ConvexHullA, ak_m4f TransformA, ak_sphere* SphereB)
{
    ak_contact_vector Result = AK_SphereHullCV(SphereB, ConvexHullA, TransformA);
    Result.Direction = -Result.Direction;
    return Result;
}

ak_contact_vector AK_HullCapsuleCV(ak_convex_hull* ConvexHullA, ak_m4f TransformA, ak_capsule* CapsuleB)
{
    ak_contact_vector Result = AK_CapsuleHullCV(CapsuleB, ConvexHullA, TransformA);
    Result.Direction = -Result.Direction;
    return Result;
}

ak_contact_vector AK_HullOBBCV(ak_convex_hull* ConvexHull, ak_m4f Transform, ak_obb* OBB)
{
    ak_contact_vector Result = AK_OBBHullCV(OBB, ConvexHull, Transform);
    Result.Direction = -Result.Direction;
    return Result;
}

ak_contact_vector AK_HullHullCV(ak_convex_hull* ConvexHullA, ak_m4f TransformA, 
                                ak_convex_hull* ConvexHullB, ak_m4f TransformB)
{
    ak_m3f TransposeA = AK_Transpose(AK_M3(TransformA));
    ak_m3f TransposeB = AK_Transpose(AK_M3(TransformB));
    
    internal__margin_convex_hull_support AEPA = {ConvexHullA, TransformA, TransposeA, EPA_MARGIN};
    internal__margin_convex_hull_support BEPA = {ConvexHullB, TransformB, TransposeB, 
        EPA_MARGIN};
    
    epa_internal__result EPAResult = EPA_Internal__Evaluate(&AEPA, &BEPA);
    if(!EPAResult.IsValid)
    {
        internal__convex_hull_support AGJK = {ConvexHullA, TransformA, TransposeA};
        internal__convex_hull_support BGJK = {ConvexHullB, TransformB, TransposeB};
        
        gjk_internal__distance GJKDistance = GJK_Internal__Distance(&AGJK, &BGJK);
        
        ak_contact_vector Result = {};
        Result.Direction = AK_Normalize(GJKDistance.ClosestPoints.PointB-GJKDistance.ClosestPoints.PointA);
        return Result;
    }
    
    ak_contact_vector Result;
    Result.Direction = EPAResult.Normal;
    Result.Penetration = AK_Max((AEPA.Margin+BEPA.Margin)-EPAResult.Penetration, 0.0f);
    return Result;
}

ak_closest_points AK_ClosestPointsLineSegments(ak_v3f* A, ak_v3f* B)
{
    ak_closest_points Result = {};
    ak_v3f D1 = A[1]-A[0];
    ak_v3f D2 = B[1]-B[0];
    ak_v3f R = A[0]-B[0];
    
    ak_f32 a = AK_SqrMagnitude(D1);
    ak_f32 e = AK_SqrMagnitude(D2);
    ak_f32 f = AK_Dot(D2, R);
    
    if(AK_EqualZeroEps(a) && AK_EqualZeroEps(e))
    {
        Result.PointA = A[0];
        Result.PointB = B[0];
        return Result;
    }
    
    ak_f32 t;
    ak_f32 s;
    
    if(AK_EqualZeroEps(a))
    {
        s = 0.0f;
        t = AK_Saturate(f/e);        
    }
    else
    {
        ak_f32 c = AK_Dot(D1, R);
        if(AK_EqualZeroEps(e))
        {
            t = 0.0f;
            s = AK_Saturate(-c/a);
        }
        else
        {
            ak_f32 b = AK_Dot(D1, D2);
            ak_f32 Denom = (a*e)-AK_Square(b);
            
            if(Denom != 0.0f)            
                s = AK_Saturate((b*f - c*e) / Denom);            
            else            
                s = 0.0f;    
            
            ak_f32 tnom = b*s + f;
            
            if(tnom < 0.0f)
            {
                t = 0.0f;
                s = AK_Saturate(-c / a);
            }
            else if(tnom > e)
            {
                t = 1.0f;
                s = AK_Saturate((b - c) / a);
            }
            else
            {
                t = tnom / e;
            }             
        }
    }
    
    Result.PointA = A[0] + D1*s;
    Result.PointB = B[0] + D2*t;
    
    return Result;
}

ak_closest_points AK_ClosestPointsPointLineSegment(ak_v3f P, ak_v3f* LineSegment)
{
    ak_v3f AB = LineSegment[1]-LineSegment[0];    
    ak_f32 t = AK_Saturate(AK_Dot(P - LineSegment[0], AB) / AK_SqrMagnitude(AB));
    
    ak_closest_points Result;
    Result.PointA = P;
    Result.PointB = LineSegment[0] + t*AB;
    return Result;
}

ak_closest_points AK_LineSegmentOBBClosestPoints(ak_v3f* LineSegment, ak_obb* OBB)
{
    internal__line_segment_support ASupport = {LineSegment[0], LineSegment[1]};
    internal__obb_support BSupport = {OBB};
    gjk_internal__distance Distance = GJK_Internal__Distance(&ASupport, &BSupport);
    return Distance.ClosestPoints;
}

ak_closest_points AK_PointOBBClosestPoints(ak_v3f P, ak_obb* OBB)
{
    internal__point_support ASupport = {P};
    internal__obb_support BSupport = {OBB};
    gjk_internal__distance Distance = GJK_Internal__Distance(&ASupport, &BSupport);
    return Distance.ClosestPoints;
}

ak_closest_points AK_CapsuleOBBClosestPoints(ak_capsule* CapsuleA, ak_obb* OBB)
{
    internal__capsule_support ASupport = {CapsuleA->P0, CapsuleA->P1, CapsuleA->Radius};
    internal__obb_support BSupport = {OBB};
    gjk_internal__distance Distance = GJK_Internal__Distance(&ASupport, &BSupport);
    return Distance.ClosestPoints;
}

ak_closest_points AK_CapsuleHullClosestPoints(ak_capsule* CapsuleA, ak_convex_hull* ConvexHullB, ak_m4f TransformB)
{
    internal__capsule_support ASupport = {CapsuleA->P0, CapsuleA->P1, CapsuleA->Radius};
    internal__convex_hull_support BSupport = {ConvexHullB, TransformB, AK_Transpose(AK_M3(TransformB))};
    gjk_internal__distance Distance = GJK_Internal__Distance(&ASupport, &BSupport);
    return Distance.ClosestPoints;
}

ak_aabb AK_ComputeAABB(ak_v3f* Vertices, ak_u16* Indices, ak_u32 IndexCount)
{
    ak_aabb Result = {};
    Result.Min = AK_V3( INFINITY,  INFINITY,  INFINITY);
    Result.Max = AK_V3(-INFINITY, -INFINITY, -INFINITY);
    
    for(ak_u32 Index = 0; Index < IndexCount; Index++)
    {   
        ak_v3f Vertex = Vertices[Indices[Index]];
        Result.Min.x = AK_Min(Result.Min.x, Vertex.x);
        Result.Min.y = AK_Min(Result.Min.y, Vertex.y);
        Result.Min.z = AK_Min(Result.Min.z, Vertex.z);
        Result.Max.x = AK_Max(Result.Max.x, Vertex.x);
        Result.Max.y = AK_Max(Result.Max.y, Vertex.y);
        Result.Max.z = AK_Max(Result.Max.z, Vertex.z);
    }
    
    return Result;
}

ak_aabb AK_ComputeAABB(ak_v3f* Vertices, ak_u32* Indices, ak_u32 IndexCount)
{
    ak_aabb Result = {};
    Result.Min = AK_V3( INFINITY,  INFINITY,  INFINITY);
    Result.Max = AK_V3(-INFINITY, -INFINITY, -INFINITY);
    
    for(ak_u32 Index = 0; Index < IndexCount; Index++)
    {   
        ak_v3f Vertex = Vertices[Indices[Index]];
        Result.Min.x = AK_Min(Result.Min.x, Vertex.x);
        Result.Min.y = AK_Min(Result.Min.y, Vertex.y);
        Result.Min.z = AK_Min(Result.Min.z, Vertex.z);
        Result.Max.x = AK_Max(Result.Max.x, Vertex.x);
        Result.Max.y = AK_Max(Result.Max.y, Vertex.y);
        Result.Max.z = AK_Max(Result.Max.z, Vertex.z);
    }
    
    return Result;
}

ak_obb AK_ComputeOBB(ak_aabb AABB)
{
    ak_obb Result = {};
    Result.Dimensions = AABB.Max-AABB.Min;
    Result.Orientation = AK_IdentityM3<ak_f32>();
    Result.CenterP = AABB.Min + Result.Dimensions*0.5f;
    return Result;
}

inline ak_f32
GJK_Internal__BoxVolume(ak_v3f a, ak_v3f b, ak_v3f c)
{
    ak_v3f n = AK_Cross(a, b);
    return AK_Dot(n, c);
}

inline ak_bool GJK_Internal__SimplexContains(gjk_internal__simplex* Simplex, gjk_internal__vertex Vertex)
{
    for(ak_u32 VertexIndex = 0; VertexIndex < Simplex->Count; VertexIndex++)
    {
        if(AK_EqualEps(Simplex->Vertex[VertexIndex].W, Vertex.W))
            return true;
    }
    return false;
}

inline ak_bool GJK_Internal__ValidateSimplex(gjk_internal__simplex* Simplex)
{
    ak_u32 StartIndex = 0;
    for(StartIndex; StartIndex < Simplex->Count; StartIndex++)
    {
        ak_v3f V = Simplex->Vertex[StartIndex].W;
        for(ak_u32 Index = StartIndex+1; Index < Simplex->Count; Index++)
        {
            if(V==Simplex->Vertex[Index].W)
                return false;
        }
    }
    return true;
}

inline void GJK_Internal__AddVertex(gjk_internal__simplex* Simplex, gjk_internal__vertex Vertex)
{
    AK_Assert(Simplex->Count < 4, "GJK Error: Simplex overflow");
    ak_u32 Index = Simplex->Count++;
    Simplex->Vertex[Index] = Vertex;
    Simplex->Barycentric[Index] = 1.0f;
}

ak_f32 GJK_Internal__GetBarycentricRatio(gjk_internal__simplex* Simplex)
{
    ak_f32 Result = 0;
    for(ak_u32 Index = 0; Index < Simplex->Count; Index++) Result += Simplex->Barycentric[Index];
    return (ak_f32)1/Result;
}

ak_v3f GJK_Internal__FindClosestPoint(gjk_internal__simplex* Simplex)
{
    ak_f32 Denom = GJK_Internal__GetBarycentricRatio(Simplex);
    switch(Simplex->Count)
    {
        case 1: { return Simplex->Vertex[0].W; };
        
        case 2: 
        {
            ak_v3f A = Simplex->Vertex[0].W*Simplex->Barycentric[0]*Denom;
            ak_v3f B = Simplex->Vertex[1].W*Simplex->Barycentric[1]*Denom;
            return A+B;
        }
        
        case 3:
        {
            ak_v3f A = Simplex->Vertex[0].W*Simplex->Barycentric[0]*Denom;
            ak_v3f B = Simplex->Vertex[1].W*Simplex->Barycentric[1]*Denom;
            ak_v3f C = Simplex->Vertex[2].W*Simplex->Barycentric[2]*Denom;
            return A+B+C;
        }
        
        AK_INVALID_DEFAULT_CASE;
    }
    return {};
}

ak_f32 GJK_Internal__LargestMagnitude(gjk_internal__simplex* Simplex)
{
    ak_f32 Result = 0;
    for(ak_u32 Index = 0; Index < Simplex->Count; Index++)
    {
        ak_f32 Mag = AK_SqrMagnitude(Simplex->Vertex[Index].W);
        if(Mag > Result)
            Result = Mag;
    }
    return Result;
}

template <typename typeA, typename typeB>
gjk_internal__vertex GJK_Internal__GetSupport(typeA* ObjectA, typeB* ObjectB, ak_v3f V)
{
    gjk_internal__vertex Vertex;    
    Vertex.V = V;
    Vertex.A = ObjectA->Support( V);
    Vertex.B = ObjectB->Support(-V);
    Vertex.W = Vertex.A-Vertex.B;
    return Vertex;
}

void GJK_Internal__SimplexSolver(gjk_internal__simplex* Simplex)
{
    switch (Simplex->Count) 
    {
        case 1: break;
        case 2: 
        {
            /* -------------------- Line ----------------------- */
            ak_v3f a = Simplex->Vertex[0].W;
            ak_v3f b = Simplex->Vertex[1].W;
            
            /* compute barycentric coordinates */
            ak_v3f ab = a-b;
            ak_v3f ba = b-a;
            
            ak_f32 u = AK_Dot(b, ba);
            ak_f32 v = AK_Dot(a, ab);
            if (v <= 0.0f) 
            {
                /* region A */
                Simplex->Barycentric[0] = 1.0f;
                Simplex->Count = 1;
                break;
            }
            
            if (u <= 0.0f) 
            {
                /* region B */
                Simplex->Vertex[0] = Simplex->Vertex[1];
                Simplex->Barycentric[0] = 1.0f;
                Simplex->Count = 1;
                break;
            }
            
            /* region AB */
            Simplex->Barycentric[0] = u;
            Simplex->Barycentric[1] = v;
            Simplex->Count = 2;
        } break;
        
        case 3: 
        {
            /* -------------------- Triangle ----------------------- */
            ak_v3f a = Simplex->Vertex[0].W;
            ak_v3f b = Simplex->Vertex[1].W;
            ak_v3f c = Simplex->Vertex[2].W;
            
            ak_v3f ab = a-b;
            ak_v3f ba = b-a;
            ak_v3f bc = b-c;
            ak_v3f cb = c-b;
            ak_v3f ca = c-a;
            ak_v3f ac = a-c;
            
            /* compute barycentric coordinates */
            ak_f32 u_ab = AK_Dot(b, ba);
            ak_f32 v_ab = AK_Dot(a, ab);
            
            ak_f32 u_bc = AK_Dot(c, cb);
            ak_f32 v_bc = AK_Dot(b, bc);
            
            ak_f32 u_ca = AK_Dot(a, ac);
            ak_f32 v_ca = AK_Dot(c, ca);
            
            if (v_ab <= 0.0f && u_ca <= 0.0f) 
            {
                /* region A */
                Simplex->Barycentric[0] = 1.0f;
                Simplex->Count = 1;
                break;
            }
            
            if (u_ab <= 0.0f && v_bc <= 0.0f) 
            {
                /* region B */
                Simplex->Vertex[0] = Simplex->Vertex[1];
                Simplex->Barycentric[0] = 1.0f;
                Simplex->Count = 1;
                break;
            }
            
            if (u_bc <= 0.0f && v_ca <= 0.0f) 
            {
                /* region C */
                Simplex->Vertex[0] = Simplex->Vertex[2];
                Simplex->Barycentric[0] = 1.0f;
                Simplex->Count = 1;
                break;
            }
            
            ak_v3f n = AK_Cross(ba, ca);
            ak_v3f n1 = AK_Cross(b, c);
            ak_v3f n2 = AK_Cross(c, a);
            ak_v3f n3 = AK_Cross(a, b);
            
            ak_f32 u_abc = AK_Dot(n1, n);
            ak_f32 v_abc = AK_Dot(n2, n);
            ak_f32 w_abc = AK_Dot(n3, n);
            
            if (u_ab > 0.0f && v_ab > 0.0f && w_abc <= 0.0f) 
            {
                /* region AB */
                Simplex->Barycentric[0] = u_ab;
                Simplex->Barycentric[1] = v_ab;
                Simplex->Count = 2;
                break;
            }
            
            if (u_bc > 0.0f && v_bc > 0.0f && u_abc <= 0.0f) 
            {
                /* region BC */
                Simplex->Vertex[0] = Simplex->Vertex[1];
                Simplex->Vertex[1] = Simplex->Vertex[2];
                Simplex->Barycentric[0] = u_bc;
                Simplex->Barycentric[1] = v_bc;
                Simplex->Count = 2;
                break;
            }
            
            if (u_ca > 0.0f && v_ca > 0.0f && v_abc <= 0.0f) 
            {
                /* region CA */
                Simplex->Vertex[1] = Simplex->Vertex[0];
                Simplex->Vertex[0] = Simplex->Vertex[2];
                Simplex->Barycentric[0] = u_ca;
                Simplex->Barycentric[1] = v_ca;
                Simplex->Count = 2;
                break;
            }
            
            /* region ABC */
            AK_Assert(u_abc > 0.0f && v_abc > 0.0f && w_abc > 0.0f, "Invalid barycentric coordinates");
            Simplex->Barycentric[0] = u_abc;
            Simplex->Barycentric[1] = v_abc;
            Simplex->Barycentric[2] = w_abc;
            Simplex->Count = 3;
        } break;
        
        case 4: 
        {
            /* -------------------- Tetrahedron ----------------------- */
            ak_v3f a = Simplex->Vertex[0].W;
            ak_v3f b = Simplex->Vertex[1].W;
            ak_v3f c = Simplex->Vertex[2].W;
            ak_v3f d = Simplex->Vertex[3].W;
            
            ak_v3f ab = a-b;
            ak_v3f ba = b-a;
            ak_v3f bc = b-c;
            ak_v3f cb = c-b;
            ak_v3f ca = c-a;
            ak_v3f ac = a-c;
            
            ak_v3f db = d-b;
            ak_v3f bd = b-d;
            ak_v3f dc = d-c;
            ak_v3f cd = c-d;
            ak_v3f da = d-a;
            ak_v3f ad = a-d;
            
            /* compute barycentric coordinates */
            ak_f32 u_ab = AK_Dot(b, ba);
            ak_f32 v_ab = AK_Dot(a, ab);
            
            ak_f32 u_bc = AK_Dot(c, cb);
            ak_f32 v_bc = AK_Dot(b, bc);
            
            ak_f32 u_ca = AK_Dot(a, ac);
            ak_f32 v_ca = AK_Dot(c, ca);
            
            ak_f32 u_bd = AK_Dot(d, db);
            ak_f32 v_bd = AK_Dot(b, bd);
            
            ak_f32 u_dc = AK_Dot(c, cd);
            ak_f32 v_dc = AK_Dot(d, dc);
            
            ak_f32 u_ad = AK_Dot(d, da);
            ak_f32 v_ad = AK_Dot(a, ad);
            
            /* check verticies for closest point */
            if (v_ab <= 0.0f && u_ca <= 0.0f && v_ad <= 0.0f) 
            {
                /* region A */
                Simplex->Barycentric[0] = 1.0f;
                Simplex->Count = 1;
                break;
            }
            
            if (u_ab <= 0.0f && v_bc <= 0.0f && v_bd <= 0.0f) 
            {
                /* region B */
                Simplex->Vertex[0] = Simplex->Vertex[1];
                Simplex->Barycentric[0] = 1.0f;
                Simplex->Count = 1;
                break;
            }
            
            if (u_bc <= 0.0f && v_ca <= 0.0f && u_dc <= 0.0f) 
            {
                /* region C */
                Simplex->Vertex[0] = Simplex->Vertex[2];
                Simplex->Barycentric[0] = 1.0f;
                Simplex->Count = 1;
                break;
            }
            
            if (u_bd <= 0.0f && v_dc <= 0.0f && u_ad <= 0.0f) 
            {
                /* region D */
                Simplex->Vertex[0] = Simplex->Vertex[3];
                Simplex->Barycentric[0] = 1.0f;
                Simplex->Count = 1;
                break;
            }
            
            /* calculate fractional area */
            ak_v3f n  = AK_Cross(da, ba);
            ak_v3f n1 = AK_Cross(d, b);
            ak_v3f n2 = AK_Cross(b, a);
            ak_v3f n3 = AK_Cross(a, d);
            
            ak_f32 u_adb = AK_Dot(n1, n);
            ak_f32 v_adb = AK_Dot(n2, n);
            ak_f32 w_adb = AK_Dot(n3, n);
            
            n = AK_Cross(ca, da);
            n1 = AK_Cross(c, d);
            n2 = AK_Cross(d, a);
            n3 = AK_Cross(a, c);
            
            ak_f32 u_acd = AK_Dot(n1, n);
            ak_f32 v_acd = AK_Dot(n2, n);
            ak_f32 w_acd = AK_Dot(n3, n);
            
            n = AK_Cross(bc, dc);
            n1 = AK_Cross(b, d);
            n2 = AK_Cross(d, c);
            n3 = AK_Cross(c, b);
            
            ak_f32 u_cbd = AK_Dot(n1, n);
            ak_f32 v_cbd = AK_Dot(n2, n);
            ak_f32 w_cbd = AK_Dot(n3, n);
            
            n = AK_Cross(ba, ca);
            n1 = AK_Cross(b, c);
            n2 = AK_Cross(c, a);
            n3 = AK_Cross(a, b);
            
            ak_f32 u_abc = AK_Dot(n1, n);
            ak_f32 v_abc = AK_Dot(n2, n);
            ak_f32 w_abc = AK_Dot(n3, n);
            
            /* check edges for closest point */
            if (w_abc <= 0.0f && v_adb <= 0.0f && u_ab > 0.0f && v_ab > 0.0f) 
            {
                /* region AB */
                Simplex->Barycentric[0] = u_ab;
                Simplex->Barycentric[1] = v_ab;
                Simplex->Count = 2;
                break;
            }
            
            if (u_abc <= 0.0f && w_cbd <= 0.0f && u_bc > 0.0f && v_bc > 0.0f) 
            {
                /* region BC */
                Simplex->Vertex[0] = Simplex->Vertex[1];
                Simplex->Vertex[1] = Simplex->Vertex[2];
                Simplex->Barycentric[0] = u_bc;
                Simplex->Barycentric[1] = v_bc;
                Simplex->Count = 2;
                break;
            }
            
            if (v_abc <= 0.0f && w_acd <= 0.0f && u_ca > 0.0f && v_ca > 0.0f) 
            {
                /* region CA */
                Simplex->Vertex[1] = Simplex->Vertex[0];
                Simplex->Vertex[0] = Simplex->Vertex[2];
                Simplex->Barycentric[0] = u_ca;
                Simplex->Barycentric[1] = v_ca;
                Simplex->Count = 2;
                break;
            }
            
            if (v_cbd <= 0.0f && u_acd <= 0.0f && u_dc > 0.0f && v_dc > 0.0f) 
            {
                /* region DC */
                Simplex->Vertex[0] = Simplex->Vertex[3];
                Simplex->Vertex[1] = Simplex->Vertex[2];
                Simplex->Barycentric[0] = u_dc;
                Simplex->Barycentric[1] = v_dc;
                Simplex->Count = 2;
                break;
            }
            
            if (v_acd <= 0.0f && w_adb <= 0.0f && u_ad > 0.0f && v_ad > 0.0f) 
            {
                /* region AD */
                Simplex->Vertex[1] = Simplex->Vertex[3];
                Simplex->Barycentric[0] = u_ad;
                Simplex->Barycentric[1] = v_ad;
                Simplex->Count = 2;
                break;
            }
            
            if (u_cbd <= 0.0f && u_adb <= 0.0f && u_bd > 0.0f && v_bd > 0.0f) 
            {
                /* region BD */
                Simplex->Vertex[0] = Simplex->Vertex[1];
                Simplex->Vertex[1] = Simplex->Vertex[3];
                Simplex->Barycentric[0] = u_bd;
                Simplex->Barycentric[1] = v_bd;
                Simplex->Count = 2;
                break;
            }
            
            /* calculate fractional volume (volume can be negative!) */
            ak_f32 denom = GJK_Internal__BoxVolume(cb, ab, db);
            ak_f32 volume = (denom == 0) ? 1.0f: 1.0f/denom;
            ak_f32 u_abcd = GJK_Internal__BoxVolume(c, d, b) * volume;
            ak_f32 v_abcd = GJK_Internal__BoxVolume(c, a, d) * volume;
            ak_f32 w_abcd = GJK_Internal__BoxVolume(d, a, b) * volume;
            ak_f32 x_abcd = GJK_Internal__BoxVolume(b, a, c) * volume;
            
            /* check faces for closest point */
            if (x_abcd <= 0.0f && u_abc > 0.0f && v_abc > 0.0f && w_abc > 0.0f) 
            {
                /* region ABC */
                Simplex->Barycentric[0] = u_abc;
                Simplex->Barycentric[1] = v_abc;
                Simplex->Barycentric[2] = w_abc;
                Simplex->Count = 3;
                break;
            }
            
            if (u_abcd <= 0.0f && u_cbd > 0.0f && v_cbd > 0.0f && w_cbd > 0.0f) 
            {
                /* region CBD */
                Simplex->Vertex[0] = Simplex->Vertex[2];
                Simplex->Vertex[2] = Simplex->Vertex[3];
                Simplex->Barycentric[0] = u_cbd;
                Simplex->Barycentric[1] = v_cbd;
                Simplex->Barycentric[2] = w_cbd;
                Simplex->Count = 3;
                break;
            }
            
            if (v_abcd <= 0.0f && u_acd > 0.0f && v_acd > 0.0f && w_acd > 0.0f) 
            {
                /* region ACD */
                Simplex->Vertex[1] = Simplex->Vertex[2];
                Simplex->Vertex[2] = Simplex->Vertex[3];
                Simplex->Barycentric[0] = u_acd;
                Simplex->Barycentric[1] = v_acd;
                Simplex->Barycentric[2] = w_acd;
                Simplex->Count = 3;
                break;
            }
            
            if (w_abcd <= 0.0f && u_adb > 0.0f && v_adb > 0.0f && w_adb > 0.0f) 
            {
                /* region ADB */
                Simplex->Vertex[2] = Simplex->Vertex[1];
                Simplex->Vertex[1] = Simplex->Vertex[3];
                Simplex->Barycentric[0] = u_adb;
                Simplex->Barycentric[1] = v_adb;
                Simplex->Barycentric[2] = w_adb;
                Simplex->Count = 3;
                break;
            }
            
            /* region ABCD */
            
            //CONFIRM(JJ): Invalid simplex. Is this ok? We generally will break after this so no calculations will take place. However if we are trying to calculate closest points this will be a problem.
            //AK_Assert(u_abcd >= 0.0f && v_abcd >= 0.0f && w_abcd >= 0.0f && x_abcd >= 0.0f, "Invalid barycentric coordinates"); 
            Simplex->Barycentric[0] = u_abcd;
            Simplex->Barycentric[1] = v_abcd;
            Simplex->Barycentric[2] = w_abcd;
            Simplex->Barycentric[3] = x_abcd;
            Simplex->Count = 4;
        } break;
    }
}

ak_bool GJK_Internal__NewSearchDirection(gjk_internal__simplex* Simplex, ak_f32* BestDistance, ak_v3f* V)
{
    ak_v3f Point = {};
    ak_f32 Denom = GJK_Internal__GetBarycentricRatio(Simplex);
    switch(Simplex->Count)
    {
        case 1:
        {
            Point = Simplex->Vertex[0].W;
        } break;
        
        case 2:
        {
            ak_v3f A = Simplex->Vertex[0].W*(Denom*Simplex->Barycentric[0]);
            ak_v3f B = Simplex->Vertex[1].W*(Denom*Simplex->Barycentric[1]);
            Point = A+B;
        } break;
        
        case 3:
        {
            ak_v3f A = Simplex->Vertex[0].W*(Denom*Simplex->Barycentric[0]);
            ak_v3f B = Simplex->Vertex[1].W*(Denom*Simplex->Barycentric[1]);
            ak_v3f C = Simplex->Vertex[2].W*(Denom*Simplex->Barycentric[2]);
            Point = A+B+C;
        } break;
        
        AK_INVALID_DEFAULT_CASE;
    }
    
    ak_f32 SqrDistance = AK_SqrMagnitude(Point);
    if(SqrDistance >= *BestDistance) return false;
    *BestDistance = SqrDistance;
    
    switch(Simplex->Count)
    {
        case 1:
        {
            *V = -Simplex->Vertex[0].W;
        } break;
        
        case 2:
        {
            ak_v3f ba = Simplex->Vertex[1].W-Simplex->Vertex[0].W;
            ak_v3f t = AK_Cross(ba, -Simplex->Vertex[1].W);
            *V = AK_Cross(t, ba);
        } break;
        
        case 3:
        {
            ak_v3f ba = Simplex->Vertex[1].W-Simplex->Vertex[0].W;
            ak_v3f ca = Simplex->Vertex[2].W-Simplex->Vertex[0].W;
            ak_v3f n = AK_Cross(ba, ca);
            *V = AK_Dot(n, Simplex->Vertex[0].W) <= 0.0f ? n : -n;
        } break;
        
        AK_INVALID_DEFAULT_CASE;
    }
    
    if(AK_SqrMagnitude(*V) < AK_Square(GJK_INTERNAL__EPSILON)) return false;
    return true;
}

template <typename typeA, typename typeB>
gjk_internal__distance GJK_Internal__Distance(typeA* ObjectA, typeB* ObjectB)
{
    ak_v3f V = GJK_Internal__GetSupport(ObjectA, ObjectB, AK_XAxisF32()).W;
    return GJK_Internal__Distance(ObjectA, ObjectB, V);
}

template <typename typeA, typename typeB>
gjk_internal__distance GJK_Internal__Distance(typeA* ObjectA, typeB* ObjectB, ak_v3f InitialV)
{
    ak_v3f V = InitialV;
    
    gjk_internal__distance Result = {};
    gjk_internal__simplex* Simplex = &Result.Simplex;
    
    ak_bool Hit = false;
    ak_f32 BestDistance = INFINITY;
    
    ak_u32 Iterations = 0;
    for(;;)
    {
        Iterations++;
        AK_Assert(Iterations < 5000, "Collision detection encountered an infinte loop");
        gjk_internal__vertex Vertex = GJK_Internal__GetSupport(ObjectA, ObjectB, V);
        if(GJK_Internal__SimplexContains(Simplex, Vertex)) break;
        
        GJK_Internal__AddVertex(Simplex, Vertex);
        GJK_Internal__SimplexSolver(Simplex);
        if(Simplex->Count == 4)
        {
            Hit = true;
            break;
        }
        
        if(!GJK_Internal__NewSearchDirection(Simplex, &BestDistance, &V))
            break;
    }
    
    ak_f32 Denom = GJK_Internal__GetBarycentricRatio(Simplex);
    
    ak_closest_points* ClosestPoints = &Result.ClosestPoints;
    switch(Simplex->Count)
    {
        case 1:
        {
            ClosestPoints->PointA = Simplex->Vertex[0].A;
            ClosestPoints->PointB = Simplex->Vertex[0].B;
        } break;
        
        case 2:
        {
            ak_f32 BC0 = Denom*Simplex->Barycentric[0];
            ak_f32 BC1 = Denom*Simplex->Barycentric[1];
            
            ak_v3f A0 = Simplex->Vertex[0].A * BC0;
            ak_v3f A1 = Simplex->Vertex[1].A * BC1;
            
            ak_v3f B0 = Simplex->Vertex[0].B * BC0;
            ak_v3f B1 = Simplex->Vertex[1].B * BC1;
            
            ClosestPoints->PointA = A0+A1;
            ClosestPoints->PointB = B0+B1;
        } break;
        
        case 3:
        {
            ak_f32 BC0 = Denom*Simplex->Barycentric[0];
            ak_f32 BC1 = Denom*Simplex->Barycentric[1];
            ak_f32 BC2 = Denom*Simplex->Barycentric[2];
            
            ak_v3f A0 = Simplex->Vertex[0].A * BC0;
            ak_v3f A1 = Simplex->Vertex[1].A * BC1;
            ak_v3f A2 = Simplex->Vertex[2].A * BC2;
            
            ak_v3f B0 = Simplex->Vertex[0].B * BC0;
            ak_v3f B1 = Simplex->Vertex[1].B * BC1;
            ak_v3f B2 = Simplex->Vertex[2].B * BC2;
            
            ClosestPoints->PointA = A0+A1+A2;
            ClosestPoints->PointB = B0+B1+B2;
        } break;
        
        case 4:
        {
            ak_f32 BC0 = Denom*Simplex->Barycentric[0];
            ak_f32 BC1 = Denom*Simplex->Barycentric[1];
            ak_f32 BC2 = Denom*Simplex->Barycentric[2];
            ak_f32 BC3 = Denom*Simplex->Barycentric[3];
            
            ak_v3f A0 = Simplex->Vertex[0].A * BC0;
            ak_v3f A1 = Simplex->Vertex[1].A * BC1;
            ak_v3f A2 = Simplex->Vertex[2].A * BC2;
            ak_v3f A3 = Simplex->Vertex[3].A * BC3;
            
            ClosestPoints->PointA = A0+A1+A2+A3;
            ClosestPoints->PointB = ClosestPoints->PointA;
        } break;
        
        AK_INVALID_DEFAULT_CASE;
    }
    
    if(!Hit)
        Result.SqrDistance = AK_SqrMagnitude(ClosestPoints->PointB-ClosestPoints->PointA);
    Result.CacheV = V;
    return Result;
}

ak_bool GJK_Internal__QuadraticIntersected(gjk_internal__distance Distance, ak_f32 RadiusA, ak_f32 RadiusB)
{
    ak_f32 Radius = RadiusA+RadiusB;
    return Distance.SqrDistance < AK_Square(Radius) || Distance.SqrDistance < GJK_INTERNAL__EPSILON;
}

template <typename typeA, typename typeB>
ak_bool GJK_Internal__Intersected(typeA* ObjectA, typeB* ObjectB, gjk_internal__simplex* Simplex)
{
    ak_v3f V = GJK_Internal__GetSupport(ObjectA, ObjectB, AK_XAxisF32()).W;
    *Simplex = {};
    
    ak_f32 BestDistance = INFINITY;
    
    ak_u32 Iterations = 0;
    for(;;)
    {
        Iterations++;
        AK_Assert(Iterations < 5000, "Collision detection encountered an infinte loop");
        gjk_internal__vertex Vertex = GJK_Internal__GetSupport(ObjectA, ObjectB, V);
        if(GJK_Internal__SimplexContains(Simplex, Vertex)) return false;
        
        GJK_Internal__AddVertex(Simplex, Vertex);
        GJK_Internal__SimplexSolver(Simplex);
        if(Simplex->Count == 4) return true;
        
        if(!GJK_Internal__NewSearchDirection(Simplex, &BestDistance, &V)) return false;
    }
}

template <typename typeA, typename typeB>
ak_bool GJK_Internal__Intersected(typeA* ObjectA, typeB* ObjectB)
{
    gjk_internal__simplex Simplex = {};
    return GJK_Internal__Intersected(ObjectA, ObjectB, &Simplex);
}

template <typename typeA, typename typeB> 
ak_toi GJK_Internal__TOI(typeA* ObjectA, typeB* ObjectB, ak_v3f R)
{
    ak_f32 tHit = 0.0f;
    ak_v3f S = {};
    ak_v3f N = {};
    ak_v3f V = GJK_Internal__GetSupport(ObjectA, ObjectB, AK_XAxisF32()).W;
    gjk_internal__simplex _Simplex = {};
    gjk_internal__simplex* Simplex = &_Simplex;
    
    ak_bool Hit = false;
    
    ak_u32 Iterations = 0;
    ak_f32 BestDistance = INFINITY;
    for(;;)
    {
        Iterations++;
        AK_Assert(Iterations < 5000, "Collision detection encountered an infinte loop");
        
        gjk_internal__vertex Vertex = GJK_Internal__GetSupport(ObjectA, ObjectB, V);
        
        ak_v3f NTemp = -V;
        
        ak_f32 NDotW = AK_Dot(NTemp, Vertex.W);
        ak_f32 NDotR = AK_Dot(NTemp, R);
        
        ak_bool RayHit = NDotW > tHit*NDotR;
        Vertex.W -= S;
        if(RayHit)
        {
            if(NDotR > 0)
            {
                ak_f32 tTemp = NDotW/NDotR;
                AK_Assert(tTemp != 0.0f, "Invalid tTemp");
                if(tTemp == tHit)
                {
                    //NOTE(EVERYONE): Numerically instability occurred in the simplex solver. Break out to prevent infinite looping
                    Hit = true;
                    break;
                }
                
                tHit = tTemp;
                if(tHit > 1) break;
                
                S = tHit*R;
                *Simplex = {};
                BestDistance = INFINITY;
                N = V;
                Vertex.W -= S;
            }
            else break;
        }
        
        if(GJK_Internal__SimplexContains(Simplex, Vertex))
        {
            Hit = true;
            break;
        }
        
        GJK_Internal__AddVertex(Simplex, Vertex);
        
        AK_Assert(GJK_Internal__ValidateSimplex(Simplex), "Invalid simplex");
        
        GJK_Internal__SimplexSolver(Simplex);
        if(Simplex->Count == 4)
        {
            Hit = true;
            break;
        }
        
        if(!GJK_Internal__NewSearchDirection(Simplex, &BestDistance, &V))
        {
            Hit = true;
            break;
        }
    }
    
    if(Hit)
    {
        ak_toi Result;
        Result.Intersected = true;
        Result.t = tHit;
        Result.Normal = AK_Normalize(N);
        return Result;
    }
    
    return {};
}

ak_u32 Internal__ConvexHullSupport(ak_convex_hull* ConvexHull, ak_v3f Direction)
{               
#if GJK_SUPPORT_BRUTE_FORCE
    ak_u32 BestVertexIndex = (ak_u32)-1;        
    ak_f32 BestDot = -AK_MAX32;
    for(ak_u32 VertexIndex = 0; VertexIndex < ConvexHull->VertexCount; VertexIndex++)
    {
        half_vertex* Vertex = ConvexHull->Vertices + VertexIndex;
        ak_f32 TempDot = AK_Dot(Direction, Vertex->V);
        if(TempDot > BestDot)
        {
            BestVertexIndex = VertexIndex;
            BestDot = TempDot;
        }
    }
#else
    ak_u32 BestVertexIndex = 0;    
    ak_half_vertex* BestVertex = ConvexHull->Vertices + BestVertexIndex;
    ak_f32 BestDot = AK_Dot(BestVertex->V, Direction);    
    for(;;)
    {
        ak_i32 E = BestVertex->Edge;
        ak_i32 StartEdge = E;
        ak_f32 OldBestDot = BestDot;
        
        do
        {
            AK_Assert(E != -1, "Invalid half edge");
            ak_half_edge* Edge = ConvexHull->Edges + E;
            
            ak_i32 VertexIndex = Edge->Vertex;
            ak_half_vertex* Vertex = ConvexHull->Vertices + VertexIndex;
            
            ak_f32 TempDot = AK_Dot(Direction, Vertex->V);
            if(TempDot > BestDot)
            {
                BestVertexIndex = VertexIndex;
                BestVertex = Vertex;
                BestDot = TempDot;
            }
            
            E = ConvexHull->Edges[Edge->EdgePair].NextEdge;     
            if(E == -1)
                E = Edge->NextEdge;
            
        } while(StartEdge != E);     
        
        if(BestDot == OldBestDot)
            break;
    }
#endif
    return BestVertexIndex;
}

ak_v3f Internal__OBBSupport(ak_obb* OBB, ak_v3f Direction)
{
    ak_v3f HalfDimensions = OBB->Dimensions*0.5f;
    ak_v3f P = OBB->CenterP;
    P += OBB->Orientation.XAxis * (AK_Dot(Direction, OBB->Orientation.XAxis) >= 0.f ? HalfDimensions.x : -HalfDimensions.x);
    P += OBB->Orientation.YAxis * (AK_Dot(Direction, OBB->Orientation.YAxis) >= 0.f ? HalfDimensions.y : -HalfDimensions.y);
    P += OBB->Orientation.ZAxis * (AK_Dot(Direction, OBB->Orientation.ZAxis) >= 0.f ? HalfDimensions.z : -HalfDimensions.z);
    return P;
}

ak_v3f Internal__ConvexHullSupportPoint(ak_convex_hull* ConvexHull, ak_m4f T, ak_m3f BTransposed, ak_v3f D)
{   
    D = D*BTransposed;
    ak_u32 VertexID = Internal__ConvexHullSupport(ConvexHull, D);
    ak_v3f Result = AK_TransformPoint(ConvexHull->Vertices[VertexID].V, T);
    return Result;
}

ak_v3f Internal__CapsuleSupportPoint(ak_v3f P0, ak_v3f P1, ak_f32 Radius, ak_v3f D)
{
    if(AK_SqrMagnitude(D) < AK_Square(AK_EPSILON32))
        D = AK_XAxisF32();
    D = AK_Normalize(D);
    ak_f32 Dy = AK_Dot(D, P1-P0);
    return (Dy >= 0.0f ? P1 : P0) + D*Radius;
}

ak_u32 Internal__LineSegmentSupport(ak_v3f* LineSegment, ak_v3f Direction)
{
    return AK_Dot(Direction, LineSegment[1]-LineSegment[0]) >= 0.0f ? 1 : 0;    
}

ak_v3f internal__line_segment_support::Support(ak_v3f D)
{
    return P[Internal__LineSegmentSupport(P, D)];
}

ak_v3f internal__moving_line_segment_support::Support(ak_v3f D)
{
    ak_v3f Result = P[Internal__LineSegmentSupport(P, D)];
    return AK_Dot(D, Delta) > 0 ? Result+Delta : Result;
}

ak_v3f internal__sphere_support::Support(ak_v3f D)
{
    if(AK_SqrMagnitude(D) < AK_Square(AK_EPSILON32))
        D = AK_XAxisF32();
    D = AK_Normalize(D);
    return CenterP + D*Radius;
}

ak_v3f internal__capsule_support::Support(ak_v3f D)
{
    return Internal__CapsuleSupportPoint(P0, P1, Radius, D);
}

ak_v3f internal__obb_support::Support(ak_v3f D)
{
    return Internal__OBBSupport(OBB, D);
}

ak_v3f internal__moving_obb_support::Support(ak_v3f D)
{
    ak_v3f Result = Internal__OBBSupport(OBB, D);
    return AK_Dot(D, Delta) > 0 ? Result+Delta : Result;
}

ak_v3f internal__margin_obb_support::Support(ak_v3f D)
{
    if(AK_SqrMagnitude(D) < AK_Square(AK_EPSILON32))
        D = AK_V3f(-1, -1, -1);
    D = AK_Normalize(D);
    ak_v3f Result = Internal__OBBSupport(OBB, D);
    Result += (Margin*D);
    return Result;
}

ak_v3f internal__convex_hull_support::Support(ak_v3f D)
{
    return Internal__ConvexHullSupportPoint(Hull, T, BTransposed, D);
}

ak_v3f internal__moving_convex_hull_support::Support(ak_v3f D)
{
    ak_v3f Result = Internal__ConvexHullSupportPoint(Hull, T, BTransposed, D);
    if(AK_Dot(D, Delta) > 0) Result += Delta;
    return Result;
}

ak_v3f internal__margin_convex_hull_support::Support(ak_v3f D)
{
    if(AK_SqrMagnitude(D) < AK_Square(AK_EPSILON32))
        D = AK_V3f(-1, -1, -1);
    D = AK_Normalize(D);
    ak_v3f Result = Internal__ConvexHullSupportPoint(Hull, T, BTransposed, D);
    Result += (Margin*D);
    return Result;
}

void epa_internal__list::Add(epa_internal__face* Face)
{
    Face->ChildFaces[0] = NULL;
    Face->ChildFaces[1] = Root;
    if(Root) Root->ChildFaces[0] = Face;
    Root = Face;
    Count++;
}

void epa_internal__list::Remove(epa_internal__face* Face)
{
    if(Face->ChildFaces[1]) Face->ChildFaces[1]->ChildFaces[0] = Face->ChildFaces[0];
    if(Face->ChildFaces[0]) Face->ChildFaces[0]->ChildFaces[1] = Face->ChildFaces[1];
    if(Face == Root) Root = Face->ChildFaces[1];
    Count--;
}


inline void 
EPA_Internal__BindFaces(epa_internal__face* Face0, ak_u32 Edge0, epa_internal__face* Face1, ak_u32 Edge1)
{
    Face0->Edges[Edge0] = (ak_u8)Edge1;
    Face1->Edges[Edge1] = (ak_u8)Edge0;    
    Face0->AdjFaces[Edge0] = Face1;
    Face1->AdjFaces[Edge1] = Face0;
}

ak_bool EPA_Internal__GetEdgeDistance(ak_f32* Distance, ak_v3f P0, ak_v3f P1, ak_v3f N)
{
    ak_v3f Edge = P1-P0;
    ak_v3f EdgeNormal = AK_Cross(Edge, N);
    ak_f32 DotResult = AK_Dot(P0, EdgeNormal);
    
    if(DotResult < 0)
    {
        ak_f32 EdgeSqr = AK_SqrMagnitude(Edge);
        ak_f32 a = AK_Dot(P0, Edge);
        ak_f32 b = AK_Dot(P1, Edge);
        
        if(a > 0)
        {
            *Distance = AK_Magnitude(P0);
        }
        else if(b < 0)
        {
            *Distance = AK_Magnitude(P1);
        }
        else
        {
            ak_f32 ab = AK_Dot(P0, P1);
            ak_f32 TempDistance = (AK_SqrMagnitude(P0)*AK_SqrMagnitude(P1) - AK_Square(ab))/EdgeSqr;
            *Distance = (TempDistance > 0) ? AK_Sqrt(TempDistance) : 0;
        }
        
        return true;
    }
    
    return false;
}

ak_bool EPA_Internal__ContainsOrigin(gjk_internal__vertex** Tetrahedron)
{
    ak_v3f n0 = AK_Cross(Tetrahedron[1]->W-Tetrahedron[0]->W, Tetrahedron[2]->W-Tetrahedron[0]->W);
    ak_f32 a = AK_Dot(n0, Tetrahedron[0]->W);
    ak_f32 b = AK_Dot(n0, Tetrahedron[3]->W);    
    
    if(!AK_EqualZeroEps(a) && !AK_EqualZeroEps(b))       
    {
        if((a > 0) == (b > 0))
            return false;
    }
    
    ak_v3f n1 = AK_Cross(Tetrahedron[2]->W-Tetrahedron[1]->W, Tetrahedron[3]->W-Tetrahedron[1]->W);
    a = AK_Dot(n1, Tetrahedron[1]->W);
    b = AK_Dot(n1, Tetrahedron[0]->W);    
    
    if(!AK_EqualZeroEps(a) && !AK_EqualZeroEps(b))       
    {
        if((a > 0) == (b > 0))
            return false;
    }
    
    ak_v3f n2 = AK_Cross(Tetrahedron[3]->W-Tetrahedron[2]->W, Tetrahedron[0]->W-Tetrahedron[2]->W);
    a = AK_Dot(n2, Tetrahedron[2]->W);
    b = AK_Dot(n2, Tetrahedron[1]->W);    
    
    if(!AK_EqualZeroEps(a) && !AK_EqualZeroEps(b))       
    {
        if((a > 0) == (b > 0))
            return false;
    }
    
    ak_v3f n3 = AK_Cross(Tetrahedron[0]->W-Tetrahedron[3]->W, Tetrahedron[1]->W-Tetrahedron[3]->W);
    a = AK_Dot(n3, Tetrahedron[3]->W);
    b = AK_Dot(n3, Tetrahedron[2]->W);    
    
    if(!AK_EqualZeroEps(a) && !AK_EqualZeroEps(b))       
    {
        if((a > 0) == (b > 0))
            return false;
    }
    
    return true;
}


epa_internal__face* epa_internal__context::CreateFace(gjk_internal__vertex P0, gjk_internal__vertex P1, gjk_internal__vertex P2, ak_bool Forced)
{
    if(Free.Root)
    {
        epa_internal__face* Face = Free.Root;
        Free.Remove(Face);
        Hull.Add(Face);
        
        Face->Pass = 0;
        Face->Vertices[0] = P0;
        Face->Vertices[1] = P1;
        Face->Vertices[2] = P2;
        Face->Normal = AK_Cross(P1.W-P0.W, P2.W-P0.W);
        ak_f32 Length = AK_Magnitude(Face->Normal);
        if(Length > EPA_INTERNAL__EPSILON)
        {
            ak_bool FoundEdgeDistance = (EPA_Internal__GetEdgeDistance(&Face->Distance, P0.W, P1.W, Face->Normal) ||
                                         EPA_Internal__GetEdgeDistance(&Face->Distance, P1.W, P2.W, Face->Normal) ||
                                         EPA_Internal__GetEdgeDistance(&Face->Distance, P2.W, P0.W, Face->Normal));
            if(!FoundEdgeDistance)
                Face->Distance = AK_Dot(P0.W, Face->Normal) / Length;
            
            Face->Normal /= Length;
            if(Forced || (Face->Distance >= -EPA_INTERNAL__PLANE_EPSILON))
                return Face;
            else
                Status = EPA_STATUS_NOT_CONVEX;
            
        }
        else            
            Status = EPA_STATUS_DEGENERATE;            
        
        Hull.Remove(Face);
        Free.Add(Face);
        return NULL;
        
    }
    else
    {
        Status = EPA_STATUS_OUT_OF_FACES;
        return NULL;
    }
}

epa_internal__face* epa_internal__context::GetBestFace()
{
    epa_internal__face* Result = Hull.Root;
    ak_f32 BestSqrDistance = AK_Square(Result->Distance);
    for(epa_internal__face* Face = Result->ChildFaces[1]; Face; Face = Face->ChildFaces[1])
    {
        ak_f32 SqrDistance = AK_Square(Face->Distance);
        if(SqrDistance < BestSqrDistance)
        {
            BestSqrDistance = SqrDistance;
            Result = Face;
        }
    }
    
    return Result;
}

ak_bool epa_internal__context::Expand(ak_u32 Pass, gjk_internal__vertex Vertex, epa_internal__face* Face, ak_u32 Edge, epa_internal__silhouette* Silhouette)
{
    local const ak_u32 Indices0[] = {1, 2, 0};
    local const ak_u32 Indices1[] = {2, 0, 1};
    if(Face->Pass != Pass)
    {
        ak_u32 E1 = Indices0[Edge];
        if((AK_Dot(Face->Normal, Vertex.W) - Face->Distance) < -EPA_INTERNAL__PLANE_EPSILON)
        {
            epa_internal__face* NewFace = CreateFace(Face->Vertices[E1], Face->Vertices[Edge], Vertex, false);
            if(NewFace)
            {
                EPA_Internal__BindFaces(NewFace, 0, Face, Edge);
                if(Silhouette->Face0)
                    EPA_Internal__BindFaces(Silhouette->Face0, 1, NewFace, 2);
                else
                    Silhouette->Face1 = NewFace;
                Silhouette->Face0 = NewFace;
                Silhouette->NextFace++;
                
                return true;
            }
        }
        else
        {                
            ak_u32 E2 = Indices1[Edge];
            Face->Pass = (ak_u8)Pass;
            if(Expand(Pass, Vertex, Face->AdjFaces[E1], Face->Edges[E1], Silhouette) &&
               Expand(Pass, Vertex, Face->AdjFaces[E2], Face->Edges[E2], Silhouette))
            {
                Hull.Remove(Face);
                Free.Add(Face);
                return true;
            }
        }
    }
    
    return false;
}

template <typename typeA, typename typeB> 
epa_internal__result epa_internal__context::Evaluate(typeA* ObjectA, typeB* ObjectB, gjk_internal__vertex* Vertices)
{
    epa_internal__result Result = {};
    
    if(AK_Determinant(Vertices[0].W - Vertices[3].W, Vertices[1].W - Vertices[3].W, Vertices[2].W - Vertices[3].W) < 0)        
        AK_Swap(Vertices[0], Vertices[1]);       
    
    for(ak_u32 FaceIndex = 1; FaceIndex <= EPA_INTERNAL__MAX_FACES; FaceIndex++)
        Free.Add(&FaceStore[EPA_INTERNAL__MAX_FACES-FaceIndex]);
    
    epa_internal__face* Tetrahedron[4] = 
    {
        CreateFace(Vertices[0], Vertices[1], Vertices[2], true),
        CreateFace(Vertices[1], Vertices[0], Vertices[3], true),
        CreateFace(Vertices[2], Vertices[1], Vertices[3], true),
        CreateFace(Vertices[0], Vertices[2], Vertices[3], true)
    };
    
    if(Hull.Count == 4)
    {
        epa_internal__face* Best = GetBestFace();            
        ak_u32 Pass = 0;
        ak_u32 Iterations = 0;
        
        EPA_Internal__BindFaces(Tetrahedron[0], 0, Tetrahedron[1], 0);
        EPA_Internal__BindFaces(Tetrahedron[0], 1, Tetrahedron[2], 0);
        EPA_Internal__BindFaces(Tetrahedron[0], 2, Tetrahedron[3], 0);
        EPA_Internal__BindFaces(Tetrahedron[1], 1, Tetrahedron[3], 2);
        EPA_Internal__BindFaces(Tetrahedron[1], 2, Tetrahedron[2], 1);
        EPA_Internal__BindFaces(Tetrahedron[2], 2, Tetrahedron[3], 1);
        
        Status = EPA_STATUS_VALID;
        
        for(;;)
        {
            Iterations++;
            
            epa_internal__silhouette Silhouette = {};
            ak_bool Valid = true;
            Best->Pass = (ak_u8)(++Pass);
            
            gjk_internal__vertex Vertex = GJK_Internal__GetSupport(ObjectA, ObjectB, Best->Normal);
            ak_f32 Distance = AK_Dot(Best->Normal, Vertex.W) - Best->Distance;
            if(Distance > EPA_INTERNAL__EPSILON)
            {
                for(ak_u32 Index = 0; (Index < 3) && Valid; Index++)
                    Valid &= Expand(Pass, Vertex, Best->AdjFaces[Index], Best->Edges[Index], &Silhouette);
                
                if(Valid && (Silhouette.NextFace >= 3))
                {
                    EPA_Internal__BindFaces(Silhouette.Face0, 1, Silhouette.Face1, 2);
                    Hull.Remove(Best);
                    Free.Add(Best);
                    Best = GetBestFace();                        
                }
                else
                {
                    Status = EPA_STATUS_INVALID_HULL;
                    break;
                }                    
            }            
            else
            {
                Status = EPA_STATUS_ACCURACY_REACHED;
                break;
            }                
        }
        
        ak_v3f Projection = Best->Normal*Best->Distance;
        
        ak_f32 P[3] = 
        {
            AK_Magnitude(AK_Cross(Best->Vertices[1].W - Projection, Best->Vertices[2].W - Projection)),
            AK_Magnitude(AK_Cross(Best->Vertices[2].W - Projection, Best->Vertices[0].W - Projection)), 
            AK_Magnitude(AK_Cross(Best->Vertices[0].W - Projection, Best->Vertices[1].W - Projection))
        };
        
        ak_f32 Sum = P[0]+P[1]+P[2];
        P[0] /= Sum;
        P[1] /= Sum;
        P[2] /= Sum;
        
        
        for(ak_u32 Index = 0; Index < 3; Index++)
        {                
            gjk_internal__vertex Support = GJK_Internal__GetSupport(ObjectA, ObjectB, Best->Vertices[Index].V);
            Result.Witness[0] += Support.A*P[Index];
            Result.Witness[1] += Support.B*P[Index];                
        }
        
        Result.Status  = Status;
        Result.IsValid = true;                
        Result.Normal = Best->Normal;
        Result.Penetration = Best->Distance;                                                            
        
        return Result;
    }
    else
    {
        Result.IsValid = false;            
        Result.Status = EPA_STATUS_INVALID_TETRAHEDRON;
        return Result;
    }
}

template <typename typeA, typename typeB>
epa_internal__result EPA_Internal__Evaluate(typeA* ObjectA, typeB* ObjectB)
{
    epa_internal__result Result = {};
    
    gjk_internal__simplex Simplex;
    ak_bool Intersected = GJK_Internal__Intersected(ObjectA, ObjectB, &Simplex);
    if(!Intersected)
        return Result;
    
    gjk_internal__vertex Tetrahedron[4] = {};
    switch(Simplex.Count)
    {
        case 1:
        {
            Result.Status = EPA_STATUS_INVALID_TETRAHEDRON;
            return Result;
        } break;
        
        case 2:
        case 3:
        {
            gjk_internal__vertex Hexahedron[5] = {};
            
            if(Simplex.Count == 2)
            {
                Hexahedron[0] = Simplex.Vertex[0];
                Hexahedron[1] = Simplex.Vertex[1];                                           
                
                ak_v3f D = Hexahedron[0].W - Hexahedron[1].W;
                ak_v3f A = {};            
                if(AK_Abs(D.x) < AK_Abs(D.y)) 
                    A = (AK_Abs(D.x) < AK_Abs(D.z)) ? AK_XAxisF32() : AK_ZAxisF32();
                else 
                    A = (AK_Abs(D.y) < AK_Abs(D.z)) ? AK_YAxisF32() : AK_ZAxisF32();
                
                ak_m3f R = AK_QuatToMatrix(AK_RotQuat(AK_Normalize(D), AK_PI/3.0f)); 
                ak_v3f V0 = AK_Cross(D, A);                        
                ak_v3f V1 = V0*R;
                ak_v3f V2 = V1*R;
                
                Hexahedron[2] = GJK_Internal__GetSupport(ObjectA, ObjectB, V0);
                Hexahedron[3] = GJK_Internal__GetSupport(ObjectA, ObjectB, V1);
                Hexahedron[4] = GJK_Internal__GetSupport(ObjectA, ObjectB, V2);                
            }
            else
            {
                Hexahedron[0] = Simplex.Vertex[0];
                Hexahedron[1] = Simplex.Vertex[1];
                Hexahedron[2] = Simplex.Vertex[2];
                
                ak_v3f N = AK_Cross(Hexahedron[1].W-Hexahedron[0].W, Hexahedron[2].W-Hexahedron[0].W);
                Hexahedron[3] = GJK_Internal__GetSupport(ObjectA, ObjectB,  N);
                Hexahedron[4] = GJK_Internal__GetSupport(ObjectA, ObjectB, -N);
            }
            
            gjk_internal__vertex* Tetrahedron0[4] = { &Hexahedron[0], &Hexahedron[1], &Hexahedron[2], &Hexahedron[4] };
            gjk_internal__vertex* Tetrahedron1[4] = { &Hexahedron[0], &Hexahedron[1], &Hexahedron[3], &Hexahedron[4] };
            if(AK_Determinant(Tetrahedron0[0]->W - Tetrahedron0[3]->W, Tetrahedron0[1]->W - Tetrahedron0[3]->W, Tetrahedron0[2]->W - Tetrahedron0[3]->W) < 0)        
                AK_Swap(Tetrahedron0[0], Tetrahedron0[1]);
            
            if(AK_Determinant(Tetrahedron1[0]->W - Tetrahedron1[3]->W, Tetrahedron1[1]->W - Tetrahedron1[3]->W, Tetrahedron1[2]->W - Tetrahedron1[3]->W) < 0)        
                AK_Swap(Tetrahedron1[0], Tetrahedron1[1]);            
            
            ak_bool Contains = EPA_Internal__ContainsOrigin(Tetrahedron0);
            if(!Contains)
            {
                Contains = EPA_Internal__ContainsOrigin(Tetrahedron1);                
                if(!Contains)
                {
                    Result.Status = EPA_STATUS_INVALID_TETRAHEDRON;
                    return Result;                
                }
                
                Tetrahedron[0] = *Tetrahedron1[0];
                Tetrahedron[1] = *Tetrahedron1[1];
                Tetrahedron[2] = *Tetrahedron1[2];
                Tetrahedron[3] = *Tetrahedron1[3];
            }
            else
            {
                Tetrahedron[0] = *Tetrahedron0[0];
                Tetrahedron[1] = *Tetrahedron0[1];
                Tetrahedron[2] = *Tetrahedron0[2];
                Tetrahedron[3] = *Tetrahedron0[3];
            }                        
        } break;
        
        case 4:
        {
            Tetrahedron[0] = Simplex.Vertex[0];            
            Tetrahedron[1] = Simplex.Vertex[1];            
            Tetrahedron[2] = Simplex.Vertex[2];
            Tetrahedron[3] = Simplex.Vertex[3];
        } break;
        
        AK_INVALID_DEFAULT_CASE;
    }
    
    epa_internal__context Context = {};
    Result = Context.Evaluate(ObjectA, ObjectB, Tetrahedron);
    return Result;
}