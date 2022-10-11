#include "../../hdr/graphics/render.hpp"

namespace engine {
	namespace render {
		const float backgroundScale = 1.5f; 
		const float backgroundVerticalOffset = -400.0f; 
		const float groundScale = 1.875f; 
		const int tileTextureSize = 64; 
		const int cubeTextureSize = 60; 

		sf::Texture tBackground;
		sf::Sprite sBackground;
		sf::Texture tGround;
		sf::Sprite sGround;
		sf::Texture tTileSheet;
		sf::Sprite sTile;
		sf::Texture tPortals[8];
		sf::Sprite sPortal;
		std::vector<PortalPair> portalPairs;
		sf::Texture tCubeSheet;
		sf::Sprite sCube;
		sf::Texture tShips[2];
		sf::Sprite sShip;
		sf::Texture tJumpOrb;
		sf::Sprite sJumpOrb;
		sf::Texture tGroundHighlight;
		sf::Sprite sGroundHighlight;
		sf::Texture tCrown;
		sf::Sprite sCrown;
		gs::Color globalLevelColor = gs::Color(0, 220, 255);

		void loadAssets() {
			// Loads the background.
			if (tBackground.loadFromFile(mAssetDirectory + "background.png"))
				sBackground.setTexture(tBackground); 
			// Loads the ground. 
			if (tGround.loadFromFile(mAssetDirectory + "ground.png"))
				sGround.setTexture(tGround); 
			// Loads the tile-sheet. 
			if (tTileSheet.loadFromFile(mAssetDirectory + "tileSheet.png"))
				sTile.setTexture(tTileSheet); 
			// Loads the portals. 
			for (int portalIndex = 0; portalIndex < 8; portalIndex++) {
				if (!tPortals[portalIndex].loadFromFile(
					mAssetDirectory + "portal" + std::to_string(portalIndex)
						+ ".png"
				))
					break; 
			}
			// Loads the cube-sheet. 
			if (tCubeSheet.loadFromFile(mAssetDirectory + "newCubeSheet.png"))
				sCube.setTexture(tCubeSheet); 
			// Loads the ship textures. 
			for (int shipIndex = 0; shipIndex < 2; shipIndex++) {
				if (!tShips[shipIndex].loadFromFile(
					mAssetDirectory + "ship" + std::to_string(shipIndex)
						+ ".png"
				))
					break; 
			}
			// Loads the jump-orb texture. 
			if (tJumpOrb.loadFromFile(mAssetDirectory + "jumpOrb.png"))
				sJumpOrb.setTexture(tJumpOrb); 
			// Loads the ground highlight.
			if (tGroundHighlight.loadFromFile(mAssetDirectory + "highlight.png"))
				sGroundHighlight.setTexture(tGroundHighlight);
			// Loads the crown. 
			if (tCrown.loadFromFile(mAssetDirectory + "crown.png"))
				sCrown.setTexture(tCrown); 

			handleAssets(); 
		}
		void handleAssets() {
			// sBackground
			sBackground.setScale(backgroundScale, backgroundScale); 

			// sGround
			sGround.setScale(groundScale, groundScale); 

			// sTile
			sTile.setScale(
				tileSize / static_cast<float>(tileTextureSize),
				tileSize / static_cast<float>(tileTextureSize)
			);

			// sPortal
			sPortal.setOrigin(gs::Vec2f(
				tPortals[0].getSize().x, tPortals[0].getSize().y 
			) * 0.5f);
			sPortal.setScale(sTile.getScale()); 
			sPortal.setRotation(-90.0f); 

			// sJumpOrb
			sJumpOrb.setOrigin(gs::Vec2f(
				tJumpOrb.getSize().x, tJumpOrb.getSize().y
			) * 0.5f); 
			sJumpOrb.setScale(0.5, 0.5f);

			// sCube
			sCube.setScale(
				tileSize / static_cast<float>(cubeTextureSize),
				tileSize / static_cast<float>(cubeTextureSize)
			);

			// sShip
			sShip.setScale(sTile.getScale()); 

			// sGroundHighlight
			sGroundHighlight.setPosition(
				(window::defaultWindowSize.x / 2.0f) - 
					(tGroundHighlight.getSize().x / 2.0f), 0.0f
			);
		}

		const gs::Vec2i clippingRange = gs::Vec2i(11, 8); 

