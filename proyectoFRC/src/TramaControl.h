/*
 *  Creado el: 28/2/2018
 *      Curso: 2º
 *      Autor: David Trujillo Torres
 *		Autor: Alberto Diaz Martin
 */

//Libreria de tramas de control
#ifndef TRAMACONTROL_H_
#define TRAMACONTROL_H_

#include <iostream>
#include "PuertoSerie.h"
#include <conio.h>

using namespace std;

#define SYN 22
#define ENQ 05
#define EOT 04
#define ACK 06
#define NACK 21

struct TramaControl {
	unsigned char S; //Sincronismo = SYN =22
	unsigned char D; //Direccion=(En principio fijo a ’T’)
	unsigned char C; //Control = (05 (ENQ), 04 (EOT), 06 (ACK), 21 (NACK))
	unsigned char NT; //Numero de Trama = (En principio fijo a ‘0’)
};

void mostrarTramaControl(TramaControl t, bool enviada);
void seleccionarTramaControl(HANDLE PuertoCOM);
void enviarTramaControl(HANDLE PuertoCOM, unsigned char dir,
		unsigned char control, unsigned char num);

#endif /* TRAMACONTROL_H_ */
