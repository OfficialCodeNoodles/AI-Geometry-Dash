#include "../../hdr/graphics/window.hpp"

namespace engine {
	namespace render {
		namespace window {
			const gs::Vec2i defaultWindowSize = gs::Vec2i(1920, 1080);
			const gs::Vec2f screenCenter = gs::Vec2f(
				1920.0f / 2.0f, 1080.0f / 2.0f
			);

			sf::RenderWindow* winmain = nullptr; 
			bool isFullscreen = false;

			int eventFramerate = 60;
			int renderFramerate = 60;

			int mouseDelta = 0;
			sf::Image icon;

			void create(bool fullscreen) {
				isFullscreen = fullscreen; 

				if (winmain == nullptr) {
					winmain = new sf::RenderWindow(
						sf::VideoMode(
							defaultWindowSize.x,
							defaultWindowSize.y
						), mTitle, mGetStyle(isFullscreen)
					);
				}
				else {
					winmain->create(
						sf::VideoMode(
							defaultWindowSize.x,
							defaultWindowSize.y
						), mTitle, mGetStyle(isFullscreen)
					);
				}

				handleStates(); 
			}
			void handleStates() {
				const sf::VideoMode desktopMode = 
					sf::VideoMode::getDesktopMode();

				gs::input::setWindow(winmain); 

				// Resizes window to size of desktop. 
				winmain->setSize(gs::Vec2u(
					desktopMode.width, desktopMode.height
				));

				if (icon.getSize().x == 0)
					icon.loadFromFile(mAssetDirectory + "icon.png"); 

				winmain->setIcon(
					icon.getSize().x, icon.getSize().y, icon.getPixelsPtr()
				);
			}
			void update() {
				sf::Event action; 

				gs::input::updateInputs(); 

				mouseDelta = 0; 

				while (winmain->pollEvent(action)) {
					gs::input::updateEvents(action); 

					switch (action.type) {
					case sf::Event::Closed:
						winmain->close(); 
						break; 
					case sf::Event::MouseWheelScrolled:
						mouseDelta = action.mouseWheelScroll.delta; 
						break; 
					case sf::Event::KeyPressed:
						switch (action.key.code) {
						case sf::Keyboard::Escape:
							winmain->close();
							break;
						case sf::Keyboard::F11:
							create(!isFullscreen);
							break; 
						}
						break; 
					}
				}
			}
			void close() {
				delete winmain; 
			}

			void begin(gs::Color backgroundColor) {
				winmain->clear(backgroundColor); 
			}
			void end() {
				winmain->display(); 
			}
		}
	}
}