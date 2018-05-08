/*
 *  Creado el: 1/3/2018
 *      Curso: 2º
 *      Autor: David Trujillo Torres
 *		Autor: Alberto Diaz Martin
 */
#include "TramaDatos.h"

void enviarTramaDatos(HANDLE PuertoCOM, char vector[], int i) {
	int numTramasDatos;
	int c;
	int indice = 0;
	char datos[255];

	vector[i] = '\n'; //Anadimos \n en el final del mensaje e incrementamos el tamano de este
	i++;

	numTramasDatos = calcularNumeroTramasDatos(i);

	for (int j = 0; j < numTramasDatos; j++) {
		EnviarCaracter(PuertoCOM, SYN); //Sincronismo = SYN =22
		EnviarCaracter(PuertoCOM, 'T'); //Direccion=(En principio fijo a ’T’)
		EnviarCaracter(PuertoCOM, STX); //Control = STX = 02;
		EnviarCaracter(PuertoCOM, '0'); //NumTrama = (En principio fijo a ‘0’);

		if (i >= 254)
			EnviarCaracter(PuertoCOM, 254); //Si la trama es de mas de 254 caracteres, se enviaran 254 que es el maximo
		else
			EnviarCaracter(PuertoCOM, i); //Si la trama es de menos de 254 caracteres, se enviaran el valor de i que contiene lo que queda por enviar

		for (c = 0; c < i && c < 254; c++) {

			EnviarCaracter(PuertoCOM, vector[indice]);
			datos[c] = vector[indice];
			indice++;
		}

		i = i - c;

		EnviarCaracter(PuertoCOM, calcularBCE(datos, c)); //Calculo y envio del BCE

	}

}

int calcularNumeroTramasDatos(int i) {
	int numTramasDatos = 0;

	if (i < 254 || i == 254)
		numTramasDatos = 1;
	else {
		numTramasDatos = i / 254;
		if (i % 254 != 0)
			numTramasDatos++;
	}
	return numTramasDatos;
}

unsigned char calcularBCE(char datos[], int l) {

	unsigned char bce = datos[0];

	for (int i = 1; i < l; i++) {
		bce = bce ^ datos[i];
	}

	if (bce == 255 || bce == 0)
		bce = 1;

	return bce;
}

void mostrarTramaDatos(TramaDatos td) {
	if (td.BCE == calcularBCE(td.Datos, td.L)) {
		//printf("\nSe ha recibido una trama de datos\n");

		for (int i = 0; i < td.L; i++)
			printf("%c", td.Datos[i]);

	} else {
		printf("\nSe ha recibido una trama de datos defectuosa\n");

	}

}


