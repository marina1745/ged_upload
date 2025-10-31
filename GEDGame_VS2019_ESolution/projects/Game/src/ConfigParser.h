#pragma once

#include <string>
#include <list>

// Define a new class
class ConfigParser
{
public:
	struct MeshOnDisk {
		std::string identifier;
		std::string pathMesh;
		std::string pathDiffuse;
		std::string pathSpecular;
		std::string pathGlow;
	};

	struct ObjectOnDisk {
		std::string identifier;
		std::string parentIdentifier;
		std::string meshIdentifer;
		float pos_x = 0, pos_y = 0, pos_z = 0;
		float rot_x = 0, rot_y = 0, rot_z = 0;
		float scale = 1;
	};

	struct EnemyOnDisk {
		std::string identifier;
		std::string meshIdentifer;
		int hp = 1;
		float speed = 0, size = 1;
		float pos_x = 0, pos_y = 0, pos_z = 0;
		float rot_x = 0, rot_y = 0, rot_z = 0;
		float scale = 1;
	};

	struct SpawnBehaviour {
		float interval = 1.0f;
		float spawn_radius = 1000.0f;
		float despawn_radius = 2000.0f;
		float target_radius = 100.0f;
		float min_height = 0.0f;
		float max_height = 1.0f;
	};

	struct WeaponOnDisk {
		std::string parentIdentifier;
		std::string meshIdentifer;
		std::string projectile_identifier;
		float spawnpoint_x = 0, spawnpoint_y = 0, spawnpoint_z = 0;
		float firerate = 1;
	};

	struct ProjectileOnDisk {
		std::string identifier;
		int damage = 1;
		float projectileSpeed = 1;
		float spriteSize = 1;
		bool gravity = true;
		std::string spritePath;
	};

	struct ExplosionOnDisk
	{
		float scale = 1;
		float duration = 1;
		std::string spritePath;

		int particle_count = 0;
		float particle_min_velocity = 0;
		float particle_max_velocity = 1;
		float particle_min_lifetime = 0;
		float particle_max_lifetime = 1;
	};

	// returns true on success, false on failure
	bool load(std::string filename);

	// Implement getters using implicit inlining
	const std::string& get_terrainPathHeight() const { return terrainPathHeight; }
	const std::string& get_terrainPathColor() const { return terrainPathColor; }
	const std::string& get_terrainPathNormal() const { return terrainPathNormal; }
	float get_TerrainWidth() const { return terrainWidth; }
	float get_TerrainHeight() const { return terrainHeight; }
	float get_TerrainDepth() const { return terrainDepth; }
	bool get_UseShadows() const { return useShadows; }
	float get_ShadowRes() const { return shadow_res; }
	const std::list<MeshOnDisk>& get_Meshes() const { return meshes; }
	const std::list<ObjectOnDisk>& get_Objects() const { return objects; }
	const std::list<EnemyOnDisk>& get_Enemies() const { return enemies; }
	const std::list<WeaponOnDisk>& get_Weapons() const { return weapons; }
	const std::list<ProjectileOnDisk>& get_Projectiles() const { return projectiles; }
	const SpawnBehaviour& get_SpawnBehaviour() const { return spawnBehaviour; }
	const ExplosionOnDisk& get_Explosion() const { return explosion; }

private:
	std::string terrainPathHeight;
	std::string terrainPathColor;
	std::string terrainPathNormal;

	float terrainWidth = -1;
	float terrainHeight = -1;
	float terrainDepth = -1;

	bool useShadows = false;
	int shadow_res = 2048 * 4;

	std::list<MeshOnDisk> meshes;
	std::list<ObjectOnDisk> objects;
	std::list<EnemyOnDisk> enemies;
	std::list<WeaponOnDisk> weapons;
	std::list<ProjectileOnDisk> projectiles;

	SpawnBehaviour spawnBehaviour;
	ExplosionOnDisk explosion;
};

extern ConfigParser g_ConfigParser;