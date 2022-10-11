#include "../../hdr/logic/map.hpp"

namespace engine {
	Tile::CollisionType Tile::getCollisionType(Id id) {
		return tiles[id].collisionType;
	}
	Tile::ActionType Tile::getActionType(Id id) {
		return tiles[id].actionType;
	}
	int Tile::getTextureIndex(Id id) {
		return tiles[id].textureIndex; 
	}
	bool Tile::isColorable(Id id) {
		return tiles[id].colorable; 
	}

	Tile tiles[Tile::End] = {
		{ Tile::CollisionType::Empty },
		{ Tile::CollisionType::Solid, Tile::ActionType::None, 0, false },
		{ Tile::CollisionType::Spike, Tile::ActionType::None, 1, false },
		{ Tile::CollisionType::Spike, Tile::ActionType::None, 2, false },
		{ Tile::CollisionType::Solid, Tile::ActionType::None, 3, false },
		{ Tile::CollisionType::Solid, Tile::ActionType::None, 4, false },
		{ Tile::CollisionType::Solid, Tile::ActionType::None, 5, false },
		{ Tile::CollisionType::Solid, Tile::ActionType::None, 6, false },
		{ Tile::CollisionType::Solid, Tile::ActionType::None, 7, false },
		{ Tile::CollisionType::Solid, Tile::ActionType::None, 8, false },
		{ Tile::CollisionType::Solid, Tile::ActionType::None, 9, false },
		{ Tile::CollisionType::Solid, Tile::ActionType::None, 10, false },
		{ Tile::CollisionType::Solid, Tile::ActionType::None, 11, false },
		{ Tile::CollisionType::Solid, Tile::ActionType::None, 12, false },
		{ Tile::CollisionType::Solid, Tile::ActionType::None, 13, false },
		{ Tile::CollisionType::Solid, Tile::ActionType::None, 14, false },
		{ Tile::CollisionType::Solid, Tile::ActionType::None, 15, false },
		{ Tile::CollisionType::Solid, Tile::ActionType::None, 16, false },
		{ Tile::CollisionType::Solid, Tile::ActionType::None, 17, false },
		{ Tile::CollisionType::Solid, Tile::ActionType::None, 18, false },
		{ Tile::CollisionType::Semisolid, Tile::ActionType::None, 19, false },
		{ Tile::CollisionType::Spike, Tile::ActionType::None, 20, false },
		{ Tile::CollisionType::Spike, Tile::ActionType::None, 21, true },
		{ Tile::CollisionType::Spike, Tile::ActionType::None, 22, false },
		{ Tile::CollisionType::Empty, Tile::ActionType::None, 23, true },
		{ Tile::CollisionType::Empty, Tile::ActionType::None, 24, true },
		{ Tile::CollisionType::Empty, Tile::ActionType::None, 25, true },
		{ Tile::CollisionType::Solid, Tile::ActionType::None, 26, false },
		{ Tile::CollisionType::Solid, Tile::ActionType::None, 27, false },
		{ Tile::CollisionType::Solid, Tile::ActionType::None, 28, false },
		{ Tile::CollisionType::Solid, Tile::ActionType::None, 29, false },
		{ Tile::CollisionType::Portal, Tile::ActionType::Flying, 0 },
		{ Tile::CollisionType::Portal, Tile::ActionType::Flying, 2 },
		{ Tile::CollisionType::JumpPad, Tile::ActionType::None, 30, false },
		{ Tile::CollisionType::JumpOrb, Tile::ActionType::None },
		{ Tile::CollisionType::Portal, Tile::ActionType::FlipGravity, 4 },
		{ Tile::CollisionType::Portal, Tile::ActionType::FlipGravity, 6 },
		{ Tile::CollisionType::Empty, Tile::ActionType::None, 18, false },
		{ Tile::CollisionType::Spike, Tile::ActionType::None, 31 },
		{ Tile::CollisionType::Spike, Tile::ActionType::None, 32 },
		{ Tile::CollisionType::Spike, Tile::ActionType::None, 33 },
		{ Tile::CollisionType::Spike, Tile::ActionType::None, 34, true },
		{ Tile::CollisionType::Spike, Tile::ActionType::None, 35, true },
		{ Tile::CollisionType::FlippedSemiSolid, Tile::ActionType::None, 36 }
	};

