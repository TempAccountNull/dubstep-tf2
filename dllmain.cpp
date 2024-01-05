#include <windows.h>

#include <optional>
#include <unordered_set>

#include "CHLClient.hpp"
#include "CInput.hpp"
#include "ClientModeShared.hpp"
#include "Globals.hpp"
#include "hooks.hpp"
#include "IClientEntityList.hpp"
#include "IDebugOverlay.hpp"
#include "IEngineClient.hpp"
#include "ISurface.hpp"
#include "IVModelInfoClient.hpp"
#include "IVPanel.hpp"
#include "module.hpp"
#include "SourceHelpers.hpp"

void create_hooks()
{
	original_paint_traverse = GetVFunc<paint_traverse_t>(VPanel, 46);
	printf("[Dubstep] VPanel::PaintTraverse -> 0x%p\n", original_paint_traverse);
	add_hook(&reinterpret_cast<PVOID&>(original_paint_traverse), PaintTraverse);

	original_create_move = GetVFunc<create_move_t>(g_Input, 4); 
	printf("[Dubstep] CInput::CreateMove -> 0x%p\n", original_create_move);
	add_hook(&reinterpret_cast<PVOID&>(original_create_move), CreateMove);

	original_set_view_angles = GetVFunc<set_view_angles_t>(EngineClient, 64);
	printf("[Dubstep] EngineClient::SetViewAngles -> 0x%p\n", original_set_view_angles);
	add_hook(&reinterpret_cast<PVOID&>(original_set_view_angles), SetViewAngles);

	original_get_view_angles = GetVFunc<get_view_angles_t>(EngineClient, 63);
	printf("[Dubstep] EngineClient::SetViewAngles -> 0x%p\n", original_get_view_angles);
	add_hook(&reinterpret_cast<PVOID&>(original_get_view_angles), GetViewAngles);

	original_base_entity_fire_bullets = reinterpret_cast<base_entity_fire_bullets_t>(reinterpret_cast<uintptr_t>(GetModuleHandleA("client.dll")) + 0xC0280);
	printf("[Dubstep] CBaseEntity::FireBullets -> 0x%p\n", original_base_entity_fire_bullets);
	add_hook(&reinterpret_cast<PVOID&>(original_base_entity_fire_bullets), BaseEntityFireBullets);

	original_base_entity_get_bullet_info = reinterpret_cast<base_entity_get_bullet_info_t>(reinterpret_cast<uintptr_t>(GetModuleHandleA("client.dll")) + 0x14F8E0);
	printf("[Dubstep] CBaseEntity::BaseEntityGetBulletInfo -> 0x%p\n", original_base_entity_get_bullet_info);
	add_hook(&reinterpret_cast<PVOID&>(original_base_entity_get_bullet_info), BaseEntityGetBulletInfo);

	//original_chl_client_write_usercmd_delta_to_buffer = GetVFunc<chl_client_write_usercmd_delta_to_buffer_t>(g_Input, 6);
	//printf("[Dubstep] CInput::WriteUserCmdDeltaToBuffer -> 0x%p\n", original_base_entity_get_bullet_info);
	//add_hook(&reinterpret_cast<PVOID&>(original_chl_client_write_usercmd_delta_to_buffer), CHLClientWriteUsercmdDeltaToBuffer);

}

void destroy_hooks()
{
	remove_hooks();
	// hooks have exited

	return;
}

void create_cheat()
{
	// Alloc console & Test logging
	AllocConsole();
	FILE* dummy;
	freopen_s(&dummy, "CONOUT$", "w", stdout);
	
	Client = CaptureInterface< CHLClient >( "client.dll", "VClient018" );
	Surface = CaptureInterface< ISurface >( "vguimatsurface.dll", "VGUI_Surface031" );
	VPanel = CaptureInterface< IVPanel >( "vgui2.dll", "VGUI_Panel009" );
	ClientEntityList = CaptureInterface< IClientEntityList >( "client.dll", "VClientEntityList003" );
	EngineClient = CaptureInterface< IEngineClient >( "engine.dll", "VEngineClient013" );
	DebugOverlay = CaptureInterface< IVDebugOverlay >( "engine.dll", "VDebugOverlay004" );
	VModelInfoClient = CaptureInterface< IVModelInfoClient >( "engine.dll", "VModelInfoClient004" );
	ViewRender = *reinterpret_cast<CViewRender**>(reinterpret_cast<uintptr_t>(GetModuleHandleA("client.dll")) + 0x216f9c0); // 48 8B 0D ? ? ? ? 83 CA FF 48 8B 01 FF 50 60 48
	ClientModeShared = *reinterpret_cast<CClientModeShared**>(reinterpret_cast<uintptr_t>(GetModuleHandleA("client.dll")) + 0xFB3780); // 48 8B 0D ? ? ? ? 83 CA FF 48 8B 01 FF 50 60 48 // 48 8D 05 ?? ?? ?? ?? 48 3B C1 74 19
	
	g_Input = reinterpret_cast<CInput*>(reinterpret_cast<uintptr_t>(GetModuleHandleA("client.dll")) + 0xB1D380);
	g_ClientGlobalVariables = *reinterpret_cast<CGlobalVarsBase**>(reinterpret_cast<uintptr_t>(GetModuleHandleA("client.dll")) + 0xB06AD0); //40 53 48 81 EC ? ? ? ? 83 C9 FF E8 ? ? ? ? 48 8B D8 48 85 C0 0F 84 ? ? ? ? 4C 8B 00  // 48 8B 0D ? ? ? ? 83 CA FF 48 8B 01 FF 50 60 48
	gp_LocalPlayer = reinterpret_cast<CBaseEntity**>(reinterpret_cast<uintptr_t>(GetModuleHandleA("client.dll")) + 0xB18740 + 0x8);// 48 89 05 ? ? ? ? 48 85 C9 74 10 48 8B 01  local player

	printf("[Dubstep] DebugOverlay -> 0x%p\n", DebugOverlay);
	printf("[Dubstep] ViewRender -> 0x%p\n", ViewRender);
	printf("[Dubstep] CHLClient -> 0x%p\n", Client);
	printf("[Dubstep] ISurface -> 0x%p\n", Surface);
	printf("[Dubstep] IVPanel -> 0x%p\n", VPanel);
	printf("[Dubstep] IClientEntityList -> 0x%p\n", ClientEntityList);
	printf("[Dubstep] IEngineClient -> 0x%p\n", EngineClient);
	
	CNetVarManager::Initialize();
	//CNetVarManager::DumpNetvars();
	create_hooks();

	for(;;Sleep(100))
	{
		if(GetAsyncKeyState(VK_END) & 0x1)
		{
			printf("[Dubstep] Unloading...\n");
			FreeLibraryAndExitThread(gmodule::this_dll, 0);
		}
	}
}

BOOL APIENTRY DllMain( HMODULE hModule,
					   DWORD  ul_reason_for_call,
					   LPVOID lpReserved
)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		gmodule::this_dll = hModule;
		DisableThreadLibraryCalls(hModule);
		CloseHandle(
		CreateThread(nullptr,
		0,
		reinterpret_cast<LPTHREAD_START_ROUTINE>(&create_cheat),
		0, 0, 0));
	}

	if(ul_reason_for_call == DLL_PROCESS_DETACH)
	{
		destroy_hooks();
	}

	return TRUE;
}

