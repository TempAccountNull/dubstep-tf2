#pragma once
#include <vector>

#include "CInput.hpp"
#include "detours.h"
#pragma comment(lib, "detours.lib")

struct detour
{
	void* hook;
	void* function;
};

inline std::vector<detour> detours{};

inline bool add_hook(PVOID* ppPointer, PVOID pDetour, PDETOUR_TRAMPOLINE* ppRealTrampolin = nullptr)
{
	DetourTransactionBegin( );
	DetourUpdateThread( GetCurrentThread( ) );
	if (ppRealTrampolin == nullptr)
		DetourAttach( ppPointer, pDetour );
	else
		DetourAttachEx( ppPointer, pDetour, ppRealTrampolin, NULL, NULL );

	if (DetourTransactionCommit( ) == NO_ERROR)
	{
		detours.push_back( { ppPointer, pDetour } );
		return true;
	}

	return false;
}

inline void remove_hooks()
{
	for(const auto& detour : detours)
	{
		DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
		DetourDetach( static_cast<PVOID*>(detour.hook), detour.function);
		DetourTransactionCommit();
	}
}

using paint_traverse_t = void( __fastcall*)( void*, unsigned long long, bool, bool );
inline paint_traverse_t original_paint_traverse;

using create_move_t = uint64_t( __fastcall*)( CInput*, int, float, char );
inline create_move_t original_create_move = nullptr;

using set_view_angles_t = void( __fastcall*)( void*, vec3_t& );
inline set_view_angles_t original_set_view_angles = nullptr;

using get_view_angles_t = void( __fastcall*)( void*, vec3_t& );
inline get_view_angles_t original_get_view_angles = nullptr;

using base_entity_fire_bullets_t = void( __fastcall*)( void*, FireBulletsInfo_t* );
inline base_entity_fire_bullets_t original_base_entity_fire_bullets = nullptr;

using base_entity_get_bullet_info_t = void(__fastcall*)(void*, vec3_t* spawn, vec3_t* dir);
inline base_entity_get_bullet_info_t original_base_entity_get_bullet_info = nullptr;

using chl_client_write_usercmd_delta_to_buffer_t = bool(__fastcall*)(void* this_ptr, int nSlot, struct bf_write*buf, int from, int to);
inline chl_client_write_usercmd_delta_to_buffer_t original_chl_client_write_usercmd_delta_to_buffer = nullptr;

void BaseEntityGetBulletInfo(void* this_ptr, vec3_t* spawn, vec3_t* dir);
void BaseEntityFireBullets(void* this_ptr, FireBulletsInfo_t* info);
bool CHLClientWriteUsercmdDeltaToBuffer(CInput* this_ptr, int nSlot, struct bf_write *buf, int from, int to );
void CreateMove(CInput* this_ptr, int sequence_number, float input_sample_frametime, char active);
void SetViewAngles(void* this_ptr, vec3_t& new_angles);
void GetViewAngles(void* this_ptr, vec3_t& angles);
void PaintTraverse(void* this_ptr, unsigned long long panel, bool forceRepaint, bool allowForce );
void CreateMoveProlouge();