#pragma once

//Dependencies
#include "../resources.hpp"

namespace engine {
	namespace input {
		enum Key {
			MoveLeft,
			MoveRight,
			MoveUp,
			MoveDown,
			Jump,
			EnableDebugMode,
			ExpandLevelX,
			ExpandLevelY,
			ShrinkLevelX,
			ShrinkLevelY,
			PickTile,
			StartStopSimulation,
			PanToNpcs,
			HideInfo,
			ResetSimulation,
			ChangeFramerate, 
			ChangeQuickStop, 
			HideInfoMenu,
			IncLevel,
			IncHue, 
			End
		};
		extern sf::Keyboard::Key keyMap[Key::End]; 
		extern bool keyLocks[Key::End]; 
		extern bool keySwitches[Key::End]; 
		extern int keyTicks[Key::End]; 

		bool isKeyPressed(Key key); 
		bool isKeyClicked(Key key); 
		int getKeyTicks(Key key);

		void initKeys(); 
		void updateKeys(); 
	}
}