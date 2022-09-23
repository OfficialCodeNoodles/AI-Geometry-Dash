#pragma once

//Dependencies
#include <fstream>

#include <Glass/glass.hpp>
#include <Windows.h>

//Macros

#define mDebug
#define mTitle std::string("Geometry Dash - AI")
#define mAssetDirectory std::string("assets/")
#define mMapDirectory std::string("maps/")
#define mNumOfMaps 10
#define mNumOfParticles 1000
#define mGetStyle(fullscreen) fullscreen ? sf::Style::Fullscreen \
	: sf::Style::Default

//Globals

extern int ticks; 
extern int levelIndex; 
extern int attempt; 
extern int generation; 
extern float farthestDistance; 

//Others 
