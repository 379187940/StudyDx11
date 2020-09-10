
#pragma once

//#include "BasicMath.hpp"
#include "InputController.hpp"

class FirstPersonCamera
{
public:
    void Update(InputController& Controller, float ElapsedTime);
    void SetRotation(float Yaw, float Pitch);
	Quaternion GetRotation();
    void SetLookAt(const float3& LookAt);
    void SetMoveSpeed(float MoveSpeed) { m_fMoveSpeed = MoveSpeed; }
	float GetMoveSpeed() { return m_fMoveSpeed; }
    void SetRotationSpeed(float RotationSpeed) { m_fRotationSpeed = RotationSpeed; }
    void SetPos(const float3& Pos) { m_Pos = Pos; }
    void SetProjAttribs(float NearClipPlane,
                        float FarClipPlane,
                        float AspectRatio,
                        float FOV,
                        bool    IsGL);
    void SetSpeedUpScales(float SpeedUpScale, float SuperSpeedUpScale);


    // clang-format off
    const float4x4& GetViewMatrix()  const { return m_ViewMatrix;  }
    const float4x4& GetProjMatrix()  const { return m_ProjMatrix;  }

    float3 GetWorldRight() const { return float3(m_ViewMatrix._11, m_ViewMatrix._21, m_ViewMatrix._31); }
    float3 GetWorldUp()    const { return float3(m_ViewMatrix._12, m_ViewMatrix._22, m_ViewMatrix._32); }
    float3 GetWorldAhead() const { return float3(m_ViewMatrix._13, m_ViewMatrix._23, m_ViewMatrix._33); }
    // clang-format on

    float3 GetPos() const { return m_Pos; }
    float  GetCurrentSpeed() const { return m_fCurrentSpeed; }

    struct ProjectionAttribs
    {
        float NearClipPlane = 1.f;
        float FarClipPlane  = 1000.f;
        float AspectRatio   = 1.f;
        float FOV           = PI_F / 4.f;
        bool    IsGL          = false;
    };
    const ProjectionAttribs& GetProjAttribs() { return m_ProjAttribs; }

    void SetReferenceAxes(const float3& ReferenceRightAxis, const float3& ReferenceUpAxis, bool IsRightHanded = false);

    void SetHandness(bool IsRightHanded)
    {
        m_fHandness = IsRightHanded ? +1.f : -1.f;
    }

protected:
    float4x4 GetReferenceRotiation() const;

    ProjectionAttribs m_ProjAttribs;

    MouseState m_LastMouseState;

    float3 m_ReferenceRightAxis = float3{1, 0, 0};
    float3 m_ReferenceUpAxis    = float3{0, 1, 0};
    float3 m_ReferenceAheadAxis = float3{0, 0, 1};

    float3 m_Pos;

    float4x4 m_ViewMatrix;
    float4x4 m_ProjMatrix;
    float    m_fRotationSpeed = 0.01f;
    float    m_fMoveSpeed     = 1.f;
    float    m_fCurrentSpeed  = 0.f;

    float m_fYawAngle          = 0; // Yaw angle of camera
    float m_fPitchAngle        = 0; // Pitch angle of camera
    float m_fSpeedUpScale      = 1.f;
    float m_fSuperSpeedUpScale = 1.f;
    float m_fHandness          = 1.f; // -1 - left handed
                                      // +1 - right handed
};
