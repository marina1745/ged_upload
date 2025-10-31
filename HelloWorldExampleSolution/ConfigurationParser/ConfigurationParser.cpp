// ConfigurationParser.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
// Include the custom class
#include "ConfigParser.h"

int main()
{
    // Create a parser on the stack
    ConfigParser parser;

    // Load the config, exit early when the config could not be loaded
    if (!parser.load("game.cfg"))
        return EXIT_FAILURE;

    // Print all values in the config parser
    std::cout << "Spinning: "         << parser.get_spinning()          << std::endl;
    std::cout << "Spin Speed: "       << parser.get_spinSpeed()         << std::endl;
    std::cout << "Background Color: " << parser.get_backgroundColor().r << " " 
                                      << parser.get_backgroundColor().g << " " 
                                      << parser.get_backgroundColor().r << std::endl;
    std::cout << "Terrain Path: "     << parser.get_terrainPath()       << std::endl;
    std::cout << "Terrain Width: "    << parser.get_terrainWidth()      << std::endl;
    std::cout << "Terrain Height: "   << parser.get_terrainHeight()     << std::endl;
    std::cout << "Terrain Depth: "    << parser.get_terrainDepth()      << std::endl;

    return EXIT_SUCCESS;
}

