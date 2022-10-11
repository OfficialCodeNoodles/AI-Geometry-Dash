#include "../hdr/gameEngine.hpp"

//Globals

int ticks = 0;
int levelIndex = 0; 
int attempt = 0; 
int generation = 0; 
float farthestDistance = 0.0f; 

namespace gameEngine {
	gs::util::Clock timer;

	void create() {
		// Removes console. 
		HWND hwnd = GetConsoleWindow();
		ShowWindow(hwnd, 0);

		srand(time(0));
		engine::render::window::create(true); 
		engine::input::initKeys(); 
		engine::render::loadAssets(); 
		engine::render::ui::loadAssets(); 
		engine::loadMaps(); 
		engine::prepareMap(levelIndex); 

		engine::player.textureIndex = 0;
		engine::player.primaryHue = 60.0f; 
		engine::player.secondaryHue = 180.0f; 

		engine::render::ui::simulationRunning = false; 
	}
	void close() {
#ifdef mDebug
		engine::saveMaps(); 
#endif

		engine::render::window::close(); 
	}

	void updateEvents() {
		engine::render::window::update(); 
		engine::input::updateKeys(); 

		// Doesn't update anything if the window isn't being focused on. 
		if (!engine::render::window::winmain->hasFocus())
			return;

		engine::render::ui::updateUI(); 
		if (!engine::render::panToFarthestNpc)
			engine::player.update(); 
		if (engine::render::ui::simulationRunning)
			engine::updateNpcs(); 

		// Npc farthest from the start of the map. 
		const std::pair<engine::Player, engine::NeuralNet>* farthestNpc = 
			engine::getFarthestNpc();
		// This is the poi (point of interest) player that is being focused on.
		const engine::Player& poi = !engine::render::panToFarthestNpc || 
			!engine::render::ui::simulationRunning
				|| farthestNpc == nullptr ?
					engine::player : farthestNpc->first; 

		engine::render::applyCamera(poi); 
		engine::render::updateCamera(); 
		engine::updateParticles(); 
		engine::render::updateColor(poi); 

#ifdef mDebug
		if (engine::input::isKeyClicked(engine::input::EnableDebugMode)) 
			engine::debugMode = !engine::debugMode; 

		if (engine::debugMode) 
			engine::render::ui::updateDebugMenu(engine::player); 
#endif 
	}
	void updateGraphics() {
		// Npc farthest from the start of the map. 
		const std::pair<engine::Player, engine::NeuralNet>* farthestNpc = 
			engine::getFarthestNpc();

		engine::render::window::begin(gs::Color::White);

		engine::render::renderBackground(*engine::currentMap); 
		engine::render::renderGround(*engine::currentMap); 
		engine::render::renderMap(*engine::currentMap); 
		engine::render::renderParticles(); 
		if (engine::render::ui::simulationRunning)
			engine::render::renderNpcs(); 
		if (!engine::render::panToFarthestNpc)
			engine::render::renderPlayer(engine::player); 
		engine::render::renderPortalFronts(); 
		engine::render::ui::renderVignette(); 
		if (engine::render::ui::simulationRunning && 
			engine::render::ui::displayGeneration
		)
			engine::render::ui::renderNeuralNet(
				farthestNpc == nullptr ? engine::npcs[0].second : 
					farthestNpc->second
			); 
		engine::render::ui::renderUI(); 

#ifdef mDebug
		if (engine::debugMode)
			engine::render::ui::renderDebugMenu(); 
#endif

		engine::render::window::end(); 
	}

	void update() {
		timer.begin(); 

		updateEvents();

		if (engine::render::window::renderFramerate !=
			engine::render::window::eventFramerate
		) {
			if (ticks % (engine::render::window::eventFramerate /
				engine::render::window::renderFramerate) == 0
			)
				updateGraphics(); 
		}
		else
			updateGraphics(); 

		timer.end(); 
		timer.wait(engine::render::window::eventFramerate);
		ticks++; 
	}

	bool isOpen() {
		return engine::render::window::winmain->isOpen(); 
	}
}