#pragma once
#include "AEEngine.h"
#include "Sprite.h"
#include <vector>


struct Particle {
	Sprite sprite;
	Vector2 velocity {};
	f32 lifetime{ 0.0f };
	bool isActive{ false };

	Particle(AEGfxVertexList* particleMesh);
};

class ParticleSystem {
	public:
		std::vector<Particle> particles;
		s32 activeParticleCount{ 0 };

		ParticleSystem(AEGfxVertexList* particleMesh);
		bool CreateParticles(s32 particleCount = 1, f32 baseLifetime = 1.0f, Vector2 particleVelocity = {}, Vector2 particlePosition = {}, Color particleColor = { 0.0f, 0.0f, 0.0f, 1.0f });
		void UpdateParticles(f32 dt);
		void DestroyParticles();
};

