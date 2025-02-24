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

    void ShowBone(bool _bBone) { m_WindowData.iIsBone = _bBone; }
    void ShowCartilage(bool _bCartilage) { m_WindowData.iIsCartilage = _bCartilage; }
    void UpdateConstantBuffer(ComPtr<ID3D11DeviceContext> _pContext);

    bool IsShowBone() { return static_cast<bool>(m_WindowData.iIsBone); }
    bool IsShowCartilage() { return static_cast<bool>(m_WindowData.iIsCartilage); }

private:
    void CreateVertexShader(ComPtr<ID3D11Device> _pDevice);
    void CreateInputLayout(ComPtr<ID3D11Device> _pDevice);
    void CreatePixelShader(ComPtr<ID3D11Device> _pDevice);
    void CreateConstantBuffer(ComPtr<ID3D11Device> _pDevice, int _iWidth, int _iHeight);

private:
    WindowSizeBuffer m_WindowData;
    ComPtr<ID3D11Buffer> m_pWindowSizeBuffer;
    ComPtr<ID3DBlob> m_vsBlob;
    ComPtr<ID3DBlob> m_psBlob;
};

