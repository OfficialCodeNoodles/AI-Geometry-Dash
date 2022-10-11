#include "../../hdr/logic/player.hpp"
#include "../../hdr/logic/input.hpp"
#include "../../hdr/logic/collision.hpp"
#include "../../hdr/logic/map.hpp"
#include "../../hdr/logic/particle.hpp"
#include "../../hdr/graphics/render.hpp"

namespace engine {
	Player::Player() : angle(0.0f), textureIndex(0), respawnTicks(0), 
		isFlying(false), isFlipped(false), onGround(false), onCeiling(false), 
		isNpc(false), isClone(false), jump(false) {
	}

	void Player::spawn() {
		position = gs::Vec2f(-1.0f, currentMap->size.y-1.5f); 
		velocity = gs::Vec2f(); 
		angle = 0.0f; 
		respawnTicks = 0; 
		lastPortalEntry = gs::Vec2i(-1, -1); 
		isFlying = false; 
		isFlipped = false; 

		if (attempt > 0)
			position.x += 9.0f; 
	}
	void Player::update() {
#ifdef mDebug
		if (debugMode && !isNpc) {
			const float speed = 0.5f;

			if (input::isKeyPressed(input::MoveLeft)) 
				position.x -= speed;
			else if (input::isKeyPressed(input::MoveRight)) 
				position.x += speed;

			if (input::isKeyPressed(input::MoveUp)) 
				position.y -= speed;
			else if (input::isKeyPressed(input::MoveDown)) 
				position.y += speed;

			velocity.y = 0.0f; 
			angle = 0.0f; 
			respawnTicks = 0; 

			return; 
		}
#endif

		if (respawnTicks > 0) {
			respawnTicks--;

			// Respawns player at the start once the respawn timer runs out. 
			if (!isNpc && respawnTicks == 1) {
				spawn();
				attempt++;
			}
			else if (isNpc)
				respawnTicks = std::max(respawnTicks, 1);

			return; 
		}

		const float forwardSpeed = 0.17f; 
		const bool prvsOnGround = onGround; 

		velocity.x = forwardSpeed;

#ifdef mDebug
		// Holding left will freeze the player in place for debugging purposes. 
		if (!isNpc && input::isKeyPressed(input::MoveLeft)) 
			position.x -= forwardSpeed;
#endif

		if (!isNpc) 
			jump = (input::isKeyPressed(input::Jump) && (onGround || isFlying)) ||
				(input::isKeyClicked(input::Jump) && !onGround);

		// Standard movement. 
		if (!isFlying) {
			// Can jump when key is pressed and is on the ground. 
			if (jump && onGround)
				velocity.y = -0.34f * (isFlipped ? -1 : 1);

			// Applies gravity. 
			velocity.y += 0.0255f * (isFlipped ? -1 : 1);
			// Limits vertical velocity (terminal velocity). 
			if (!isFlipped)
				velocity.y = std::min(velocity.y, 0.3f);
			else
				velocity.y = std::max(velocity.y, -0.3f); 

			updatePosition();
			collision::applyCollision(*this);

			// Rotates player while in the air. 
			if (velocity.y != 0.0f)
				angle += 6.25f;
			// Flattens the angle out once on the ground. 
			else
				gs::util::approach(
					&angle, std::round(angle / 90.0f) * 90.0f, 50.0f
				);

			// Leaves a trail of particles while the player is on the ground. 
			if (!isClone && onGround && ticks % 2 == 0)
				Particle::spawnParticles(
					position + gs::Vec2f(-0.3f, 0.3f), ParticleInfo::Movement,
					secondaryColor, 1
				);
		}
		// Flying movement. 
		else {
			// Rate in which the ship turns. 
			const float angleAdjustSpeed = 3.0f * (isFlipped ? -1 : 1);

			if (jump) {
				if (!isClone && ticks % 2 == 0)
					Particle::spawnParticles(
						position + gs::Vec2f(
							std::cos(gs::util::toRadians(angle + 140.0f)) * 0.5f, 
							std::sin(gs::util::toRadians(angle + 140.0f)) * 0.5f
						) + gs::Vec2f(0.0f, isFlipped ? -0.6f : 0.0f), 
						ParticleInfo::Movement, secondaryColor, 1
					);
				
				if (onCeiling)
					goto FLATTEN_SHIP; 

				angle -= angleAdjustSpeed;
			}
			else if (!onGround)
				angle += angleAdjustSpeed;
			else 
FLATTEN_SHIP:
				angle = gs::util::approach(angle, 0.0f, 15.0f); 

			// Bases the vertical velocity of the angle of the ship. 
			velocity.y = std::sin(gs::util::toRadians(angle)) / 6.0f; 
			// Angle cannot be greater than 45 degrees. 
			gs::util::clamp(&angle, -45.0f, 45.0f); 
		   
			updatePosition();
			collision::applyCollision(*this);
		}

		if (!isClone && !prvsOnGround && onGround)
			Particle::spawnParticles(
				position + gs::Vec2f(0.0f, 0.3f), ParticleInfo::GroundHit,
				secondaryColor, 15
			);

		// Kills player if they get under the map. 
		if (isNpc || (!isNpc && !debugMode)) {
			if (position.y > currentMap->size.y)
				respawnTicks = 60; 
		}

		jump = false; 
		primaryColor = render::hsvToRgb(primaryHue, 100.0f, 100.0f); 
		secondaryColor = render::hsvToRgb(secondaryHue, 100.0f, 100.0f); 
	}

	void Player::updatePosition() {
		position += velocity; 
	}

	Player player;
	bool debugMode = false; 
}