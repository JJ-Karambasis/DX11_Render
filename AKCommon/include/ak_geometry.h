#ifndef AK_GEOMETRY_H
#define AK_GEOMETRY_H

struct ak_sphere
{
    ak_v3f CenterP;
    ak_f32 Radius;
};

struct ak_aabb
{
    union
    {
        ak_v3f Min;
        struct { ak_f32 MinX, MinY, MinZ; };
    };
    
    union
    {
        ak_v3f Max;
        struct { ak_f32 MaxX, MaxY, MaxZ; };
    };
};

struct ak_obb
{
    ak_v3f CenterP;
    ak_m3f Orientation;
    ak_v3f Dimensions;
};

struct ak_capsule
{
    union
    {
        ak_v3f P[2];
        struct
        {
            ak_v3f P0;
            ak_v3f P1;
        };
    };
    ak_f32 Radius;
    
    inline ak_f32 GetHeight() { return AK_Magnitude(P1-P0); }
    inline ak_v3f GetCenter() { return P0 + (P1-P0)*0.5f; }
    inline ak_v3f GetBottom() { return P0 - AK_Normalize(P1-P0)*Radius; }
    inline void SetHeight(ak_f32 Height) { P1 = P0 + AK_Normalize(P1-P0)*Height; }
};

union ak_triangle
{
    ak_v3f P[3];
    struct
    {
        ak_v3f P0;
        ak_v3f P1;
        ak_v3f P2;
    };
};

struct ak_plane
{
    ak_f32 Equation[4];
    ak_v3f Origin;
    ak_v3f Normal;
};

struct ak_half_edge
{
    ak_i32 Vertex;
    ak_i32 EdgePair;
    ak_i32 Face;
    ak_i32 NextEdge;
};

struct ak_half_vertex
{
    ak_v3f V;
    ak_i32 Edge;
};

struct ak_half_face
{
    ak_i32 Edge;
};

struct ak_convex_hull
{
    ak_sqtf Transform;
    ak_u32 VertexCount;
    ak_u32 EdgeCount;
    ak_u32 FaceCount;
    ak_half_vertex* Vertices;
    ak_half_edge* Edges;
    ak_half_face* Faces;
};

struct ak_ray
{
    ak_v3f Origin;
    ak_v3f Direction;
};

struct ak_toi
{
    ak_bool Intersected;
    ak_f32 t;
    ak_v3f Normal;
};

struct ak_ray_cast
{
    ak_bool Intersected;
    ak_f32  t;
};

struct ak_ray_barycentric_cast
{
    ak_bool Intersected;
    ak_f32  t;
    ak_f32  u;
    ak_f32  v;
};

struct ak_closest_points
{
    ak_v3f PointA;
    ak_v3f PointB;
};

struct ak_contact_vector
{
    ak_v3f Direction;
    ak_f32 Penetration;
};

ak_sphere AK_CreateSphere(ak_v3f CenterP, ak_f32 Radius);
ak_capsule AK_CreateCapsule(ak_v3f P0, ak_v3f P1, ak_f32 Radius);
ak_capsule AK_CreateCapsule(ak_v3f Bottom, ak_f32 Radius, ak_f32 Height);
ak_aabb AK_CreateAABB(ak_f32 MinX, ak_f32 MinY, ak_f32 MinZ, ak_f32 MaxX, ak_f32 MaxY, ak_f32 MaxZ);
ak_aabb AK_CreateAABB(ak_v3f Min, ak_v3f Max);
ak_obb AK_CreateOBB(ak_v3f CenterP, ak_m3f Orientation, ak_v3f Scale);
ak_plane AK_CreatePlane(ak_v3f Origin, ak_v3f Normal);
ak_capsule AK_TransformCapsule(ak_v3f P0, ak_v3f P1, ak_f32 Radius, ak_sqtf Transform);
ak_capsule AK_TransformCapsule(ak_capsule* Capsule, ak_sqtf Transform);
ak_capsule AK_TransformCapsule(ak_v3f P0, ak_v3f P1, ak_f32 Radius, ak_v3f Translation, ak_m3f Orientation, ak_v3f Scale);
ak_capsule AK_TransformCapsule(ak_capsule* Capsule, ak_v3f Translation, ak_m3f Orientation, ak_v3f Scale);
ak_sphere AK_TransformSphere(ak_v3f CenterP, ak_f32 Radius, ak_sqtf Transform);
ak_sphere AK_TransformSphere(ak_sphere* Sphere, ak_sqtf Transform);
ak_obb AK_TransformOBB(ak_obb* OBB, ak_sqtf Transform);
void AK_TranslateCapsule(ak_capsule* Capsule, ak_v3f Delta);
void AK_TranslateSphere(ak_sphere* Sphere, ak_v3f Delta);
ak_f32 AK_SignDistance(ak_plane Plane, ak_v3f Point);

