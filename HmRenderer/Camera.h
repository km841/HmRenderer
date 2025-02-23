#pragma once
#include "Common.h"
class Camera
{
public:
	Camera();
	~Camera() {}

	void Initialize(int _iWidth, int _iHeight);
	void Update(float _fDeltaTime);
	void Render(float _fDeltaTime);

public:
	Matrix GetViewMatrix() const { return m_matView; }
	Matrix GetPerspectiveMatrix() const { return m_matPerspective; }
	Matrix GetVP() const { return m_matVP; }

	Vec3 GetLook() const { return m_matWorld.Backward(); }
	Vec3 GetRight() const { return m_matWorld.Right(); }
	Vec3 GetUp() const { return m_matWorld.Up(); }

private:
	void CreateVP();

private:
	Vec3 m_Scale;
	Vec3 m_Rotation;
	Vec3 m_Position;

	Matrix m_matWorld;
	Matrix m_matView;
	Matrix m_matPerspective;
	Matrix m_matVP;

	float m_fSpeed;
	float m_fFov;
	float m_fNear;
	float m_fFar;

	float m_fAspectRatio;
};

