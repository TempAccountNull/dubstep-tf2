#include <unordered_set>

#include "Entity.hpp"
#include "features.hpp"
#include "IClientEntityList.hpp"
#include "ISurface.hpp"
#include "module.hpp"
#include "SourceTypes.hpp"

void add_text(Color col, vec3_t pos, bool centered, const char* format, ...) 
{
	char buffer[ 1024 ];
	va_list vaList;

	memset( buffer, 0, sizeof( buffer ) );

	va_start( vaList, format );
	wvsprintfA( buffer, format, vaList );
	va_end( vaList );

	wchar_t wbuffer[ 1024 ]{};
	char* bufPtr = buffer;
	wchar_t* wbufPtr = wbuffer;
	while ( *bufPtr ) {
		*wbufPtr++ = static_cast<wchar_t>(*bufPtr++);
	}

	Surface->DrawSetTextFont(esp::custom_font);
	Surface->DrawSetTextColor(col);
	int w, h;
	Surface->GetTextSize(esp::custom_font, wbuffer, w, h);
	float x = centered ? pos.x - w / 2 : pos.x;
	float y = centered ? pos.y - h / 2 : pos.y;
	Surface->DrawSetTextPos(x, y);
	Surface->DrawPrintText(wbuffer, wcslen( wbuffer ));
}

void draw_bb(const vec3_t& min, const vec3_t max)
{
	// Define 8 corners of the bounding box
	vec3_t corners[8];
	corners[0] = { min.x, min.y, min.z };
	corners[1] = { max.x, min.y, min.z };
	corners[2] = { max.x, max.y, min.z };
	corners[3] = { min.x, max.y, min.z };
	corners[4] = { min.x, min.y, max.z };
	corners[5] = { max.x, min.y, max.z };
	corners[6] = { max.x, max.y, max.z };
	corners[7] = { min.x, max.y, max.z };

	// Define the 12 edges of the bounding box by the indices of the start and end corners.
	int edges[12][2] = {
	    {0, 1}, {1, 2}, {2, 3}, {3, 0},
	    {4, 5}, {5, 6}, {6, 7}, {7, 4},
	    {0, 4}, {1, 5}, {2, 6}, {3, 7}
	};

	// Projection of the bounding box corners
	vec3_t screenCorners[8]{};

	// Project each corner point to the screen.
	for(int i = 0; i < 8; i++) {
	    if(!world_to_screen(corners[i], screenCorners[i])) {
	        // If any of the corners could not be projected, you might want to skip drawing the bounding box.
	        return;
	    }
	}

	// Draw the 12 edges of the bounding box.
	for(const auto& edge: edges) {
	    const auto& start = screenCorners[edge[0]];
	    const auto& end = screenCorners[edge[1]];
	    Surface->DrawLine(start.x, start.y, end.x, end.y);
	}
}

enum PlayerBones : uint32_t
{
	head = 12,
	neck = 9,
	right_shoulder = 39,
	right_elbow = 42,
	right_hand = 46,
	right_knee = 67,
	right_foot = 68,
	right_leg = 66,
	mid_spine = 6,
	pelvis = 5,
	left_shoulder = 15,
	left_elbow = 16,
	left_hand = 20,
	left_knee = 62,
	left_foot = 63,
	left_leg = 61
};

constexpr uint32_t head_left[7] {head, neck, mid_spine, pelvis, left_leg, left_knee, left_foot};
constexpr uint32_t mid_right[4] {pelvis, right_leg, right_knee, right_foot};
constexpr uint32_t arms[7] {left_hand, left_elbow, left_shoulder, neck, right_shoulder, right_elbow, right_hand};