ak_v3f AK_RayPixelToView(ak_v2i PixelCoords, ak_v2i PixelDim, ak_m4f Perspective);
ak_v3f AK_RayPixelToWorld(ak_v2i PixelCoords, ak_v2i PixelDim, ak_m4f Perspective, ak_m4f View);

ak_toi AK_RaySphereCast(ak_v3f Origin, ak_v3f Direction, ak_v3f CenterP, ak_f32 Radius);
ak_toi AK_RaySphereCast(ak_ray Ray, ak_sphere Sphere);

ak_toi AK_RayCapsuleCast(ak_v3f Origin, ak_v3f Direction, ak_v3f P0, ak_v3f P1, ak_f32 Radius);
ak_toi AK_RayCapsuleCast(ak_ray Ray, ak_capsule Capsule);

ak_ray_barycentric_cast AK_RayTriangleCast(ak_v3f Origin, ak_v3f Direction, ak_v3f P0, ak_v3f P1, ak_v3f P2);
ak_ray_barycentric_cast AK_RayTriangleCast(ak_ray Ray, ak_triangle Triangle);

ak_ray_barycentric_cast AK_RayTriangleCastNoCull(ak_v3f Origin, ak_v3f Direction, ak_v3f P0, ak_v3f P1, ak_v3f P2);
ak_ray_barycentric_cast AK_RayTriangleCastNoCull(ak_ray Ray, ak_triangle Triangle);

ak_ray_barycentric_cast AK_RayTriangleMeshCast(ak_v3f RayOrigin, ak_v3f RayDirection, 
                                               ak_v3f* Vertices, ak_u16* Indices, ak_u32 IndexCount, 
                                               ak_m4f MeshTransform);

ak_ray_barycentric_cast AK_RayTriangleMeshCast(ak_ray Ray, ak_v3f* Vertices, ak_u16* Indices, ak_u32 IndexCount, 
                                               ak_m4f MeshTransform);

ak_ray_barycentric_cast AK_RayTriangleMeshCastNoCull(ak_v3f RayOrigin, ak_v3f RayDirection, 
                                                     ak_v3f* Vertices, ak_u16* Indices, ak_u32 IndexCount, 
                                                     ak_m4f MeshTransform);

ak_ray_barycentric_cast AK_RayTriangleMeshCastNoCull(ak_ray Ray, ak_v3f* Vertices, ak_u16* Indices, ak_u32 IndexCount, 
                                                     ak_m4f MeshTransform);

ak_ray_barycentric_cast AK_RayTriangleMeshCast(ak_v3f RayOrigin, ak_v3f RayDirection, 
                                               ak_v3f* Vertices, ak_u32* Indices, ak_u32 IndexCount, 
                                               ak_m4f MeshTransform);

ak_ray_barycentric_cast AK_RayTriangleMeshCast(ak_ray Ray, ak_v3f* Vertices, ak_u32* Indices, ak_u32 IndexCount, 
                                               ak_m4f MeshTransform);

ak_ray_barycentric_cast AK_RayTriangleMeshCastNoCull(ak_v3f RayOrigin, ak_v3f RayDirection, 
                                                     ak_v3f* Vertices, ak_u32* Indices, ak_u32 IndexCount, 
                                                     ak_m4f MeshTransform);

ak_ray_barycentric_cast AK_RayTriangleMeshCastNoCull(ak_ray Ray, ak_v3f* Vertices, ak_u32* Indices, ak_u32 IndexCount, 
                                                     ak_m4f MeshTransform);
ak_ray_barycentric_cast AK_RayQuadCast(ak_v3f Origin, ak_v3f Direction, ak_v3f P0, ak_v3f P1, ak_v3f P2, ak_v3f P3);
ak_ray_barycentric_cast AK_RayQuadCast(ak_ray Ray, ak_v3f* Quad);

ak_ray_cast AK_RayPlaneCast(ak_v3f Origin, ak_v3f Direction, ak_v3f PlaneNormal, ak_v3f PlanePoint);
ak_ray_cast AK_RayPlaneCast(ak_ray Ray, ak_plane Plane);
ak_ray_cast AK_RayAABBCast(ak_v3f Origin, ak_v3f Direction, ak_v3f Min, ak_v3f Max);
ak_ray_cast AK_RayAABBCast(ak_ray Ray, ak_aabb AABB);
ak_ray_cast AK_RayOBBCast(ak_v3f Origin, ak_v3f Direction, ak_v3f CenterP, ak_m3f Orientation, ak_v3f Dimensions);
ak_ray_cast AK_RayOBBCast(ak_ray Ray, ak_obb OBB);

