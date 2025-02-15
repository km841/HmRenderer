#include "TimeManager.h"

TimeManager::TimeManager()
	: m_fStepSize(1.0f/60.0f)
	, m_iFPS(0)
	, m_iFrequency(0)
	, m_iPrevCount(0)
	, m_iFrameCount(0)
	, m_fFrameTime(0.0f)
	, m_fDeltaTime(0.0f)
{	  
}

TimeManager::~TimeManager()
{

}

void TimeManager::Initialize()
{
	::QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&m_iFrequency));
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&m_iPrevCount));
}

void TimeManager::Update()
{
	uint64 iCurrentCount;
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&iCurrentCount));

	m_fDeltaTime = (iCurrentCount - m_iPrevCount) / static_cast<float>(m_iFrequency);

	if (m_fDeltaTime > m_fStepSize)
		m_fDeltaTime = m_fStepSize;

	m_iFrameCount++;
	m_fFrameTime += m_fDeltaTime;
	m_iPrevCount = iCurrentCount;

	if (m_fFrameTime > 1.f)
	{
		m_iFPS = static_cast<uint32>(m_iFrameCount / m_fFrameTime);
		m_fFrameTime = 0.f;
		m_iFrameCount = 0;
	}
}
