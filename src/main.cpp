#include "../hdr/gameEngine.hpp"

int main() {
	gameEngine::create(); 

	while (gameEngine::isOpen()) 
		gameEngine::update(); 

	gameEngine::close();

	return 0;
}