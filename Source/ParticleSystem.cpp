#include "AEEngine.h"
#include "ParticleSystem.h"
#include "Loaders/DataLoader.h"

Particle::Particle(AEGfxVertexList* particleMesh) : sprite{Sprite(particleMesh, Vector2{}, Vector2{25, 25})}
{}

ParticleSystem::ParticleSystem(AEGfxVertexList* particleMesh) {
	particles.reserve(500);

	for (int i{}; i < 500; ++i) {
		particles.push_back(Particle(particleMesh));
	}
}

bool ParticleSystem::CreateParticles(s32 particleCount, f32 baseLifetime, Vector2 particleVelocity, Vector2 particlePosition, Color particleColor) {
	s32 spawnedCount{};

	for (int i{}; i < 500; ++i) {
		if (particles[i].isActive)
			continue;

		particles[i].sprite.position = particlePosition;
		particles[i].velocity = particleVelocity * (0.5f + AERandFloat() * 0.5f);
		particles[i].lifetime = (0.75f + AERandFloat() * 0.25f) * baseLifetime;
		particles[i].sprite.color = particleColor;
		particles[i].isActive = true;

		++spawnedCount;
		++activeParticleCount;
		if (spawnedCount == particleCount) 
			return true;
	}

	return false;
}

void ParticleSystem::UpdateParticles(f32 dt) {
	for (int i{}; i < 500; ++i) {
		if (!particles[i].isActive)
			continue;

		particles[i].sprite.position += particles[i].velocity * dt;
		particles[i].sprite.UpdateTransform();

		particles[i].lifetime -= dt;
		if (particles[i].lifetime <= 0) {
			particles[i].isActive = false;
			--activeParticleCount;
		}
			
	}
}

void ParticleSystem::DestroyParticles() {
	for (int i{}; i < 500; ++i) {
		if (!particles[i].isActive) 
			continue;

		particles[i].isActive = false;
		--activeParticleCount;
	}
}