	Map::Map() : size(gs::Vec2i(50, 25)), mapNumber(0), hue(210.0f), 
		tiles(nullptr), blockSize(0) {
	}
	Map::~Map() {
		// Cleans up memory. 
		deallocate(); 
	}

	void Map::create(gs::Vec2i size) {
		// Copy of tiles that is used for resizing the map. 
		Tile::Id* tilesCopy = nullptr; 
		// Copy of the original size when the map is resized. 
		gs::Vec2i sizeCopy; 

		// Ensures size isn't a negative number. Note: This is only really here
		// for when the map is resized as it allows for shrinking. 
		size.x = std::max(size.x, 0); 
		size.y = std::max(size.y, 0); 

		int sizeChangeState = 0; 

		if (size.x > 0) {
			// Constrains the map to a minimum size. 
			size.x = std::max(size.x, 21);
			size.y = std::max(size.y, 10);
		}
		if (this->size.x > 0) 
			sizeChangeState = gs::util::sign(size.x - this->size.x);
		
		// True when map is already allocated. 
		if (blockSize > 0) {
			tilesCopy = new Tile::Id[blockSize]; 
			// Copys elements from the original tiles array to the copy. 
			for (int elementIndex = 0; elementIndex < blockSize; elementIndex++)
				tilesCopy[elementIndex] = tiles[elementIndex]; 
			sizeCopy = this->size; 
		}

		// Only changes size if specifically set. Note: This is because the 
		// size might be preset when being loaded in from a text file. 
		if (size != gs::Vec2i())
			this->size = size; 


		// Makes sure any previous allocation is deleted. 
		deallocate(); 

		blockSize = this->size.x * this->size.y;
		tiles = new Tile::Id[blockSize]; 

		clear(Tile::Air);

		// Copys memory back from original grid when resized. 
		if (tilesCopy != nullptr) {
			for (int xpos = 0; xpos < sizeCopy.x; xpos++) {
				for (int ypos = 0; ypos < sizeCopy.y; ypos++) {
					int xposOffset = 0; 
					int xArrayOffset = 0; 

					if (sizeChangeState != 0) {
						if (xpos > playerLocation.x) {
							if (sizeChangeState > 0)
								xposOffset = 1;
							else
								xArrayOffset = 1;
						}
					}

					setTile(
						{ xpos + xposOffset, ypos }, tilesCopy[(ypos * sizeCopy.x) 
							+ xpos + xArrayOffset]
					); 
				}
			}

			// Clean up copy. Sry no smart pointers today :)
			delete[] tilesCopy; 
		}
	}
	void Map::clear(Tile::Id id) {
		for (int xpos = 0; xpos < size.x; xpos++) {
			for (int ypos = 0; ypos < size.y; ypos++) {
				setTile({ xpos, ypos }, id); 
			}
		}
	}

	void Map::setTile(gs::Vec2i position, Tile::Id id) {
		if (isValidTilePosition(position))
			tiles[getTileArrayCoordinate(position)] = id; 
	}

	Tile::Id Map::getTile(gs::Vec2i position) const {
		if (isValidTilePosition(position))
			return tiles[getTileArrayCoordinate(position)]; 
		return Tile::Invalid; 
	}
	bool Map::isValidTilePosition(gs::Vec2i position) const {
		return position.x >= 0 && position.x < size.x &&
			position.y >= 0 && position.y < size.y; 
	}

	void Map::deallocate() {
		delete[] tiles; 
	}

	int Map::getTileArrayCoordinate(gs::Vec2i position) const {
		// Calculates the coordinate by assuming each row moves from the top
		// left to the bottom right. 
		return (position.y * size.x) + position.x;
	}

	Map maps[mNumOfMaps];
	Map* currentMap = nullptr; 
	gs::Vec2i playerLocation; 

	void loadMaps() {
		for (int mapIndex = 0; mapIndex < mNumOfMaps; mapIndex++) {
			Map& currentMap = maps[mapIndex]; 

			currentMap.mapNumber = mapIndex; 
			loadMap(currentMap); 
		}
	}
	void saveMaps() {
		for (int mapIndex = 0; mapIndex < mNumOfMaps; mapIndex++) {
			Map& currentMap = maps[mapIndex]; 

			saveMap(currentMap); 
		}
	}
}