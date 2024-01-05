#pragma once
#include <cstdint>
#include <windows.h>

#include "SourceHelpers.hpp"
#include "SourceTypes.hpp"

typedef uint32_t CRC32_t;

struct CUserCmd
{
	int				command_number; //0
	int				tick_count; //4
	int				command_time;//8
	vec3_t			viewangles; //12
	char			pad1[28];//24
	float			forwardmove;	 // members clamped [-1;1]//52	//38		
	float			sidemove;			
	float			upmove;	
	int				buttons;//64
	byte			impulse; //68
	char			pad2[0x68];
	char			pad3[0x98];

	CRC32_t GetChecksum() const
	{
		using GetChecksumFn = CRC32_t(__fastcall*)(CUserCmd const*);
		static GetChecksumFn GetChecksum_ = reinterpret_cast<GetChecksumFn>(reinterpret_cast<uintptr_t>(GetModuleHandleA("client.dll")) + 0x33C650);
		return GetChecksum_(this);
	}
};

struct CVerifiedUserCmd
{
	CUserCmd	m_cmd;
	CRC32_t		m_crc;
	uint32_t unused_padding;
};

class CInput
{
public:
	CUserCmd* GetUserCmd(int nSlot, int sequence_number)
	{
		using GetUserCmd_t = CUserCmd*(__fastcall*)(CInput*,int,int);
		return GetVFunc<GetUserCmd_t>(this, 9)(this, nSlot, sequence_number);
	}

	static void WriteUserCmd(void* buf, void* to, void* from)
	{
		using WriteUserCmd_t = void(__fastcall*)(void*,void*,void*);
		static auto WriteUserCmdFn = reinterpret_cast<WriteUserCmd_t>(reinterpret_cast<uintptr_t>(GetModuleHandleA("client.dll")) + 0x33F160);
		WriteUserCmdFn(buf, to, from);
	}
	static void GetNullCmd(void* cmd)
	{
		using NullCmd_t = void(__fastcall*)(void*);
		static auto NullCmdFn = reinterpret_cast<NullCmd_t>(reinterpret_cast<uintptr_t>(GetModuleHandleA("client.dll")) + 0x33C350);
		NullCmdFn(cmd);
	}

	unsigned char pad[0xf8]; //0xf8
	struct CUserCmd* m_pCommands; //0xf8
	struct CVerifiedUserCmd* m_pVerifiedCommands; //0x100
};

inline CInput* g_Input;