#include <assert.h>

#include "common-tests.h"
#include "ej3.h"
#include "stb_image.h"
#include "stb_image_write.h"

/**
 * Convierte un mapa de profundidad de 8 bits sin signo a 32 bits con signo.
 *
 * Parámetros:
 *   - dst:    Imagen destino. 32 bits con signo por píxel.
 *   - src:    Imagen fuente. 8 bits sin signo por píxel.
 *   - width:  Ancho de la imagen. Debe ser positivo.
 *   - height: Alto de la imagen. Debe ser positivo.
 */
static
void depth_convert_uint8_to_int32(
	int32_t* dst,
	uint8_t* src,
	uint32_t width, uint32_t height
) {
	for (uint32_t y = 0; y < height; y++) {
		for (uint32_t x = 0; x < width; x++) {
			uint32_t i = y * width + x;
			dst[i] = src[i];
		}
	}
}

/**
 * Corre un test del ejercicio 3A.
 *
 * Parámetros:
 *   - name_out: Nombre del test.
 *   - name:     Nombre de la imagen de entrada.
 *   - scale:    Factor de escalado.
 *   - offset:   Factor de corrimiento.
 */
static
void ej3_do_test_a(
	const char* name_out,
	const char* name, int32_t scale, int32_t offset
) {
	if (!EJERCICIO_3A_HECHO) {
		printf(SKIP "El ejercicio 3A no está hecho aún.\n");
		return;
	}

	char filepath[MAX_FILEPATH_SIZE + 1];
	build_filepath(filepath, "img/depth/", name, ".png");

	char filepath_out[MAX_FILEPATH_SIZE + 1];
	build_filepath(filepath_out, "outputs/ej3_", name_out, ".png");

	img_t depth = load_image(filepath, DEPTH_CHANNEL_COUNT);

	uint32_t width = depth.width;
	uint32_t height = depth.height;

	uint64_t depth_fixed_size = width * height * sizeof(int32_t);
	int32_t* depth_fixed = malloc(depth_fixed_size);

	ej3a(depth_fixed, depth.data, scale, offset, width, height);
	// No existe PNG con 32 bits por canal. Entonces vamos a escribirlo como
	// escala de grises pero 4X el ancho.
	stbi_write_png(
		filepath_out,
		width * sizeof(int32_t), height,
		DEPTH_CHANNEL_COUNT,
		depth_fixed,
		width * sizeof(int32_t)
	);
	if (has_differences("ej3", name_out, depth_fixed, depth_fixed_size, 0)) {
		printf(FAIL "ej3_do_test_a(\"%s\", \"%s\", %d, %d)\n", name_out, name, scale, offset);
		printf(FAIL "  Se encontraron diferencias:\n");
		printf(FAIL "    Imagen de entrada: %s\n", filepath);
		printf(FAIL "    Escala: %d\n", scale);
		printf(FAIL "    Offset: %d\n", offset);
		printf(FAIL "    Imagen de salida: %s\n", filepath_out);
		printf(FAIL "    Imagen de salida esperada: expected/ej3_%s.png\n", name_out);
		printf(FAIL "\n");
		failed_tests++;
	} else {
		printf(DONE "ej3_do_test_a(\"%s\", \"%s\", %d, %d)\n", name_out, name, scale, offset);
		successful_tests++;
	}

	free(depth_fixed);
	stbi_image_free(depth.data);
}

/**
 * Corre un test del ejercicio 3B.
 *
 * Parámetros:
 *   - name_out: Nombre del test.
 *   - name_a:   Nombre de la imagen de entrada A.
 *   - name_b:   Nombre de la imagen de entrada B.
 */
