#include "../../hdr/logic/input.hpp"

namespace engine {
	namespace input {
		sf::Keyboard::Key keyMap[Key::End] = {
			sf::Keyboard::A,
			sf::Keyboard::D, 
			sf::Keyboard::W, 
			sf::Keyboard::S, 
			sf::Keyboard::Space,
			sf::Keyboard::Q,
			sf::Keyboard::Right, 
			sf::Keyboard::Down, 
			sf::Keyboard::Left, 
			sf::Keyboard::Up, 
			sf::Keyboard::R,
			sf::Keyboard::F1, 
			sf::Keyboard::F2, 
			sf::Keyboard::F3, 
			sf::Keyboard::F4, 
			sf::Keyboard::F5, 
			sf::Keyboard::F6, 
			sf::Keyboard::H,
			sf::Keyboard::Enter, 
			sf::Keyboard::Equal 
		}; 
		bool keyLocks[Key::End];
		bool keySwitches[Key::End];
		int keyTicks[Key::End];

		bool isKeyPressed(Key key) {
			return sf::Keyboard::isKeyPressed(keyMap[key]); 
		}
		bool isKeyClicked(Key key) {
			return keySwitches[key]; 
		}
		int getKeyTicks(Key key) {
			return keyTicks[key]; 
		}

		void initKeys() {
			for (int keyIndex = 0; keyIndex < Key::End; keyIndex++) {
				keyLocks[keyIndex] = false; 
				keySwitches[keyIndex] = false; 
				keyTicks[keyIndex] = 0; 
			}
		}
		void updateKeys() {
			for (int keyIndex = 0; keyIndex < Key::End; keyIndex++) {
				const sf::Keyboard::Key currentKey = keyMap[keyIndex]; 
				const bool keyPressed = sf::Keyboard::isKeyPressed(currentKey); 

				keySwitches[keyIndex] = !keyLocks[keyIndex] && keyPressed;
				keyLocks[keyIndex] = keyPressed; 

				if (!keyLocks[keyIndex])
					keyTicks[keyIndex] = 0;
				else
					keyTicks[keyIndex]++; 
			}
		}
	}
}