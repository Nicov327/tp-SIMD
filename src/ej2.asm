section .rodata
; Poner acá todas las máscaras y coeficientes que necesiten para el filtro
soloRojo: times 4 dd 0x000000FF
soloVerde: times 4 dd 0x0000FF00
soloAzul: times 4 dd 0x00FF0000
div3: times 4 dd 3.0
sumaParaVerde: times 4 dd 64
sumaParaAzules: times 4 dd 128
CONST_128: times 4 dd 128
alfaEn255: times 4 dd 0xFF000000

;	Las siguientes máscaras son usadas en f(x)
resto192: times 4 dd 192			;	Máscara que servirá para realizar el x-192 de f(x)
multiplicoNegativo4: times 4 dd -4	;	Servirá para hacer la multiplicación por -4 a los resultados de |x-192| en f(x)
multiplico4: times 4 dd 4	;	Servirá para hacer la multiplicación por -4 a los resultados de |x-192| en f(x)
cargoCuatro384s: times 4 dd 384		;	Valor a ser restado dentro de f(x)
cargoCuatro255s: times 4 dd 255

section .text

; Marca un ejercicio como aún no completado (esto hace que no corran sus tests)
FALSE EQU 0
; Marca un ejercicio como hecho
TRUE  EQU 1

; Marca el ejercicio 2 como hecho (`true`) o pendiente (`false`).
;
; Funciones a implementar:
;   - ej1
global EJERCICIO_2_HECHO
EJERCICIO_2_HECHO: db TRUE ; Cambiar por `TRUE` para correr los tests.

; Aplica un efecto de "mapa de calor" sobre una imagen dada (`src`). Escribe la
; imagen resultante en el canvas proporcionado (`dst`).
;
; Para calcular el mapa de calor lo primero que hay que hacer es computar la
; "temperatura" del pixel en cuestión:
; ```
; temperatura = (rojo + verde + azul) / 3
; ```
;
; Cada canal del resultado tiene la siguiente forma:
; ```
; |          ____________________
; |         /                    \
; |        /                      \        Y = intensidad
; | ______/                        \______
; |
; +---------------------------------------
;              X = temperatura
; ```
;
; Para calcular esta función se utiliza la siguiente expresión:
; ```
; f(x) = min(255, max(0, 384 - 4 * |x - 192|))
; ```
;
; Cada canal esta offseteado de distinta forma sobre el eje X, por lo que los
; píxeles resultantes son:
; ```
; temperatura  = (rojo + verde + azul) / 3
; salida.rojo  = f(temperatura)
; salida.verde = f(temperatura + 64)
; salida.azul  = f(temperatura + 128)
; salida.alfa  = 255
; ```
;
; Parámetros:
;   - dst:    La imagen destino. Está a color (RGBA) en 8 bits sin signo por
;             canal.
;   - src:    La imagen origen A. Está a color (RGBA) en 8 bits sin signo por
;             canal.
;   - width:  El ancho en píxeles de `src` y `dst`.
;   - height: El alto en píxeles de `src` y `dst`.
global ej2
ej2:
	; Te recomendamos llenar una tablita acá con cada parámetro y su
	; ubicación según la convención de llamada. Prestá atención a qué
	; valores son de 64 bits y qué valores son de 32 bits.
	;
	; r/m64 = rgba_t*  dst		->	rdi
	; r/m64 = rgba_t*  src		->	rsi
	; r/m32 = uint32_t width	->	rcx
	; r/m32 = uint32_t height	-> 	rdx
	
	; Prólogo
	push rbp
	mov rbp, rsp ; ALineado

	xor r8, r8 ; = 0 
	mov r8, rdx ; copio width
	imul r8, rcx ; width * height y tengo cantidad de pixeles a realizar
	shr r8, 2 ; Divido por 4 para saber la cantidad de iteraciones con 4 pixeles

	xor rax, rax ; = 0 
