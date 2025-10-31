// Include the class so that we can implement its method(s)
#include "ConfigParser.h"

#include <fstream>
#include <iostream>

#ifndef _DEBUG // works in VS
#define DEBUGLOAD(number, items) 
#else
#define DEBUGLOAD(number, items) do { std::cerr << number << " " << items << " were loaded!" << std::endl; } while (0)
#endif


bool ConfigParser::load(std::string filename)
{
	// Create a new filestream
	std::ifstream configfile;

	// Open the config file in read mode (default)
	configfile.open(filename);

	// If we could not open the file, return early with an "error"
	if (!configfile.is_open())  return false;

	// Read the file while we have not reached the end and the filestream is working
	while (configfile.is_open() && configfile.good() && !configfile.eof())
	{
		std::string key;
		// Stream the next word into key
		configfile >> key;

		std::cout << key << std::endl;

		//Comments
		if (key.substr(0, 1) == "#")
			configfile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		// Terrain
		else if (key == "Terrain") terrain = TerrainOnDisk::from_stream(configfile);

		// Meshes
		else if (key == "Mesh") meshes.push_back(MeshOnDisk::from_stream(configfile));
		
		// Objects
		else if (key == "Object") objects.push_back(ObjectOnDisk::from_stream(configfile));

		// Enemies
		else if (key == "Enemy") enemies.push_back(EnemyOnDisk::from_stream(configfile));		
		
		// Spawn
		else if (key == "Spawn") spawnBehaviour = SpawnBehaviour::from_file(configfile);

		// Weapon
		else if (key == "Weapon") weapons.push_back(WeaponOnDisk::from_file(configfile));

		// Projectiles
		else if (key == "Projectile") projectiles.push_back(ProjectileOnDisk::from_file(configfile));

		// Explosion
		else if (key == "Explosion") explosion = ExplosionOnDisk::from_file(configfile);

		// Shadows
		else if (key == "Shadow") shadows = Shadows::from_file(configfile);
	}

	DEBUGLOAD(meshes.size(), "Meshes");
	DEBUGLOAD(objects.size(), "Objects");
	DEBUGLOAD(enemies.size(), "Enemies");
		

	// Close the file
	configfile.close();

	return true;
}