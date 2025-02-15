#pragma once
#include "Common.h"

class GraphicsCore;
class Graphics
{
	DECLARE_SINGLE(Graphics);
public:
	void   Initialize();
	void   Update(float _fDeltaTime);
	void   Render();

private:
	GraphicsCore* m_pCore;
};

