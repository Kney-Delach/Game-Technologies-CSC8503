#pragma once
#include <vector>
#include <string>

namespace NCL
{
	namespace CSC8503
	{
		class Leaderboard
		{
		public:
			Leaderboard() = default;
			~Leaderboard();
		private:
			std::vector<std::string> playerNames;
			std::vector<int> playerScores;
		};
	}
}