static
void ej3_do_test_b(
	const char* name_out,
	const char* name_a,
	const char* name_b
) {
	if (!EJERCICIO_3B_HECHO) {
		printf(SKIP "El ejercicio 3B no está hecho aún.\n");
		return;
	}

	char filepath_a[MAX_FILEPATH_SIZE + 1];
	build_filepath(filepath_a, "img/", name_a, ".png");

	char filepath_b[MAX_FILEPATH_SIZE + 1];
	build_filepath(filepath_b, "img/", name_b, ".png");

	char depth_filepath_a[MAX_FILEPATH_SIZE + 1];
	build_filepath(depth_filepath_a, "img/depth/", name_a, ".png");

	char depth_filepath_b[MAX_FILEPATH_SIZE + 1];
	build_filepath(depth_filepath_b, "img/depth/", name_b, ".png");

	char filepath_out[MAX_FILEPATH_SIZE + 1];
	build_filepath(filepath_out, "outputs/ej3_", name_out, ".png");

	img_t a = load_image(filepath_a, RGBA_CHANNEL_COUNT);
	img_t b = load_image(filepath_b, RGBA_CHANNEL_COUNT);
	img_t depth_a = load_image(depth_filepath_a, DEPTH_CHANNEL_COUNT);
	img_t depth_b = load_image(depth_filepath_b, DEPTH_CHANNEL_COUNT);

	assert(a.width == b.width && a.width == depth_a.width && a.width == depth_b.width);
	assert(a.height == b.height && a.height == depth_a.height && a.height == depth_b.height);

	uint32_t width = a.width;
	uint32_t height = a.height;
	uint64_t out_size = width * height * sizeof(rgba_t);
	rgba_t* out = malloc(out_size);
	int32_t* depth_a_fixed = malloc(width * height * sizeof(int32_t));
	int32_t* depth_b_fixed = malloc(width * height * sizeof(int32_t));

	depth_convert_uint8_to_int32(depth_a_fixed, depth_a.data, width, height);
	depth_convert_uint8_to_int32(depth_b_fixed, depth_b.data, width, height);
	ej3b(
		out,
		(rgba_t*) a.data, depth_a_fixed,
		(rgba_t*) b.data, depth_b_fixed,
		width, height
	);
	stbi_write_png(
		filepath_out,
		width, height,
		RGBA_CHANNEL_COUNT,
		out,
		width * sizeof(rgba_t)
	);
	if (has_differences("ej3", name_out, out, out_size, 0)) {
		printf(FAIL "ej3_do_test_b(\"%s\", \"%s\", \"%s\")\n", name_out, name_a, name_b);
		printf(FAIL "  Se encontraron diferencias:\n");
		printf(FAIL "    Imagen de entrada A: %s\n", filepath_a);
		printf(FAIL "    Imagen de entrada B: %s\n", filepath_b);
		printf(FAIL "    Mapa de profundidad A: %s\n", depth_filepath_a);
		printf(FAIL "    Mapa de profundidad B: %s\n", depth_filepath_b);
		printf(FAIL "    Imagen de salida: %s\n", filepath_out);
		printf(FAIL "    Imagen de salida esperada: expected/ej3_%s.png\n", name_out);
		printf(FAIL "\n");
		failed_tests++;
	} else {
		printf(DONE "ej3_do_test_b(\"%s\", \"%s\", \"%s\")\n", name_out, name_a, name_b);
		successful_tests++;
	}

	free(out);
	free(depth_a_fixed);
	free(depth_b_fixed);
	stbi_image_free(a.data);
	stbi_image_free(b.data);
	stbi_image_free(depth_a.data);
	stbi_image_free(depth_b.data);
}

/**
 * Corre un test que ejercita tanto el ejercicio 3A como el 3B.
 *
 * Parámetros:
 *   - name_out: Nombre del test.
 *   - name_a:   Nombre de la imagen de entrada A.
 *   - scale_a:  Factor de escalado de la entrada A.
 *   - offset_a: Factor de corrimiento de la entrada A.
 *   - name_b:   Nombre de la imagen de entrada B.
 *   - scale_b:  Factor de escalado de la entrada B.
 *   - offset_b: Factor de corrimiento de la entrada B.
 */
