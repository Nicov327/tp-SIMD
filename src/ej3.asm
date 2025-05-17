section .text

; Marca un ejercicio como aún no completado (esto hace que no corran sus tests)
FALSE EQU 0
; Marca un ejercicio como hecho
TRUE  EQU 1

; Marca el ejercicio 3A como hecho (`true`) o pendiente (`false`).
;
; Funciones a implementar:
;   - ej3a
global EJERCICIO_3A_HECHO
EJERCICIO_3A_HECHO: db TRUE ; Cambiar por `TRUE` para correr los tests.

; Dada una imagen origen escribe en el destino `scale * px + offset` por cada
; píxel en la imagen.
;
; Parámetros:
;   - dst_depth: La imagen destino (mapa de profundidad). Está en escala de
;                grises a 32 bits con signo por canal.
;   - src_depth: La imagen origen (mapa de profundidad). Está en escala de
;                grises a 8 bits sin signo por canal.
;   - scale:     El factor de escala. Es un entero con signo de 32 bits.
;                Multiplica a cada pixel de la entrada.
;   - offset:    El factor de corrimiento. Es un entero con signo de 32 bits.
;                Se suma a todos los píxeles luego de escalarlos.
;   - width:     El ancho en píxeles de `src_depth` y `dst_depth`.
;   - height:    El alto en píxeles de `src_depth` y `dst_depth`.
global ej3a
ej3a:
	; Te recomendamos llenar una tablita acá con cada parámetro y su
	; ubicación según la convención de llamada. Prestá atención a qué
	; valores son de 64 bits y qué valores son de 32 bits.
	;
	; r/m64 = int32_t* dst_depth	rdi
	; r/m64 = uint8_t* src_depth	rsi
	; r/m32 = int32_t  scale		edx
	; r/m32 = int32_t  offset		ecx
	; r/m32 = int      width		r8d
	; r/m32 = int      height		r9d

	push rbp
	mov rbp, rsp

	movd xmm0, edx ; xmm0 = |scale|---|----|---|
	movd xmm1, ecx ; xmm1 = |offset|----|----|----|
	pshufd xmm0, xmm0, 0 ; xmm0 = |scale|scale|scale|scale|
	pshufd xmm1, xmm1, 0 ; xmm1 = |offset|offset|offset|offset|

	xor rax, rax	;	Limpio el rax (Para dejar los bits menos significativos en 0)
	mov eax, r9d	;	Muevo a la parte baja el contenido de r9d
	mul r8d			;	Multiplico lo del eax con r8d, quedando almacenado en el primer registro
	mov r8d, eax	;	Muevo el resultado a r8d

.loop:
	movd xmm2, [rsi] ; Cargo 16 bytes de src

	pmovzxbd xmm2, xmm2 ; Extiendo en cero, de 8 a 32 para la cuenta signed
	
	pmulld xmm2, xmm0 ; Multiplico scale de a 32
	paddd xmm2, xmm1 ; Sumo offset de a 32

	movdqu [rdi], xmm2 ; Coloco en dst

	add rdi, 16 ; Me desplazo para cargar los próximos 4 pixeles

	add rsi, 4 	; Me desplazo de a pixel (porque src apunta a 4 bytes)

	sub r8d, 4
    jnz .loop

.end:
	pop rbp
	ret

; Marca el ejercicio 3B como hecho (`true`) o pendiente (`false`).
;
; Funciones a implementar:
;   - ej3b
global EJERCICIO_3B_HECHO
EJERCICIO_3B_HECHO: db TRUE ; Cambiar por `TRUE` para correr los tests.

; Dadas dos imágenes de origen (`a` y `b`) en conjunto con sus mapas de
; profundidad escribe en el destino el pixel de menor profundidad por cada
; píxel de la imagen. En caso de empate se escribe el píxel de `b`.
;
; Parámetros:
;   - dst:     La imagen destino. Está a color (RGBA) en 8 bits sin signo por
;              canal.
;   - a:       La imagen origen A. Está a color (RGBA) en 8 bits sin signo por
;              canal.
;   - depth_a: El mapa de profundidad de A. Está en escala de grises a 32 bits
;              con signo por canal.
;   - b:       La imagen origen B. Está a color (RGBA) en 8 bits sin signo por
;              canal.
;   - depth_b: El mapa de profundidad de B. Está en escala de grises a 32 bits
;              con signo por canal.
;   - width:  El ancho en píxeles de todas las imágenes parámetro.
;   - height: El alto en píxeles de todas las imágenes parámetro.
global ej3b
ej3b:
	; Te recomendamos llenar una tablita acá con cada parámetro y su
	; ubicación según la convención de llamada. Prestá atención a qué
	; valores son de 64 bits y qué valores son de 32 bits.
	;
	; r/m64 = rgba_t*  dst		rdi
	; r/m64 = rgba_t*  a		rsi
	; r/m64 = int32_t* depth_a	rdx
	; r/m64 = rgba_t*  b		rcx
	; r/m64 = int32_t* depth_b	r8
	; r/m32 = int      width	r9d
	; r/m32 = int      height	[rbp + 0x10]

	push rbp
	mov rbp, rsp

  ; Como puedo procesar de a 16 bytes, y cada pixel son 4 bytes, puedo procesar de a 4 pixeles, 4 valores de depth_a, y 4 de depth_b.

	imul r9d, dword[rbp + 0x10] ; rbp + 16 porque quiero llenar la double word de r9d (32)

.loop:

	cmp r9d, 0 ; Chequeo si termino
	je .fin

	movdqu xmm0, [rsi]	; xmm0 = | pixel a0 | pixel a1 | pixel a2 | pixel a3 |
	movdqu xmm1, [rcx]	; xmm1 = | pixel b0 | pixel b1 | pixel b2 | pixel b3 |

	movdqu xmm2, [rdx]	; xmm2 = | depth a0 | depth a1 | depth a2 | depth a3 |
	movdqu xmm3, [r8]	; xmm3 = | depth b0 | depth b1 | depth b2 | depth b3 |

	pcmpgtd xmm3, xmm2  ; Comparo xmm2 y xmm3, cosa que me quede 1 en el lugar donde bi >= ai y 0 en bi < ai
	
	movdqu xmm5, xmm0	; Copio de a 4 pixeles en xmm5

	pand xmm5, xmm3		; Ahora hago un and entre xmm5 y xmm3 cosa que me quede xmm5 = |pixel 0B |   0   |   0   |pixel 3B|
	pandn xmm3, xmm1	; y un nand entre xmm3 y xmm1 cosa que me quede xmm3 = |   0    |pixel 1A|pixel 3A|   0   |
	
	por xmm5, xmm3		; Hago un or bit a bit para acoplar los pixeles

	movdqu [rdi], xmm5 ; Cargo en dst

	add rdi, 16
	add rsi, 16
	add rcx, 16
	add rdx, 16
	add r8, 16
	sub r9d, 4
	jmp .loop


.fin:
	pop rbp
	ret
