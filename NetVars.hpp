#pragma once
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <sstream>

#include "CHLClient.hpp"

typedef enum
{
	DPT_Int=0,
	DPT_Float,
	DPT_Vector,
	DPT_VectorXY,
	DPT_String,
	DPT_Array,
	DPT_Quaternion,
	DPT_Int64,
	DPT_DataTable,
	DPT_NUMSendPropTypes
} SendPropType;

class DVariant
{
public:
	union
	{
		float m_Float;
		long m_Int;
		char* m_pString;
		void* m_pData;
		float m_Vector[3];
	};
};

struct RecvTable
{
	char			_unk0x0000[0x8];		//0x0000
	struct RecvProp**		m_pProps;				//0x0008
	int				m_nProps;				//0x0010
	char			_unk0x0014[0x10C];		//0x0014
	void*			m_pDecoder;				//0x0120
	char*			m_pNetTableName;		//0x0128
	bool			m_bInitialized;			//0x0130
	bool			m_bInMainList;			//0x0131
 
}; //Size = 0x132
 
struct RecvProp
{
	SendPropType	m_RecvType;				//0x0000
	int				m_Offset;				//0x0004
	int				_unk0x0008;				//0x0008
	int				_unk0x000C;				//0x000C size?
	char			_unk0x0010[0x10];		//0x0010
	RecvTable*		m_pDataTable;			//0x0020
	char*			m_pVarName;				//0x0028
	bool			m_bInsideArray;			//0x0030
	char			_unk0x0031[0x7];		//0x0031
	RecvProp*		m_pArrayProp;			//0x0038
	void*			m_ProxyFn;				//0x0040
	char			_unk0x0048[0xC];		//0x0048
	int				m_Flags;				//0x0054
	char			_unk0x0058[0x4];		//0x0058
	int				m_nElements;			//0x005C
	char			_unk0x0060[0x8];		//0x0060
}; //Size = 0x68
 
struct ClientClass
{
	void*			m_pCreateFn;		//0x0000
	void*			m_pCreateEventFn;	//0x0008
	char*			m_pNetworkName;		//0x0010
	RecvTable*		m_pRecvTable;		//0x0018
	ClientClass*	m_pNext;			//0x0020
	char			_unk0028[0x8];		//0x0028
	char*			name;				//0x0030
}; //Size = 0x38

class CRecvProxyData
{
public:
	const RecvProp* m_pRecvProp;
	DVariant m_Value;
	int m_iElement;
	int m_ObjectID;
};

typedef void ( *RecvVarProxyFn )( const CRecvProxyData* pData, void* pStruct, void* pOut );

class CNetVarManager
{
public:
	static void Initialize();
	static int GetOffset( const char* tableName, const char* propName );
	static bool HookProp( const char* tableName, const char* propName, RecvVarProxyFn fun );
	static void DumpNetvars();
private:
	static int Get_Prop( const char* tableName, const char* propName, RecvProp** prop = 0 );
	static int Get_Prop(const RecvTable* recvTable, const char* propName, RecvProp** prop = 0 );
	static RecvTable* GetTable( const char* tableName );
	static void DumpTable(RecvTable* table, int depth);

	inline static std::vector< RecvTable* > m_tables;
	static inline std::ofstream m_file;
};

inline void CNetVarManager::Initialize()
{
	m_tables.clear();

	const ClientClass* clientClass = Client->GetAllClasses();
	if( !clientClass )
		return;

	while( clientClass )
	{
		RecvTable* recvTable = clientClass->m_pRecvTable;
		m_tables.push_back( recvTable );

		clientClass = clientClass->m_pNext;
	}
}

inline int CNetVarManager::GetOffset( const char* tableName, const char* propName )
{
	int offset = Get_Prop( tableName, propName );
	if( !offset )
	{
		return 0;
	}
	return offset;
}

inline bool CNetVarManager::HookProp( const char* tableName, const char* propName, RecvVarProxyFn fun )
{
	RecvProp* recvProp = 0;
	Get_Prop( tableName, propName, &recvProp );
	if( !recvProp )
		return false;

	recvProp->m_ProxyFn = fun;

	return true;
}

inline int CNetVarManager::Get_Prop( const char* tableName, const char* propName, RecvProp** prop )
{
	RecvTable* recvTable = GetTable( tableName );
	if( !recvTable )
		return 0;

	const int offset = Get_Prop( recvTable, propName, prop );
	if( !offset )
		return 0;

	return offset;
}

inline int CNetVarManager::Get_Prop(const RecvTable* recvTable, const char* propName, RecvProp** prop )
{
	int extraOffset = 0;
	for( int i = 0; i < recvTable->m_nProps; ++i )
	{
		RecvProp* recvProp = recvTable->m_pProps[ i ];
		const RecvTable* child = recvProp->m_pDataTable;

		if( child && ( child->m_nProps > 0 ) )
		{
			if(const int tmp = Get_Prop( child, propName, prop ) )
				extraOffset += ( recvProp->m_Offset + tmp );
		}

		if( _stricmp( recvProp->m_pVarName, propName ) != 0)
			continue;

		if( prop )
			*prop = recvProp;

		return ( recvProp->m_Offset + extraOffset );
	}

	return extraOffset;
}

inline RecvTable* CNetVarManager::GetTable( const char* tableName )
{
	if( m_tables.empty() )
		return 0;

	for ( RecvTable* table : m_tables )
	{
		if( !table )
			continue;

		if( _stricmp( table->m_pNetTableName, tableName ) == 0 )
			return table;
	}

	return 0;
}

//inline void CNetVarManager::DumpTable( RecvTable* table, int depth )
//{
//	std::string pre( "" );
//	for( int i = 0; i < depth; i++ )
//		pre.append( "\t" );
//
//	m_file << pre << table->m_pNetTableName << "\n";
//
//	for( int i = 0; i < table->m_nProps; i++ )
//	{
//		const RecvProp* prop = table->m_pProps[ i ];
//		if( !prop )
//			continue;
//
//		std::string varName( prop->m_pVarName );
//
//		if( varName.find( ( "baseclass" ) ) == 0 || varName.find( "0" ) == 0 || varName.find( "1" ) == 0 || varName.find( "2" ) == 0 )
//			continue;
//
//		m_file << pre << "\t " << varName << " "
//				<< std::setfill( '_' ) << std::setw( 60 - varName.length() - ( depth * 4 ) )
//				<< "[0x" << std::setfill( '0' ) << std::setw( 8 ) << std::hex
//				<< std::uppercase << prop->m_Offset << "]\n";
//
//		if( prop->m_pDataTable )
//			DumpTable( prop->m_pDataTable, depth + 1 );
//	}
//}
//
//inline void CNetVarManager::DumpNetvars()
//{
//	m_file.open(R"(C:\Users\Exceptis\Desktop\netvars.txt)");
//
//	for( ClientClass* pClass = Client->GetAllClasses(); pClass != NULL; pClass = pClass->m_pNext )
//	{
//		RecvTable* table = pClass->m_pRecvTable;
//		DumpTable( table, 0 );
//	}
//
//	m_file.close();
//}
//
