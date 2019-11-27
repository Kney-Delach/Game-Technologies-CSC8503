/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#pragma once
#include <algorithm>

namespace NCL {
	namespace Maths {
		class Vector2;
		class Vector3;

		//It's pi(ish)...
		static const float		PI = 3.14159265358979323846f;

		//It's pi...divided by 360.0f!
		static const float		PI_OVER_360 = PI / 360.0f;

		//Radians to degrees
		inline float RadiansToDegrees(float rads) {
			return rads * 180.0f / PI;
		};

		//Degrees to radians
		inline float DegreesToRadians(float degs) {
			return degs * PI / 180.0f;
		};

		template<class T>
		inline float Clamp(T value, T min, T max) {
			if (value < min) {
				return min;
			}
			if (value > max) {
				return max;
			}
			return value;
		}

		Vector3 Clamp(const Vector3& a, const Vector3&mins, const Vector3& maxs);

		template<class T>
		inline T Lerp(const T& a, const T&b, float by) {
			return (a * (1.0f - by) + b*by);
		}

		void ScreenBoxOfTri(const Vector3& v0, const Vector3& v1, const Vector3& v2, Vector2& topLeft, Vector2& bottomRight);

		int ScreenAreaOfTri(const Vector3 &a, const Vector3 &b, const Vector3 & c);
		float FloatAreaOfTri(const Vector3 &a, const Vector3 &b, const Vector3 & c);

		float CrossAreaOfTri(const Vector3 &a, const Vector3 &b, const Vector3 & c);
	}
}