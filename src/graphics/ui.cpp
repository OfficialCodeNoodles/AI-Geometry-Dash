#include "../../hdr/graphics/ui.hpp"
#include "../../hdr/logic/input.hpp"
#include "../../hdr/logic/collision.hpp"

namespace engine {
	namespace render {
		namespace ui {
			sf::Font font;
			sf::Text text;
			sf::Texture tVignette;
			sf::Sprite sVignette;

			void loadAssets() {
				// Loads the font.
				if (font.loadFromFile(mAssetDirectory + "font.otf"))
					text.setFont(font); 
				// Loads the vignette. 
				if (tVignette.loadFromFile(mAssetDirectory + "vignette.png"))
					sVignette.setTexture(tVignette); 

				handleAssets(); 
			}
			void handleAssets() {

			}

			Tile::Id mouseTile = Tile::Air + 1;
			gs::Vec2i mouseTileLocation;
			bool displayInfoMenu = true; 
			bool displayGeneration = true;
			bool simulationRunning = true; 

			void updateUI() {
				if (input::isKeyClicked(input::StartStopSimulation)) {
					simulationRunning = !simulationRunning;
					
					if (!simulationRunning)
						panToFarthestNpc = false; 
				}
				if (input::isKeyClicked(input::PanToNpcs))
					panToFarthestNpc = !panToFarthestNpc; 
				if (input::isKeyClicked(input::HideInfo))
					displayGeneration = !displayGeneration; 
				if (input::isKeyClicked(input::ResetSimulation))
					resetSimulation(); 
				if (input::isKeyClicked(input::ChangeFramerate))
					window::renderFramerate = window::renderFramerate == 30 ?
						60 : 30; 
				if (input::isKeyClicked(input::ChangeQuickStop))
					endGenerationEarly = !endGenerationEarly; 

				if (input::isKeyClicked(input::HideInfoMenu))
					displayInfoMenu = !displayInfoMenu; 

				if (input::isKeyClicked(input::IncLevel)) {
					levelIndex++;
					levelIndex %= mNumOfMaps;
					prepareMap(levelIndex); 
				}
				if (input::isKeyPressed(input::IncHue)) {
					currentMap->hue++;
					currentMap->hue = std::fmod(currentMap->hue, 360.0f); 
				}
			}
			void updateDebugMenu(const Player& player) {
				const gs::Vec2f globalMousePosition = gs::Vec2f(
					gs::input::mousePosition.x, gs::input::mousePosition.y
				) + (cameraPosition * tileSize);

				mouseTileLocation.x = globalMousePosition.x / tileSize; 
				mouseTileLocation.y = globalMousePosition.y / tileSize; 

				// Scrolls through tiles using the mouse wheel delta. 
				mouseTile += window::mouseDelta; 
				// Constrains mouseTile to only valid tiles. 
				gs::util::clamp(&mouseTile, Tile::Air + 1, Tile::End - 1); 

				if (gs::input::activeMouseClickR)
					currentMap->setTile(mouseTileLocation, Tile::Air);
				else if (gs::input::activeMouseClickL)
					currentMap->setTile(mouseTileLocation, mouseTile); 
				
				// Changes the tile to match what is being hovered over when R 
				// is pressed. Acts as a dropper. 
				if (input::isKeyClicked(input::Key::PickTile)) {
					const Tile::Id selectedTile = 
						currentMap->getTile(mouseTileLocation); 

					if (selectedTile != Tile::Air)
						mouseTile = selectedTile; 
				}

				// Controls the size of the map. 
				if (input::isKeyClicked(input::ExpandLevelX))
					currentMap->create(currentMap->size + gs::Vec2i(1, 0));
				else if (input::isKeyClicked(input::ExpandLevelY))
					currentMap->create(currentMap->size + gs::Vec2i(0, 1));
				else if (input::isKeyClicked(input::ShrinkLevelX))
					currentMap->create(currentMap->size + gs::Vec2i(-1, 0));
				else if (input::isKeyClicked(input::ShrinkLevelY))
					currentMap->create(currentMap->size + gs::Vec2i(0, -1)); 

				playerLocation = collision::getPlayerTile(player); 
			}

