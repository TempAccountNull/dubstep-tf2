#include "hooks.hpp"

#include <intrin.h>
#include <optional>


#include "CInput.hpp"
#include "features.hpp"
#include "IVPanel.hpp"
#include "SourceTypes.hpp"

bool bSendPacket;

//__attribute__((naked)) void CreateMoveEpilouge() {
//	__asm {
//		mov rbp, bSendPacket
//		ret
//	}
//}

void CreateMove(CInput* this_ptr, int sequence_number, float input_sample_frametime, char active)
{
	original_create_move(this_ptr, sequence_number, input_sample_frametime, active);

	//printf("SendPacket (%d)\n",bSendPacket);

	const int cmd_idx = sequence_number % 300;
	CVerifiedUserCmd* VerifiedUserCmd = &this_ptr->m_pVerifiedCommands[cmd_idx];
	CUserCmd* UserCmd = &this_ptr->m_pCommands[cmd_idx];

	try
	{
		aimbot::CreateMove(UserCmd, bSendPacket);
	} catch(...){}

	VerifiedUserCmd->m_cmd = *UserCmd;
	VerifiedUserCmd->m_crc = UserCmd->GetChecksum();
	//CreateMoveEpilouge();
}

struct bf_write
{
	char pad[0x14];
	bool bIsOverflown;
};


// broken needs validate
bool CHLClientWriteUsercmdDeltaToBuffer(CInput* this_ptr, int nSlot, struct bf_write *buf, int from, int to )
{
	CUserCmd nullcmd{};
	CUserCmd* f;
	CUserCmd* t;
		
	CInput::GetNullCmd(&nullcmd);
	if(from == -1)
	{
		f = &nullcmd;
	}
	else
	{
		f = this_ptr->GetUserCmd(nSlot, from);
		//validate?
	}

	if(to == -1)
	{
		t = &nullcmd;
	}
	else
	{
		t = this_ptr->GetUserCmd(nSlot, to);
		//validate?
	}
	// validate
	
	const auto backup_angles = t->viewangles;
	aimbot::WriteUserCmd(t);
	CInput::WriteUserCmd(buf, t, f);
	t->viewangles = backup_angles;

	return *(char *)((uintptr_t)buf + 0x14) == '\0';
}

//__attribute__((naked)) void CreateMoveProlouge() {
//	__asm {
//		mov bSendPacket, rbp
//        jmp CreateMove // Call the function.
//	}
//}

void PaintTraverse(void* this_ptr, unsigned long long panel, bool forceRepaint, bool allowForce )
{
	original_paint_traverse(this_ptr, panel, forceRepaint, allowForce);
	static unsigned long long mat_top_panel = 0;

	if(!mat_top_panel)
	{
		const char* name = VPanel->GetName(panel);
		//FocusOverlayPanel

		if(name[0] == 'F' && name[2] == 'c')
			mat_top_panel = panel;
	}

	if(!mat_top_panel || mat_top_panel != panel) return;

	if(!esp::custom_font)
	{
		esp::custom_font = Surface->Create_Font();
		Surface->SetFontGlyphSet(esp::custom_font, "Tahoma", 12, 150, 0, 0, FONTFLAG_OUTLINE | FONTFLAG_ANTIALIAS);
	}
	try
	{
		esp::PaintTraverse();
	}catch(...) {printf("Caught PaintTraverse Exception!\nvb");}
}

void BaseEntityGetBulletInfo(void* this_ptr, vec3_t* spawn, vec3_t* dir)
{
	original_base_entity_get_bullet_info(this_ptr, spawn, dir);
	if(this_ptr == *gp_LocalPlayer)
	{
		try
		{
			aimbot::GetBulletInfo(spawn, dir);
		} catch(...){}
	}
	
}

void BaseEntityFireBullets(void* this_ptr, FireBulletsInfo_t* info)
{
	if(this_ptr == *gp_LocalPlayer)
	{
		try
		{
			aimbot::FireBullets(info);
		} catch(...){}
	}
	return original_base_entity_fire_bullets(this_ptr, info);
}

void SetViewAngles(void* this_ptr, vec3_t& new_angles)
{
	//static const uint64_t client_dll_2561ff = reinterpret_cast<uint64_t>(GetModuleHandleA("client.dll")) + 0x2561ff;
	//static const uint64_t client_dll_255365 = reinterpret_cast<uint64_t>(GetModuleHandleA("client.dll")) + 0x255365;

	return original_set_view_angles(this_ptr, new_angles);
}

void GetViewAngles(void* this_ptr, vec3_t& angles)
{
	//printf("IVEngineClient::GetViewAngles called from %p\n", _ReturnAddress());
	return original_get_view_angles(this_ptr, angles);
} 