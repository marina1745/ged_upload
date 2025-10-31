// Include the class so that we can implement its method(s)
#include "ConfigParser.h"

#include <fstream>
#include <iostream>

constexpr auto data_path = "resources/";

bool ConfigParser::load(std::string filename)
{
	// Create a new filestream
	std::ifstream configfile;

	// Open the config file in read mode (default)
	configfile.open(filename);

	// If we could not open the file, return early with an "error"
	if (!configfile.is_open()) 
	{
		return false;
	}

	// Read the file while we have not reached the end and the filestream is working
	while (configfile.is_open() && configfile.good() && !configfile.eof())
	{
		std::string key;
		// Stream the next word into key
		configfile >> key;

		//Comments
		if (key.substr(0, 1) == "#")
			configfile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		// Terrain Paths
		else if (key == "TerrainPath") 
		{
			configfile >> terrainPathHeight;
			configfile >> terrainPathColor;
			configfile >> terrainPathNormal;
			
			terrainPathHeight = data_path + terrainPathHeight;
			terrainPathColor = data_path + terrainPathColor;
			terrainPathNormal = data_path + terrainPathNormal;
		}
		// Terrain config
		else if (key == "TerrainWidth")
			configfile >> terrainWidth;
		else if (key == "TerrainHeight")
			configfile >> terrainHeight;
		else if (key == "TerrainDepth")
			configfile >> terrainDepth;
		// Meshes
		else if (key == "Mesh")
		{
			MeshOnDisk new_mesh;
			
			configfile >> new_mesh.identifier;
			configfile >> new_mesh.pathMesh;
			configfile >> new_mesh.pathDiffuse;
			configfile >> new_mesh.pathSpecular;
			configfile >> new_mesh.pathGlow;
			
			new_mesh.pathMesh = data_path + new_mesh.pathMesh;
			new_mesh.pathDiffuse = data_path + new_mesh.pathDiffuse;
			if (new_mesh.pathSpecular != "-")
				new_mesh.pathSpecular = data_path + new_mesh.pathSpecular;
			if (new_mesh.pathGlow != "-")
				new_mesh.pathGlow = data_path + new_mesh.pathGlow;
			
			meshes.push_back(new_mesh);
		}
		// Objects
		else if (key == "Object")
		{
			ObjectOnDisk new_object;
			
			configfile >> new_object.identifier;
			configfile >> new_object.meshIdentifer;
			configfile >> new_object.pos_x >> new_object.pos_y >> new_object.pos_z;
			configfile >> new_object.rot_x >> new_object.rot_y >> new_object.rot_z;
			configfile >> new_object.scale;
			configfile >> new_object.parentIdentifier;
			
			objects.push_back(new_object);
		}
		// Enemies
		else if (key == "Enemy")
		{
			EnemyOnDisk new_object;

			configfile >> new_object.identifier;
			configfile >> new_object.hp;
			configfile >> new_object.speed;
			configfile >> new_object.size;
			configfile >> new_object.meshIdentifer;
			configfile >> new_object.pos_x >> new_object.pos_y >> new_object.pos_z;
			configfile >> new_object.rot_x >> new_object.rot_y >> new_object.rot_z;
			configfile >> new_object.scale;

			enemies.push_back(new_object);
		}
		// Spawn
		else if (key == "Spawn")
		{
			configfile >> spawnBehaviour.interval;
			configfile >> spawnBehaviour.spawn_radius;
			configfile >> spawnBehaviour.despawn_radius;
			configfile >> spawnBehaviour.target_radius;
			configfile >> spawnBehaviour.min_height >> spawnBehaviour.max_height;
		}
		else if (key == "Weapon")
		{
			WeaponOnDisk new_weapon;

			configfile >> new_weapon.meshIdentifer;
			configfile >> new_weapon.firerate;
			configfile >> new_weapon.spawnpoint_x >> new_weapon.spawnpoint_y >> new_weapon.spawnpoint_z;
			configfile >> new_weapon.parentIdentifier;
			configfile >> new_weapon.projectile_identifier;

			weapons.push_back(new_weapon);
		}
		else if (key == "Projectile")
		{
			ProjectileOnDisk new_projectile;

			configfile >> new_projectile.identifier;
			configfile >> new_projectile.damage;
			configfile >> new_projectile.projectileSpeed;
			configfile >> new_projectile.gravity;
			configfile >> new_projectile.spritePath;
			configfile >> new_projectile.spriteSize;

			new_projectile.spritePath = data_path + new_projectile.spritePath;

			projectiles.push_back(new_projectile);
		}
		else if (key == "Explosion")
		{
			configfile >> explosion.spritePath;
			configfile >> explosion.duration;
			configfile >> explosion.scale;

			configfile >> explosion.particle_count;
			configfile >> explosion.particle_min_velocity;
			configfile >> explosion.particle_max_velocity;
			configfile >> explosion.particle_min_lifetime;
			configfile >> explosion.particle_max_lifetime;

			explosion.spritePath = data_path + explosion.spritePath;
		}
		else if (key == "Shadow")
		{
			configfile >> useShadows;
			configfile >> shadow_res;
		}
	}

	// Close the file
	configfile.close();

	return true;
}