#include "../../Common/Window.h"

#include "../CSC8503Common/StateMachine.h"
#include "../CSC8503Common/StateTransition.h"
#include "../CSC8503Common/State.h"

#include "../CSC8503Common/GameServer.h"
#include "../CSC8503Common/GameClient.h"

#include "../CSC8503Common/NavigationGrid.h"

#include "GooseGame.h"
#include "NetworkedGame.h"

using namespace NCL;
using namespace CSC8503;

void TestStateMachine()
{
	// setup state machine 
	StateMachine* testStateMachine = new StateMachine();

	// setup data to be used within state machine
	int data = 0;

	// setup state specific update functions 
	StateFunction stateFunctionA = [](void* data)
	{
		int* realData = static_cast<int*>(data);
		(*realData)++;
		std::cout << "Current State: A\n";
	};

	StateFunction stateFunctionB = [](void* data)
	{
		int* realData = static_cast<int*>(data);
		(*realData)--;
		std::cout << "Current State: B\n";
	};

	GenericState* stateA = new GenericState(stateFunctionA, static_cast<void*>(&data));
	GenericState* stateB = new GenericState(stateFunctionB, static_cast<void*>(&data));

	// add states to state machine 
	testStateMachine->AddState(stateA);
	testStateMachine->AddState(stateB);

	// setup transitions between states created above

	// 1. transition to go from state A to state B 
	GenericTransition<int&, int>* transitionA = new GenericTransition<int&, int>(GenericTransition<int&, int>::GreaterThanTransition, data, 10, stateA, stateB);

	// 2. transition to go from state B to state A
	GenericTransition<int&, int>* transitionB = new GenericTransition<int&, int>(GenericTransition<int&, int>::EqualsTransition, data, 0, stateB, stateA);

	// add transitions to state machine
	testStateMachine->AddTransition(transitionA);
	testStateMachine->AddTransition(transitionB);

	for (int i = 0; i < 100; i++)
	{
		testStateMachine->Update();
	}

	delete testStateMachine;
}

void TestNetworking() {
}

vector<Vector3> testNodes;

void TestPathfinding() {

}

void DisplayPathfinding() {

}



/*

The main function should look pretty familar to you!
We make a window, and then go into a while loop that repeatedly
runs our 'game' until we press escape. Instead of making a 'renderer'
and updating it, we instead make a whole game, and repeatedly update that,
instead. 

This time, we've added some extra functionality to the window class - we can
hide or show the 

*/
int main()
{
	Window*w = Window::CreateGameWindow("CSC8503 Game technology!", 1920, 1080);

	if (!w->HasInitialised()) 
		return -1;
	
	TestStateMachine(); // this funciton displays the outcome of the simple state machine initialized in this file (located above)
	//TestNetworking();
	//TestPathfinding();
	
	w->ShowOSPointer(false);
	w->LockMouseToWindow(true);

	GooseGame* g = new GooseGame();

	while (w->UpdateWindow() && !Window::GetKeyboard()->KeyDown(KeyboardKeys::ESCAPE)) 
	{
		const float dt = w->GetTimer()->GetTimeDeltaSeconds();

		if (dt > 1.0f) 
		{
			std::cout << "Skipping large time delta\n";
			continue; //must have hit a breakpoint or something to have a 1 second frame time!
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::PRIOR)) 
		{
			w->ShowConsole(true);
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NEXT)) 
		{
			w->ShowConsole(false);
		}

		DisplayPathfinding();

		w->SetTitle("CSC8503 | Physics | Networking | AI | FPS:" + std::to_string(1.0f / dt));

		g->UpdateGame(dt);

	}
	Window::DestroyGameWindow();
	delete g; 
	
	return 0;
}