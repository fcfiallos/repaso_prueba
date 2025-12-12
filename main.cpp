#include <iostream>
#include <omp.h>
#include <fmt/core.h>
#include <SFML/Graphics.hpp>

#ifdef _WIN32
#include <windows.h>
#endif

#include "escala_grises.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define WIDTH 1600
#define HEIGHT 900
#define RED 0.21f
#define GREEN 0.72f
#define BLUE 0.07f

// asteristo para apuntar a la direccion de memoria
uint8_t *imagen_buffer = nullptr;
uint8_t *imagen_filter_buffer = nullptr;
uint8_t *imagen_filter_buffer_texture = nullptr;

int width, height, canales = 4;

void create_gray_texture()
{
    // Crear la textura
    for (int i = 0, j = 0; i < width * height; i++, j += 4)
    {
        uint8_t gray = imagen_filter_buffer[i];
        imagen_filter_buffer_texture[j + 0] = gray; // R
        imagen_filter_buffer_texture[j + 1] = gray; // G
        imagen_filter_buffer_texture[j + 2] = gray; // B
        imagen_filter_buffer_texture[j + 3] = 255;  // A
    }
}

void filter_open_mp()
{
    std::memset(imagen_filter_buffer, 0, width * height);
    std::memset(imagen_filter_buffer_texture, 0, width * height * 4);

    // Llamar a la función de escala_grises.cpp
    gray_scale_open_mp(imagen_buffer, imagen_filter_buffer, width, height, canales);

    create_gray_texture();
}

void filter_simd()
{
    std::memset(imagen_filter_buffer, 0, width * height);
    std::memset(imagen_filter_buffer_texture, 0, width * height * 4);

    // Llamar a la función de escala_grises.cpp
    gray_scale_simd(imagen_buffer, imagen_filter_buffer, width, height, canales);

    create_gray_texture();
}

int main()
{
#pragma omp parallel // define una sección paralela
    {
#pragma omp master
        {
            int num_threads = omp_get_num_threads(); // obtiene el número de hilos
            fmt::println("Number of threads: {}", num_threads);
            std::cout.flush();
        }
    }

    // leer imagen en formato RGBA
    image_buffer = stbi_load("image01.jpg", &width, &height, &channels, STBI_rgb_alpha);

    channels = 4;

    int buffer_size = width * height * channels;

    int sizeOut = height * width * 1;
    image_filter_buffer = new uint8_t[sizeOut];           
    image_filter_buffer_texture = new uint8_t[buffer_size]; 
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Escala de Grises - OpenMP y SIMD");
#ifdef _WIN32
    HWND hwnd = window.getNativeHandle();
    ShowWindow(hwnd, SW_MAXIMIZE);
#endif

    sf::Texture texture ({width, height});
    texture.update(imagen_buffer);
    sf::Sprite sprite (texture);

    float scaleX = 

    return 0;
}