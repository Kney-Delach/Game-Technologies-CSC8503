#include "../../Common/Window.h"

#include "../CSC8503Common/StateMachine.h"
#include "../CSC8503Common/StateTransition.h"
#include "../CSC8503Common/State.h"

#include "../CSC8503Common/GameServer.h"
#include "../CSC8503Common/GameClient.h"

#include "../CSC8503Common/NavigationGrid.h"

#include "GooseGame.h"
#include "NetworkedGame.h"
#include "StateManager.h"

#include "TestPacketReceiver.h"

using namespace NCL;
using namespace CSC8503;

void TestNetworking()
{
	NetworkBase::Initialise();
	TestPacketReceiver serverReceiver(" Server ");
	TestPacketReceiver clientReceiver(" Client ");
	int port = NetworkBase::GetDefaultPort();
	GameServer * server = new GameServer(port, 1);
	GameClient * client = new GameClient();
	server->RegisterPacketHandler(String_Message, &serverReceiver);
	client->RegisterPacketHandler(String_Message, &clientReceiver);
	bool canConnect = client-> Connect(127, 0, 0, 1, port);
	for (int i = 0; i < 100; ++i) 
	{
		server->SendGlobalPacket(StringPacket(" Server says hello ! " + std::to_string(i)));
		client->SendPacket(StringPacket(" Client says hello ! " + std::to_string(i)));
		server->UpdateServer();
		client->UpdateClient();
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	NetworkBase::Destroy();
}

StateManager* stateManager;
GameWorld* world;
OGLRenderer* renderer;

//todo: abstract renderer
int main()
{
	//TestNetworking();
	// window stuff
	Window*w = Window::CreateGameWindow("CSC8503 Game technology!", 1920, 1080);
	if (!w->HasInitialised()) 
		return -1;
	w->ShowOSPointer(false);
	w->LockMouseToWindow(true);

	// debug renderer
	world = new GameWorld();
	renderer = new GameTechRenderer(*world);
	Debug::SetRenderer(renderer);

	// game stuff
	stateManager = new StateManager(renderer, 6);
	stateManager->Update(0);
	stateManager->Update(0);
	while (w->UpdateWindow() && !(stateManager->GetStateStack().empty()))
	{
		// windowing stuff
		const float dt = w->GetTimer()->GetTimeDeltaSeconds();
		if (dt > 1.0f) 
		{
			std::cout << "Skipping large time delta\n";
			continue;
		}

		// console stuff
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::PRIOR)) 
		{
			w->ShowConsole(true);
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NEXT)) 
		{
			w->ShowConsole(false);
		}

		stateManager->Update(dt);

		if(!(stateManager->currentState == 1 || stateManager->currentState >=4))
		{
			Debug::FlushRenderables();
			renderer->Update(dt);
			renderer->Render();
		}
		w->SetTitle("CSC8503 | Physics | Networking | AI | FPS:" + std::to_string(1.0f / dt));
	}
	delete world;
	delete renderer;
	delete stateManager;
	Window::DestroyGameWindow();
	
	return 0;
}