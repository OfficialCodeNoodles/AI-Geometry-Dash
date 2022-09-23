#pragma once

//Dependencies
#include "logic/input.hpp"
#include "logic/file.hpp"
#include "logic/particle.hpp"
#include "logic/neuralNet.hpp"
#include "graphics/ui.hpp"

namespace gameEngine {
	extern gs::util::Clock timer; 

	void create(); 
	void close();  

	void updateEvents(); 
	void updateGraphics(); 

	void update(); 

	bool isOpen(); 
}