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
				playerTile.x, currentMap->size.y
			);

			// Resets flags. 
			player.onGround = false; 
			player.onCeiling = false; 

			// Applies base ground collision. 
			if (ground(player, groundLocation) && player.velocity.y > 0.0f) {
				player.position.y = playerTile.y - halfUnit;

				if (!player.isFlying)
					player.velocity.y = 0.0f;
				
				player.onGround = player.onGround || !player.isFlipped;
				player.onCeiling = player.onCeiling || player.isFlipped;
			}

			for (int xpos = playerTile.x - 1; xpos <= playerTile.x + 1; xpos++) {
				for (int ypos = playerTile.y - 2; ypos <= playerTile.y + 1;
					ypos++
				) {
					const gs::Vec2i currentTilePosition = gs::Vec2i(xpos, ypos);
					const gs::Vec2i	aboveCurrentTilePosition = gs::Vec2i(
						xpos, ypos + (player.isFlipped ? 1 : -1)
					); 

					if (currentMap->isValidTilePosition(currentTilePosition)) {
						const Tile::Id currentTileId = currentMap->getTile(
							currentTilePosition
						);

						switch (Tile::getCollisionType(currentTileId)) {
						case Tile::CollisionType::Solid:
							// Makes sure that tiles with solid vertical 
							// neighbors don't have their collisions run.
							if (currentMap->isValidTilePosition(aboveCurrentTilePosition))
								if (Tile::getCollisionType(currentMap->getTile(
									aboveCurrentTilePosition
								)) == Tile::CollisionType::Solid)
									goto GROUND_SKIP; 

							if (!player.isFlipped) {
								if (ground(player, currentTilePosition) &&
									player.velocity.y > 0.0f
								) {
									player.position.y = currentTilePosition.y
										- halfUnit;
									player.velocity.y = 0.0f;
									player.onGround = true;
								}
							}
							else {
								if (ceiling(player, currentTilePosition) &&
									player.velocity.y < 0.0f
								) {
									player.position.y = currentTilePosition.y
										+ unit + halfUnit;
									player.velocity.y = 0.0f;
									player.onGround = true;
								}
							}
GROUND_SKIP:
							if (player.isFlying) {
								if (!player.isFlipped) {
									if (ceiling(player, currentTilePosition) &&
										player.velocity.y < 0.0f
									) {
										player.position.y = currentTilePosition.y
											+ unit + halfUnit;
										player.velocity.y = 0.0f;
										player.onCeiling = true;
									}
								}
								else {
									if (ground(player, currentTilePosition) &&
										player.velocity.y > 0.0f
									) {
										player.position.y = currentTilePosition.y
											- halfUnit;
										player.velocity.y = 0.0f;
										player.onCeiling = true; 
									}
								}
							}

							if (solid(player, currentTilePosition, 0.1f)) {
								player.respawnTicks = 60;

								if (!player.isClone) {
									Particle::spawnParticles(
										player.position, ParticleInfo::GroundHit,
										player.secondaryColor, 30
									);
									attempt++;
								}
							}
							break;
						case Tile::CollisionType::Spike:
							if (solid(player, currentTilePosition, 0.4f)) {
								player.respawnTicks = 60;

								if (!player.isClone) {
									Particle::spawnParticles(
										player.position, ParticleInfo::GroundHit,
										player.secondaryColor, 30
									);
									attempt++;
								}
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
						case Tile::CollisionType::FlippedSemiSolid:
							if (ceiling(player, currentTilePosition) &&
								player.velocity.y < 0.0f
							) {
								player.position.y = currentTilePosition.y
									+ unit + halfUnit;
								player.velocity.y = 0.0f;
								 
								if (!player.isFlipped)
									player.onCeiling = true;
								else
									player.onGround = true; 
							}
							break; 
						case Tile::CollisionType::Portal:
							if (portal(player, currentTilePosition) &&
								currentTilePosition != player.lastPortalEntry
							) {
								switch (Tile::getActionType(currentTileId)) {
								case Tile::ActionType::Flying:
									player.lastPortalEntry = currentTilePosition;
									player.isFlying = !player.isFlying;
									break; 
								case Tile::ActionType::FlipGravity:
									player.lastPortalEntry = currentTilePosition;
									player.isFlipped = !player.isFlipped; 
									break; 
								}
							}

							break;
						case Tile::CollisionType::JumpPad:
							if (solid(player, currentTilePosition, 0.35f) &&
								player.velocity.y >= 0.0f && !player.isFlying
							) {
								player.velocity.y = -0.43f; 
								player.position.y += player.velocity.y; 
							}

							break; 
						case Tile::CollisionType::JumpOrb:
							if (solid(player, currentTilePosition, -0.5f) &&
								player.jump && !player.isFlying
							) 
								player.velocity.y = -0.34f; 

							break; 
						}
					}
				}
			}
		}
	}
}