.loop:
	movdqu xmm0, [rsi + rax]			;	Cargo 4 pixeles a trabajar

	movdqu xmm1, xmm0			;	Este registro va a tener los rojos
	movdqu xmm2, xmm0			;	Este registro va a tener los verdes
	movdqu xmm3, xmm0			;	Este registro va a tener los azules
	
	movdqu xmm4, [soloRojo]		;	Cargo máscara para solo rojos y la aplico
	pand xmm1, xmm4

	movdqu xmm4, [soloVerde]	;	Cargo máscara para solo verdes y la aplico
	pand xmm2, xmm4

	movdqu xmm4, [soloAzul]		;	Cargo máscara para solo azules y la aplico
	pand xmm3, xmm4

	psrld xmm2, 8				;	Acomodo los bits del verde a la derecha del registro	->	xmm3 = [0 0 0 G 0 0 0 G 0 0 0 G 0 0 0 G]
	psrld xmm3, 16				;	Acomodo los bits del azul a la derecha del registro		->	xmm4 = [0 0 0 B 0 0 0 B 0 0 0 B 0 0 0 B]

	paddd xmm1, xmm2			;	Sobre los rojos, le sumo los verdes
	paddd xmm1, xmm3			;	Sobre lo anterior, sumo los azules

	cvtdq2ps xmm1, xmm1			;	Convierto a float el resultado (Porque voy a dividir por 3)

	movdqu xmm4, [div3]			;	Cargo 4 valores 3.0 para poder hacer la divisón que viene

	divps xmm1, xmm4				;	Divido por 3. Este registro funciona como la variable "Temperatura"

	cvttps2dq xmm1, xmm1				;	Temperatura vuelve a ser un valor entero. Será positivo, pues matemáticamente no puede ser negativo

	movdqu xmm2, xmm1				;	xmm2 va a ser Temperatura + 64
	movdqu xmm4, [sumaParaVerde]	;	Cargo 64 en xmm4 4 veces (32 bits para cada valor 64)
	paddd xmm2, xmm4

	movdqu xmm3, xmm1				;	xmm3 va a ser Temperatura + 128
	movdqu xmm4, [sumaParaAzules]	;	Cargo 128 en xmm4 4 veces (32 bits para cada valor 128)
	paddd xmm3, xmm4

	movdqu xmm5, xmm1			;	Cargo Temperatura en xmm5
	movdqu xmm6, xmm2			;	Cargo Temperatura+64 en xmm6
	movdqu xmm7, xmm3			;	Cargo Temperatura+128 en xmm7

	;	PARÉNTESIS: Los 3 registros anteriores los cargué con esos valores para poder operar las variables mencionadas
	;	sin la preocupación de perder los valores originales
	;	Las siguientes lineas realizan f(x) sobre los valores correspondientes (Temperatura, Temperatura+64, Temperatura+128)

	movdqu xmm4, [resto192]		;	Cargo 4 veces el valor 192 en xmm4

	psubd xmm5, xmm4			;	Le resto 192 a todos los rojos
	psubd xmm6, xmm4			;	Le resto 192 a todos los verdes
	psubd xmm7, xmm4			;	Le resto 192 a todos los azules

	pabsd xmm5, xmm5			;	Saco valor absoluto a los 4 valores de los 3 colores en cada registro
	pabsd xmm6, xmm6
	pabsd xmm7, xmm7

	movdqu xmm4, [multiplico4]	;	Cargo 4 veces el valor 4 en xmm4
	
	pmulld xmm5, xmm4					;	A los resultados anteriores (|x-192| en cada canal de cada pixel) los multiplico por 4
	pmulld xmm6, xmm4
	pmulld xmm7, xmm4

	movdqu xmm1, [cargoCuatro384s]		;	Sobre xmm1, cargo 4 384 a ser restados por los valores de rojo
	movdqu xmm2, [cargoCuatro384s]		;	Sobre xmm2, cargo 4 384 a ser restados por los valores de verde
	movdqu xmm3, [cargoCuatro384s]		;	Sobre xmm3, cargo 4 384 a ser restados por los valores de azul

	psubd xmm1, xmm5			;	xmm1 = 384 - resultados anteriores para ROJOS
	psubd xmm2, xmm6			;	xmm2 = 384 - resultados anteriores para VERDES
	psubd xmm3, xmm7			;	xmm3 = 384 - resultados anteriores para AZULES
	
	movdqu xmm5, xmm1			;	Los resultados anteriores los guardo en xmm5, xmm6 y xmm7 porque algunos de estos valores pueden ser borrados con las siguientes instrucciones
	movdqu xmm6, xmm2
	movdqu xmm7, xmm3

	pxor xmm4, xmm4		 	;	Necesito un registro en todos 0s para hacer el max(0, valores) del f(x)

	pxor xmm8, xmm8
	movdqu xmm8, [CONST_128]		;	Cargo 128 
	
	paddd xmm1, xmm8 ; Sumo 128 para no perder el valor cuando compare con signo
	paddd xmm2, xmm8
	paddd xmm3, xmm8 

	pmaxsd xmm5, xmm4 ; max(0, 384 − 4∣x−192∣)
	pmaxsd xmm6, xmm4
	pmaxsd xmm7, xmm4

	movdqu xmm1, xmm5 ;	Por el mismo motivo del comentario de la linea 159, guardo en xmm1, xmm2 y xmm3 los valores de xmm5, xmm6, xmm7
	movdqu xmm2, xmm6
	movdqu xmm3, xmm7

	movdqu xmm4, [cargoCuatro255s]		;	Cargo 255 4 veces para hacer la operación min(255, valores) del f(x)

	pminsd xmm5, xmm4 ; min(255, max(0,384 − 4∣x−192∣))
	pminsd xmm6, xmm4
	pminsd xmm7, xmm4

	;	Hasta la linea anterior se operó el f(x), cuyos resultados están en xmm5, xmm6 y xmm7. Reacomodo los canales antes de hacer una suma horizontal

	pslld xmm6, 8				;	xmm6 contiene los colores verdes. Shifteo 8 bits a la izquierda a todos para dejarlo en el lugar correspondiente
	pslld xmm7, 16				;	xmm7 contiene los colores azules. Shifteo 16 bits a la izquierda a todos para dejarlo en el lugar correspondiente

	por xmm5, xmm6			;	xmm5 contiene los colores rojos. Le pego los colores verdes
	por xmm5, xmm7			;	Al resultado anterior, le pego los colores azules

	movdqu xmm4, [alfaEn255]
	
	por xmm5, xmm4				;	Pongo los bits de alfa en 255 para los 4 pixeles

	movdqu [rdi + rax], xmm5			;	Cargo la respuesta

	add rax, 16 ; Proximos 4 pixeles
	dec r8 ; -1 iteración 
	jnz .loop
.fin:
	pop rbp
	ret

