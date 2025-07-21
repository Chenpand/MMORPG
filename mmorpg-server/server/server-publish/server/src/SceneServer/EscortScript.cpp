#include "EscortScript.h"

EscortScript::EscortScript(Avalon::ScriptState* state)
	:Avalon::ScriptFile(state)
{
	memset(m_RefreshRates, 0, sizeof(m_RefreshRates));
	memset(m_IncRates, 0, sizeof(m_IncRates));
}

EscortScript::~EscortScript()
{
}

void EscortScript::Init()
{
	Call<void>("Init", this);
}

void EscortScript::SetRefreshRate(int quality, UInt16 greenRate, UInt16 blueRate, UInt16 purpleRate, UInt16 goldRate)
{
	if(quality < 0 || quality > 4) return;
	m_RefreshRates[quality][1] = greenRate;
	m_RefreshRates[quality][2] = blueRate;
	m_RefreshRates[quality][3] = purpleRate;
	m_RefreshRates[quality][4] = goldRate;
}

void EscortScript::SetIncRefreshRate(int quality, UInt16 greenRate, UInt16 blueRate, UInt16 purpleRate, UInt16 goldRate)
{
	if(quality < 0 || quality > 4) return;
	m_IncRates[quality][1] = greenRate;
	m_IncRates[quality][2] = blueRate;
	m_IncRates[quality][3] = purpleRate;
	m_IncRates[quality][4] = goldRate;
}

void EscortScript::GetRefreshRate(int quality, UInt16 rates[5])
{
	if(quality < 0 || quality > 4) return;
	memmove(rates, m_RefreshRates[quality], sizeof(UInt16)*5);
}

void EscortScript::GetIncRefreshRate(int quality, UInt16 rates[5])
{
	if(quality < 0 || quality > 4) return;
	memmove(rates, m_IncRates[quality], sizeof(UInt16)*5);
}
