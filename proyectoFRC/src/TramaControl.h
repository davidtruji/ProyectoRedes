/*
 * TramaControl.h
 *
 *  Created on: 22/2/2018
 *      Author: Truji
 */

#ifndef TRAMACONTROL_H_
#define TRAMACONTROL_H_

#include <iostream>
#include "PuertoSerie.h"
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

void mostrarTramaControl(TramaControl t);
void seleccionarTrama(HANDLE PuertoCOM);
void enviarTrama(HANDLE PuertoCOM,unsigned char control);


#endif /* TRAMACONTROL_H_ */
