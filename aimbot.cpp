#include <algorithm>
#include <optional>

#include "CViewRender.hpp"

#include "features.hpp"
#include "IClientEntityList.hpp"
#include "math.hpp"

std::optional<CBaseEntity*> aimbot::best_target()
{
	std::optional<CBaseEntity*> target;

	const auto local_player = *gp_LocalPlayer;
	if(!local_player) return std::nullopt;

	const int local_team_id = local_player->GetTeam();
	float min_distance = 300.f; // screen distance in pixels1111
	
	for(auto i = 1; i < ClientEntityList->GetHighestEntityIndex(); ++i)
	{
		const auto BaseEntity = ClientEntityList->GetClientEntity(i);
		if(!BaseEntity) continue;

		const auto name = BaseEntity->GetName();

		if(name == "npc_soldier" || name == "player" || name == "npc_titan")
		{
			if(BaseEntity->GetTeam() != local_team_id)
			{
				if(!BaseEntity->GetAlive()) continue;
				vec3_t screen{};
				if(!world_to_screen(BaseEntity->GetOrigin(), screen)) continue;
				const float distance = get_display_distance(screen);
				if(min_distance > distance)
				{
					min_distance = distance;
					target = BaseEntity;
					esp::target_entity_idx = i;
				}
			}
		}
	}

	return target;
}

void aimbot::CreateMove(CUserCmd* cmd, bool& bSendPacket)
{
	const auto target = best_target();

	if(target.has_value() && GetAsyncKeyState(VK_F6))
	{
		const auto base_entity = target.value();
		vec3_t vec_center, screen_center;
		vec_center = base_entity->GetRenderable()->GetHitboxCenter(base_entity->GetModel(), 3);
		if (world_to_screen(vec_center, screen_center))
		{
			// Camera Position
			const auto local = *reinterpret_cast<vec3_t*>(reinterpret_cast<uintptr_t>(GetModuleHandleA("client.dll")) + 0x11ED5B0);
			
			vec3_t angle = calc_angle(local, vec_center);
			angle.x = std::clamp(angle.x, -89.f, 89.f);

			cmd->viewangles.x = lerp(cmd->viewangles.x, angle.x, 0.1f);
			cmd->viewangles.y = lerp(cmd->viewangles.y, angle.y, 0.1f);
		}
	}

	if(!target.has_value())
	{
		esp::target_entity_idx = 0;
	}
}

void aimbot::FireBullets(FireBulletsInfo_t* info)
{
	if(esp::target_entity_idx)
	{
		const auto target = ClientEntityList->GetClientEntity(esp::target_entity_idx);
		const auto target_head = target->GetRenderable()->GetHitboxCenter(target->GetModel(), 3);
		const auto local = *reinterpret_cast<vec3_t*>(reinterpret_cast<uintptr_t>(GetModuleHandleA("client.dll")) + 0x11ED5B0);

		vec3_t aim_dir = target_head - local;
		aim_dir = aim_dir.normalize();

		info->m_vecDirShooting = aim_dir;
	}
}

// not called
void aimbot::WriteUserCmd(CUserCmd* cmd)
{
	if(esp::target_entity_idx)
	{
		const auto target = ClientEntityList->GetClientEntity(esp::target_entity_idx);
		const auto target_head = target->GetRenderable()->GetHitboxCenter(target->GetModel(), 3);
		const auto local = *reinterpret_cast<vec3_t*>(reinterpret_cast<uintptr_t>(GetModuleHandleA("client.dll")) + 0x11ED5B0);
		
		vec3_t angle = calc_angle(local, target_head);
		angle.x = std::clamp(angle.x, -89.f, 89.f);
		angle.z = 0.f;
		cmd->viewangles = angle;
	}
}

void aimbot::GetBulletInfo(vec3_t* spawn, vec3_t* direction)
{
	if(esp::target_entity_idx)
	{
		const auto target = ClientEntityList->GetClientEntity(esp::target_entity_idx);
		if(!target) return;
		const auto target_head = target->GetRenderable()->GetHitboxCenter(target->GetModel(), 3);
		const auto local = *spawn;
		
		vec3_t angle = calc_angle(local, target_head);
		angle.x = std::clamp(angle.x, -89.f, 89.f);

		*direction = angle;
	}
}