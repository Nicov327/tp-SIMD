section .rodata
; Poner acá todas las máscaras y coeficientes que necesiten para el filtro
rojoMul: times 4 dd 0.2126			;	Coeficiente con el que voy a multiplicar el rojo
verdeMul: times 4 dd 0.7152			;	Coeficiente con el que voy a multiplicar el verde
azulMul: times 4 dd 0.0722			;	Coeficiente con el que voy a multiplicar el azul
soloRojo: times 4 dd 0x000000FF
soloVerde: times 4 dd 0x0000FF00
soloAzul: times 4 dd 0x00FF0000
alfa: times 4 dd 0xFF000000			; 	[FF 00 00 00 FF 00 00 00 FF 00 00 00 FF 00 00 00]
shuffle: db 0x00, 0x00, 0x00, 0x00, 0x04, 0x04, 0x04, 0x04, 0x08, 0x08, 0x08, 0x08, 0x0C, 0x0C, 0x0C, 0x0C
section .text

; Marca un ejercicio como aún no completado (esto hace que no corran sus tests)
FALSE EQU 0
; Marca un ejercicio como hecho
TRUE  EQU 1

; Marca el ejercicio 1 como hecho (`true`) o pendiente (`false`).
;
; Funciones a implementar:
;   - ej1
global EJERCICIO_1_HECHO
EJERCICIO_1_HECHO: db TRUE ; Cambiar por `TRUE` para correr los tests.

; Convierte una imagen dada (`src`) a escala de grises y la escribe en el
; canvas proporcionado (`dst`).
;
; Para convertir un píxel a escala de grises alcanza con realizar el siguiente
; cálculo:
; ```
; luminosidad = 0.2126 * rojo + 0.7152 * verde + 0.0722 * azul 
; ```
;
; Como los píxeles de las imágenes son RGB entonces el píxel destino será
; ```
; rojo  = luminosidad
; verde = luminosidad
; azul  = luminosidad
; alfa  = 255
; ```
;
; Parámetros:
;   - dst:    La imagen destino. Está a color (RGBA) en 8 bits sin signo por
;             canal.
;   - src:    La imagen origen A. Está a color (RGBA) en 8 bits sin signo por
;             canal.
;   - width:  El ancho en píxeles de `src` y `dst`.
;   - height: El alto en píxeles de `src` y `dst`.
global ej1
ej1:
	; Te recomendamos llenar una tablita acá con cada parámetro y su
	; ubicación según la convención de llamada. Prestá atención a qué
	; valores son de 64 bits y qué valores son de 32 bits.
	;
	; r/m64 = rgba_t*  dst		-> rdi
	; r/m64 = rgba_t*  src		-> rsi
	; r/m32 = uint32_t width	-> ecx
	; r/m32 = uint32_t height	-> edx

	push rbp
	mov rbp, rsp
	imul ecx, edx						;	Longitud del array de origen/destino
	shr ecx, 2							;	Divido para tener la cantidad exacta de iteraciones necesarias

.loop:
	movdqu xmm0, [rsi]					;	xmm0 lo voy a usar para cargar los pixeles

	movdqu xmm2, xmm0					;	Registro a trabajar los rojos		[A B G R A B G R A B G R A B G R]
	movdqu xmm3, xmm0					;	Registro a trabajar los verdes		[A B G R A B G R A B G R A B G R]
	movdqu xmm4, xmm0					;	Registro a trabajar los azules		[A B G R A B G R A B G R A B G R]

	movdqu xmm1, [soloRojo]				;	Cargo en xmm1 la máscara soloRojo
	pand xmm2, xmm1						;	xmm2 = [0 0 0 R 0 0 0 R 0 0 0 R 0 0 0 R]

	movdqu xmm1, [soloVerde]			;	Cargo en xmm1 la máscara soloVerde
	pand xmm3, xmm1						;	xmm3 = [0 0 G 0 0 0 G 0 0 0 G 0 0 0 G 0]

	movdqu xmm1, [soloAzul]				;	Cargo en xmm1 la máscara soloAzul
	pand xmm4, xmm1						;	xmm4 = [0 B 0 0 0 B 0 0 0 B 0 0 0 B 0 0]

	psrld xmm3, 8						;	Acomodo los bits del verde a la derecha del registro	->	xmm3 = [0 0 0 G 0 0 0 G 0 0 0 G 0 0 0 G]
	psrld xmm4, 16						;	Acomodo los bits del azul a la derecha del registro		->	xmm4 = [0 0 0 B 0 0 0 B 0 0 0 B 0 0 0 B]

	cvtdq2ps xmm2, xmm2					;	Convierto a float todos los valores de los 3 colores
	cvtdq2ps xmm3, xmm3
	cvtdq2ps xmm4, xmm4

	movdqu xmm5, [rojoMul]				;	Cargo los siguientes registros con las máscaras correspondientes para hacer las multiplicaciones
	movdqu xmm6, [verdeMul]
	movdqu xmm7, [azulMul]

	mulps xmm2, xmm5					;	Realizo las multiplicaciones
	mulps xmm3, xmm6
	mulps xmm4, xmm7

	addps xmm2, xmm3					;	Sumo cada color para obtener la "luminosidad". Primero le agrego el verde al rojo
	addps xmm2, xmm4					;	luego agrego el azul a lo anterior

	cvtps2dq xmm2, xmm2					;	Debo convertir esos float de 32 bits a enteros de 8 bits antes de hacer el shuffle correspondiente. Este dq no es DoubleQuad
 
	;	Tengo que hacer el shuffle acá abajo

	movdqu xmm1, [shuffle]				;	xmm1 va indicar cómo deben ser acomodados las luminosidades en el registro de respuesta
	pshufb xmm2, xmm1
	movdqu xmm1, [alfa]					;	Acomodadas las luminosidades, saturo la transparencia (alfa)
	por xmm2, xmm1					

	;	Tengo que hacer el shuffle acá arriba

	movdqu [rdi], xmm2					;	Pongo en donde indique destino el resultado de la operación previa

	add rsi, 16							;	Me muevo 16 bytes para agarrar (de *src) los siguientes 4 pixeles
	add rdi, 16							;	Me muevo 16 bytes para guardar (en *dst) los siguientes 4 pixeles
	sub ecx, 1							;	4 pixeles ya fueron procesados. Los descuento de los restantes
	jnz .loop

	pop rbp
	ret

