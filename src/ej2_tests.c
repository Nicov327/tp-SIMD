#include <assert.h>

#include "common-tests.h"
#include "ej2.h"
#include "stb_image.h"
#include "stb_image_write.h"

/**
 * Corre un test del ejercicio 2.
 *
 * Parámetros:
 *   - name_out: Nombre del test.
 *   - name:     Nombre de la imagen de entrada.
 */
static
void ej2_do_test(const char* name_out, const char* name) {
	if (!EJERCICIO_2_HECHO) {
		printf(SKIP "El ejercicio 2 no está hecho aún.\n");
		return;
	}

	char filepath[MAX_FILEPATH_SIZE + 1];
	build_filepath(filepath, "img/", name, ".png");

	char filepath_out[MAX_FILEPATH_SIZE + 1];
	build_filepath(filepath_out, "outputs/ej2_", name_out, ".png");

	img_t src = load_image(filepath, RGBA_CHANNEL_COUNT);

	uint32_t width = src.width;
	uint32_t height = src.height;

	uint64_t out_size = width * height * sizeof(rgba_t);
	rgba_t* out = malloc(out_size);

	ej2(out, (rgba_t*) src.data, width, height);
	stbi_write_png(
		filepath_out,
		width, height,
		RGBA_CHANNEL_COUNT,
		out,
		width * sizeof(rgba_t)
	);
	if (has_differences("ej2", name_out, out, out_size, 1)) {
		printf(FAIL "ej2_do_test(\"%s\", \"%s\")\n", name_out, name);
		printf(FAIL "  Se encontraron diferencias:\n");
		printf(FAIL "    Imagen de entrada: %s\n", filepath);
		printf(FAIL "    Imagen de salida: %s\n", filepath_out);
		printf(FAIL "    Imagen de salida esperada: expected/%s.png\n", name_out);
		printf(FAIL "\n");
		failed_tests++;
	} else {
		printf(DONE "ej2_do_test(\"%s\", \"%s\")\n", name_out, name);
		successful_tests++;
	}

	free(out);
	stbi_image_free(src.data);
}

void test_ej2(void) {
	uint64_t failed_at_start = failed_tests;
	if (!EJERCICIO_2_HECHO) {
		printf(SKIP "El ejercicio 2 no está hecho aún.\n");
		return;
	}

	ej2_do_test("1", "martini1841");
	ej2_do_test("2", "mascotas");
	ej2_do_test("3", "grim/jb_0_garin");
	ej2_do_test("4", "grim/ei_1_intha");
	ej2_do_test("Wargames", "Wargames");
	ej2_do_test("NoCountryForOldMen", "NoCountryForOldMen");

	if (failed_at_start < failed_tests) {
		printf(FAIL "El ejercicio 2 tuvo tests que fallaron.\n");
	}
}
