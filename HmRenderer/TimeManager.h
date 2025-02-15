#pragma once
#include "Common.h"

class TimeManager
{
	DECLARE_SINGLE(TimeManager);

public:
	void   Initialize();
	void   Update();

	float GetDeltaTime() const { return m_fDeltaTime; }

private:
	uint32 m_iFPS;

	uint64 m_iFrequency;
	uint64 m_iPrevCount;

	uint32 m_iFrameCount;

	float  m_fFrameTime;
	float  m_fDeltaTime;
	float  m_fStepSize;
};

