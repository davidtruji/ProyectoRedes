/*
 *  Creado el: 8/5/2018
 *      Curso: 2�
 *      Autor: David Trujillo Torres
 *		Autor: Alberto Diaz Martin
 */

#ifndef LIBFRC_H_
#define LIBFRC_H_
#include <iostream>
#include <conio.h>
#include <fstream>
#include "PuertoSerie.h"
#include "TramaControl.h"
#include "TramaDatos.h"

void enviarFichero(HANDLE PuertoCOM);
void reenviarTramaDatos(HANDLE PuertoCOM, TramaDatos td);
void recepcion(HANDLE PuertoCOM, int &numCampo, int &numDato, TramaControl &t,
		TramaDatos &td, bool &esTramaControl, bool &esFichero,
		ofstream &flujoFichero);
void seleccionMaestroEsclavo(HANDLE PuertoCOM);
void seleccionMaestro(HANDLE PuertoCOM);
void seleccion(HANDLE PuertoCOM);
void sondeo(HANDLE PuertoCOM);
bool recibirConfirmacionSeleccion(HANDLE PuertoCOM, int &campo,
		unsigned char num, TramaControl &t);
bool recibirConfirmacionSondeo(HANDLE PuertoCOM, int &campo, unsigned char num);
void enviarFicheroME(HANDLE PuertoCOM, unsigned char direccion);
void liberacionSeleccion(HANDLE PuertoCOM);
void solicitarCierreSondeo(HANDLE PuertoCOM);
bool recibirCierreSondeo(HANDLE PuertoCOM, int& campo, unsigned char num,
		TramaControl &t);
void responderSolicitudCierre(HANDLE PuertoCOM, unsigned char num);
bool recibirConfirmacionError(HANDLE PuertoCOM, int& campo, unsigned char dir,
		unsigned char num, TramaControl &t);

#endif /* LIBFRC_H_ */