ak_toi AK_LineSegmentSphereCast(ak_v3f* LineSegment, ak_v3f CenterP, ak_f32 Radius);
ak_toi AK_LineSegmentSphereCast(ak_v3f* LineSegment, ak_sphere Sphere);

ak_toi AK_LineSegmentCapsuleCast(ak_v3f* LineSegment, ak_v3f P0, ak_v3f P1, ak_f32 Radius);
ak_toi AK_LineSegmentCapsuleCast(ak_v3f* LineSegment, ak_capsule Capsule);

ak_bool AK_SphereSphereOverlap(ak_sphere* SphereA, ak_sphere* SphereB);
ak_bool AK_SphereCapsuleOverlap(ak_sphere* Sphere, ak_capsule* Capsule);
ak_bool AK_SphereOBBOverlap(ak_sphere* Sphere, ak_obb* OBB);
ak_bool AK_SphereHullOverlap(ak_sphere* Sphere, ak_convex_hull* ConvexHull, ak_m4f ConvexHullTransform);

ak_bool AK_CapsuleSphereOverlap(ak_capsule* Capsule, ak_sphere* Sphere);
ak_bool AK_CapsuleCapsuleOverlap(ak_capsule* CapsuleA, ak_capsule* CapsuleB);
ak_bool AK_CapsuleOBBOverlap(ak_capsule* Capsule, ak_obb* OBB);
ak_bool AK_CapsuleHullOverlap(ak_capsule* Capsule, ak_convex_hull* ConvexHull, ak_m4f ConvexHullTransform);

ak_bool AK_OBBSphereOverlap(ak_obb* OBB, ak_sphere* Sphere);
ak_bool AK_OBBCapsuleOverlap(ak_obb* OBB, ak_capsule* Capsule);
ak_bool AK_OBBOBBOverlap(ak_obb* AOBB, ak_obb* BOBB);
ak_bool AK_OBBHullOverlap(ak_obb* OBB, ak_convex_hull* ConvexHull, ak_m4f ConvexHullTransform);

ak_bool AK_HullSphereOverlap(ak_convex_hull* ConvexHull, ak_m4f ConvexHullTransform, ak_sphere* Sphere);
ak_bool AK_HullCapsuleOverlap(ak_convex_hull* ConvexHull, ak_m4f ConvexHullTransform, ak_capsule* Capsule);
ak_bool AK_HullOBBOverlap(ak_convex_hull* ConvexHull, ak_m4f ConvexHullTransform, ak_obb* OBB);
ak_bool AK_HullHullOverlap(ak_convex_hull* ConvexHullA, ak_m4f ConvexHullTransformA, ak_convex_hull* ConvexHullB, ak_m4f ConvexHullTransformB);

ak_toi AK_SphereSphereTOI(ak_sphere* SphereA, ak_v3f DeltaA, ak_sphere* SphereB, ak_v3f DeltaB);
ak_toi AK_SphereCapsuleTOI(ak_sphere* SphereA, ak_v3f DeltaA, ak_capsule* CapsuleB, ak_v3f DeltaB);
ak_toi AK_SphereOBBTOI(ak_sphere* Sphere, ak_v3f DeltaA, ak_obb* OBB, ak_v3f DeltaB);
ak_toi AK_SphereHullTOI(ak_sphere* SphereA, ak_v3f DeltaA, ak_convex_hull* ConvexHullB, ak_m4f TransformB, ak_v3f DeltaB);

ak_toi AK_CapsuleSphereTOI(ak_capsule* CapsuleA, ak_v3f DeltaA, ak_sphere* SphereB, ak_v3f DeltaB);
ak_toi AK_CapsuleCapsuleTOI(ak_capsule* CapsuleA, ak_v3f DeltaA, ak_capsule* CapsuleB, ak_v3f DeltaB);
ak_toi AK_CapsuleOBBTOI(ak_capsule* Capsule, ak_v3f DeltaA, ak_obb* OBB, ak_v3f DeltaB);
ak_toi AK_CapsuleHullTOI(ak_capsule* CapsuleA, ak_v3f DeltaA, ak_convex_hull* ConvexHullB, ak_m4f TransformB, ak_v3f DeltaB);