		gs::Vec2f fluidCameraPosition;
		gs::Vec2f cameraPosition;
		gs::Vec2i clippedCameraPosition;
		bool panToFarthestNpc = false; 

		void applyCamera(const Player& player) {
			fluidCameraPosition.x = player.position.x + 2.0f; 

			if (!player.isFlying)
				gs::util::approach(
					&fluidCameraPosition.y, player.position.y, 8.0f
				);
			else 
				gs::util::approach(
					&fluidCameraPosition.y, currentMap->size.y - 5.5f, 8.0f
				);
		}
		void updateCamera() {
			const gs::Vec2f tiledScreenCenter = gs::Vec2f(
				window::screenCenter.x / tileSize,
				window::screenCenter.y / tileSize
			);

			cameraPosition = fluidCameraPosition - tiledScreenCenter; 

			gs::util::clamp(
				&cameraPosition.x, 0.0f, static_cast<float>(currentMap->size.x)
					- (tiledScreenCenter.x * 2.0f) - 1.0f
			); 
			gs::util::clamp(
				&cameraPosition.y, 0.0f, static_cast<float>(currentMap->size.y) 
					- (tiledScreenCenter.y * 2.0f) - 1.0f + 4.0f
			); 

			clippedCameraPosition = gs::Vec2i(
				cameraPosition.x, cameraPosition.y
			); 
			// Centers the rendering at the middle of the screen. 
			clippedCameraPosition += gs::Vec2i(
				tiledScreenCenter.x, tiledScreenCenter.y
			); 
		}
		gs::Vec2f getGlobalPosition(gs::Vec2f localPosition) {
			localPosition -= cameraPosition; 
			localPosition *= tileSize; 
			localPosition = gs::Vec2f(
				std::floor(localPosition.x), 
				std::floor(localPosition.y)
			); 
			return localPosition; 
		}
		gs::Color hsvToRgb(float h, float s, float v) {
			// This function is inspired by this website page. 
			// https://www.codespeedy.com/hsv-to-rgb-in-cpp/

			gs::Color color; 

			h = gs::util::mod(h, 360.0f); 

			s /= 100.0f;
			v /= 100.0f;

			float C = s * v;
			float X = C * (1.0f - abs(std::fmod(h / 60.0, 2.0f) - 1.0f));
			float m = v - C;
			float r, g, b;

			if (h >= 0.0f && h < 60.0f) 
				r = C, g = X, b = 0.0f;
			else if (h >= 60.0f && h < 120.0f) 
				r = X, g = C, b = 0.0f;
			else if (h >= 120.0f && h < 180.0f) 
				r = 0, g = C, b = X;
			else if (h >= 180.0f && h < 240.0f) 
				r = 0, g = X, b = C;
			else if (h >= 240 && h < 300.0f) 
				r = X, g = 0.0f, b = C;
			else 
				r = C, g = 0.0f, b = X;
			
			color.r = (r + m) * 255;
			color.g = (g + m) * 255;
			color.b = (b + m) * 255;

			return color;  
		}
		void updateColor(const Player& player) {
			float hueOffset = 0.0f; 

			if (player.isFlying)
				hueOffset = 45.0f; 

			globalLevelColor = hsvToRgb(
				currentMap->hue + (cameraPosition.x / 4.0f) + hueOffset, 
				100.0f, 100.0f
			);
		}

