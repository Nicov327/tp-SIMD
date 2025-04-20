#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "stb_image.h"
#include "stb_image_write.h"

/**
 * Forma de especificarle a `load_image` que no nos importa la cantidad de
 * canales de una imagen.
 */
#define ANY_CHANNEL_COUNT 0
/**
 * Cantidad de canales en un mapa de profundidad
 */
#define DEPTH_CHANNEL_COUNT 1
/**
 * Cantidad de canales en una imagen RGBA
 */
#define RGBA_CHANNEL_COUNT 4

/**
 * Una imagen.
 *
 * Campos:
 *   - width:                  El ancho de la imagen. Es positivo.
 *   - height:                 El alto de la imagen. Es positivo.
 *   - original_channel_count: La cantidad de canales de la imagen fuente.
 *   - channel_count:          La cantidad de canales (de 8 bits) cargados a
 *                             memoria.
 *   - data:                   Un buffer de (width * height * channel_count)
 *                             bytes que tiene los píxeles de la imagen.
 */
typedef struct img {
	int width;
	int height;
	int original_channel_count;
	int channel_count;
	uint8_t* data;
} img_t;

/**
 * Carga una imagen.
 *
 * Parámetros:
 *   - filepath:         Ruta del sistema de archivos en dónde se encuentra la
 *                       imagen.
 *   - desired_channels: Cantidad de canales a esperar encontrar en la imagen.
 *                       Cero representa _"no sé"_.
 */
static
img_t load_image(const char* filepath, int desired_channels) {
	img_t result;
	result.data = stbi_load(
		filepath,
		&result.width,
		&result.height,
		&result.original_channel_count,
		desired_channels
	);
	result.channel_count = desired_channels;
	return result;
}

/**
 * Tamaño máximo soportado para rutas del sistema de archivos
 */
#define MAX_FILEPATH_SIZE 127

/**
 * Construye una ruta en el sistema operativo por medio de agregarle un prefijo
 * y un sufijo a un nombre dado.
 *
 * Parámetros:
 *   - filepath: Buffer en dónde escribir el resultado.
 *   - prefix:   Texto a poner al principio de la ruta.
 *   - name:     Texto a poner en el medio de la ruta. Es algún tipo de nombre.
 *   - suffix:   Texto a poner al final de la ruta. Normalmente es una
 *               extensión.
 */
static
void build_filepath(char filepath[MAX_FILEPATH_SIZE + 1], const char* prefix, const char* name, const char* suffix) {
	filepath[0] = 0;
	strncat(filepath, prefix, MAX_FILEPATH_SIZE);
	strncat(filepath, name, MAX_FILEPATH_SIZE);
	strncat(filepath, suffix, MAX_FILEPATH_SIZE);
}

/**
 * Cuenta cuántos tests corrieron exitosamente.
 */
extern uint64_t successful_tests;
/**
 * Cuenta cuántos tests test fallaron.
 */
extern uint64_t failed_tests;

/**
 * El mensaje [DONE] escrito en verde.
 */
#define DONE "[\033[32;1mDONE\033[0m] "

/**
 * El mensaje [FAIL] escrito en rojo.
 */
#define FAIL "[\033[31;1mFAIL\033[0m] "

/**
 * El mensaje [SKIP] escrito en magenta.
 */
#define SKIP "[\033[95;1mSKIP\033[0m] "

/**
 * Compara un resultado con la imagen esperada. Devuelve `true` si encuentra
 * diferencias.
 *
 * Parámetros:
 *   - ex:          Nombre del ejercicio.
 *   - name:        Nombre del test.
 *   - buffer:      Resultado a comparar.
 *   - buffer_size: Tamaño en bytes del resultado a comparar.
 */
static
bool has_differences(const char* ex, const char* name, void* buffer, uint64_t buffer_size, uint64_t tolerance) {
	bool result = false;
	char filepath[MAX_FILEPATH_SIZE + 1];
	snprintf(filepath, sizeof(filepath), "expected/%s_%s.png", ex, name);

	img_t expected = load_image(filepath, ANY_CHANNEL_COUNT);
	uint64_t expected_data_size = expected.width * expected.height * expected.original_channel_count;

	if (buffer_size != expected_data_size) {
		result = true;
	} else if (tolerance == 0 && memcmp(buffer, expected.data, buffer_size) != 0) {
		result = true;
	} else if (tolerance != 0) {
		rgba_t* expected_img = (rgba_t*) expected.data;
		rgba_t* actual_img = buffer;
		for (int32_t i = 0; i < expected.height * expected.width; i++) {
			bool red_differs   = tolerance < abs(expected_img[i].r - actual_img[i].r);
			bool green_differs = tolerance < abs(expected_img[i].g - actual_img[i].g);
			bool blue_differs  = tolerance < abs(expected_img[i].b - actual_img[i].b);
			bool alpha_differs = tolerance < abs(expected_img[i].a - actual_img[i].a);
			if (red_differs || green_differs || blue_differs || alpha_differs) {
				result = true;
				break;
			}
		}
	}

	stbi_image_free(expected.data);
	return result;
}

