#ifndef ESCALA_GRISES_H
#define ESCALA_GRISES_H

#include <cstdint>

void escala_grises_simd(uint8_t* entrada,  int width, int height, uint8_t* salida, int canales);
void escala_grises_omp(uint8_t* entrada,  int width, int height, uint8_t* salida, int canales);
#endif