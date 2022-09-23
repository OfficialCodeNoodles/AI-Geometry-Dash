#pragma once

//Dependencies
#include "../resources.hpp"

namespace engine {
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

		static void spawnMovementParticles(
			gs::Vec2f position, gs::Color color, int count
		); 
		static void spawnGroundHitParticles(
			gs::Vec2f position, gs::Color color, int count
		); 
	private:
		// Adds a new particle to the particles array if spot is available. 
		static void pushParticle(const Particle& newParticle); 
	};

	extern Particle particles[mNumOfParticles]; 

	void updateParticles(); 
}