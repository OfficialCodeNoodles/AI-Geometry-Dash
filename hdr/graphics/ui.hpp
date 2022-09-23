#pragma once

//Dependencies
#include "render.hpp"

namespace engine {
	namespace render {
		namespace ui {
			extern sf::Font font; 
			extern sf::Text text; 
			extern sf::Texture tVignette; 
			extern sf::Sprite sVignette; 

			void loadAssets(); 
			void handleAssets(); 

			// Id of tile used for the editing mode. 
			extern Tile::Id mouseTile; 
			// The location on the grid of mouse. 
			extern gs::Vec2i mouseTileLocation; 
			extern bool displayInfoMenu; 
			extern bool displayGeneration; 
			extern bool simulationRunning; 

			void updateUI(); 
			void updateDebugMenu(const Player& player); 

			void renderText(
				const std::string& string, gs::Vec2f position,
				float scale = 1.0f, gs::Color fillColor = gs::Color::White,
				float outlineThickness = 0.0f, 
				gs::Color outlineColor = gs::Color()
			);
			void renderUI(); 
			// Makes the edges of the screen slightly darker. 
			void renderVignette(); 
			void renderDebugMenu(); 
			// Displays a visualization of a neural-net in the top left corner. 
			void renderNeuralNet(const NeuralNet& neuralNet);
		}
	}
}