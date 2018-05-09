/*
 *  Creado el: 28/2/2018
 *      Curso: 2º
 *      Autor: David Trujillo Torres
 *		Autor: Alberto Diaz Martin
 */

#include "TramaControl.h"

void mostrarTramaControl(TramaControl t) {
	unsigned char control = t.C;

	switch (control) {

	case ENQ:
		printf("\n[RECIBIDA] TRAMA ENQ - %c\n", t.NT);
		break;
	case EOT:
		printf("\n[RECIBIDA] TRAMA EOT - %c\n", t.NT);
		break;
	case ACK:
		printf("\n[RECIBIDA] TRAMA ACK - %c\n", t.NT);
		break;
	case NACK:
		printf("\n[RECIBIDA] TRAMA NACK - %c\n", t.NT);
		break;
	default:
		printf("Trama de control desconocida\n");
		break;

	}

}

void seleccionarTramaControl(HANDLE PuertoCOM) {

	char tecla;
	bool opcion = false;
	while (!opcion) {
		printf("\nTrama de control a enviar:\n");
		printf("o 1: Trama ENQ.\n");
		printf("o 2: Trama EOT.\n");
		printf("o 3: Trama ACK.\n");
		printf("o 4: Trama NACK.\n");
		while (!kbhit()) {
		}

		opcion = true;
		tecla = getch();

		switch (tecla) {
		case '1':
			enviarTramaControl(PuertoCOM, ENQ);
			//opcionIncorrecta = false;
			break;
		case '2':
			enviarTramaControl(PuertoCOM, EOT);
			//opcionIncorrecta = false;
			break;
		case '3':
			enviarTramaControl(PuertoCOM, ACK);
			//opcionIncorrecta = false;
			break;
		case '4':
			enviarTramaControl(PuertoCOM, NACK);
			//opcionIncorrecta = false;
			break;
		default:
			opcion = false;
			printf("OPCION NO VALIDA\n");
			break;
		}

	}
}

void enviarTramaControl(HANDLE PuertoCOM, unsigned char control) {

	EnviarCaracter(PuertoCOM, SYN); //Sincronismo = SYN =22
	EnviarCaracter(PuertoCOM, 'T'); //Direccion=(En principio fijo a ’T’)
	EnviarCaracter(PuertoCOM, control); //Control = (05 (ENQ), 04 (EOT), 06 (ACK), 21 (NACK))
	EnviarCaracter(PuertoCOM, '0'); //Numero de Trama = (En principio fijo a ‘0’)

}
