//============================================================================
// ----------- PR�CTICAS DE FUNDAMENTOS DE REDES DE COMUNICACIONES -----------
// ---------------------------- CURSO 2017/18 --------------------------------
// ----------------------------- SESION1.CPP ---------------------------------
//============================================================================
//AUTORES: Alberto Diaz Martin y David Trujillo Torres  CURSO: 2�
#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <iostream>
#include "PuertoSerie.h"
#include "TramaControl.h"

using namespace std;

HANDLE PuertoCOM;
#define ESC 27
#define BACK 8
#define INTRO 13
#define F1 59
#define FN '\0'

const int MAX = 600;

void elegirPuerto() {
	int x;

	// Par�metros necesarios al llamar a AbrirPuerto:
	// - Nombre del puerto a abrir ("COM1", "COM2", "COM3", ...).
	// - Velocidad (1200, 1400, 4800, 9600, 19200, 38400, 57600, 115200).
	// - N�mero de bits en cada byte enviado o recibido (4, 5, 6, 7, 8).
	// - Paridad (0=sin paridad, 1=impar, 2=par, 3=marca, 4=espacio).
	// - Bits de stop (0=1 bit, 1=1.5 bits, 2=2 bits)
	bool flag = true;
	while (flag) {
		cout << "Elija un puerto(1, 2, 3, 4): \n";
		cin >> x;
		switch (x) {
		case 1:
			PuertoCOM = AbrirPuerto("COM1", 9600, 8, 0, 0);
			flag = false;
			cout << "Puerto com1 abierto... ";
			break;
		case 2:
			PuertoCOM = AbrirPuerto("COM2", 9600, 8, 0, 0);
			flag = false;
			cout << "Puerto com2 abierto... ";
			break;
		case 3:
			PuertoCOM = AbrirPuerto("COM3", 9600, 8, 0, 0);
			flag = false;
			cout << "Puerto com3 abierto... ";
			break;
		case 4:
			PuertoCOM = AbrirPuerto("COM4", 9600, 8, 0, 0);
			flag = false;
			cout << "Puerto com4 abierto... ";
			break;
		}

	}

	if (PuertoCOM == NULL) {
		printf("Error al abrir el puerto\n");
		getch();

	} else
		printf("Puerto abierto correctamente\n");

}

void retroceso(int &i) {
	if (i > 0) {
		i--;
		printf("\b");
		printf(" ");
		printf("\b");

	}

}

void salto(int &i, char vector[]) {
	if (i < 600) {
		vector[i] = '\n';
		cout << vector[i];
		i++;
	}

}

void envio(char &car, char vector[], int &i) {

	if (kbhit()) {
		car = getch();
		if (car == F1) {
			vector[i] = '\n';
			cout << vector[i];
			vector[i + 1] = '\0';
			EnviarCadena(PuertoCOM, vector, i + 1);
			vector[0] = '\0';
			i = 0;
		}
	}

}

int main() {
	char car = 0;
	char vector[MAX + 2];
	int i = 0;
	elegirPuerto();
// Lectura y escritura simult�nea de caracteres:

	while (car != ESC) {
		car = RecibirCaracter(PuertoCOM);

		if (car)
			printf("%c", car);       //Recepci�n

		if (kbhit()) {

			car = getch();
			vector[i] = car;

			switch (vector[i]) {
			case BACK: //Retroceso.
				retroceso(i);
				break;
			case INTRO: //Salto de l�nea.
				salto(i, vector);
				break;
			case FN: //Envio.
				envio(car, vector, i);
				break;
			case ESC:
				break;
			default: //salida por pantalla.
				if (i < 600) {
					printf("%c", vector[i]);
					i++;
				}

			}

		}
	}

// Para cerrar el puerto:
	CerrarPuerto(PuertoCOM);

	return 0;
}

