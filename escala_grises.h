#ifndef ESCALA_GRISES_H
#define ESCALA_GRISES_H

#include <cstdint>

void escala_grises_simd(uint8_t* entrada,  int width, int height, uint8_t* salida, int canales);
void escala_grises_omp(uint8_t* entrada,  int width, int height, uint8_t* salida, int canales);
void update_gray_texture(uint8_t* buffer, int width, int height, uint8_t* texture_buffer);
#endif