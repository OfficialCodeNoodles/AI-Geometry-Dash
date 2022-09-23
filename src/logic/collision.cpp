#include "../../hdr/logic/collision.hpp"
#include "../../hdr/logic/map.hpp"
#include "../../hdr/logic/particle.hpp"

namespace engine {
	namespace collision {
		const float unit = 1.0f, 
			halfUnit = 0.5f;

		bool ground(const Player& player, gs::Vec2i position) {
			const gs::Vec2f floatPosition = gs::Vec2f(position.x, position.y);

			if (player.position.x + halfUnit >= floatPosition.x &&
				player.position.x - halfUnit <= floatPosition.x + unit
			) {
				if (player.position.y + halfUnit >= floatPosition.y &&
					player.position.y <= floatPosition.y
				) 
					return true; 
			}

			return false; 
		}
		bool ceiling(const Player& player, gs::Vec2i position) {
			const gs::Vec2f floatPosition = gs::Vec2f(position.x, position.y);

			if (player.position.x + halfUnit >= floatPosition.x &&
				player.position.x - halfUnit <= floatPosition.x + unit
			) {
				if (player.position.y - halfUnit <= floatPosition.y + unit &&
					player.position.y >= floatPosition.y + halfUnit
				) 
					return true;
			}

			return false;
		}
		bool solid(const Player& player, gs::Vec2i position, float tolerance) {
			const gs::Vec2f floatPosition = gs::Vec2f(position.x, position.y); 
			
			if (player.position.x + halfUnit >= floatPosition.x + tolerance &&
				player.position.x - halfUnit <= floatPosition.x + unit - tolerance
			) {
				if (player.position.y + halfUnit >= floatPosition.y + tolerance &&
					player.position.y - halfUnit <= floatPosition.y + unit - tolerance
				)
					return true; 
			}

			return false; 
		}
		bool portal(const Player& player, gs::Vec2i position) {
			const gs::Vec2f floatPosition = gs::Vec2f(position.x, position.y); 
			bool collide = false; 
			
			for (int ypos = position.y - 1; ypos < position.y + 2; ypos++) 
				collide = collide || solid(
					player, gs::Vec2i(position.x, ypos), 0.0f
				); 

			return collide; 
		}

		gs::Vec2i getPlayerTile(const Player& player) {
			return gs::Vec2i(
				player.position.x, player.position.y + unit
			);
		}

		void applyCollision(Player& player) {
			const gs::Vec2i playerTile = getPlayerTile(player); 
			// Tile location of base ground. Note: Its ypos is always at the 
			// bottom of the map. 
			const gs::Vec2i groundLocation = gs::Vec2i(
				playerTile.x, currentMap->size.y - 1
			);

			// Resets flags. 
			player.onGround = false; 
			player.onCeiling = false; 

			// Applies base ground collision. 
			if (ground(player, groundLocation)) {
				player.position.y = playerTile.y - halfUnit; 
				player.velocity.y = 0.0f; 
				player.onGround = true;
			}

			for (int xpos = playerTile.x - 1; xpos <= playerTile.x + 1; xpos++) {
				for (int ypos = playerTile.y - 2; ypos <= playerTile.y + 1;
					ypos++
				) {
					const gs::Vec2i currentTilePosition = gs::Vec2i(xpos, ypos);
					const gs::Vec2i	aboveCurrentTilePosition = gs::Vec2i(xpos, ypos - 1); 

					if (currentMap->isValidTilePosition(currentTilePosition)) {
						const Tile::Id currentTileId = currentMap->getTile(
							currentTilePosition
						);

						switch (Tile::getCollisionType(currentTileId)) {
						case Tile::CollisionType::Empty:
							break;
						case Tile::CollisionType::Solid:
							if (currentMap->isValidTilePosition(aboveCurrentTilePosition))
								if (Tile::getCollisionType(currentMap->getTile(
									aboveCurrentTilePosition
								)) == Tile::CollisionType::Solid)
									goto GROUND_SKIP; 

							if (ground(player, currentTilePosition) &&
								player.velocity.y > 0.0f
							) {
								player.position.y = currentTilePosition.y
									- halfUnit;
								player.velocity.y = 0.0f;
								player.onGround = true; 
							}
GROUND_SKIP:
							if (player.isFlying) {
								if (ceiling(player, currentTilePosition) &&
									player.velocity.y < 0.0f
								) {
									player.position.y = currentTilePosition.y
										+ unit + halfUnit;
									player.velocity.y = 0.0f;
									player.onCeiling = true; 
								}
							}
							if (solid(player, currentTilePosition, 0.1f)) {
								player.respawnTicks = 60;

								if (!player.isClone)
									Particle::spawnGroundHitParticles(
										player.position, player.secondaryColor, 30
									);
							}
							break;
						case Tile::CollisionType::Spike:
							if (solid(player, currentTilePosition, 0.35f)) {
								player.respawnTicks = 60;

								if (!player.isClone)
									Particle::spawnGroundHitParticles(
										player.position, player.secondaryColor, 30
									);
							}
							break;
						case Tile::CollisionType::Semisolid:
							if (ground(player, currentTilePosition) &&
								player.velocity.y > 0.0f
							) {
								player.position.y = currentTilePosition.y
									- halfUnit;
								player.velocity.y = 0.0f;
								player.onGround = true; 
							}
							break;
						case Tile::CollisionType::Portal:
							if (portal(player, currentTilePosition) &&
								currentTilePosition != player.lastPortalEntry
							) {
								player.lastPortalEntry = currentTilePosition; 
								player.isFlying = !player.isFlying; 
							}

							break; 
						}
					}
				}
			}
		}
	}
}