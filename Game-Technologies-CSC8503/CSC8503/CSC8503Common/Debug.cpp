#include "Debug.h"

using namespace NCL;

OGLRenderer* Debug::renderer = nullptr;

std::vector<Debug::DebugStringEntry> Debug::stringEntries;
std::vector<Debug::DebugLineEntry> Debug::lineEntries;
std::vector<Debug::DebugCircleEntry> Debug::circleEntries;


void Debug::Print(const std::string& text, const Vector2&pos, const Vector4& colour)
{
	DebugStringEntry newEntry;

	newEntry.data		= text;
	newEntry.position	= pos;
	newEntry.colour		= colour;

	stringEntries.emplace_back(newEntry);
}

void Debug::DrawLine(const Vector3& startpoint, const Vector3& endpoint, const Vector4& colour)
{
	DebugLineEntry newEntry;

	newEntry.start	= startpoint;
	newEntry.end	= endpoint;
	newEntry.colour = colour;

	lineEntries.emplace_back(newEntry);
}

void Debug::DrawCircle(const Vector3& position, float radius, const Vector4& colour)
{
	DebugCircleEntry newEntry;

	newEntry.position = position;
	newEntry.radius = radius;
	newEntry.colour = colour;
	
	circleEntries.emplace_back(newEntry);
}

void Debug::FlushRenderables()
{
	if (!renderer) 
	{
		return;
	}
	for (const auto& i : stringEntries) 
	{
		renderer->DrawString(i.data, i.position);
	}

	for (const auto& i : lineEntries) 
	{
		renderer->DrawLine(i.start, i.end, i.colour);
	}

	for (const auto& i : circleEntries)
	{
		renderer->DrawCircle(i.position, i.radius, i.colour);
	}
	stringEntries.clear();
	lineEntries.clear();
	circleEntries.clear();
}