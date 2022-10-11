#include "../../hdr/logic/particle.hpp"
#include "../../hdr/graphics/render.hpp"
#include "../../hdr/logic/neuralNet.hpp"

namespace engine {
	gs::Vec2f ParticleInfo::getPositionOffset(Id id) {
		return particleInfo[id].positionOffset; 
	}
	gs::Vec2f ParticleInfo::getXPositionRandomOffset(Id id) {
		return particleInfo[id].xPositionRandomOffset;
	}
	gs::Vec2f ParticleInfo::getYPositionRandomOffset(Id id) {
		return particleInfo[id].yPositionRandomOffset;
	}
	gs::Vec2f ParticleInfo::getXVelocityRange(Id id) {
		return particleInfo[id].xVelocityRange;
	}
	gs::Vec2f ParticleInfo::getYVelocityRange(Id id) {
		return particleInfo[id].yVelocityRange;
	}
	gs::Vec2f ParticleInfo::getAngleRange(Id id) {
		return particleInfo[id].angleRange; 
	}
	gs::Vec2f ParticleInfo::getAngleSpeedRange(Id id) {
		return particleInfo[id].angleSpeedRange;
	}
	gs::Vec2f ParticleInfo::getSizeRange(Id id) {
		return particleInfo[id].sizeRange;
	}
	int ParticleInfo::getTickCount(Id id) {
		return particleInfo[id].tickCount; 
	}

	ParticleInfo particleInfo[ParticleInfo::End] = {
		{ 
			gs::Vec2f(), gs::Vec2f(), gs::Vec2f(), 
			gs::Vec2f(-0.1f, 0.0f), gs::Vec2f(-0.05f, 0.0f), 
			gs::Vec2f(0.0f, 360.0f),
			gs::Vec2f(0.0f, 20.0f), 
			gs::Vec2f(5.0f, 15.0f),
			30
		},
		{
			gs::Vec2f(), gs::Vec2f(), gs::Vec2f(),
			gs::Vec2f(-0.1f, 0.1f), gs::Vec2f(-0.1f, 0.0f),
			gs::Vec2f(0.0f, 360.0f),
			gs::Vec2f(0.0f, 20.0f),
			gs::Vec2f(5.0f, 20.0f),
			30
		},
		{
			gs::Vec2f(), gs::Vec2f(-0.4f, 0.4f), gs::Vec2f(),
			gs::Vec2f(0.0f, 0.0f), gs::Vec2f(-0.1f, 0.0f),
			gs::Vec2f(0.0f, 0.0f),
			gs::Vec2f(0.0f, 0.0f),
			gs::Vec2f(5.0f, 10.0f),
			30
		},
		{
			gs::Vec2f(), gs::Vec2f(0.0f, 0.0f), gs::Vec2f(),
			gs::Vec2f(0.0f, 0.0f), gs::Vec2f(0.0f, 0.0f),
			gs::Vec2f(0.0f, 0.0f),
			gs::Vec2f(0.0f, 0.0f),
			gs::Vec2f(10.0f, 20.0f),
			30
		},
		{
			gs::Vec2f(), gs::Vec2f(0.0f, 0.0f), gs::Vec2f(),
			gs::Vec2f(0.0f, 0.0f), gs::Vec2f(0.0f, 0.0f),
			gs::Vec2f(0.0f, 360.0f),
			gs::Vec2f(0.0f, 20.0f),
			gs::Vec2f(5.0f, 15.0f),
			20
		}
	}; 

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

	void Particle::spawnParticles(
		gs::Vec2f position, ParticleInfo::Id id, gs::Color color, int count,
		gs::Vec2f velocityOffset
	) {
		for (int particleIndex = 0; particleIndex < count; particleIndex++) {
			Particle newParticle;

			newParticle.position = position +
				ParticleInfo::getPositionOffset(id); 
			newParticle.position += gs::Vec2f(
				NeuralNet::randRange(ParticleInfo::getXPositionRandomOffset(id)),
				NeuralNet::randRange(ParticleInfo::getYPositionRandomOffset(id))
			); 
			newParticle.velocity = gs::Vec2f(
				NeuralNet::randRange(ParticleInfo::getXVelocityRange(id)),
				NeuralNet::randRange(ParticleInfo::getYVelocityRange(id))
			) + velocityOffset;  
			newParticle.angle = NeuralNet::randRange(
				ParticleInfo::getAngleRange(id)
			); 
			newParticle.angleSpeed = NeuralNet::randRange(
				ParticleInfo::getAngleSpeedRange(id)
			); 
			newParticle.color = color; 
			newParticle.size = NeuralNet::randRange(
				ParticleInfo::getSizeRange(id)
			);
			newParticle.ticks = ParticleInfo::getTickCount(id); 

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