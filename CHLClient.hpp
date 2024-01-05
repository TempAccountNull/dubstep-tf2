#pragma once
#include "SourceHelpers.hpp"

class CHLClient
{
public:
	struct ClientClass* GetAllClasses()
	{
		typedef ClientClass*(__thiscall* OriginalFn)( void* );
		return GetVFunc< OriginalFn >( this, 11 )( this );
	}
};

inline CHLClient* Client = nullptr;