void esp::PaintTraverse()
{
	if(!gp_LocalPlayer) return;
	const auto local_player = *gp_LocalPlayer;

	if(!local_player) return;
	try
	{
		const int local_team_id = local_player->GetTeam();

	for(auto i = 1; i < ClientEntityList->GetHighestEntityIndex(); ++i)
	{
		const auto BaseEntity = ClientEntityList->GetClientEntity(i);
		if(!BaseEntity) continue;
		static std::unordered_set<std::string_view> names;
		auto name = BaseEntity->GetName();
		if(name != "no_name" && !names.contains(name))
		{
			names.insert(name);
			printf("Entity Name -> %s\n", name.data());
		}

		player_info_t info{};
		if(!EngineClient->GetPlayerInfo(i, &info)){
			const vec3_t position = BaseEntity->GetOrigin();
			vec3_t screen{};
			if(world_to_screen(position, screen))
			{
				//add_text(Color::White(), screen, true, "%s", name.data());
				
				//add_text(draw_color, screen + vec3_t{0, 30.f, 0}, true, "%s", sz_name);
			}
		}
			

		if(!BaseEntity->GetAlive()) continue;
		if(BaseEntity == local_player) continue;
		Color draw_color;
		if(BaseEntity->GetTeam() == local_team_id)
		{
			draw_color = Color::Blue();
		}
		else
		{
			draw_color = Color::Red();
		}
		if(target_entity_idx == i)
		{
			draw_color = Color::Green();
		}

		const vec3_t position = BaseEntity->GetOrigin();
		ICollideable* collision = BaseEntity->GetCollideable();
		vec3_t min = collision->OBBMinsPreScaled();
		vec3_t max = collision->OBBMaxsPreScaled();
	
		Surface->DrawSetColor(draw_color);
		//draw_bb(min + position, max + position);

		const auto model = BaseEntity->GetModel();
		if(!model) continue;

		const auto draw_skeleton = [=](const uint32_t* bones, int numbones) -> void
			{
				std::optional<vec3_t> last_bone_screen;
				for(int i = 0; i < numbones; ++i)
				{
				
					vec3_t current_bone = BaseEntity->GetRenderable()->GetBone(bones[i]);
					vec3_t current_screen;
					if(world_to_screen(current_bone, current_screen))
					{
						if(!last_bone_screen.has_value())
						{
							last_bone_screen = current_screen;
							continue;
						}
						const auto last_bone = last_bone_screen.value();

						Surface->DrawLine(last_bone.x, last_bone.y, current_screen.x, current_screen.y);
						last_bone_screen = current_screen;
					}
				}
			};

		auto sz_name = std::string(model->szName);
		if(name=="player" && sz_name.find("titan") != std::string::npos)
		{
			Surface->DrawSetColor(draw_color);
			draw_bb(position + min, position +  max);
			vec3_t screen{};
			if(world_to_screen(position, screen))
			{
				add_text(draw_color, screen, true, "%s [%d]", info.szName, BaseEntity->GetHealth());
			}
		}
		else if (name == "npc_titan")
		{
			AI_BaseNPC* npc = (AI_BaseNPC*)(BaseEntity);
			Surface->DrawSetColor(draw_color);
			draw_bb(position + min, position +  max);
			vec3_t screen{};
			if(world_to_screen(position, screen))
			{
				add_text(draw_color, screen, true, "%s [%d]", info.szName, npc->GetHealth());
			}
		}
		else if (name == "player")
		{
			draw_skeleton(head_left, 7);
			draw_skeleton(mid_right, 4);
			draw_skeleton(arms, 7);

			vec3_t screen{};
			if(world_to_screen(position, screen))
			{
				add_text(draw_color, screen, true, "%s [%d]", info.szName, BaseEntity->GetHealth());
			}
		}
		else if(name == "npc_soldier")
		{
			draw_skeleton(head_left, 7);
			draw_skeleton(mid_right, 4);
			draw_skeleton(arms, 7);
			AI_BaseNPC* npc = (AI_BaseNPC*)(BaseEntity);
			vec3_t screen{};
			if(world_to_screen(position, screen))
			{
				add_text(draw_color, screen, true, "Soldier [%d]", info.szName, npc->GetHealth());
			}
		}
	}
	}catch(...)
	{
		printf("painttraverse exception");
	}
}
