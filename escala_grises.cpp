#include <iostream>
#include <omp.h>
#include <immintrin.h>
#include <cstdint>
#include <cstring>
#include <cmath>

#include "escala_grises.h"

#define RED_FACTOR 0.21f
#define GREEN_FACTOR 0.72f
#define BLUE_FACTOR 0.07f

void gray_scale_open_mp(uint8_t* image_entrada, uint8_t* image_salida, int width, int height, int canales) {
    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        int num_threads = omp_get_num_threads();

        int delta = std::ceil(height * 1.0 / num_threads);

        int start = thread_id * delta;
        int end = (thread_id + 1) * delta;

        if(end >= height) {
            end = height;
        }

        for (int row = start; row < end; row++) {
            for (int col = 0; col < width; col++) {
                int idx = row * width + col;

                uint8_t r = image_entrada[canales * idx];
                uint8_t g = image_entrada[canales * idx + 1];
                uint8_t b = image_entrada[canales * idx + 2];

                uint8_t gray_scaled = RED_FACTOR * r + GREEN_FACTOR * g + BLUE_FACTOR * b;

                image_salida[idx] = gray_scaled;
            }
        }
    }
}

void gray_scale_simd(uint8_t* image_entrada, uint8_t* image_salida, int width, int height, int canales) {
    __m256 _021 = _mm256_set1_ps(RED_FACTOR);
    __m256 _071 = _mm256_set1_ps(GREEN_FACTOR);
    __m256 _007 = _mm256_set1_ps(BLUE_FACTOR);

    int gray_index = 0;

    // Determinar el max-col
    int max_width = (width / 8) * 8;

    for (int row = 0; row < height; row++) {
        for (int col = 0; col < max_width; col += 8) {
            int idx = canales * (row * width + col);
            
            __m256 red = _mm256_set_ps(
                image_entrada[idx + 0], 
                image_entrada[idx + 4], 
                image_entrada[idx + 8], 
                image_entrada[idx + 12], 
                image_entrada[idx + 16], 
                image_entrada[idx + 20], 
                image_entrada[idx + 24], 
                image_entrada[idx + 28]
            );

            __m256 green = _mm256_set_ps(
                image_entrada[idx + 1], 
                image_entrada[idx + 5], 
                image_entrada[idx + 9], 
                image_entrada[idx + 13], 
                image_entrada[idx + 17], 
                image_entrada[idx + 21], 
                image_entrada[idx + 25], 
                image_entrada[idx + 29]
            );

            __m256 blue = _mm256_set_ps(
                image_entrada[idx + 2], 
                image_entrada[idx + 6], 
                image_entrada[idx + 10], 
                image_entrada[idx + 14], 
                image_entrada[idx + 18], 
                image_entrada[idx + 22], 
                image_entrada[idx + 26], 
                image_entrada[idx + 30]
            );

            __m256 red021 = _mm256_mul_ps(red, _021);
            __m256 green071 = _mm256_mul_ps(green, _071);
            __m256 blue007 = _mm256_mul_ps(blue, _007);

            __m256 gray = _mm256_add_ps(red021, _mm256_add_ps(green071, blue007));

            float d[8];
            _mm256_storeu_ps(d, gray);

            gray_index = (row * width + col);

            for(int i = 0; i < 8; i++) {
                image_salida[gray_index + i] = d[7 - i];
            }
        }
    }

    // Pixeles faltantes
    for (int row = 0; row < height; row++) {
        for (int col = max_width; col < width; col++) {
            int idx = row * width + col;

            uint8_t r = image_entrada[canales * idx];
            uint8_t g = image_entrada[canales * idx + 1];
            uint8_t b = image_entrada[canales * idx + 2];

            uint8_t gray_scaled = RED_FACTOR * r + GREEN_FACTOR * g + BLUE_FACTOR * b;

            image_salida[idx] = gray_scaled;
        }
    }
}

