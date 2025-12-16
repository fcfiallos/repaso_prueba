#include <iostream>
#include <omp.h>
#include <fmt/core.h>
#include <SFML/Graphics.hpp>
#include <immintrin.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include "escala_grises.h"
#include "arial.ttf.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define WIDTH 1600
#define HEIGHT 900
float RED = 0.21f;
float GREEN = 0.72f;
float BLUE = 0.07f;

uint8_t *imagen_buffer = nullptr;
uint8_t *imagen_filter_buffer = nullptr;
uint8_t *imagen_filter_buffer_texture = nullptr;

int canales = 4;
int main()
{
    int width;
    int height;
    imagen_buffer = stbi_load("img.jpg", &width, &height, &canales, STBI_rgb_alpha);

    canales = 4;
#pragma omp parallel
    {
#pragma omp master
        {
            int num_threads = omp_get_num_threads();
            int thread_id = omp_get_thread_num();
            fmt::println("Numero de hilos: {}, ID del hilo master: {}", num_threads, thread_id);
        }
    }

    imagen_filter_buffer = new uint8_t[width * height * 1];
    imagen_filter_buffer_texture = new uint8_t[width * height * 4];

    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(sf::VideoMode({WIDTH, HEIGHT}), "Repaso Prueba SIMD-OMP");

#ifdef _WIN32
    HWND hwnd = window.getNativeHandle();
    ShowWindow(hwnd, SW_MAXIMIZE);
#endif

    sf::Texture texture({(unsigned int)width, (unsigned int)height});
    texture.update((const uint8_t *)imagen_buffer);
    sf::Sprite sprite(texture);

    const sf::Font font(arial_ttf, sizeof(arial_ttf));
    sf::Text text(font, "Repaso Prueba", 24);
    text.setFillColor(sf::Color::White);
    text.setPosition({10, 10});
    text.setStyle(sf::Text::Bold);

    sf::Text modeText(font, "", 20);
    modeText.setFillColor(sf::Color::White);
    modeText.setPosition({10, 50});

    sf::Text instructionsText(font, "Presiona 1 para Original, 2 para SIMD, 3 para OMP, 4 para guardar imagen", 16);
    instructionsText.setFillColor(sf::Color::White);
    instructionsText.setPosition({10, 80});

    float x_scale = WIDTH * 1.0 / width;
    float y_scale = HEIGHT * 1.0 / height;
    sprite.setScale({x_scale, y_scale});

    // FPS
    sf::Clock clock;
    float fps = 0.0f;
    int fps_frames = 0;

    std::string mode = "";
    while (window.isOpen())
    {
        // Process events
        while (const std::optional event = window.pollEvent())
        {
            // Close window: exit
            if (event->is<sf::Event::Closed>())
                window.close();
            else if (event->is<sf::Event::KeyReleased>())
            {
                auto keyEvent = event->getIf<sf::Event::KeyReleased>();
                switch (keyEvent->scancode)
                {
                case sf::Keyboard::Scan::Num1:
                    mode = "Original";
                    texture.update(imagen_buffer);
                    break;
                case sf::Keyboard::Scan::Num2:
                    mode = "SIMD";
                    escala_grises_simd(imagen_buffer, width, height, imagen_filter_buffer, canales);
                    update_gray_texture(imagen_filter_buffer, width, height, imagen_filter_buffer_texture);
                    texture.update(imagen_filter_buffer_texture);
                    break;

                case sf::Keyboard::Scan::Num3:
                    mode = "OMP";
                    escala_grises_omp(imagen_buffer, width, height, imagen_filter_buffer, canales);
                    update_gray_texture(imagen_filter_buffer, width, height, imagen_filter_buffer_texture);
                    texture.update(imagen_filter_buffer_texture);
                    break;
                case sf::Keyboard::Scan::Num4:
                {
                    std::string filename = "output_" + mode + ".png";
                    stbi_write_png(filename.c_str(), width, height, STBI_grey, imagen_filter_buffer, width * 1);
                    fmt::println("Imagen guardada en {}", filename);
                    break;
                }
                }
            }
        }
        // Actualizar texto de modo y FPS
        modeText.setString(fmt::format("Modo: {}, FPS: {}", mode, (int)fps));
        // contar FPS
        fps_frames++;
        if (clock.getElapsedTime().asSeconds() >= 1.0f)
        {
            fps = fps_frames;
            fps_frames = 0;
            clock.restart();
        }

        window.clear();

        {
            window.draw(sprite);
            window.draw(text);
            window.draw(modeText);
            window.draw(instructionsText);
        }

        window.display();
    }
    delete[] imagen_buffer;
    delete[] imagen_filter_buffer;
    delete[] imagen_filter_buffer_texture;
    return 0;
}