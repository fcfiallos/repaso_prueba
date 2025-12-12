#ifndef ESCALA_GRISES_H
#define ESCALA_GRISES_H

#include <cstdint>

void gray_scale_simd(uint8_t* image_entrada, uint8_t* image_salida, int width, int height, int canales);

void gray_scale_open_mp(uint8_t* image_entrada, uint8_t* image_salida, int width, int height, int canales);

#endif