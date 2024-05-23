#include "XCamera.h"
#include <raymath.h>

XCamera::XCamera() 
{
    internal.target = {};
    internal.projection = CAMERA_PERSPECTIVE;
    internal.position = {};
    internal.up = { 0, 1, 0 };
    internal.fovy = 60.0f;
};

XCamera::~XCamera()
{
    
}

void XCamera::Begin()
{
    forward = Vector3Normalize(Vector3Subtract(internal.target, internal.position));
    left = Vector3Normalize(Vector3CrossProduct(UNIT_Y, forward));

    internal.projection = perspective ? CAMERA_PERSPECTIVE: CAMERA_ORTHOGRAPHIC;
    fovy = Clamp(fovy, 1, 179);
    orthoSize = Clamp(orthoSize, 0.1, 10.0f);
    internal.fovy = perspective ? fovy: orthoSize;
    BeginMode3D(internal);
}

void XCamera::RotateH(float deg)
{
    internal.position = Vector3RotateByAxisAngle(internal.position, UNIT_Y, DEG2RAD*deg);
}

void XCamera::RotateV(float deg)
{
    internal.position = Vector3RotateByAxisAngle(internal.position, left, DEG2RAD*deg);
}

void XCamera::AddRadius(float f)
{
    Vector3 targetToPos = Vector3Subtract(internal.position, internal.target);
    float currentRadius = Vector3Length(targetToPos);
    internal.position = Vector3Add(internal.target, 
        Vector3Scale(Vector3Normalize(targetToPos), Clamp(f+currentRadius, 0.01f, 100.0f)));
}

float XCamera::GetRadius()
{
    return Vector3Distance(internal.position, internal.target);
}