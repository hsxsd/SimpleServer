#pragma once
#include <Windows.h>

class TdxTrader
{
private:
	HMODULE m_hDll;

public:
	TdxTrader();
	~TdxTrader();

	bool Init();
};