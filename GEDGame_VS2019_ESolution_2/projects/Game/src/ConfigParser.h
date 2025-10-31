#pragma once

#include <string>
#include <list>
#include <fstream>
#include <iostream>


// Define a new class
class ConfigParser
{
public:
	struct TerrainOnDisk
	{
		float width = 800.0, depth = 800.0, height = 200.0;
		std::string heightMap, colorMap, normalMap;

		static TerrainOnDisk from_stream(std::ifstream& file)
		{
			TerrainOnDisk terrain;

			file >> terrain.heightMap >> terrain.colorMap >> terrain.normalMap;
			file >> terrain.width >> terrain.depth >> terrain.height;

			terrain.heightMap = res_path(terrain.heightMap);
			terrain.colorMap = res_path(terrain.colorMap);
			terrain.normalMap = res_path(terrain.normalMap);

			return terrain;
		}
	};

	struct MeshOnDisk
	{
		std::string identifier;
		std::string pathMesh;
		std::string pathDiffuse;
		std::string pathSpecular;
		std::string pathGlow;

		static MeshOnDisk from_stream(std::ifstream& file)
		{
			MeshOnDisk mesh;
			
		    file >> mesh.identifier;
		    file >> mesh.pathMesh;
		    file >> mesh.pathDiffuse;
		    file >> mesh.pathSpecular;
		    file >> mesh.pathGlow;

			std::cout << mesh.identifier << std::endl;

			mesh.pathMesh = res_path(mesh.pathMesh);
			mesh.pathDiffuse = res_path(mesh.pathDiffuse);
			if (mesh.pathSpecular != "-")
				mesh.pathSpecular = res_path(mesh.pathSpecular);
			if (mesh.pathGlow != "-")
				mesh.pathGlow = res_path(mesh.pathGlow);
            return mesh;
		}
	};

	struct ObjectOnDisk
	{
		std::string identifier;
		std::string parentIdentifier;
		std::string meshIdentifier;
		float pos_x = 0, pos_y = 0, pos_z = 0;
		float rot_x = 0, rot_y = 0, rot_z = 0;
		float scale = 1;

		static ObjectOnDisk from_stream(std::ifstream& file)
		{
			ObjectOnDisk object;
			
		    file >> object.identifier;
		    file >> object.parentIdentifier;
		    file >> object.meshIdentifier;
		    file >> object.pos_x >> object.pos_y >> object.pos_z;
		    file >> object.rot_x >> object.rot_y >> object.rot_z;
		    file >> object.scale;

            return object;
		}
	};

	struct EnemyOnDisk
	{
		std::string identifier;
		std::string meshIdentifier;
		int hp = 1;
		float speed = 0, size = 1;
		float pos_x = 0, pos_y = 0, pos_z = 0;
		float rot_x = 0, rot_y = 0, rot_z = 0;
		float scale = 1;

		static EnemyOnDisk from_stream(std::ifstream& file)
		{
			EnemyOnDisk enemy;
			
		    file >> enemy.identifier;
		    file >> enemy.meshIdentifier;
		    file >> enemy.hp;
		    file >> enemy.speed >> enemy.size;
            file >> enemy.pos_x >> enemy.pos_y >> enemy.pos_z;
            file >> enemy.rot_x >> enemy.rot_y >> enemy.rot_z;
            file >> enemy.scale;

            return enemy;
		}
	};

	struct SpawnBehaviour
	{
		float interval = 1.0f;
		float spawn_radius = 1000.0f;
		float despawn_radius = 2000.0f;
		float target_radius = 100.0f;
		float min_height = 0.0f;
		float max_height = 1.0f;

		static SpawnBehaviour from_file(std::ifstream& file)
		{
			SpawnBehaviour spawn;
			
		    file >> spawn.interval;
		    file >> spawn.spawn_radius;
		    file >> spawn.despawn_radius;
		    file >> spawn.target_radius;
		    file >> spawn.min_height;
		    file >> spawn.max_height;

            return spawn;
		}
	};

	struct WeaponOnDisk
	{
		std::string parentIdentifier;
		std::string meshIdentifer;
		std::string projectile_identifier;
		float spawnpoint_x = 0, spawnpoint_y = 0, spawnpoint_z = 0;
		float firerate = 1;

		static WeaponOnDisk from_file(std::ifstream& file)
		{
			WeaponOnDisk weapon;
			
		    file >> weapon.parentIdentifier;
		    file >> weapon.meshIdentifer;
		    file >> weapon.projectile_identifier;
		    file >> weapon.spawnpoint_x >> weapon.spawnpoint_y >> weapon.spawnpoint_z;
		    file >> weapon.firerate;

            return weapon;
		}
	};

	struct ProjectileOnDisk
	{
		std::string identifier;
		int damage = 1;
		float projectileSpeed = 1;
		float spriteSize = 1;
		bool gravity = true;
		std::string spritePath;

        static ProjectileOnDisk from_file(std::ifstream& file)
		{
			ProjectileOnDisk projectile;
        	
            file >> projectile.identifier;
            file >> projectile.damage;
            file >> projectile.projectileSpeed;
            file >> projectile.spriteSize;
            file >> projectile.gravity;
            file >> projectile.spritePath;

			projectile.spritePath = res_path(projectile.spritePath);

            return projectile;
        }
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

        static ExplosionOnDisk from_file(std::ifstream& file)
		{
			ExplosionOnDisk explosion;
        	
            file >> explosion.scale;
            file >> explosion.duration;
            file >> explosion.spritePath;
            file >> explosion.particle_count;
            file >> explosion.particle_min_velocity;
            file >> explosion.particle_max_velocity;
            file >> explosion.particle_min_lifetime;
            file >> explosion.particle_max_lifetime;

			explosion.spritePath = res_path(explosion.spritePath);

            return explosion;
        }
	};

	struct Shadows
	{
		bool use = false;
		int resolution = 2048 * 4;

		static Shadows from_file(std::ifstream& file)
		{
			Shadows shadows;

			file >> shadows.use >> shadows.resolution;

			shadows.resolution *= 4;
			
			return shadows;
		}
	};

	// returns true on success, false on failure
	bool load(std::string filename);

	// Implement getters using implicit inlining
	const std::list<MeshOnDisk>& get_Meshes() const { return meshes; }
	const std::list<ObjectOnDisk>& get_Objects() const { return objects; }
	const std::list<EnemyOnDisk>& get_Enemies() const { return enemies; }
	const std::list<WeaponOnDisk>& get_Weapons() const { return weapons; }
	const std::list<ProjectileOnDisk>& get_Projectiles() const { return projectiles; }
	const TerrainOnDisk& get_terrain() const { return terrain; }
	const SpawnBehaviour& get_SpawnBehaviour() const { return spawnBehaviour; }
	const ExplosionOnDisk& get_Explosion() const { return explosion; }
	const Shadows& get_Shadows() const { return shadows; }

private:
    static constexpr auto data_path = "resources/";

	std::list<MeshOnDisk> meshes;
	std::list<ObjectOnDisk> objects;
	std::list<EnemyOnDisk> enemies;
	std::list<WeaponOnDisk> weapons;
	std::list<ProjectileOnDisk> projectiles;

	TerrainOnDisk terrain;
	SpawnBehaviour spawnBehaviour;
	ExplosionOnDisk explosion;
	Shadows shadows;

	static std::string res_path(std::string path)
	{
        return data_path + path;
	}
};

extern ConfigParser g_ConfigParser;
