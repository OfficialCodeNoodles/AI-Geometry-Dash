#pragma once

//Dependencies
#include "../resources.hpp"

namespace engine {
	class Player {
	public:
		gs::Vec2f position; 
		gs::Vec2f velocity; 
		float angle; 
		// Which texture to render the player as. 
		int textureIndex; 
		// Time left until a respawn after dying. 
		int respawnTicks;
		bool isFlying; 
		bool isFlipped; 
		// The coordinate of the last portal that was entered. This is used to 
		// prevent a player from immidiatly re-entering a portal. 
		gs::Vec2i lastPortalEntry; 
		bool onGround; 
		bool onCeiling; 
		gs::Color primaryColor, 
			secondaryColor; 
		float primaryHue,
			secondaryHue; 
		// True if being controlled automatically. 
		bool isNpc; 
		// Prevents the player from generating particles when testing for
		// unsafe routes. 
		bool isClone; 
		// Used in place of a jump button for the npcs. 
		bool jump; 

		Player(); 

		void spawn(); 
		void update(); 
	private:
		void updatePosition();
	};

	// The controllable player. 
	extern Player player;
	extern bool debugMode;
}