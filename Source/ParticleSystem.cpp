/* Start Header ************************************************************************/
/*!
\file       ParticleSystem.cpp
\author     Yee Kiat Lim, yeekiat.lim, 2503993
\par        yeekiat.lim@digipen.edu
\brief		This file implements the necessary functionality for the particle system.

Copyright (C) 2026 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "AEEngine.h"
#include "ParticleSystem.h"
#include "Loaders/DataLoader.h"

// Constructor for particle
Particle::Particle(AEGfxVertexList* particleMesh) : sprite{Sprite(particleMesh, Vector2{}, Vector2{25, 25})}
{}

// Constructor for particle system
ParticleSystem::ParticleSystem(AEGfxVertexList* particleMesh) {
	particles.reserve(500);

	// Preload particles into particle system (Better performance)
	for (int i{}; i < 500; ++i) {
		particles.push_back(Particle(particleMesh));
	}
}

// Creates specified number of particles in particle system, all with random velocities and lifetimes (Provided values are set as maximum)
bool ParticleSystem::CreateParticles(s32 particleCount, f32 baseLifetime, Vector2 particleVelocity, Vector2 particlePosition, Color particleColor) {
	s32 spawnedCount{};

	for (int i{}; i < 500; ++i) {
		// Already stores active particle, skip over
		if (particles[i].isActive)
			continue;

		particles[i].sprite.position = particlePosition;
		particles[i].velocity = particleVelocity * (0.5f + AERandFloat() * 0.5f);
		particles[i].lifetime = (0.75f + AERandFloat() * 0.25f) * baseLifetime;
		particles[i].sprite.color = particleColor;
		particles[i].isActive = true;

		++spawnedCount;
		++activeParticleCount;
		// Able to create specified number of particles
		if (spawnedCount == particleCount) 
			return true;
	}

	// Unable to create specified number of particles, no more inactive particles in particle system
	return false;
}

// Update all active particles in particle system
void ParticleSystem::UpdateParticles(f32 dt) {
	for (int i{}; i < 500; ++i) {
		// Inactive particle, skip
		if (!particles[i].isActive)
			continue;

		particles[i].sprite.position += particles[i].velocity * dt;
		particles[i].sprite.UpdateTransform();

		particles[i].lifetime -= dt;
		// Destroy if lifetime has ended
		if (particles[i].lifetime <= 0) {
			particles[i].isActive = false;
			--activeParticleCount;
		}
			
	}
}

// Destroys all active particles in one go
// Needed when player moves between rooms (Particles in previous room must be removed)
void ParticleSystem::DestroyParticles() {
	for (int i{}; i < 500; ++i) {
		if (!particles[i].isActive) 
			continue;

		particles[i].isActive = false;
		--activeParticleCount;
	}
}

