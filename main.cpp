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

int canales = 4;

int main()
{

// #ifdef _WIN32
//     HWND hwnd = window.getNativeHandle();
//     ShowWindow(hwnd, SW_MAXIMIZE);
// #endif


    return 0;
}