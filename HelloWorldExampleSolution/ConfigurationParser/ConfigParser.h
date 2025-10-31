#pragma once

#include <string>

// Define a new class
class ConfigParser
{
public:
	struct Color {
		float r = 0;
		float g = 0;
		float b = 0;
	};

	// Declare our load method
	// returns true on success, false on failure
	bool load(std::string filename);

	// Declare getters
	float get_spinning();
	float get_spinSpeed();
	Color get_backgroundColor();
	std::string get_terrainPath();
	float get_terrainWidth();
	float get_terrainHeight();
	float get_terrainDepth();

private:
	float spinning = -1;
	float spinSpeed = -1;

	Color backgroundColor;

	std::string terrainPath;

	float terrainWidth = -1;
	float terrainHeight = -1;
	float terrainDepth = -1;
};

