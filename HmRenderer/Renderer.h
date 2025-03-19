#pragma once
#include "Common.h"

class GraphicsCore;
class ModelShader;
class RaycastShader;
class FinalShader;
class Camera;

typedef OpenMesh::TriMesh_ArrayKernelT<> MyMesh;

class Renderer
{
public:
	Renderer();
	~Renderer();

public:
	void Initialize(HWND _hHwnd, int _iWidth, int _iHeight);
	void Update(float _fDeltaTime);
	void Render(float _fDeltaTime);

public:
	void SetOwner(GraphicsCore* _pOwner) { m_pOwner = _pOwner; }

private:
	void CreateCube();
	void CreateRect();
	void CreateSampler();
	void CreateRenderTexture(int _iWidth, int _iHeight);
	void CreateLUT(int _iWidth, int _iHeight);
	void CreateUAV(int _iWidth, int _iHeight);
	void LoadVolume(const wstring& _strFilename);
	void CreateViewProjMatrix();
	void LoadSTL(const string& _strFilename);

private:
	GraphicsCore* m_pOwner;
	Camera* m_pCamera;
	ModelShader* m_pModelShader;
	RaycastShader* m_pRaycastShader;
	FinalShader* m_pFinalShader;

	float m_fRotation;
	DirectX::XMFLOAT4 m_matRotation;

	//render textures
	ComPtr<ID3D11Texture2D> m_pModelTex2DFront;
	ComPtr<ID3D11ShaderResourceView> m_pModelSRVFront;
	ComPtr<ID3D11RenderTargetView> m_pModelRTVFront;

	ComPtr<ID3D11Texture2D> m_pModelText2DBack;
	ComPtr<ID3D11ShaderResourceView> m_pModelSRVBack;
	ComPtr<ID3D11RenderTargetView> m_pModelRTVBack;

	ComPtr<ID3D11Texture1D> m_pLUT;
	ComPtr<ID3D11ShaderResourceView> m_pLUTSRV;

	ComPtr<ID3D11Texture2D> m_pRWTex;
	ComPtr<ID3D11UnorderedAccessView> m_pRWUAV;
	ComPtr<ID3D11ShaderResourceView> m_pRWSRV;

	//sampler 
	ComPtr <ID3D11SamplerState> m_pSamplerLinear;

	//volume texture
	ComPtr<ID3D11Texture3D> m_pVolumeTex3D;
	ComPtr<ID3D11ShaderResourceView> m_pVolRSV;

	//vertex and index buffers
	ComPtr<ID3D11Buffer> m_pCubeVB;
	ComPtr<ID3D11Buffer> m_pCubeIB;

	ComPtr<ID3D11Buffer> m_pRectVB;
	ComPtr<ID3D11Buffer> m_pRectIB;

	// project matrix for render
	DirectX::XMFLOAT4X4 m_ViewProj;

	MyMesh m_Mesh;
	std::vector<Vec3> m_vecMeshVertices;
	std::vector<uint32> m_vecMeshIndices;

	ComPtr<ID3D11Buffer> m_pMeshVB;
	ComPtr<ID3D11Buffer> m_pMeshIB;

};

