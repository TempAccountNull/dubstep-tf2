#pragma once
#include <windows.h>
#include <string>

template< typename T >
T* CaptureInterface( const std::string& strModule, const std::string& strInterface )
{
	typedef T* ( *CreateInterfaceFn )( const char* szName, int iReturn );
	const CreateInterfaceFn CreateInterface = reinterpret_cast<CreateInterfaceFn>(GetProcAddress(GetModuleHandleA(strModule.c_str()), "CreateInterface"));

	return CreateInterface( strInterface.c_str(), 0 );
}

template< typename T = void* >
T GetVFunc(void* vTable, int iIndex ) 
{
	return ( *( T** )vTable )[ iIndex ];
}