		void renderBackground(const Map& map) {
			const float backgroundHorizontalOffset = 
				sBackground.getTexture()->getSize().x * backgroundScale;

			for (int backgroundIndex = 0; backgroundIndex < 3;
				backgroundIndex++
			) {
				sBackground.setPosition(
					std::fmod(
						-cameraPosition.x * 8.0f, 
						backgroundHorizontalOffset
					),
					backgroundVerticalOffset
				);
				sBackground.move(
					backgroundHorizontalOffset * backgroundIndex, 0.0f
				);
				sBackground.setColor(globalLevelColor); 

				window::winmain->draw(sBackground);
			}
		}
		void renderGround(const Map& map) {
			const float groundHorizontalOffset = 
				sGround.getTexture()->getSize().x * groundScale; 
			const float groundVerticalOffset = -192.0f; 

			for (int groundIndex = 0; groundIndex < 5; groundIndex++) {
				sGround.setPosition( 
					std::fmod(
						-cameraPosition.x * tileSize,
						groundHorizontalOffset
					),
					groundVerticalOffset
				);
				sGround.move(
					groundHorizontalOffset * groundIndex, 
					(-cameraPosition.y * tileSize) + ((currentMap->size.y + 2) 
						* tileSize)
				);
				sGround.setColor(
					gs::util::approach(globalLevelColor, gs::Color::Black, 10.0f)
				); 

				window::winmain->draw(sGround); 
			}

			sGroundHighlight.setPosition(
				sGroundHighlight.getPosition().x, sGround.getPosition().y
			);

			window::winmain->draw(sGroundHighlight); 
		}
		inline void setTileTexture(int index) {
			// Number of tiles per row in the tile-sheet. 
			const int sheetWidth = 10; 

			sf::IntRect bounds; 

			bounds.width = tileTextureSize; 
			bounds.height = bounds.width;

			// Xpos of the tile in the tile-sheet. 
			int xpos = index % sheetWidth; 
			// Ypos of the tile in the tile-sheet. 
			int ypos = index / sheetWidth;

			// Scales xpos and ypos to be in pixels. 
			bounds.left = (xpos * bounds.width) + xpos; 
			bounds.top = (ypos * bounds.height) + ypos;

			sTile.setTextureRect(bounds); 
		}
		inline void renderTile(gs::Vec2i position, Tile::Id id) {
			// Position of tile to be rendered on the screen. 
			gs::Vec2f transformedPosition = gs::Vec2f(position.x, position.y); 

			transformedPosition = getGlobalPosition(transformedPosition); 
			sTile.setPosition(transformedPosition); 
			sTile.setColor(
				Tile::isColorable(id) ? gs::util::approach(
					globalLevelColor, gs::Color::White, 10.0f) 
				: gs::Color::White
			); 

			if (Tile::getCollisionType(id) == Tile::CollisionType::JumpOrb) {
				sJumpOrb.setPosition(transformedPosition + gs::Vec2f(
					tileSize / 2.0f, tileSize / 2.0f
				)); 
				window::winmain->draw(sJumpOrb); 

				return; 
			}

			const gs::Vec2f originalScale = sTile.getScale(); 

			const int edgeEffectSize = 2; 
			const float rightEdgeOffset = transformedPosition.x - 
				(window::defaultWindowSize.x - tileSize * edgeEffectSize);
			const float leftEdgeOffset = transformedPosition.x - 
				(tileSize * edgeEffectSize);

			if (rightEdgeOffset > 0.0f) 
				sTile.setScale(
					(tileSize + rightEdgeOffset) / tileTextureSize,
					originalScale.y
				);
			else if (leftEdgeOffset < 0.0f)
				sTile.move(leftEdgeOffset, 0.0f); 

			int textureId = Tile::getTextureIndex(id); 

			setTileTexture(textureId); 

			window::winmain->draw(sTile); 

			sTile.setScale(originalScale); 
		}
		inline void renderPortalBack(const PortalPair& portalPair) {
			gs::Vec2f transformedPosition = gs::Vec2f(
				portalPair.position.x, portalPair.position.y
			); 

			transformedPosition = getGlobalPosition(transformedPosition); 
			sPortal.setPosition(transformedPosition); 

			int textureId = Tile::getTextureIndex(portalPair.id) + 1;

			sPortal.setTexture(tPortals[textureId]); 

			window::winmain->draw(sPortal); 
		}
		inline void renderPortalFront(const PortalPair& portalPair) {
			gs::Vec2f transformedPosition = gs::Vec2f(
				portalPair.position.x, portalPair.position.y
			);

			transformedPosition = getGlobalPosition(transformedPosition);
			sPortal.setPosition(transformedPosition);

			int textureId = Tile::getTextureIndex(portalPair.id) ; 

			sPortal.setTexture(tPortals[textureId]);

			window::winmain->draw(sPortal);
		}
		void renderPortalBacks() {
			for (const PortalPair& portalPair : portalPairs) 
				renderPortalBack(portalPair); 
		}
		void renderPortalFronts() {
			for (const PortalPair& portalPair : portalPairs)
				renderPortalFront(portalPair);
		}
		void renderMap(const Map& map) {
			// Constrains the xpos to be within the bounds of the map. 
			auto xClamp = [&](int xpos) {
				return gs::util::clamp(xpos, 0, map.size.x); 
			}; 
			// Constrains the ypos to be within the bounds of the map. 
			auto yClamp = [&](int ypos) { 
				return gs::util::clamp(ypos, 0, map.size.y); 
			};

			portalPairs.clear(); 

			for (int xpos = xClamp(clippedCameraPosition.x - clippingRange.x);
				xpos < xClamp(clippedCameraPosition.x + clippingRange.x); xpos++
			) {
				for (int ypos = yClamp(clippedCameraPosition.y - clippingRange.y);
					ypos < yClamp(clippedCameraPosition.y + clippingRange.y); ypos++
				) {
					const gs::Vec2i tilePosition = gs::Vec2i(xpos, ypos); 
					const Tile::Id tileId = map.getTile(tilePosition); 
					const Tile::CollisionType tileCollision = 
						Tile::getCollisionType(tileId); 

					if (tileId != Tile::Air) {
						if (tileCollision != Tile::CollisionType::Portal) {

							switch (tileCollision) {
							case Tile::CollisionType::JumpPad:
								if (rand() % 2 == 0)
									Particle::spawnParticles(
										gs::Vec2f(tilePosition.x, tilePosition.y) 
											+ gs::Vec2f(0.5f, 0.9f), 
										ParticleInfo::JumpPad, 
										gs::Color::Yellow, 
										3 - (window::renderFramerate / 30)
									); 
								break;
							case Tile::CollisionType::JumpOrb:
								float angle = rand() % 360;
								gs::Vec2f centerOffset = gs::util::polarToCartesian(
									gs::Vec2f(0.5f, angle)
								);

								Particle::spawnParticles(
									gs::Vec2f(tilePosition.x, tilePosition.y) +
										centerOffset + gs::Vec2f(0.5f, 0.5f),
									ParticleInfo::JumpOrb,
									gs::Color::Yellow,
									3 * (3 - (window::renderFramerate / 30)),
									centerOffset * -0.05f
								);

								break; 
							}

							renderTile(tilePosition, tileId);
						}
						else {
							if (rand() % 2 == 0) {
								float angle = 90.0f + (rand() % 180); 
								gs::Vec2f centerOffset = gs::util::polarToCartesian(
									gs::Vec2f(1.5f, angle)
								); 

								Particle::spawnParticles(
									gs::Vec2f(tilePosition.x, tilePosition.y) +
										centerOffset,
									ParticleInfo::Portal,
									gs::Color::White,
									3 - (window::renderFramerate / 30),
									centerOffset * -0.03f
								); 
							}

							PortalPair newPortalPair = { tilePosition, tileId }; 
							portalPairs.push_back(newPortalPair); 
						}
					}
				}
			}

			renderPortalBacks(); 
		}
		inline void setCubeTexture(int index) {
			// Number of cubes per row in the cube-sheet. 
			const int sheetWidth = 10;

			sf::IntRect bounds;

			bounds.width = cubeTextureSize;
			bounds.height = bounds.width;

			// Xpos of the cube in the cube-sheet. 
			int xpos = index % sheetWidth;
			// Ypos of the cube in the cube-sheet. 
			int ypos = index / sheetWidth;

			// Scales xpos and ypos to be in pixels. 
			bounds.left = (xpos * bounds.width) + xpos;
			bounds.top = (ypos * bounds.height) + ypos;

			sCube.setTextureRect(bounds);
		}
		void renderPlayer(const Player& player, bool renderCrown) {
			// Used to color the background of each cube. 
			static sf::RectangleShape cubeBackground; 

			// Only renders players that are actually alive. 
			if (player.respawnTicks > 0)
				return; 

			const gs::Vec2f transformedPosition = getGlobalPosition(player.position); 
			const gs::Vec2f originalScale = sCube.getScale(); 

			sCube.setPosition(transformedPosition); 
			sCube.setOrigin(cubeTextureSize / 2.0f, cubeTextureSize / 2.0f);
			sCube.setRotation(player.angle); 
			sCube.setColor(player.primaryColor); 

			sCrown.setPosition(sCube.getPosition()); 
			sCrown.setScale(sCube.getScale()); 
			sCrown.setOrigin(
				gs::Vec2f(0.0f, sCrown.getTexture()->getSize().y) * 0.5f +
					sCube.getOrigin()
			);
			sCrown.setRotation(sCube.getRotation()); 

			int textureId = player.textureIndex; 

			setCubeTexture(textureId); 

			const float backgroundSizeDifference = 10.0f; 

			cubeBackground.setSize(gs::Vec2f(
				(cubeTextureSize * originalScale.x) - backgroundSizeDifference, 
				(cubeTextureSize * originalScale.y) - backgroundSizeDifference
			)); 
			cubeBackground.setPosition(sCube.getPosition()); 
			cubeBackground.setOrigin(cubeBackground.getSize() * 0.5f); 
			cubeBackground.setRotation(sCube.getRotation()); 
			cubeBackground.setFillColor(player.secondaryColor);

			if (!player.isFlying) {
				window::winmain->draw(cubeBackground);
				window::winmain->draw(sCube);

				if (renderCrown)
					window::winmain->draw(sCrown); 
			}
			else {
				sCube.move(0.0f, 20.0f);

				if (player.isFlipped)
					sCube.move(0.0f, -40.0f); 

				sShip.setPosition(sCube.getPosition());

				sCube.setScale(
					originalScale.x * 0.5f, originalScale.y * 0.5f
						* (player.isFlipped ? -1 : 1)
				);
				sCube.move(
					std::cos(gs::util::toRadians(player.angle - 80.0f)) 
						* 30.0f * (player.isFlipped ? -1 : 1),
					std::sin(gs::util::toRadians(player.angle - 80.0f)) 
						* 30.0f * (player.isFlipped ? -1 : 1)
				);
				sCrown.setPosition(sCube.getPosition()); 
				sCrown.setScale(sCube.getScale()); 

				cubeBackground.setSize(gs::Vec2f(
					cubeTextureSize * sCube.getScale().x,
					cubeTextureSize * sCube.getScale().y
				));
				cubeBackground.setPosition(sCube.getPosition());
				cubeBackground.setOrigin(cubeBackground.getSize() * 0.5f);

				window::winmain->draw(cubeBackground);
				window::winmain->draw(sCube);

				if (renderCrown)
					window::winmain->draw(sCrown); 

				const gs::Vec2f originalScale = sShip.getScale();

				sShip.setScale(
					originalScale.x, originalScale.y
						* (player.isFlipped ? -1 : 1)
				); 
				sShip.setTexture(tShips[0]);
				sShip.setOrigin(
					sShip.getTexture()->getSize().x / 2.0f,
					sShip.getTexture()->getSize().y / 2.0f
				);
				sShip.setColor(player.primaryColor);
				sShip.setRotation(player.angle);

				window::winmain->draw(sShip);

				sShip.setTexture(tShips[1]);
				sShip.setColor(player.secondaryColor);

				window::winmain->draw(sShip);

				sShip.setScale(originalScale); 
			}

			sCube.setScale(originalScale); 
		}
		void renderNpcs() {
			for (int pairIndex = 0; pairIndex < npcs.size(); pairIndex++) {
				const auto& pair = npcs[pairIndex]; 
				renderPlayer(pair.first, pairIndex == bestNpcIndex);
			}
		}
		inline void renderParticle(const Particle& particle) {
			static sf::RectangleShape particleBox; 

			gs::Vec2f transformedPosition =
				getGlobalPosition(particle.position);

			particleBox.setSize(gs::Vec2f(particle.size, particle.size)); 
			particleBox.setOrigin(
				particleBox.getSize().x / 2.0f, particleBox.getSize().y / 2.0f
			);
			particleBox.setPosition(transformedPosition); 
			particleBox.setRotation(particle.angle); 
			particleBox.setFillColor(particle.color); 

			window::winmain->draw(particleBox); 
		}
		void renderParticles() {
			for (const Particle& particle : particles) {
				if (particle.ticks > 0)
					renderParticle(particle); 
			}
		}
		void renderLine(
			gs::Vec2f p1, gs::Vec2f p2, float thickness, gs::Color fillColor
		) {
			static sf::RectangleShape line; 

			line.setSize(gs::Vec2f(
				gs::util::distance(p1, p2), thickness
			));
			line.setOrigin(line.getSize() * 0.5f); 
			line.setPosition((p1.x + p2.x) / 2.0f, (p1.y + p2.y) / 2.0f);
			line.setRotation(gs::util::angleBetween(p1, p2)); 
			line.setFillColor(fillColor); 

			window::winmain->draw(line); 
		}
	}
}