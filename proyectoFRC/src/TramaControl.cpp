/*
 * TramaControl.cpp
 *
 *  Created on: 22/2/2018
 *      Author: Truji
 */
#include "TramaControl.h"

void mostrarTramaControl(TramaControl t) {
	unsigned char control = t.C;

	switch (control) {

	case ENQ:
		printf("Se ha recibido una trama ENQ\n");
		break;
	case EOT:
		printf("Se ha recibido una trama EOT\n");
		break;
	case ACK:
		printf("Se ha recibido una trama ACK\n");
		break;
	case NACK:
		printf("Se ha recibido una trama NACK\n");
		break;
	default:
		printf("Trama de control desconocida\n");
		break;

	}

}

void seleccionarTrama(HANDLE PuertoCOM) {
	printf("\nTrama de control a enviar:\n");
	printf("o 1: Trama ENQ.\n");
	printf("o 2: Trama EOT.\n");
	printf("o 3: Trama ACK.\n");
	printf("o 4: Trama NACK.\n");

	int opcion;

	bool opcionIncorrecta = true;
	while (opcionIncorrecta) {
		cin >> opcion;
		switch (opcion) {
		case 1:
			enviarTrama(PuertoCOM, ENQ);
			opcionIncorrecta = false;
			break;
		case 2:
			enviarTrama(PuertoCOM, EOT);
			opcionIncorrecta = false;
			break;
		case 3:
			enviarTrama(PuertoCOM, ACK);
			opcionIncorrecta = false;
			break;
		case 4:
			enviarTrama(PuertoCOM, NACK);
			opcionIncorrecta = false;
			break;
		default:
			cout << "Opcion introducida incorrecta..." << endl;

		}

	}
}

void enviarTrama(HANDLE PuertoCOM, unsigned char control) {

	EnviarCaracter(PuertoCOM, SYN); //Sincronismo = SYN =22
	EnviarCaracter(PuertoCOM, 'T'); //Direccion=(En principio fijo a ’T’)
	EnviarCaracter(PuertoCOM, control); //Control = (05 (ENQ), 04 (EOT), 06 (ACK), 21 (NACK))
	EnviarCaracter(PuertoCOM, '0'); //Numero de Trama = (En principio fijo a ‘0’)

}
