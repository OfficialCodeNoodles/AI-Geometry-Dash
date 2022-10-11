#pragma once

//Depencendies
#include "../resources.hpp"
#include "file.hpp"

namespace engine {
	// Tile is an information class used to store the properties of different 
	// tiles. 
	struct Tile {
		// Alias for int.
		typedef int Id; 
		enum { 
			Invalid = -1, Air, Square, TallSpike, ShortSpike0, GridSquare0, 
			GridSquare1, GridSquare2, GridSquare3, GridSquare4, GridSquare5, 
			GridSquare6, GridSquare7, GridSquare8, GridSquare9, GridSquare10, 
			GridSquare11, GridSquare12, GridSquare13, GridSquare14, GridSquare15, 
			SemisolidRectangle, SpikeGround0, SpikeGround1, FlippedTallSpike, 
			Chain0, Chain1, Chain2, SpikedSemisolidRectangle, GridSquare16, 
			GridSquare17, GridSquare18, PortalEntrance, PortalExit, JumpPad, 
			JumpOrb, PortalInvert, PortalRevert, GridSquare19, SideSpike0, 
			SideSpike1, ShortSpike1, SpikeGround2, SpikeGround3, 
			FlippedSemisoldRectangle, End
		};

		enum class CollisionType { 
			Empty, Solid, Spike, Semisolid, FlippedSemiSolid, Portal, JumpPad, 
			JumpOrb
		}
			collisionType;
		enum class ActionType { None, Flying, FlipGravity }
			actionType;
		// The index of the tiles texture within the tile-sheet. 
		int textureIndex; 
		bool colorable;

		static CollisionType getCollisionType(Id id); 
		static ActionType getActionType(Id id); 
		static int getTextureIndex(Id id); 
		static bool isColorable(Id id); 
	};

	extern Tile tiles[Tile::End]; 

	class Map {
	public:
		gs::Vec2i size; 
		int mapNumber; 
		// Used to color the background and foreground. 
		float hue; 

		Map();
		~Map(); 

		// Allocates and sets up the map based on a given size. 
		void create(gs::Vec2i size = gs::Vec2i()); 
		// Fills entire map with the given tile id. 
		void clear(Tile::Id id); 

		void setTile(gs::Vec2i position, Tile::Id id); 

		Tile::Id getTile(gs::Vec2i position) const; 
		// Checks weither position is located with the current size of the map.
		bool isValidTilePosition(gs::Vec2i position) const; 

		friend void loadMap(Map& map); 
		friend void saveMap(const Map& map); 
	private:
		// Array of tiles. Note: Since it is 1D all positions must be converted
		// to a 1D coordinate for use. This is what getTileArrayCoordinate() is
		// responsable for doing. 
		Tile::Id* tiles; 
		// Number of tiles located in the map. Note: Is equal to the area. 
		int blockSize; 

		// Deletes the tiles to free up the memort allocated. 
		void deallocate(); 

		// Converts a 2D position in the map to a 1D coordinate to access it in
		// the tiles array. 
		int getTileArrayCoordinate(gs::Vec2i position) const; 
	};

	extern Map maps[mNumOfMaps]; 
	// Map currently being used by the engine. 
	extern Map* currentMap; 
	// Used when changing the size of the map. 
	extern gs::Vec2i playerLocation; 

	void loadMaps(); 
	void saveMaps(); 
}