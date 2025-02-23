#include "Camera.h"
#include "InputManager.h"
#include "TimeManager.h"

Camera::Camera()
	: m_fFov(XM_PI / 4.f)
	, m_fNear(1.0f)
	, m_fFar(10000.0f)
	, m_fAspectRatio(0.0f)
	, m_fSpeed(10.0f)
{
}

void Camera::Initialize(int _iWidth, int _iHeight)
{
	m_fAspectRatio = _iWidth / _iHeight;
	m_Scale = Vec3(1.0f, 1.0f, 1.0f);
	m_Rotation = Vec3(0.0f, 0.0f, 0.0f);
	m_Position = Vec3(0.0f, 0.0f, -10.0f);
}

void Camera::Update(float _fDeltaTime)
{
	if (IS_PRESS(EKeyType::W))
	{
		m_Position += GetLook() * m_fSpeed * DELTA_TIME;
	}

	if (IS_PRESS(EKeyType::S))
	{
		m_Position -= GetLook() * m_fSpeed * DELTA_TIME;
	}

	if (IS_PRESS(EKeyType::A))
	{
		m_Position -= GetRight() * m_fSpeed * DELTA_TIME;
	}

	if (IS_PRESS(EKeyType::D))
	{
		m_Position += GetRight() * m_fSpeed * DELTA_TIME;
	}

	if (IS_PRESS(EKeyType::RBUTTON))
	{
		POINT PrevPos = GET_SINGLE(InputManager)->GetPrevMousePos();
		POINT CurPos = GET_SINGLE(InputManager)->GetMousePos();

		float fRotateX = static_cast<float>(CurPos.y - PrevPos.y);
		float fRotateY = static_cast<float>(CurPos.x - PrevPos.x);

		constexpr float fSensitivity = 0.05f;

		float fTargetRotX = fRotateX * fSensitivity;
		float fTargetRotY = fRotateY * fSensitivity;

		float fSmoothFactor = 3.0f * DELTA_TIME;
		m_Rotation.x = Lerp(m_Rotation.x, m_Rotation.x + fTargetRotX, fSmoothFactor);
		m_Rotation.y = Lerp(m_Rotation.y, m_Rotation.y + fTargetRotY, fSmoothFactor);
	}

	CreateVP();
}

void Camera::Render(float _fDeltaTime)
{
}

void Camera::CreateVP()
{
	Matrix matScale = Matrix::CreateScale(m_Scale);
	XMFLOAT4 quat;
	DirectX::XMStoreFloat4(&quat, DirectX::XMQuaternionRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z));
	Matrix matRot = DirectX::XMMatrixRotationQuaternion(XMLoadFloat4(&quat));
	Matrix matTr = Matrix::CreateTranslation(m_Position);
	m_matWorld = matScale * matRot * matTr;

	m_matView = DirectX::XMMatrixTranspose(m_matWorld.Invert());
	m_matPerspective = DirectX::XMMatrixTranspose(XMMatrixPerspectiveFovLH(m_fFov, 1.f, 0.1f, 1000.0f));
	XMStoreFloat4x4(&m_matVP, XMMatrixMultiply(m_matPerspective, m_matView));
}
