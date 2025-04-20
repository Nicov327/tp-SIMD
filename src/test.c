#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

/**
 * Cuenta cuántos tests corrieron exitosamente.
 */
uint64_t successful_tests = 0;

/**
 * Cuenta cuántos tests test fallaron.
 */
uint64_t failed_tests = 0;

/**
 * Corre los tests de este ejercicio.
 *
 * Las variables `EJERCICIO_3A_HECHO` y `EJERCICIO_3B_HECHO` controlan qué
 * testsuites se van a correr. Ponerlas como `false` indica que el ejercicio no
 * está implementado y por lo tanto no querés que se corran los tests
 * asociados a él.
 *
 * Recordá que el ejercicio 3A se puede implementar independientemente del 3B.
 *
 * Si algún test falla el programa va a terminar con un código de error.
 */
int main(int argc, char* argv[]) {
	void test_ej1(void);
	void test_ej2(void);
	void test_ej3(void);

	test_ej1();
	test_ej2();
	test_ej3();

	printf(
		"\nSe corrieron %ld tests. %ld corrieron exitosamente. %ld fallaron.\n",
		failed_tests + successful_tests, successful_tests, failed_tests
	);

	if (failed_tests) {
		return 1;
	} else {
		return 0;
	}
}
