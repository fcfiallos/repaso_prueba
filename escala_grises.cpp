#include <iostream>
#include <omp.h>
#include <immintrin.h>
#include <cstdint>
#include <cstring>
#include <cmath>

#include "escala_grises.h"

extern float RED;
extern float GREEN;
extern float BLUE;

void create_gray_texture(uint8_t *buffer, int width, int height, uint8_t *texture_buffer)
{
    for (int i = 0; i < width * height; ++i)
    {
        uint8_t gray_value = buffer[i * 4];
        texture_buffer[i * 4 + 0] = gray_value;
        texture_buffer[i * 4 + 1] = gray_value;
        texture_buffer[i * 4 + 2] = gray_value;
        texture_buffer[i * 4 + 3] = 255;
    }
}

void escala_grises_simd(uint8_t *entrada, int width, int height, uint8_t *salida, int canales)
{
    int total_pixels = width * height;
    int i = 0;

    __m256 red_coeff = _mm256_set1_ps(RED);
    __m256 green_coeff = _mm256_set1_ps(GREEN);
    __m256 blue_coeff = _mm256_set1_ps(BLUE);

    int gray_offset = 0;
    int max_width = (width / 8) * 8;

    for (int y = 0; y < height; ++y)
    {
        for (i = 0; i < max_width; i += 8)
        {
            int idx = (y * width + i) * canales;

            __m256 r = _mm256_set_ps(
                entrada[idx + 0],
                entrada[idx + canales + 0],
                entrada[idx + 2 * canales + 0],
                entrada[idx + 3 * canales + 0],
                entrada[idx + 4 * canales + 0],
                entrada[idx + 5 * canales + 0],
                entrada[idx + 6 * canales + 0],
                entrada[idx + 7 * canales + 0]);
            __m256 g = _mm256_set_ps(
                entrada[idx + 1],
                entrada[idx + canales + 1],
                entrada[idx + 2 * canales + 1],
                entrada[idx + 3 * canales + 1],
                entrada[idx + 4 * canales + 1],
                entrada[idx + 5 * canales + 1],
                entrada[idx + 6 * canales + 1],
                entrada[idx + 7 * canales + 1]);

            __m256 b = _mm256_set_ps(
                entrada[idx + 2],
                entrada[idx + canales + 2],
                entrada[idx + 2 * canales + 2],
                entrada[idx + 3 * canales + 2],
                entrada[idx + 4 * canales + 2],
                entrada[idx + 5 * canales + 2],
                entrada[idx + 6 * canales + 2],
                entrada[idx + 7 * canales + 2]);

            __m256 gray = _mm256_add_ps(
                _mm256_add_ps(
                    _mm256_mul_ps(r, red_coeff),
                    _mm256_mul_ps(g, green_coeff)),
                _mm256_mul_ps(b, blue_coeff));

            float gray_array[8];
            _mm256_storeu_ps(gray_array, gray);
            gray_offset = (y * width) + i;
            for (int j = 0; j < 8; ++j)
            {
                salida[gray_offset + j] = gray_array[7 - j];
            }
        }

        // Procesar los píxeles restantes
        for (int row = 0; row < height; row++)
        {
            for (int col = max_width; col < width; col++)
            {
                int idx = (row * width + col) * canales;
                float r = entrada[idx + 0];
                float g = entrada[idx + 1];
                float b = entrada[idx + 2];

                uint8_t gray = r * RED + g * GREEN + b * BLUE;
                salida[row * width + col] = gray;
            }
        }
        create_gray_texture(salida, width, height, salida);
    }
}

void escala_grises_omp(uint8_t *entrada, int width, int height, uint8_t *salida, int canales)
{
#pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        int num_threads = omp_get_num_threads();

        int delta = std::ceil(height * 1.0 / num_threads);
        int start = thread_id * delta;
        int end = (thread_id + 1) * delta;
        if (end > height)
            end = height;

        for (int y = start; y < end; y++)
        {
            for (int x = 0; x < width; x++)
            {
                int idx = (y * width + x) * canales;
                float r = entrada[idx + 0];
                float g = entrada[idx + 1];
                float b = entrada[idx + 2];

                uint8_t gray = r * RED + g * GREEN + b * BLUE;
                salida[y * width + x] = gray;
            }
        }
    }
    create_gray_texture(salida, width, height, salida);
}