#pragma once
#include "Common.h"

class GraphicsCore;
class Graphics
{
	DECLARE_SINGLE(Graphics);

public:
	void   Initialize(HWND _hHwnd, int _iWidth, int _iHeight);
	void   Update(float _fDeltaTime);
	void   Render(float _fDeltaTime);

private:
	GraphicsCore* m_pCore;
};

