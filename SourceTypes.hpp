#pragma once

#include <cmath>
#include <xmmintrin.h>

typedef unsigned long HFont;

enum ButtonStates : int
{
	IN_ATTACK =	(1 << 0),
	IN_JUMP	=	(1 << 1),
	IN_DUCK	=	(1 << 2),
	IN_FORWARD=	(1 << 3),
	IN_BACK	=	(1 << 4),
	IN_USE	=	(1 << 5),
	IN_CANCEL=	(1 << 6),
	IN_LEFT	=	(1 << 7),
	IN_RIGHT	=(1 << 8),
	IN_MOVELEFT	=(1 << 9),
	IN_MOVERIGHT= (1 << 10),
	IN_ATTACK2	=(1 << 11),
	IN_RUN     = (1 << 12),
	IN_RELOAD	=(1 << 13),
	IN_ALT1		=(1 << 14),
	IN_SCORE	=(1 << 15),
};

#define PITCH 0
#define YAW 1
#define ROLL 2

struct vec3_t
{
	vec3_t() = default;

	explicit vec3_t( float xyz ) : x( xyz ), y( xyz ), z( xyz ) {}
	vec3_t( float x, float y ) : x( x ), y( y ), z( 0 ) {}
	vec3_t( float x, float y, float z ) : x( x ), y( y ), z( z ) {}
	explicit vec3_t( float* arr ) : x( arr[ PITCH ] ), y( arr[ YAW ] ), z( arr[ ROLL ] ) {}

	[[nodiscard]] float length() const
	{
		return sqrt( x * x + y * y + z * z );
	}

	[[nodiscard]] float length_q() const
	{
		union {
		    __m128 v;    // SSE 4 x float vector
		    float a[4];  // scalar array of 4 floats
		} root;
		root.a[0] = { x * x + y * y + z * z };
		
		_mm_sqrt_ps(root.v);

		return root.a[0];
	}

	[[nodiscard]] float length_2d() const
	{
			union {
		    __m128 v;    // SSE 4 x float vector
		    float a[4];  // scalar array of 4 floats
		} root;
		root.a[0] = { x * x + y * y };

		_mm_sqrt_ps(root.v);

		return root.a[0];
	}

	[[nodiscard]] vec3_t normalize() const
	{
		vec3_t out{};
		float l = length();

		out.x = x / l;
		out.y = y / l;
		out.z = z / l;

		return out;
	}

	[[nodiscard]] float self_dot() const
	{
		return ( x * x + y * y + z * z );
	}

	[[nodiscard]] float dot( const vec3_t & in ) const
	{
		return ( x * in.x + y * in.y + z * in.z );
	}

	float dot( const float* in ) const
	{
		return ( x * in[ PITCH ] + y * in[ YAW ] + z * in[ ROLL ] );
	}

	[[nodiscard]] float distance(const vec3_t& in) const
	{
		return (*this - in).length();
	}

	float& operator[](int i)
	{
		return reinterpret_cast<float*>(this)[i];
	}

#pragma region assignment
	vec3_t& operator=( const vec3_t & in ) = default;
#pragma endregion assignment

#pragma region equality
	bool operator!=( const vec3_t & in ) const
	{
		return ( x != in.x || y != in.y || z != in.z );
	}

	bool operator==( const vec3_t & in ) const
	{
		return ( x == in.x && y == in.y && z == in.z );
	}
#pragma endregion equality

#pragma region addition
	vec3_t operator+( vec3_t in ) const
	{
		return {
			x + in.x,
			y + in.y,
			z + in.z
		};
	}

	vec3_t operator+( float in ) const
	{
		return {
			x + in,
			y + in,
			z + in
		};
	}

	vec3_t& operator+=( vec3_t in )
	{
		x += in.x;
		y += in.y;
		z += in.z;

		return *this;
	}

	vec3_t& operator+=( float in )
	{
		x += in;
		y += in;
		z += in;

		return *this;
	}
#pragma endregion addition

#pragma region substraction
	vec3_t operator-( vec3_t in ) const
	{
		return {
			x - in.x,
			y - in.y,
			z - in.z
		};
	}

