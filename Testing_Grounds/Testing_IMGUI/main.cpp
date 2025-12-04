#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "imgui-SFML.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define RADIUS 100.f
// using namespace ImGui::SFML;
#define CLEAR_COLOUR sf::Color(50, 50, 50)
int main() {
    sf::RenderWindow window(sf::VideoMode({SCREEN_WIDTH, SCREEN_HEIGHT}, 10), "Fluid Simulation");
    window.setFramerateLimit(60);
    ImGui::SFML::Init(window);
    float radius = RADIUS;
    sf::CircleShape shape(radius);
    shape.setFillColor(sf::Color::Green);
    shape.setOrigin({radius, radius});
    shape.setPosition({(SCREEN_WIDTH/2 ) , (SCREEN_HEIGHT/2 )});
    int dummy_iterations = 30;

    bool drawCircle = true;



    sf::Clock deltaClock;

    while (window.isOpen()) {
        while(const std::optional<sf::Event> event = window.pollEvent()) {
            ImGui::SFML::ProcessEvent(window, *event);
            if(event->is<sf::Event::Closed>()) {
                window.close();
            }
            else if(const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if(keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
                    window.close();
                }
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

            ImGui::Begin("Hello, world!");
            ImGui::Text("This is some useful text.");
            ImGui::Checkbox("Draw Circle", &drawCircle);
            ImGui::InputInt("Dummy Iterations", &dummy_iterations);
            ImGui::SliderFloat("Radius", &radius, 10.f, 300.f);
            shape.setRadius(radius);
            shape.setOrigin({radius, radius});
            ImGui::End();


        window.clear(CLEAR_COLOUR);
        if (drawCircle) {
            window.draw(shape);
        }

        ImGui::SFML::Render(window);
        window.display();
    }


    ImGui::SFML::Shutdown();
    return 0;
}
