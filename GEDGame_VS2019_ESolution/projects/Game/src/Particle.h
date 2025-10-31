#pragma once

#include <DirectXMath.h>

#include "GameEffect.h"
#include "SpriteRenderer.h"

class Sprite
{
public:
	DirectX::XMVECTOR position = { 0, 0, 0 };
	int spriteIndex = -1;
	float size = 1;
	float camera_distance = 0;

	virtual void update(const float fElapsedTime, const DirectX::XMVECTOR& gravity_vector) = 0;

	SpriteVertex GetSpriteVertex(const DirectX::XMVECTOR& camera_ahead, const DirectX::XMVECTOR& offset = { 0, 0, 0 })
	{
		using namespace DirectX;

		SpriteVertex vert;

		XMStoreFloat3(&vert.position, position + offset);
		vert.radius = size;
		vert.textureIndex = spriteIndex;
		vert.camera_distance = DirectX::XMVectorGetX(DirectX::XMVector3Dot(position, camera_ahead));

		return vert;
	}
};

class Projectile : public Sprite
{
public:
	DirectX::XMVECTOR velocity = { 0, 0, 0 };

	bool useGravity = true;
	int damage = 1;

	void update(const float fElapsedTime, const DirectX::XMVECTOR& gravity_vector) override
	{
		using namespace DirectX;

		if (useGravity)
			velocity += gravity_vector * fElapsedTime;
		position += velocity * fElapsedTime;
	}
};

class Explosion : public Sprite
{
public:
	class ExplosionParticle : public Sprite
	{
	public:
		DirectX::XMVECTOR velocity = { 0, 0, 0 };

		float time = 0;
		float duration = 1;

		void update(const float fElapsedTime, const DirectX::XMVECTOR& gravity_vector) override
		{
			using namespace DirectX;

			velocity += gravity_vector * fElapsedTime;
			position += velocity * fElapsedTime;
			time += fElapsedTime;
		}
	};

	float time = 0;
	float duration = 1;

	std::vector<ExplosionParticle> explosionParticles;

	Explosion(int textureIndex, int particleCount)
	{
		explosionParticles.resize(particleCount);
		spriteIndex = textureIndex;
	}

	void Init(float minVelocity, float maxVelocity, float minLifetime, float maxLifetime)
	{
		using namespace DirectX;

		for (auto& p : explosionParticles)
		{
			// rand() is good enough for explosion particles
			p.spriteIndex = spriteIndex;
			p.duration = (maxLifetime - minLifetime) * static_cast<float>(rand()) / RAND_MAX + minLifetime;
			p.velocity = DirectX::XMVector3Normalize({
				static_cast<float>(rand()) / RAND_MAX - 0.5f,
				static_cast<float>(rand()) / RAND_MAX - 0.5f,
				static_cast<float>(rand()) / RAND_MAX - 0.5f });
			p.velocity *= (maxVelocity - minVelocity) * static_cast<float>(rand()) / RAND_MAX	+ minVelocity;
		}
	}

	void update(const float fElapsedTime, const DirectX::XMVECTOR& gravity_vector) override
	{
		using namespace DirectX;

		time += fElapsedTime;

		// Update all particles
		for (auto& p : explosionParticles)
			p.update(fElapsedTime, gravity_vector);
	}
};