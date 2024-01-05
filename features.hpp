#pragma once

#include <optional>

#include "Entity.hpp"
#include "CInput.hpp"
#include "SourceTypes.hpp"

namespace esp
{
	inline HFont game_font{336};
	inline HFont custom_font;
	inline int target_entity_idx;
	void PaintTraverse(void);
}

namespace aimbot
{
	inline float flTimeUntilNewTarget;
	void CreateMove(CUserCmd* cmd, bool& bSendPacket);
	void FireBullets(FireBulletsInfo_t* info);
	void GetBulletInfo(vec3_t* spawn, vec3_t* direction);
	void WriteUserCmd(CUserCmd* cmd);
	std::optional<CBaseEntity*> best_target();
}