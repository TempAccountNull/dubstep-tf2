#pragma once
#include "CViewRender.hpp"
#include "Globals.hpp"
#include "IEngineClient.hpp"
#include "IVModelInfoClient.hpp"
#include "math.hpp"
#include "NetVars.hpp"

class ICollideable
{
public:
	const vec3_t& OBBMinsPreScaled()
	{
		using OriginalFn = vec3_t&(__thiscall*)(const void*);
		return GetVFunc<OriginalFn>(this, 1)(this);
	}
	const vec3_t& OBBMaxsPreScaled()
	{
		using OriginalFn = vec3_t&(__thiscall*)(const void*);
		return GetVFunc<OriginalFn>(this, 2)(this);
	}
};

class IClientRenderable
{
public:
	bool SetupBones(matrix3x4_t* out_mat, uint32_t max, uint32_t mask, float seed)
	{
		using OriginalFn = bool(__fastcall*)(void*, matrix3x4_t*, uint32_t, uint32_t, float);
		return GetVFunc<OriginalFn>(this, 13)(this, out_mat, max, mask, seed);
	}

	vec3_t GetHitboxCenter(model_t* model, int iHitbox, int iSet = 0)
	{
		const studiohdr_t* hdr = VModelInfoClient->GetStudioModel(model);
		matrix3x4_t bone_mat[191]{};

		if(SetupBones(bone_mat, 191, 256, g_ClientGlobalVariables->curtime))
		{
			const auto set = hdr->pHitboxSet(iSet);
			const auto hitbox = set->pHitbox(iHitbox);
					
			vec3_t min{}, max{}, vec_center;
			vector_transform( hitbox->bbmin, bone_mat[ hitbox->bone ], min );
			vector_transform( hitbox->bbmax, bone_mat[ hitbox->bone ], max );
			vec_center = ( min + max ) * 0.5;
			return vec_center;
		}
		else
		{
			int numberofbones = *(int *)((uintptr_t)this + 0x1190);
			printf("need more bone space (%d)\n", numberofbones);
		}

		return {};
	}

	vec3_t GetBone(unsigned int iBone)
	{
		if(iBone > 191) return {};
		matrix3x4_t bone_mat[191]{};

		if(SetupBones(bone_mat, 191, 256, g_ClientGlobalVariables->curtime))
		{
			auto mat = bone_mat[ iBone ];
			return {mat[0][3], mat[1][3], mat[2][3]};
		}
		else
		{
			const int numberofbones = *(int *)((uintptr_t)this + 0x1190);
			printf("need more bone space (%d)\n", numberofbones);
		}

		return {};
	}
};

class IClientNetworkable {
public:
	virtual class IClientUnknown* GetIClientUnknown();
	virtual void Unknown01();
	virtual struct ClientClass* GetClientClass();
};


class CBaseEntity
{
public:
	char __pad[0x30];
	int index;
	int GetHealth()
	{
		static uint32_t health_offset = CNetVarManager::GetOffset("DT_Player", "m_iHealth");
		return *reinterpret_cast<int*>(reinterpret_cast<uint64_t>(this) + health_offset);
	}
	int GetTeam()
	{
		static uint32_t team_id_offset = CNetVarManager::GetOffset("DT_BaseEntity", "m_iTeamNum");
		return *reinterpret_cast<int*>(reinterpret_cast<uint64_t>(this) + team_id_offset);
	};
	int GetFlags();
	int GetTickBase();
	int GetShotsFired();
	int GetMoveType();
	int GetModelIndex();
	int GetHitboxSet();
	int GetUserID();

	int GetCollisionGroup();
	int PhysicsSolidMaskForEntity();
	int GetOwner();
	int GetGlowIndex();
	bool GetAlive()
	{
		static uint32_t lifestate_offset = CNetVarManager::GetOffset("DT_Player", "m_lifeState");
		return !*reinterpret_cast<int*>(reinterpret_cast<uint64_t>(this) + lifestate_offset);
	}
	bool GetDormant();
	bool GetImmune();
	bool IsEnemy();
	bool IsVisible( int bone );
	bool IsBroken();
	bool IsTargetingLocal();

	vec3_t GetViewPunch();
	vec3_t GetPunch();
	vec3_t GetEyeAngles();
	vec3_t GetOrigin()
	{
		return *reinterpret_cast<vec3_t*>(reinterpret_cast<uint64_t>(this) + 0x12c);
	};
	vec3_t GetEyePosition();
	vec3_t GetBonePosition( int iBone );

	vec3_t GetVelocity();
	vec3_t GetPredicted( vec3_t p0 );
	class IClientRenderable* GetRenderable()
	{
		return reinterpret_cast<IClientRenderable*>((uintptr_t)this + 0x10);
	}
	class ICollideable* GetCollideable()
	{
		return reinterpret_cast<ICollideable*>((uintptr_t)this + 0x3f8);
	}
	class IClientNetworkable* GetNetworkable()
	{
		return reinterpret_cast<IClientNetworkable*>((uintptr_t)this + 0x18);
	}
	struct model_t* GetModel(){ return *reinterpret_cast<model_t**>(reinterpret_cast<uintptr_t>(this) + 0x40);};
	std::string_view GetName()
	{
		auto entity_name = *reinterpret_cast<const char**>(reinterpret_cast<uintptr_t>(this) + 0x4b0);
		if(!entity_name)
		{
			return GetNetworkable()->GetClientClass()->m_pNetworkName;
		}
		return entity_name;
	}
	std::string GetSteamID();
	std::string GetLastPlace();
	int& GetXUIDLow();
	int& GetXUIDHigh();
	class CBaseCombatWeapon* GetWeapon();
	struct ClientClass* GetClientClass();
};

class AI_BaseNPC
{
public:
	vec3_t GetOrigin()
	{
		return *reinterpret_cast<vec3_t*>(reinterpret_cast<uint64_t>(this) + 0x12c);
	}

	int GetHealth()
	{
		static uint32_t health_offset = 0x390;
		return *reinterpret_cast<int*>(reinterpret_cast<uint64_t>(this) + health_offset);
	}
};

inline CBaseEntity** gp_LocalPlayer;