static
void ej3_do_test_ab(
	const char* name_out,
	const char* name_a, int32_t scale_a, int32_t offset_a,
	const char* name_b, int32_t scale_b, int32_t offset_b
) {
	char filepath_a[MAX_FILEPATH_SIZE + 1];
	build_filepath(filepath_a, "img/", name_a, ".png");

	char filepath_b[MAX_FILEPATH_SIZE + 1];
	build_filepath(filepath_b, "img/", name_b, ".png");

	char depth_filepath_a[MAX_FILEPATH_SIZE + 1];
	build_filepath(depth_filepath_a, "img/depth/", name_a, ".png");

	char depth_filepath_b[MAX_FILEPATH_SIZE + 1];
	build_filepath(depth_filepath_b, "img/depth/", name_b, ".png");

	char filepath_out[MAX_FILEPATH_SIZE + 1];
	build_filepath(filepath_out, "outputs/ej3_", name_out, ".png");

	img_t a = load_image(filepath_a, RGBA_CHANNEL_COUNT);
	img_t b = load_image(filepath_b, RGBA_CHANNEL_COUNT);
	img_t depth_a = load_image(depth_filepath_a, DEPTH_CHANNEL_COUNT);
	img_t depth_b = load_image(depth_filepath_b, DEPTH_CHANNEL_COUNT);

	assert(a.width == b.width && a.width == depth_a.width && a.width == depth_b.width);
	assert(a.height == b.height && a.height == depth_a.height && a.height == depth_b.height);

	uint32_t width = a.width;
	uint32_t height = a.height;
	uint64_t out_size = width * height * sizeof(rgba_t);
	rgba_t* out = malloc(out_size);
	int32_t* depth_a_fixed = malloc(width * height * sizeof(int32_t));
	int32_t* depth_b_fixed = malloc(width * height * sizeof(int32_t));

	ej3a(depth_a_fixed, depth_a.data, scale_a, offset_a, width, height);
	ej3a(depth_b_fixed, depth_b.data, scale_b, offset_b, width, height);
	ej3b(
		out,
		(rgba_t*) a.data, depth_a_fixed,
		(rgba_t*) b.data, depth_b_fixed,
		width, height
	);
	stbi_write_png(
		filepath_out,
		width, height,
		RGBA_CHANNEL_COUNT,
		out,
		width * sizeof(rgba_t)
	);
	if (has_differences("ej3", name_out, out, out_size, 0)) {
		printf(
			FAIL "ej3_do_test_ab(\"%s\", \"%s\", %d, %d, \"%s\", %d, %d)\n",
			name_out, name_a, scale_a, offset_a, name_b, scale_b, offset_b
		);
		printf(FAIL "  Se encontraron diferencias:\n");
		printf(FAIL "    Imagen de entrada A: %s\n", filepath_a);
		printf(FAIL "    Imagen de entrada B: %s\n", filepath_b);
		printf(FAIL "    Mapa de profundidad A: %s\n", depth_filepath_a);
		printf(FAIL "    Mapa de profundidad B: %s\n", depth_filepath_b);
		printf(FAIL "    Escala A: %d\n", scale_a);
		printf(FAIL "    Offset A: %d\n", offset_a);
		printf(FAIL "    Escala B: %d\n", scale_b);
		printf(FAIL "    Offset B: %d\n", offset_b);
		printf(FAIL "    Imagen de salida: %s\n", filepath_out);
		printf(FAIL "    Imagen de salida esperada: expected/ej3_%s.png\n", name_out);
		printf(FAIL "\n");
		failed_tests++;
	} else {
		printf(
			DONE "ej3_do_test_ab(\"%s\", \"%s\", %d, %d, \"%s\", %d, %d)\n",
			name_out, name_a, scale_a, offset_a, name_b, scale_b, offset_b
		);
		successful_tests++;
	}

	free(out);
	free(depth_a_fixed);
	free(depth_b_fixed);
	stbi_image_free(a.data);
	stbi_image_free(b.data);
	stbi_image_free(depth_a.data);
	stbi_image_free(depth_b.data);

}

