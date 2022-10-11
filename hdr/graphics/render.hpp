#pragma once

//Dependencies
#include "window.hpp"
#include "../logic/map.hpp"
#include "../logic/player.hpp"
#include "../logic/particle.hpp"
#include "../logic/neuralNet.hpp"

namespace engine {
	namespace render {
		struct PortalPair {
			gs::Vec2i position; 
			Tile::Id id; 
		};
		
		extern const float backgroundScale;
		extern const float backgroundVerticalOffset;
		extern const float groundScale; 
		extern const int tileTextureSize; 
		extern const int cubeTextureSize; 

		extern sf::Texture tBackground; 
		extern sf::Sprite sBackground; 
		extern sf::Texture tGround; 
		extern sf::Sprite sGround; 
		extern sf::Texture tTileSheet; 
		extern sf::Sprite sTile;
		extern sf::Texture tPortals[8]; 
		extern sf::Sprite sPortal; 
		extern std::vector<PortalPair> portalPairs; 
		extern sf::Texture tCubeSheet; 
		extern sf::Sprite sCube; 
		extern sf::Texture tShips[2]; 
		extern sf::Sprite sShip;
		extern sf::Texture tJumpOrb; 
		extern sf::Sprite sJumpOrb; 
		extern sf::Texture tGroundHighlight; 
		extern sf::Sprite sGroundHighlight;
		extern sf::Texture tCrown; 
		extern sf::Sprite sCrown; 
		extern gs::Color globalLevelColor;

		void loadAssets(); 
		void handleAssets();

		constexpr float tileSize = 96.0f; 
		extern const gs::Vec2i clippingRange; 

		extern gs::Vec2f fluidCameraPosition; 
		extern gs::Vec2f cameraPosition; 
		extern gs::Vec2i clippedCameraPosition; 
		extern bool panToFarthestNpc; 

		void applyCamera(const Player& player); 
		void updateCamera(); 
		gs::Vec2f getGlobalPosition(gs::Vec2f localPosition); 
		gs::Color hsvToRgb(float h, float s, float v); 
		void updateColor(const Player& player); 

		void renderBackground(const Map& map);
		void renderGround(const Map& map); 
		inline void setTileTexture(int index); 
		inline void renderTile(gs::Vec2i position, Tile::Id id); 
		inline void renderPortalBack(const PortalPair& portalPair); 
		inline void renderPortalFront(const PortalPair& portalPair);
		void renderPortalBacks(); 
		void renderPortalFronts(); 
		void renderMap(const Map& map); 
		inline void setCubeTexture(int index); 
		void renderPlayer(const Player& player, bool renderCrown = false); 
		void renderNpcs(); 
		inline void renderParticle(const Particle& particle); 
		void renderParticles(); 
		void renderLine(
			gs::Vec2f p1, gs::Vec2f p2, float thickness = 5.0f,
			gs::Color fillColor = gs::Color::Black
		); 
	}
}