#pragma once
#include "IEngineClient.hpp"
#include "Entity.hpp"
#include <cstdint>


class IClientEntityList
{
public:
	virtual void Function0() = 0;
	virtual void Function1() = 0;
	virtual void Function2() = 0;

	virtual CBaseEntity* GetClientEntity( int iIndex ) = 0;
	virtual CBaseEntity* GetClientEntityFromHandle( uint32_t hHandle ) = 0;
	virtual int NumberOfEntities( bool bIncludeNonNetworkable ) = 0;
	virtual int GetHighestEntityIndex() = 0;
};

inline IClientEntityList* ClientEntityList = nullptr;
