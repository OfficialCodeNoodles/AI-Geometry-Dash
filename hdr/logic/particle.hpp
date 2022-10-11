#pragma once

//Dependencies
#include "../resources.hpp"

namespace engine {
	struct ParticleInfo {
		typedef int Id;
		enum {
			Movement, GroundHit, JumpPad, Portal, JumpOrb, End
		};

		gs::Vec2f positionOffset,
			xPositionRandomOffset,
			yPositionRandomOffset;
		gs::Vec2f xVelocityRange,
			yVelocityRange;
		gs::Vec2f angleRange; 
		gs::Vec2f angleSpeedRange;
		gs::Vec2f sizeRange;
		int tickCount;

		static gs::Vec2f getPositionOffset(Id id);
		static gs::Vec2f getXPositionRandomOffset(Id id);
		static gs::Vec2f getYPositionRandomOffset(Id id);
		static gs::Vec2f getXVelocityRange(Id id);
		static gs::Vec2f getYVelocityRange(Id id);
		static gs::Vec2f getAngleRange(Id id); 
		static gs::Vec2f getAngleSpeedRange(Id id);
		static gs::Vec2f getSizeRange(Id id);
		static int getTickCount(Id id);
	};

	extern ParticleInfo particleInfo[ParticleInfo::End]; 

	struct Particle {
	public:
		gs::Vec2f position; 
		gs::Vec2f velocity; 
		gs::Vec2f acceleration; 
		float angle; 
		float angleSpeed; 
		gs::Color color; 
		float size; 
		// Time left before particle disapears. 
		int ticks; 

		Particle(); 

		void update(); 

		static void spawnParticles(
			gs::Vec2f position, ParticleInfo::Id id, gs::Color color, int count,
			gs::Vec2f velocityOffset = gs::Vec2f()
		); 
	private:
		// Adds a new particle to the particles array if spot is available. 
		static void pushParticle(const Particle& newParticle); 
	};

	extern Particle particles[mNumOfParticles]; 

	void updateParticles(); 
}