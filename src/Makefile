AS = nasm
ASFLAGS = -felf64 -Fdwarf -g
LDFLAGS = -lm
CFLAGS= -Wall -Wextra -pedantic -Og -g -Wno-unused-variable -Wno-unused-parameter -no-pie -z noexecstack

# Crea los binarios en cuestión
all: test_c test_asm

# Corre los tests usando la implementación en C
run_c: test_c
	./test_c

# Corre los tests usando la implementación en ASM
run_asm: test_asm
	./test_asm

# Corre los tests en valgrind usando la implementación en C
valgrind_c: test_c
	valgrind  --show-reachable=yes --leak-check=full --error-exitcode=1 ./test_c \
	&& echo "No se detectaron errores de memoria"

# Corre los tests en valgrind usando la implementación en ASM
valgrind_asm: test_asm
	valgrind  --show-reachable=yes --leak-check=full --error-exitcode=1 ./test_asm \
	&& echo "No se detectaron errores de memoria"

# Borra todos los archivos generados
clean:
	rm -f test_c test_asm *.o outputs/*.png

test_c: test.c ej1_tests.c ej1.c ej2_tests.c ej2.c ej3_tests.c ej3.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

test_asm: test.c ej1_tests.c ej1_asm.o ej2_tests.c ej2_asm.o ej3_tests.c ej3_asm.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

ej1_asm.o: ej1.asm
	$(AS) $(ASFLAGS) $^ -o $@

ej2_asm.o: ej2.asm
	$(AS) $(ASFLAGS) $^ -o $@

ej3_asm.o: ej3.asm
	$(AS) $(ASFLAGS) $^ -o $@

.PHONY: clean run_c run_asm valgrind_c valgrind_asm

