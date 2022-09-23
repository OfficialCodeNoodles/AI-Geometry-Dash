#pragma once

//Dependencies
#include "../resources.hpp"

namespace engine {
	class Map; 

	struct WordPair {
		std::string first, last; 
	};

	// Seperates a string into two strings based on a seperator value. 
	WordPair seperate(const std::string& line, char seperator = ':'); 

	void loadMap(Map& map); 
	void saveMap(const Map& map); 
}