/*
 *  Creado el: 28/2/2018
 *      Curso: 2º
 *      Autor: David Trujillo Torres
 *		Autor: Alberto Diaz Martin
 */

#include "TramaControl.h"

void mostrarTramaControl(TramaControl t, bool enviada) {
	unsigned char control = t.C;
	char* envioRecivo;

	if (enviada)
		envioRecivo = "ENVIADA";
	else
		envioRecivo = "RECIBIDA";

	switch (control) {

	case ENQ:
		printf("\n[%s] [%c] [ ENQ ] [%c]\n", envioRecivo, t.D, t.NT);
		break;
	case EOT:
		printf("\n[%s] [%c] [ EOT ] [%c]\n", envioRecivo, t.D, t.NT);
		break;
	case ACK:
		printf("\n[%s] [%c] [ ACK ] [%c]\n", envioRecivo, t.D, t.NT);
		break;
	case NACK:
		printf("\n[%s] [%c] [ NACK ] [%c]\n", envioRecivo, t.D, t.NT);
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
			enviarTramaControl(PuertoCOM, 'T', ENQ, '0');
			//opcionIncorrecta = false;
			break;
		case '2':
			enviarTramaControl(PuertoCOM, 'T', EOT, '0');
			//opcionIncorrecta = false;
			break;
		case '3':
			enviarTramaControl(PuertoCOM, 'T', ACK, '0');
			//opcionIncorrecta = false;
			break;
		case '4':
			enviarTramaControl(PuertoCOM, 'T', NACK, '0');
			//opcionIncorrecta = false;
			break;
		default:
			opcion = false;
			printf("OPCION NO VALIDA\n");
			break;
		}

	}
}

void enviarTramaControl(HANDLE PuertoCOM, unsigned char dir,
		unsigned char control, unsigned char num) {
	TramaControl t;
	t.S = SYN;
	t.D = dir;
	t.C = control;
	t.NT = num;

	EnviarCaracter(PuertoCOM, SYN); //Sincronismo = SYN =22
	EnviarCaracter(PuertoCOM, dir); //Direccion=(En principio fijo a ’T’)
	EnviarCaracter(PuertoCOM, control); //Control = (05 (ENQ), 04 (EOT), 06 (ACK), 21 (NACK))
	EnviarCaracter(PuertoCOM, num); //Numero de Trama = (En principio fijo a ‘0’)

	mostrarTramaControl(t, true);

}
