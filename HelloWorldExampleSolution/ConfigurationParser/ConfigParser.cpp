// Include the class so that we can implement its methods
#include "ConfigParser.h"

#include <fstream>
#include <iostream>

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

		// Streams cast their content automatically
		if (key == "spinning")
			configfile >> spinning;
		else if (key == "spinSpeed")
			configfile >> spinSpeed;
		else if (key == "backgroundColor")
		{
			configfile >> backgroundColor.r;
			configfile >> backgroundColor.g;
			configfile >> backgroundColor.b;
		}
		else if (key == "terrainPath")
			configfile >> terrainPath;
		else if (key == "terrainWidth")
			configfile >> terrainWidth;
		else if (key == "terrainHeight")
			configfile >> terrainHeight;
		else if (key == "terrainDepth")
			configfile >> terrainDepth;
	}

	// Close the file
	configfile.close();

	return true;
}

float ConfigParser::get_spinning()
{
	return spinning;
}

float ConfigParser::get_spinSpeed()
{
	return spinSpeed;
}

ConfigParser::Color ConfigParser::get_backgroundColor()
{
	return backgroundColor;
}

std::string ConfigParser::get_terrainPath()
{
	return terrainPath;
}

float ConfigParser::get_terrainWidth()
{
	return terrainWidth;
}

float ConfigParser::get_terrainHeight()
{
	return terrainHeight;
}

float ConfigParser::get_terrainDepth()
{
	return terrainDepth;
}
