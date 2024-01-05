#pragma once

#include "ISurface.hpp"
#include "SourceHelpers.hpp"
#include "SourceTypes.hpp"

class CViewRender
{
public:
	matrix3x4_t* GetWorldToScreenMatrix()
	{
		typedef matrix3x4_t*(__thiscall*OriginalFn)(void*, char);
		return GetVFunc<OriginalFn>(this, 14)(this, 0x0);
	}
};

inline CViewRender* ViewRender = nullptr;

inline bool world_to_screen(const vec3_t& point, vec3_t& screen)
{
	//TODO add screen size from ISurface or IVEngineClient
    const matrix3x4_t& screen_mat = *ViewRender->GetWorldToScreenMatrix();

	static auto screen_width = static_cast<float>(GetSystemMetrics(SM_CXSCREEN));
	static auto screen_height = static_cast<float>(GetSystemMetrics(SM_CYSCREEN));

	static auto h_w = screen_width / 2.f;
	static auto h_h = screen_height / 2.f;

	const float w = screen_mat[3][0] * point.x + screen_mat[3][1] * point.y + screen_mat[3][2] * point.z + screen_mat[3][3]; 
    screen.z = 0; 
    if (w > 0.01f) 
    {
	    const float inverseWidth = 1 / w;

        screen.x = h_w + (0.5f * ((screen_mat[0][0] * point.x + screen_mat[0][1] * point.y + screen_mat[0][2] * point.z + screen_mat[0][3]) * inverseWidth) * screen_width + 0.5f);
        screen.y = h_h - (0.5f * ((screen_mat[1][0] * point.x + screen_mat[1][1] * point.y + screen_mat[1][2] * point.z + screen_mat[1][3]) * inverseWidth) * screen_height + 0.5f); 
        return true; 
    } 
    return false; 
}

inline float get_display_distance( vec3_t view )
{
	// TODO Get screen size from IVEngine / Surface
	int h_w = GetSystemMetrics(SM_CXSCREEN) / 2;
	int h_h = GetSystemMetrics(SM_CYSCREEN) / 2;
	
	return vec3_t{ view.x - h_w, view.y - h_h }.length( );
}
