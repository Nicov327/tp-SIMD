#pragma once
#include <stdint.h>

/**
 * Un píxel RGBA.
 *
 * Campos:
 *   - r: La cantidad de rojo en el píxel. Va de 0 a 255.
 *   - g: La cantidad de verde en el píxel. Va de 0 a 255.
 *   - b: La cantidad de azul en el píxel. Va de 0 a 255.
 *   - a: La transparencia del píxel. Va de 0 a 255.
 */
typedef struct rgba_pixfmt {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
} rgba_t;
