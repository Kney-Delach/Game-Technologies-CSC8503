#pragma once
#include <string>
#include <iostream>
#include "../CSC8503Common/NetworkBase.h"

using namespace std;

namespace NCL
{
	namespace CSC8503
	{
		class TestPacketReceiver : public PacketReceiver
		{
		public:
			TestPacketReceiver(string name) { this->name = name; }

			void ReceivePacket(int type, GamePacket * payload, int source)
			{
				if (type == String_Message) 
				{
					StringPacket * realPacket = (StringPacket*)payload;
					string msg = realPacket->GetStringFromData();
					std::cout << name << " received message : " << msg << "\n";
				}
			}
		protected:
			std::string name;
		};
	}
}