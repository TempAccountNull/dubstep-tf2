#pragma once
#include "SourceHelpers.hpp"

class IVPanel
{
public:
	const char* GetName( unsigned long long iPanel )
	{
		typedef const char*( __thiscall* OriginalFn )( void*, unsigned long long);
		return GetVFunc<OriginalFn>( this, 42 )( this, iPanel);
	}
};

inline IVPanel* VPanel = nullptr;