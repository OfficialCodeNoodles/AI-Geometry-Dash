#include "../../hdr/logic/particle.hpp"
#include "../../hdr/graphics/render.hpp"

namespace engine {
	Particle::Particle() : angle(0.0f), angleSpeed(0.0f), size(0.0f), 
		ticks(0) {
	}

	void Particle::update() {
		position += velocity; 
		velocity += acceleration; 
		angle += angleSpeed; 

		if (ticks > 0) ticks--; 

		// Slowly fades particle as it dissapears. 
		color.a = std::min(255, ticks * 8); 
	}

	void Particle::spawnMovementParticles(
		gs::Vec2f position, gs::Color color, int count
	) {
		for (int particleIndex = 0; particleIndex < count; particleIndex++) {
			Particle newParticle; 

			newParticle.position = position; 
			newParticle.velocity = gs::Vec2f(
				-(static_cast<float>(rand() % 1000) / 10000.0f),
				-(static_cast<float>(rand() % 1000) / 20000.0f)
			);
			newParticle.acceleration = gs::Vec2f(0.005f, 0.0005f); 
			newParticle.angle = rand() % 360; 
			newParticle.angleSpeed = rand() % 20; 
			newParticle.color = color; 
			newParticle.size = (rand() % 10) + 5; 
			newParticle.ticks = 40; 

			pushParticle(newParticle); 
		}
	}
	void Particle::spawnGroundHitParticles(
		gs::Vec2f position, gs::Color color, int count
	) {
		for (int particleIndex = 0; particleIndex < count; particleIndex++) {
			Particle newParticle;

			newParticle.position = position;
			newParticle.velocity = gs::Vec2f(
				(static_cast<float>(rand() % 1000) / 5000.0f) - 0.1f,
				-(static_cast<float>(rand() % 1000) / 10000.0f)
			);
			newParticle.acceleration = gs::Vec2f(0.0f, 0.001f);
			newParticle.angle = rand() % 360;
			newParticle.angleSpeed = rand() % 20;
			newParticle.color = color; 
			newParticle.size = (rand() % 15) + 5;
			newParticle.ticks = 40;

			pushParticle(newParticle);
		}
	}

	void Particle::pushParticle(const Particle& newParticle) {
		// Pointer to an unused particle that can be used to store the new 
		// particle. 
		Particle* oldParticle = nullptr; 

		for (Particle& particle : particles) {
			if (particle.ticks == 0)
				oldParticle = &particle; 
		}

		if (oldParticle != nullptr) 
			*oldParticle = newParticle; 
	}

	Particle particles[mNumOfParticles];

	void updateParticles() {
		for (Particle& particle : particles) 
			particle.update(); 
	}
}