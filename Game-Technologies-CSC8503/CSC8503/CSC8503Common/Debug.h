#pragma once
#include "../../Plugins/OpenGLRendering/OGLRenderer.h"
#include <vector>
#include <string>

namespace NCL
{
	class Debug
	{
	public:
		static void Print(const std::string& text, const Vector2&pos, const Vector4& colour = Vector4(1, 1, 1, 1));
		static void DrawLine(const Vector3& startpoint, const Vector3& endpoint, const Vector4& colour = Vector4(1, 1, 1, 1));
		static void DrawCircle(const Vector3& position, float radius, const Vector4& colour = Vector4(0.75f, 0.75f, 0.75f, 1));

		static void SetRenderer(OGLRenderer* r) { renderer = r; }

		static void FlushRenderables();

	protected:
		struct DebugStringEntry
		{
			std::string	data;
			Vector2 position;
			Vector4 colour;
		};

		struct DebugLineEntry
		{
			Vector3 start;
			Vector3 end;
			Vector4 colour;
		};

		struct DebugCircleEntry
		{
			Vector3 position;
			float radius;
			Vector3 colour; 
		};

		Debug() = default;
		~Debug() = default;

		static std::vector<DebugStringEntry> stringEntries;
		static std::vector<DebugLineEntry> lineEntries;
		static std::vector<DebugCircleEntry> circleEntries;
		static OGLRenderer* renderer;
	};
}