ak_toi AK_OBBSphereTOI(ak_obb* OBB, ak_v3f DeltaA, ak_sphere* Sphere, ak_v3f DeltaB);
ak_toi AK_OBBCapsuleTOI(ak_obb* OBB, ak_v3f DeltaA, ak_capsule* Capsule, ak_v3f DeltaB);
ak_toi AK_OBBOBBTOI(ak_obb* AOBB, ak_v3f DeltaA, ak_obb* BOBB, ak_v3f DeltaB);
ak_toi AK_OBBHullTOI(ak_obb* OBB, ak_v3f DeltaA, ak_convex_hull* ConvexHull, ak_m4f Transform, ak_v3f DeltaB);

ak_toi AK_HullSphereTOI(ak_convex_hull* ConvexHullA, ak_m4f TransformA, ak_v3f DeltaA, ak_sphere* SphereB, ak_v3f DeltaB);
ak_toi AK_HullCapsuleTOI(ak_convex_hull* ConvexHullA, ak_m4f TransformA, ak_v3f DeltaA, ak_capsule* CapsuleB, ak_v3f DeltaB);
ak_toi AK_HullOBBTOI(ak_convex_hull* ConvexHull, ak_m4f Transform, ak_v3f DeltaA, ak_obb* OBB, ak_v3f DeltaB);
ak_toi AK_HullHullTOI(ak_convex_hull* ConvexHullA, ak_m4f TransformA, ak_v3f DeltaA, 
                      ak_convex_hull* ConvexHullB, ak_m4f TransformB, ak_v3f DeltaB);

ak_contact_vector AK_SphereSphereCV(ak_sphere* SphereA, ak_sphere* SphereB);
ak_contact_vector AK_SphereCapsuleCV(ak_sphere* SphereA, ak_capsule* CapsuleB);
ak_contact_vector AK_SphereOBBCV(ak_sphere* Sphere, ak_obb* OBB);
ak_contact_vector AK_SphereHullCV(ak_sphere* SphereA, ak_convex_hull* ConvexHullB, ak_m4f TransformB);

ak_contact_vector AK_CapsuleSphereCV(ak_capsule* CapsuleA, ak_sphere* SphereB);
ak_contact_vector AK_CapsuleCapsuleCV(ak_capsule* CapsuleA, ak_capsule* CapsuleB);
ak_contact_vector AK_CapsuleOBBCV(ak_capsule* Capsule, ak_obb* OBB);
ak_contact_vector AK_CapsuleHullCV(ak_capsule* CapsuleA, ak_convex_hull* ConvexHullB, ak_m4f TransformB);

ak_contact_vector AK_OBBSphereCV(ak_obb* OBB, ak_sphere* Sphere);
ak_contact_vector AK_OBBCapsuleCV(ak_obb* OBB, ak_capsule* Capsule);
ak_contact_vector AK_OBBOBBCV(ak_obb* AOBB, ak_obb* BOBB);
ak_contact_vector AK_OBBHullCV(ak_obb* OBB, ak_convex_hull* ConvexHull, ak_m4f Transform);

ak_contact_vector AK_HullSphereCV(ak_convex_hull* ConvexHullA, ak_m4f TransformA, ak_sphere* SphereB);
ak_contact_vector AK_HullCapsuleCV(ak_convex_hull* ConvexHullA, ak_m4f TransformA, ak_capsule* CapsuleB);
ak_contact_vector AK_HullOBBCV(ak_convex_hull* ConvexHull, ak_m4f Transform, ak_obb* OBB);
ak_contact_vector AK_HullHullCV(ak_convex_hull* ConvexHullA, ak_m4f TransformA, 
                                ak_convex_hull* ConvexHullB, ak_m4f TransformB);

ak_closest_points AK_ClosestPointsLineSegments(ak_v3f* A, ak_v3f* B);
ak_closest_points AK_ClosestPointsPointLineSegment(ak_v3f P, ak_v3f* LineSegment);
ak_closest_points AK_LineSegmentOBBClosestPoints(ak_v3f* LineSegment, ak_obb* OBB);
ak_closest_points AK_PointOBBClosestPoints(ak_v3f P, ak_obb* OBB);
ak_closest_points AK_CapsuleOBBClosestPoints(ak_capsule* CapsuleA, ak_obb* OBB);
ak_closest_points AK_CapsuleHullClosestPoints(ak_capsule* CapsuleA, ak_convex_hull* ConvexHullB, ak_m4f TransformB);

ak_aabb AK_ComputeAABB(ak_v3f* Vertices, ak_u16* Indices, ak_u32 IndexCount);
ak_aabb AK_ComputeAABB(ak_v3f* Vertices, ak_u32* Indices, ak_u32 IndexCount);
ak_obb AK_ComputeOBB(ak_aabb AABB);

#endif
