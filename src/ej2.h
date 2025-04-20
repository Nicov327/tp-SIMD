#pragma once
#include "common.h"
#include "stdbool.h"

/**
 * Marca el ejercicio 2 como hecho (`true`) o pendiente (`false`).
 *
 * Funciones a implementar:
 *   - ej2
 */
extern bool EJERCICIO_2_HECHO;

/**
 * Aplica un efecto de "mapa de calor" sobre una imagen dada (`src`). Escribe
 * la imagen resultante en el canvas proporcionado (`dst`).
 *
 * Para calcular el mapa de calor lo primero que hay que hacer es computar la
 * "temperatura" del pixel en cuestión:
 * ```
 * temperatura = (rojo + verde + azul) / 3
 * ```
 *
 * Cada canal del resultado tiene la siguiente forma:
 * ```
 * |          ____________________
 * |         /                    \
 * |        /                      \        Y = intensidad
 * | ______/                        \______
 * |
 * +---------------------------------------
 *              X = temperatura
 * ```
 *
 * Para calcular esta función se utiliza la siguiente expresión:
 * ```
 * f(x) = min(255, max(0, 384 - 4 * |x - 192|))
 * ```
 *
 * Cada canal esta offseteado de distinta forma sobre el eje X, por lo que los
 * píxeles resultantes son:
 * ```
 * temperatura  = (rojo + verde + azul) / 3
 * salida.rojo  = f(temperatura)
 * salida.verde = f(temperatura + 64)
 * salida.azul  = f(temperatura + 128)
 * salida.alfa  = 255
 * ```
 *
 * Parámetros:
 *   - dst:    La imagen destino. Está a color (RGBA) en 8 bits sin signo por
 *             canal.
 *   - src:    La imagen origen A. Está a color (RGBA) en 8 bits sin signo por
 *             canal.
 *   - width:  El ancho en píxeles de `src` y `dst`.
 *   - height: El alto en píxeles de `src` y `dst`.
 */
void ej2(
	rgba_t* dst,
	rgba_t* src,
	uint32_t width, uint32_t height
);
