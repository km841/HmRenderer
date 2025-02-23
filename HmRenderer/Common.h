#pragma once
#include <windows.h>
#include <tchar.h>
#include <memory>
#include <string>
#include <vector>
#include <array>
#include <list>
#include <map>
#include <functional>
#include <mutex>
#include <queue>
#include <type_traits>
#include <optional>
#include <bitset>
#include <numeric>
#include <filesystem>
#include <algorithm>
#include <typeinfo>
#include <typeindex>
#include <random>

#include "d3d11.h"
#include <d3d12.h>
#include <wrl.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXTex/DirectXTex.h>
#include <DirectXTex/DirectXTex.inl>
#include <dwrite.h>
#include <d2d1.h>
#include <dshow.h>

#include "SimpleMath.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "dxguid")
#pragma comment(lib, "d3dcompiler")
#pragma comment(lib, "d2d1")
#pragma comment(lib, "dwrite")

#define DECLARE_SINGLE(type)  \
private:					  \
	type();				  \
	~type();				  \
							  \
public:						  \
	static type* GetInstance()\
	{						  \
		static type instance; \
		return &instance;	  \
	}						  \

#define GET_SINGLE(type) type::GetInstance()
#define DELTA_TIME GET_SINGLE(TimeManager)->GetDeltaTime()

#define IS_PRESS(key) GET_SINGLE(InputManager)->GetButtonPress(key)
#define IS_DOWN(key) GET_SINGLE(InputManager)->GetButtonDown(key)
#define IS_UP(key) GET_SINGLE(InputManager)->GetButtonUp(key)
#define IS_NONE(key) GET_SINGLE(InputManager)->GetButtonNone(key)

using int8 = __int8;
using int16 = __int16;
using int32 = __int32;
using int64 = __int64;
using uint8 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;

namespace fs = std::filesystem;
using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace Microsoft::WRL;

using Vec2 = DirectX::SimpleMath::Vector2;
using Vec3 = DirectX::SimpleMath::Vector3;
using Vec4 = DirectX::SimpleMath::Vector4;
using Matrix = DirectX::SimpleMath::Matrix;
using Quaternion = DirectX::SimpleMath::Quaternion;
using std::shared_ptr;
using std::unique_ptr;
using std::static_pointer_cast;
using std::dynamic_pointer_cast;
using std::make_shared;
using std::make_unique;
using std::wstring;
using std::string;

static void AssertEx(bool _bExpression, const std::wstring& _Message)
{
	if (_bExpression)
		return;

	MessageBoxW(NULL, _Message.c_str(), L"Assert", MB_OK);
	assert(_bExpression);
}

static float Lerp(float _f1, float _f2, float _t) {
	return (1 - _t) * _f1 + _t * _f2;
}

static Vec2 Lerp(const Vec2& _v1, const Vec2& _v2, float _t) {
	Vec2 ret = {};
	ret.x = Lerp(_v1.x, _v2.x, _t);
	ret.y = Lerp(_v1.y, _v2.y, _t);
	return ret;
}

static Vec3 Lerp(const Vec3& _v1, const Vec3& _v2, float _t) {
	Vec3 ret = {};
	ret.x = Lerp(_v1.x, _v2.x, _t);
	ret.y = Lerp(_v1.y, _v2.y, _t);
	ret.z = Lerp(_v1.z, _v2.z, _t);
	return ret;
}

struct MatrixBuffer
{
	DirectX::XMMATRIX WVP;
};

struct WindowSizeBuffer
{
	float fWindowSize[2];
	float fDummy[2];
};