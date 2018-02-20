//============================================================================
// ----------- PRÁCTICAS DE FUNDAMENTOS DE REDES DE COMUNICACIONES -----------
// ---------------------------- CURSO 2017/18 --------------------------------
// ----------------------------- MAIN.CPP ------------------------------------
//============================================================================

#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <iostream>
#include "PuertoSerie.h"

using namespace std;
const int maxChar = 600;//Numero maximo de caracters que se permitiran leer
HANDLE PuertoCOM;

//Permite seleccionar un puerto serie para abrirlo
int seleccionarPuerto() {
	int puerto = 0;
	bool opcionValida = false;

	while (!opcionValida) {
		printf("Seleccionar el puerto a utilizar:\n");
		printf("  1. COM1\n");
		printf("  2. COM2\n");
		printf("  3. COM3\n");
		printf("  4. COM4\n");
		cin >> puerto;
		if (puerto >= 1 && puerto <= 4)
			opcionValida = true;
		else
			printf("Opcion no valida, introduzca un valor entre 1 y 4\n");

	}

	// Parámetros necesarios al llamar a AbrirPuerto:
	// - Nombre del puerto a abrir ("COM1", "COM2", "COM3", ...).
	// - Velocidad (1200, 1400, 4800, 9600, 19200, 38400, 57600, 115200).
	// - Número de bits en cada byte enviado o recibido (4, 5, 6, 7, 8).
	// - Paridad (0=sin paridad, 1=impar, 2=par, 3=marca, 4=espacio).
	// - Bits de stop (0=1 bit, 1=1.5 bits, 2=2 bits)
	switch (puerto) {

	case 1:
		printf("Seleccionado COM1\n");
		PuertoCOM = AbrirPuerto("COM1", 9600, 8, 0, 0); //Abrimos el puerto COM1 (en la sala siempre abrimos el COM1)
		break;

	case 2:
		printf("Seleccionado COM2\n");
		PuertoCOM = AbrirPuerto("COM2", 9600, 8, 0, 0); //Abrimos el puerto COM2 (en la sala siempre abrimos el COM1)
		break;

	case 3:
		printf("Seleccionado COM3\n");
		PuertoCOM = AbrirPuerto("COM3", 9600, 8, 0, 0); //Abrimos el puerto COM3 (en la sala siempre abrimos el COM1)

		break;

	case 4:
		printf("Seleccionado COM4\n");
		PuertoCOM = AbrirPuerto("COM4", 9600, 8, 0, 0); //Abrimos el puerto COM4 (en la sala siempre abrimos el COM1)
		break;

	}

	if (PuertoCOM == NULL) {
		printf("Error al abrir el puerto\n");
		getch();
	} else
		printf("Puerto abierto correctamente\n");

	return puerto;
}


//Comienza la rececion y envio de informacion a traves del puerto elegido
void iniciarComunicacion(){
	char car=0;
	// Lectura y escritura simultánea de caracteres:
		while (car != 27) {
			car = RecibirCaracter(PuertoCOM);
			if (car)
				printf("%c", car);       //Recepción
			if (kbhit()) {
				car = getch();
				if (car != 27) {
					printf("%c", car);   //Envío
					EnviarCaracter(PuertoCOM, car);
				}
			}
		}

	// Para cerrar el puerto:
		CerrarPuerto(PuertoCOM);
}

int main() {

	seleccionarPuerto();
	iniciarComunicacion();

	return 0;
}
