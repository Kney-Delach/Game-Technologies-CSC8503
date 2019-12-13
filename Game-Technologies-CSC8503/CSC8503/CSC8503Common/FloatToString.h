#pragma once

#include <sstream>

namespace NCL
{
	namespace CSC8503
	{
		template <typename T>
		std::string FloatToString(const T a_value, const int n = 2)
		{
			std::ostringstream out;
			out.precision(n);
			out << std::fixed << a_value;
			return out.str();
		}
	}
}