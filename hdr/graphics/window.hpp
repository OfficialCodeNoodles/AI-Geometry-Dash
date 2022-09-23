#pragma once

//Dependencies
#include "../resources.hpp"

namespace engine {
	namespace render {
		namespace window {
			extern const gs::Vec2i defaultWindowSize;
			extern const gs::Vec2f screenCenter; 
			constexpr bool defaultFullscreen = false;

			extern sf::RenderWindow* winmain; 
			extern bool isFullscreen; 

			extern int eventFramerate; 
			extern int renderFramerate; 

			extern int mouseDelta; 
			extern sf::Image icon;

			void create(bool fullscreen = defaultFullscreen); 
			void handleStates(); 
			void update(); 
			void close();

			void begin(gs::Color backgroundColor = gs::Color()); 
			void end(); 
		}
	}
}