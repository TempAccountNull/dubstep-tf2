#pragma once
#include <cmath>

#include "SourceTypes.hpp"

inline float lerp(float a, float b, float t) {
    return a + t * (b - a);
}

inline void vector_transform( const vec3_t& in, const matrix3x4_t& matrix, vec3_t& out ) {
	out.x = in.dot(*(vec3_t*)(&matrix.m_matrix[0])) + matrix[0][3];
    out.y = in.dot(*(vec3_t*)(&matrix.m_matrix[1])) + matrix[1][3];
    out.z = in.dot(*(vec3_t*)(&matrix.m_matrix[2])) + matrix[2][3];
}

inline vec3_t calc_angle(const vec3_t& local, const vec3_t& target)
{
	const vec3_t delta = target - local;
    vec3_t result{};
    // atan2 for the yaw (horizontal angle)
    result.y = atan2(delta.y, delta.x);  
    
    // sqrt and atan2 for the pitch (vertical angle)
	const float xy_distance = sqrt(delta.x * delta.x + delta.y * delta.y);
    result.x = atan2(-delta.z, xy_distance);  
    result.x *= (180.0f / 3.1415926535);
    result.y *= (180.0f / 3.1415926535);
    return result;
}