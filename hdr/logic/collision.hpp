#pragma once

//Dependencies
#include "player.hpp"

namespace engine {
	namespace collision {
		extern const float unit, halfUnit; 

		bool ground(const Player& player, gs::Vec2i position);
		bool ceiling(const Player& player, gs::Vec2i position); 
		bool solid(
			const Player& player, gs::Vec2i position, float tolerance = 0.05f
		); 
		// Detects if a player intersects with a portal tile. 
		bool portal(const Player& player, gs::Vec2i position); 

		// Converts a players position to a integet tile position. 
		gs::Vec2i getPlayerTile(const Player& player); 

		void applyCollision(Player& player);
	}
}