			void renderText(
				const std::string& string, gs::Vec2f position, float scale, 
				gs::Color fillColor, float outlineThickness, 
				gs::Color outlineColor
			) {
				text.setString(string); 
				text.setPosition(position); 
				text.setScale(scale, scale); 
				text.setFillColor(fillColor); 
				text.setOutlineThickness(outlineThickness); 
				text.setOutlineColor(outlineColor); 

				window::winmain->draw(text); 
			}
			void renderUI() {
				renderText(
					"Attempt " + std::to_string(attempt), gs::Vec2f(800.0f, 
						(currentMap->size.y - 6) * tileSize) - 
							(cameraPosition * tileSize), 2.5f, 
					gs::Color::White, 1.0f
				);

				// Only displays this information if the simulation is running
				// and is set to be displayed. 
				if (simulationRunning && displayGeneration) {
					renderText(
						"Generation " + std::to_string(generation),
						gs::Vec2f(500.0f, 80.0f), 1.5f, gs::Color::White, 1.0f
					);
					renderText(
						"Attempts " + std::to_string(attempt),
						gs::Vec2f(500.0f, 120.0f), 1.5f, gs::Color::White, 1.0f
					);
					renderText(
						"Best Attempt " + std::to_string(static_cast<int>(
							(farthestDistance / currentMap->size.x) * 100.0f)
						) + "%",
						gs::Vec2f(500.0f, 160.0f), 1.5f, gs::Color::White, 1.0f
					);
				}

				if (displayInfoMenu) {
					renderText(
						"Misc Controls\
						\n\n F1 - Pause/Unpause Sim\
						\n F2 - Change POV\
						\n F3 - Show Simulation\n  Information\
						\n F4 - Restart Sim\
						\n F5 - Change Framerate\
						\n F6 - End Generation\n  Early\
						\n F11 - Enter/Exit\n  Fullscreen\
						\n\n Space - Jump\
						\n Escape - Close App\
						\n H - Hide Menu\
						\n\nEditor Controls\
						\n\n Q - Enter Editor Mode\
						\n Arrow Keys - Alter\n  Level Size\
						\n R - Get Selected Tile\
						\n Enter - Change Level\
						\n + - Increase Background\n  Hue",
						gs::Vec2f(1380.0f, 80.0f), 1.25f, gs::Color::White, 1.0f
					);
				}
			}
			void renderVignette() {
				window::winmain->draw(sVignette); 
			}
			void renderDebugMenu() {
				const gs::Color originalTileColor = sTile.getColor(); 
				
				// Makes the tile translucent. 
				sTile.setColor(gs::Color(
					originalTileColor.r, originalTileColor.g,
					originalTileColor.b, 127
				));

				// Normal tile rendering. 
				if (Tile::getCollisionType(mouseTile) != 
					Tile::CollisionType::Portal
				)
					renderTile(mouseTileLocation, mouseTile); 
				// Code for rendering portals. 
				else {
					PortalPair portalPair = { mouseTileLocation, mouseTile }; 

					renderPortalBack(portalPair); 
					renderPortalFront(portalPair); 
				}
				
				// Returns tile to the original color. 
				sTile.setColor(originalTileColor); 

				renderText("Editor Mode", gs::Vec2f(20.0f, 1010.0f), 1.5f); 
			}
			void renderNeuralNet(const NeuralNet& neuralNet) {
				static sf::CircleShape neuronCircle;
				static auto calculateDistributionPosition = [](
					int index, int total, float span
				) {
					float gap = span / (total + 1.0f);
					return gap * (index + 1) - (span / 2.0f);
				};
				static auto getNeuronPosition = [](
					int layerIndex, int neuronIndex, int neuronTotal
				) {
					return gs::Vec2f(
						(layerIndex * 180.0f) + 60.0f, 150.0f +
						calculateDistributionPosition(
							neuronIndex, neuronTotal, 55.0f * (neuronTotal + 1)
						) 
					);
				};

				neuronCircle.setRadius(25.0f);
				neuronCircle.setOrigin(
					neuronCircle.getRadius(), neuronCircle.getRadius()
				);

				for (int layerIndex = 0; layerIndex < 3; layerIndex++) {
					const Layer& layer = neuralNet.layers[layerIndex];

					for (int neuronIndex = 0; neuronIndex < layer.neurons.size();
						neuronIndex++
					) {
						const Neuron& neuron = layer.neurons[neuronIndex];
						const gs::Vec2f position = getNeuronPosition(
							layerIndex, neuronIndex, layer.neurons.size()
						);

						if (layerIndex < 2) {
							for (int nextNeuronIndex = 0; nextNeuronIndex <
								neuralNet.layers[layerIndex + 1].neurons.size();
								nextNeuronIndex++
							) {
								const gs::Vec2f nextPosition = getNeuronPosition(
									layerIndex + 1, nextNeuronIndex,
									neuralNet.layers[layerIndex + 1].neurons.size()
								);

								renderLine(
									position, nextPosition, 5.0f,
									// Alters color based on the weight of the
									// connection.
									gs::util::approach(
										gs::Color::White, gs::Color::Black, 
										40.0f - (neuron.weights[nextNeuronIndex] * 40.0f)
									)
								);
							}
						}

						neuronCircle.setPosition(position);
						neuronCircle.setFillColor(gs::util::approach(
							gs::Color::White, gs::Color::Red, 
							(std::max(neuron.value, 0.0f) - 0.5f) * 2.0f * 100.0f
						)); 

						window::winmain->draw(neuronCircle);
					}
				}
			}
		}
	}
}