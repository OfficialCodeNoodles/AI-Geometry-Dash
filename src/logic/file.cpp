#include "../../hdr/logic/file.hpp"
#include "../../hdr/logic/map.hpp"

namespace engine {
	WordPair seperate(const std::string& line, char seperator) {
		WordPair pair; 
		bool match = false; 

		for (char chr : line) {
			if (chr == seperator)
				match = true; 
			else {
				if (!match)
					pair.first += chr;
				else
					pair.last += chr; 
			}
		}

		return pair; 
	}

	void loadMap(Map& map) {
		std::ifstream saveFile, propFile;
	
		saveFile.open(mMapDirectory + std::to_string(map.mapNumber) + ".save");
		propFile.open(mMapDirectory + std::to_string(map.mapNumber) + ".prop");

		std::string line; 

		map.hue += map.mapNumber * 36.0f; 
		map.hue = std::fmod(map.hue, 360.0f); 

		while (std::getline(propFile, line)) {
			WordPair pair = seperate(line); 

			if (pair.first == "width")
				map.size.x = std::stoi(pair.last);
			else if (pair.first == "height")
				map.size.y = std::stoi(pair.last);
			else if (pair.first == "hue")
				map.hue = std::stof(pair.last); 
		}

		map.create(); 

		for (int tileIndex = 0; std::getline(saveFile, line); tileIndex++) {
			Tile::Id id = std::stoi(line); 
			map.tiles[tileIndex] = id; 
		}

		saveFile.close(); 
		propFile.close(); 
	}
	void saveMap(const Map& map) {
		std::ofstream saveFile, propFile; 

		saveFile.open(mMapDirectory + std::to_string(map.mapNumber) + ".save");
		propFile.open(mMapDirectory + std::to_string(map.mapNumber) + ".prop"); 

		saveFile.clear(); 
		propFile.clear(); 

		for (int tileIndex = 0; tileIndex < map.blockSize; tileIndex++) 
			saveFile << map.tiles[tileIndex] << std::endl;

		propFile << "width:" << map.size.x << std::endl; 
		propFile << "height:" << map.size.y << std::endl; 
		propFile << "hue:" << map.hue << std::endl; 

		saveFile.close(); 
		propFile.close(); 
	}
}