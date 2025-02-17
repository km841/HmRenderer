#pragma once
#include "Shader.h"
class RaycastShader :
    public Shader
{
public:
    RaycastShader();
    virtual ~RaycastShader();

public:
    void Initialize(ComPtr<ID3D11Device> _pDevice, int _iWidth, int _iHeight);
    ComPtr<ID3D11Buffer> GetWindowSizeBuffer() { return m_pWindowSizeBuffer; }

private:
    void CreateVertexShader(ComPtr<ID3D11Device> _pDevice);
    void CreateInputLayout(ComPtr<ID3D11Device> _pDevice);
    void CreatePixelShader(ComPtr<ID3D11Device> _pDevice);
    void CreateConstantBuffer(ComPtr<ID3D11Device> _pDevice, int _iWidth, int _iHeight);

private:
    ComPtr<ID3D11Buffer> m_pWindowSizeBuffer;
    ComPtr<ID3DBlob> m_vsBlob;
    ComPtr<ID3DBlob> m_psBlob;
};