static
void test_ej3a(void) {
	uint64_t failed_at_start = failed_tests;
	if (!EJERCICIO_3A_HECHO) {
		printf(SKIP "El ejercicio 3A no está hecho aún.\n");
		return;
	}

	ej3_do_test_a("a_1", "grim",              4,  100);
	ej3_do_test_a("a_2", "grim/SP_0_WEBWS", 200,   -5);
	ej3_do_test_a("a_3", "grim/me_0_carla",  -1,  255);
	ej3_do_test_a("a_4", "grim/mt_1_diats",   3, -164);

	if (failed_at_start < failed_tests) {
		printf(FAIL "El ejercicio 3A tuvo tests que fallaron.\n");
	}
}

/**
 * Evalúa los tests del ejercicio 3B. Este ejercicio requiere implementar
 * `ej3b`.
 *
 * En caso de que se quieran skipear los tests alcanza con asignarle `false`
 * a `EJERCICIO_3B_HECHO`.
 */
static
void test_ej3b(void) {
	uint64_t failed_at_start = failed_tests;
	if (!EJERCICIO_3B_HECHO) {
		printf(SKIP "El ejercicio 3B no está hecho aún.\n");
		return;
	}

	ej3_do_test_b("b_1", "martini1841",     "mascotas");
	ej3_do_test_b("b_2", "grim",            "grim-flip");
	ej3_do_test_b("b_3", "grim/me_0_carla", "grim/me_1_pmpws");
	ej3_do_test_b("b_4", "grim/re_1_front", "grim/jb_0_garin");
	ej3_do_test_b("b_5", "grim/ei_1_intha", "grim/ei_1_intha-flip");

	if (failed_at_start < failed_tests) {
		printf(FAIL "El ejercicio 3B tuvo tests que fallaron.\n");
	}
}

/**
 * Corre tests que ejercitan tanto el ejercicio 3A como el 3B.
 *
 * En caso de que se quieran skipear los tests alcanza con asignarle `false`
 * a `EJERCICIO_3A_HECHO` ó a `EJERCICIO_3B_HECHO`.
 */
static
void test_ej3ab(void) {
	uint64_t failed_at_start = failed_tests;
	if (!EJERCICIO_3A_HECHO) {
		printf(SKIP "El ejercicio 3A no está hecho aún. No se correrá el test integrador.\n");
		return;
	}
	if (!EJERCICIO_3B_HECHO) {
		printf(SKIP "El ejercicio 3B no está hecho aún. No se correrá el test integrador.\n");
		return;
	}

	ej3_do_test_ab("ab_1", "grim/ly_2_sltha",  2,     100, "grim/mt_1_diats",   3, -164);
	ej3_do_test_ab("ab_2", "grim/me_0_carla", -1,     255, "grim/mt_1_diats",   1, -139);
	ej3_do_test_ab("ab_3", "grim/re_1_front",  1, 256*208, "grim/jb_0_garin", 256,    0);
	ej3_do_test_ab("ab_4", "grim/dr_0_winws", -1,       0, "grim/dr_1_dorrv",  -1,    0);
	ej3_do_test_ab("ab_5", "grim/ci_0_stpws", -1,       0, "grim/hl_0_dorrv",  -1,    0);
	ej3_do_test_ab("ab_6", "grim/tw_0_trkvw", -1,       0, "grim/le_2_ladws",  -1,    0);
	ej3_do_test_ab("ab_7", "grim/SP_1_ENTLA",  2,    -112, "grim/tr_2_pmpws",  -1,  255);
	ej3_do_test_ab("ab_8", "grim/co_3_dskrv", -1,     255, "nada",              1,  100);
	ej3_do_test_ab("ab_9", "grim/ce_2_front", -1,     255, "nada",              1,  240);

	if (failed_at_start < failed_tests) {
		printf(FAIL "El ejercicio 3AB tuvo tests que fallaron.\n");
	}
}

/**
 * Corre todos los tests del ejercicio 3
 */
void test_ej3(void) {
	// 3A
	test_ej3a();
	// 3B
	test_ej3b();
	// 3A y 3B
	test_ej3ab();
}