	vec3_t operator-( float in ) const
	{
		return {
			x - in,
			y - in,
			z - in
		};
	}

	vec3_t& operator-=( vec3_t in )
	{
		x -= in.x;
		y -= in.y;
		z -= in.z;

		return *this;
	}

	vec3_t& operator-=( float in )
	{
		x -= in;
		y -= in;
		z -= in;

		return *this;
	}
#pragma endregion substraction

#pragma region multiplication
	vec3_t operator*( vec3_t in ) const
	{
		return {
			x * in.x,
			y * in.y,
			z * in.z
		};
	}

	vec3_t operator*( float in ) const
	{
		return {
			x * in,
			y * in,
			z * in
		};
	}

	vec3_t& operator*=( vec3_t in )
	{
		x *= in.x;
		y *= in.y;
		z *= in.z;

		return *this;
	}

	vec3_t& operator*=( float in )
	{
		x *= in;
		y *= in;
		z *= in;

		return *this;
	}
#pragma endregion multiplication

#pragma region division
	vec3_t operator/( vec3_t in ) const
	{
		return {
			x / in.x,
			y / in.y,
			z / in.z
		};
	}

	vec3_t operator/( float in ) const
	{
		return {
			x / in,
			y / in,
			z / in
		};
	}

	vec3_t& operator/=( vec3_t in )
	{
		x /= in.x;
		y /= in.y;
		z /= in.z;

		return *this;
	}

	vec3_t& operator/=( float in )
	{
		x /= in;
		y /= in;
		z /= in;

		return *this;
	}
#pragma endregion division

	float x, y, z;
};

struct matrix3x4_t
{
	matrix3x4_t() = default;
	matrix3x4_t(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23 )
	{
		m_matrix[ 0 ][ 0 ] = m00;
		m_matrix[ 0 ][ 1 ] = m01;
		m_matrix[ 0 ][ 2 ] = m02;
		m_matrix[ 0 ][ 3 ] = m03;
		m_matrix[ 1 ][ 0 ] = m10;
		m_matrix[ 1 ][ 1 ] = m11;
		m_matrix[ 1 ][ 2 ] = m12;
		m_matrix[ 1 ][ 3 ] = m13;
		m_matrix[ 2 ][ 0 ] = m20;
		m_matrix[ 2 ][ 1 ] = m21;
		m_matrix[ 2 ][ 2 ] = m22;
		m_matrix[ 2 ][ 3 ] = m23;
	}
	matrix3x4_t( const vec3_t& x_axis, const vec3_t& y_axis, const vec3_t& z_axis, const vec3_t& vec_origin )
	{
		init( x_axis, y_axis, z_axis, vec_origin );
	}

	void init( const vec3_t& x_axis, const vec3_t& y_axis, const vec3_t& z_axis, const vec3_t& vec_origin )
	{
		m_matrix[ 0 ][ 0 ] = x_axis.x;
		m_matrix[ 0 ][ 1 ] = y_axis.x;
		m_matrix[ 0 ][ 2 ] = z_axis.x;
		m_matrix[ 0 ][ 3 ] = vec_origin.x;
		m_matrix[ 1 ][ 0 ] = x_axis.y;
		m_matrix[ 1 ][ 1 ] = y_axis.y;
		m_matrix[ 1 ][ 2 ] = z_axis.y;
		m_matrix[ 1 ][ 3 ] = vec_origin.y;
		m_matrix[ 2 ][ 0 ] = x_axis.z;
		m_matrix[ 2 ][ 1 ] = y_axis.z;
		m_matrix[ 2 ][ 2 ] = z_axis.z;
		m_matrix[ 2 ][ 3 ] = vec_origin.z;
	}

	float* operator[]( int i )
	{
		return m_matrix[ i ];
	}

	const float* operator[]( int i ) const
	{
		return m_matrix[ i ];
	}

	float m_matrix[ 3 ][ 4 ];
};

struct FireBulletsInfo_t
{
	int m_iShots;
	vec3_t m_vecSrc;
	vec3_t m_vecDirShooting;
};