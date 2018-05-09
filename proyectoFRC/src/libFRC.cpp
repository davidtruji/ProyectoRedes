/*
 *  Creado el: 8/5/2018
 *      Curso: 2º
 *      Autor: David Trujillo Torres
 *		Autor: Alberto Diaz Martin
 */

#include "libFRC.h"
#define F5 63
#define FN '\0'
bool maestro = false, esclavo = false;

void enviarFichero(HANDLE PuertoCOM) {
	printf("\nEnviando fichero\n");
	int c;
	char datos[255];
	ifstream flujoFicheroLectura;
	int longitudFichero = 0;
	char fichero[255];
	flujoFicheroLectura.open("FRC-E.txt");

	int numCampo = 1, numDato = 0;
	TramaControl t;
	TramaDatos td;
	bool esTramaControl = false, esFichero = false;
	ofstream flujoFicheroEscritura;

	if (flujoFicheroLectura.is_open()) {

		//Enviamos caracter fichero '$' antes de la primera trama de datos
		EnviarCaracter(PuertoCOM, '$');

		while (!flujoFicheroLectura.eof()) {

			flujoFicheroLectura.read(fichero, 254);
			longitudFichero = flujoFicheroLectura.gcount();
			fichero[longitudFichero] = '\0';

			EnviarCaracter(PuertoCOM, SYN); //Sincronismo = SYN =22
			EnviarCaracter(PuertoCOM, 'T'); //Direccion=(En principio fijo a T)
			EnviarCaracter(PuertoCOM, STX); //Control = STX = 02;
			EnviarCaracter(PuertoCOM, '0'); //NumTrama = (En principio fijo a 0);

			//Enviamos el campo LONG
			EnviarCaracter(PuertoCOM, longitudFichero);

			//Enviamos Campo de datos
			for (c = 0; c < longitudFichero && c < 254; c++) {

				EnviarCaracter(PuertoCOM, fichero[c]);
				datos[c] = fichero[c];
			}

			EnviarCaracter(PuertoCOM, calcularBCE(datos, c)); //Calculo y envio del BCE

			recepcion(PuertoCOM, numCampo, numDato, t, td, esTramaControl,
					esFichero, flujoFicheroEscritura);

		}
		//Enviamos caracter fichero '#' despues de la ultima trama
		EnviarCaracter(PuertoCOM, '#');

		flujoFicheroLectura.close();
		printf("\nFichero enviado\n");
	} else
		printf("\nError al intentar abrir el fichero...\n");

}

void recepcion(HANDLE PuertoCOM, int &numCampo, int &numDato, TramaControl &t,
		TramaDatos &td, bool &esTramaControl, bool &esFichero,
		ofstream &flujoFichero) {
	char car = 0;
	car = RecibirCaracter(PuertoCOM);

	if (car != 0) {

		switch (numCampo) {
		case 1:
			if (car == SYN) {
				t.S = car;
				td.S = car;
				numCampo++;
			} else if (car == '$') {
				esFichero = true;
				flujoFichero.open("FRC-R.txt");
				if (!maestro && !esclavo)
					printf("\nRecibiendo fichero\n");

			} else if (car == '#') {
				esFichero = false;
				flujoFichero.close();
				if (!maestro && !esclavo)
					printf("\nFichero recibido\n");
			}

			break;
		case 2:
			t.D = car;
			td.D = car;
			numCampo++;

			break;
		case 3:
			t.C = car;
			td.C = car;
			if (car != STX)
				esTramaControl = true;
			else
				esTramaControl = false;

			numCampo++;

			break;
		case 4:
			if (maestro || esclavo) {
				td.N = car;
				t.NT = car;

				if (esTramaControl) {
					numCampo = 1;
					mostrarTramaControl(t);

					if (t.D == 'R')
						enviarConfirmacion(PuertoCOM, 'R', '0'); //Confirmacion de seleccion
					else if (t.D == 'T' && esclavo) {
						enviarConfirmacion(PuertoCOM, 'T', '0'); //Confirmacion de sondeo
						enviarFicheroME(PuertoCOM, 'T');

						solicitarCierreSondeo(PuertoCOM);
					}

					if (t.C == EOT) {

						if (esclavo) {
							esclavo = false;
							printf("\nLIBERADO\n");
						} else {
							responderSolicitudCierre(PuertoCOM, t.NT);
						}

					}

				} else {
					numCampo++;
				}
			} else {
				t.NT = car;
				td.N = car;
				if (esTramaControl) {
					numCampo = 1;
					mostrarTramaControl(t);
				} else
					numCampo++;

			}

			break;
		case 5:
			td.L = (unsigned char) car;
			numCampo++;

			break;

		case 6:

			td.Datos[numDato] = car;
			if (numDato < td.L - 1)
				numDato++;
			else {
				td.Datos[numDato + 1] = '\0';
				numCampo++;
				numDato = 0;

			}

			break;
		case 7:
			td.BCE = (unsigned char) car; //Casting Importante
			numCampo = 1;
			if (esFichero) {
				if (flujoFichero.is_open()) {
					if (td.BCE == calcularBCE(td.Datos, td.L)) { //Si va bien...
						flujoFichero.write(td.Datos, td.L);
						if (esclavo) {
							if (td.N == '0') {
								printf("\n[RECIBIDA] TRAMA DE DATOS - 0\n");
								enviarConfirmacion(PuertoCOM, 'R', '0');
							} else {
								printf("\n[RECIBIDA] TRAMA DE DATOS - 1\n");
								enviarConfirmacion(PuertoCOM, 'R', '1');

							}
						} else if (maestro) {

							if (td.N == '0') {
								printf("\n[RECIBIDA] TRAMA DE DATOS - 0\n");
								//enviarConfirmacion(PuertoCOM, 'R', '0');
								enviarConfirmacion(PuertoCOM, 'T', '0');
							} else {
								printf("\n[RECIBIDA] TRAMA DE DATOS - 1\n");
								//enviarConfirmacion(PuertoCOM, 'R', '1');
								enviarConfirmacion(PuertoCOM, 'T', '1');

							}

						}

					} else {
						printf("\nError al recibir trama BCE incorrecto...\n");

						if (esclavo) {
							enviarRechazo(PuertoCOM, 'R', td.N);
						} else if (maestro) {
							enviarRechazo(PuertoCOM, 'T', td.N);

						}

					}
				} else
					printf("\nError al intentar escribir en fichero...\n");
			} else
				mostrarTramaDatos(td);

			break;
		default:
			break;
		}

	}

}

void seleccionMaestroEsclavo(HANDLE PuertoCOM) {
	bool flag = true;
	int x;
	while (flag) {
		printf("\n===== PROTOCOLO Maestro-Esclavo =====\n");
		printf("(1) - Maestro\n");
		printf("(2) - Esclavo\n");

		cin >> x;
		switch (x) {
		case 1:
			printf("MAESTRO\n");
			flag = false;
			maestro = true;
			esclavo = false;
			seleccionMaestro(PuertoCOM);
			break;
		case 2:
			printf("ESCLAVO\n");
			flag = false;
			esclavo = true;
			maestro = false;
			break;
		default:
			printf("OPCION NO VALIDA\n");
		}

	}
}

void seleccionMaestro(HANDLE PuertoCOM) {
	bool flag = true;
	int x;
	while (flag) {
		printf("\n===== MAESTRO: Elija una opcion; =====\n");
		printf("(1) - Seleccion\n");
		printf("(2) - Sondeo\n");

		cin >> x;
		switch (x) {
		case 1:
			printf("SELECCION\n");
			seleccion(PuertoCOM); //Establecimiento
			enviarFicheroME(PuertoCOM, 'R'); //Tranferencia
			liberacionSeleccion(PuertoCOM); //Liberacion
			maestro = false;
			flag = false;
			break;
		case 2:
			printf("SONDEO\n");
			sondeo(PuertoCOM); //Establecimiento
			flag = false;
			break;
		default:
			printf("OPCION NO VALIDA\n");
		}

	}
}

void seleccion(HANDLE PuertoCOM) {
	bool seleccion = false;
	int campo = 1;
	//Enviar llamada de seleccion
	enviarLlamada(PuertoCOM, 'R');
	//Espera de confirmacion
	while (!seleccion)
		seleccion = recibirConfirmacionSeleccion(PuertoCOM, campo, '0');

}

void enviarLlamada(HANDLE PuertoCOM, unsigned char direccion) {
	EnviarCaracter(PuertoCOM, SYN); //Sincronismo = SYN =22
	EnviarCaracter(PuertoCOM, direccion); //Direccion=(R indica seleccion )
	EnviarCaracter(PuertoCOM, ENQ); //Control = (05 (ENQ), 04 (EOT), 06 (ACK), 21 (NACK))
	EnviarCaracter(PuertoCOM, '0'); //Numero de Trama = (En principio fijo a 0)
	printf("\n[ENVIADA] TRAMA ENQ - %c\n", '0');
}

void enviarConfirmacion(HANDLE PuertoCOM, unsigned char direccion,
		unsigned char num) {
	EnviarCaracter(PuertoCOM, SYN); //Sincronismo = SYN =22
	EnviarCaracter(PuertoCOM, direccion); //Direccion=(En principio fijo a T)
	EnviarCaracter(PuertoCOM, ACK); //Control = (05 (ENQ), 04 (EOT), 06 (ACK), 21 (NACK))
	EnviarCaracter(PuertoCOM, num); //Numero de Trama = (En principio fijo a 0)
	printf("\n[ENVIADA] TRAMA ACK - %c\n", num);
}

bool recibirConfirmacionSeleccion(HANDLE PuertoCOM, int &campo,
		unsigned char num) {
	char car = 0;
	car = RecibirCaracter(PuertoCOM);
	bool ack = false;

	if (car) {
		switch (campo) {
		case 1:
			if (car == SYN)
				campo++;

			break;
		case 2:
			if (car == 'R')
				campo++;

			break;
		case 3:
			if (car == ACK)
				campo++;

			break;
		case 4:
			if (car == num) {
				ack = true;
				printf("[RECIBIDA] TRAMA ACK - %c", num);
			}
			campo = 1;

			break;
		}
	}
	return ack;

}

void enviarFicheroME(HANDLE PuertoCOM, unsigned char direccion) {

	//printf("\nEnviando fichero\n");
	int c;
	char datos[255], tecla;
	ifstream flujoFicheroLectura;
	int longitudFichero = 0, trama = 0, campo = 1;
	char fichero[255];
	flujoFicheroLectura.open("FRC-E.txt");
	bool tramaRecibida = false, salir = false, error = false, ack = false;
	ofstream flujoFicheroEscritura;
	TramaDatos td;
	TramaControl t;

	if (flujoFicheroLectura.is_open()) {

		//Enviamos caracter fichero '$' antes de la primera trama de datos
		EnviarCaracter(PuertoCOM, '$');

		while (!flujoFicheroLectura.eof() && !salir) {

			flujoFicheroLectura.read(fichero, 254);
			longitudFichero = flujoFicheroLectura.gcount();
			fichero[longitudFichero] = '\0';

			td.S = SYN;
			EnviarCaracter(PuertoCOM, SYN); //Sincronismo = SYN =22
			td.D = direccion;
			EnviarCaracter(PuertoCOM, direccion); //Direccion=(En principio fijo a T)
			td.C = STX;
			EnviarCaracter(PuertoCOM, STX); //Control = STX = 02;

			if (trama % 2 == 0) {
				td.N = '0';
				EnviarCaracter(PuertoCOM, '0'); //NumTrama = (0 en pares);
			} else {
				td.N = '1';
				EnviarCaracter(PuertoCOM, '1'); //NumTrama = (1 en impares);
			}

			//Enviamos el campo LONG
			td.L = longitudFichero;
			EnviarCaracter(PuertoCOM, longitudFichero);

			//Enviamos Campo de datos
			for (c = 0; c < longitudFichero && c < 254; c++) {
				if (error && c == 0) {
					EnviarCaracter(PuertoCOM, 'ç');
					error = false;
				} else {
					EnviarCaracter(PuertoCOM, fichero[c]);
				}
				datos[c] = fichero[c];
				td.Datos[c] = datos[c];
			}
			td.BCE = calcularBCE(datos, c);
			EnviarCaracter(PuertoCOM, calcularBCE(datos, c)); //Calculo y envio del BCE

			if (trama % 2 == 0)
				printf("\n[ENVIADA] TRAMA DE DATOS - 0\n");
			else
				printf("\n[ENVIADA] TRAMA DE DATOS - 1\n");

			if (direccion == 'R') {

				while (!ack) {
					while (!tramaRecibida) {
						if (trama % 2 == 0)
							tramaRecibida = recibirConfirmacionError(PuertoCOM,
									campo, 'R', '0', t);
						else
							tramaRecibida = recibirConfirmacionError(PuertoCOM,
									campo, 'R', '1', t);

					}
					tramaRecibida = false;

					if (t.C == NACK) {
						reenviarTramaDatos(PuertoCOM, td);
					} else {
						ack = true;
					}

				}
				ack = false;
			} else {
				while (!ack) {
					while (!tramaRecibida) {
						if (trama % 2 == 0)
							tramaRecibida = recibirConfirmacionError(PuertoCOM,
									campo, 'T', '0', t);
						else
							tramaRecibida = recibirConfirmacionError(PuertoCOM,
									campo, 'T', '1', t);

					}
					tramaRecibida = false;

					if (t.C == NACK) {
						reenviarTramaDatos(PuertoCOM, td);
					} else {
						ack = true;
					}

				}
				ack = false;
			}

			tramaRecibida = false;
			trama++;

			if (kbhit()) {
				tecla = getch();
				if (tecla == 27) {
					salir = true;
					printf("\nESC PRESIONADO SALIENDO...\n");
				}
				if (tecla == FN) { //Comprobamos si es una tecla de Función
					tecla = getch();
					if (tecla == F5) {
						printf("*** ERROR EN TRAMA ***\n");
						error = true;
					}
				}

			}

		}
		//Enviamos caracter fichero '#' despues de la ultima trama
		EnviarCaracter(PuertoCOM, '#');

		flujoFicheroLectura.close();
		//printf("\nFichero enviado\n");
	} else
		printf("\nError al intentar abrir el fichero...\n");

}

void enviarEOT(HANDLE PuertoCOM, unsigned char direccion, unsigned char num) {
	EnviarCaracter(PuertoCOM, SYN); //Sincronismo = SYN =22
	EnviarCaracter(PuertoCOM, direccion); //Direccion=(En principio fijo a T o 'R')
	EnviarCaracter(PuertoCOM, EOT); //Control = (05 (ENQ), 04 (EOT), 06 (ACK), 21 (NACK))
	EnviarCaracter(PuertoCOM, num); //Numero de Trama = (En principio fijo a 0)
	printf("[ENVIADA] TRAMA EOT - %c", num);

}

void sondeo(HANDLE PuertoCOM) {
	bool sondeo = false;
	int campo = 1;
	//Enviar llamada de sondeo
	enviarLlamada(PuertoCOM, 'T');
	//Espera de confirmacion
	while (!sondeo)
		sondeo = recibirConfirmacionSondeo(PuertoCOM, campo, '0');
}

bool recibirConfirmacionSondeo(HANDLE PuertoCOM, int& campo,
		unsigned char num) {

	char car = 0;
	car = RecibirCaracter(PuertoCOM);
	bool ack = false;

	if (car) {
		switch (campo) {
		case 1:
			if (car == SYN)
				campo++;

			break;
		case 2:
			if (car == 'T')
				campo++;

			break;
		case 3:
			if (car == ACK)
				campo++;

			break;
		case 4:
			if (car == num) {
				ack = true;
				printf("[RECIBIDA] TRAMA ACK - %c\n", num);
			}
			campo = 1;

			break;
		}
	}
	return ack;

}

void liberacionSeleccion(HANDLE PuertoCOM) {
	bool liberacion = false;
	int campo = 1;
	enviarEOT(PuertoCOM, 'R', '0');
	while (!liberacion)
		liberacion = recibirConfirmacionSeleccion(PuertoCOM, campo, '0');

	printf("\n Se ha confirmado liberacion\n");
}

void solicitarCierreSondeo(HANDLE PuertoCOM) {
	bool cierre = false;
	bool recibido = false;
	int intento = 0, campo = 1;
	TramaControl t;

	while (!cierre) {
		if (intento % 2 == 0)
			enviarEOT(PuertoCOM, 'T', '0');
		else
			enviarEOT(PuertoCOM, 'T', '1');

		while (!recibido) {
			if (intento % 2 == 0) {
				recibido = recibirCierreSondeo(PuertoCOM, campo, '0', t);
			} else {
				recibido = recibirCierreSondeo(PuertoCOM, campo, '1', t);
			}

		}

		recibido = false;
		if (t.C == NACK)
			intento++;
		else
			cierre = true;

	}

}

bool recibirCierreSondeo(HANDLE PuertoCOM, int& campo, unsigned char num,
		TramaControl &t) {

	char car = 0;

	car = RecibirCaracter(PuertoCOM);

	bool recibido = false;

	if (car) {
		switch (campo) {
		case 1:
			if (car == SYN) {
				campo++;

				t.S = car;
			}

			break;
		case 2:
			if (car == 'T') {
				t.D = car;
				campo++;
			}
			break;
		case 3:
			if (car == ACK || car == NACK) {
				campo++;
				t.C = car;

			}

			break;
		case 4:
			if (car == num) {

				if (t.C == ACK) {
					printf("[RECIBIDA] TRAMA ACK - %c\n", num);
				} else {
					printf("[RECIBIDA] TRAMA NACK - %c\n", num);

				}

				recibido = true;
			}
			campo = 1;

			break;
		}
	}
	return recibido;

}

void responderSolicitudCierre(HANDLE PuertoCOM, unsigned char num) {
	bool opcion = false;
	//int x;
	char tecla;

	while (!opcion) {
		printf("\n===== CONFIRMACION DE CIERRE: Elija una opcion; =====\n");
		printf("(1) - ACEPTAR\n");
		printf("(2) - RECHAZAR\n");
		if (kbhit()) {
			opcion = true;
			tecla = getch();

			switch (tecla) {
			case 1:
				printf("ACEPTADO\n");
				enviarConfirmacion(PuertoCOM, 'T', num);
				//opcion = false;
				break;
			case 2:
				printf("RECHAZADO\n");
				enviarRechazo(PuertoCOM, 'T', num);
				//flag = false;
				break;
			default:
				opcion = false;
				printf("OPCION NO VALIDA\n");
			}
		}
	}

}

void reenviarTramaDatos(HANDLE PuertoCOM, TramaDatos td) {
	EnviarCaracter(PuertoCOM, td.S); //Sincronismo = SYN =22
	EnviarCaracter(PuertoCOM, td.D); //Direccion=(En principio fijo a T)
	EnviarCaracter(PuertoCOM, td.C); //Control = STX = 02;
	EnviarCaracter(PuertoCOM, td.N); //NumTrama = (0 en pares);
	EnviarCaracter(PuertoCOM, td.L); //Enviamos el campo LONG
	//Enviamos Campo de datos
	for (int c = 0; c < td.L && c < 254; c++) {
		EnviarCaracter(PuertoCOM, td.Datos[c]);
	}
	//td.BCE = calcularBCE(datos, c);
	EnviarCaracter(PuertoCOM, td.BCE); //Calculo y envio del BCE
	printf("\n[REENVIADA] TRAMA DE DATOS - ");
	printf("%c", td.N);
	printf("\n");

}

void enviarRechazo(HANDLE PuertoCOM, unsigned char dir, unsigned char num) {
	EnviarCaracter(PuertoCOM, SYN); //Sincronismo = SYN =22
	EnviarCaracter(PuertoCOM, dir); //Direccion=(En principio fijo a T)
	EnviarCaracter(PuertoCOM, NACK); //Control = (05 (ENQ), 04 (EOT), 06 (ACK), 21 (NACK))
	EnviarCaracter(PuertoCOM, num); //Numero de Trama = (En principio fijo a 0)
	printf("[ENVIADA] TRAMA NACK - %c\n", num);
}

bool recibirConfirmacionError(HANDLE PuertoCOM, int& campo, unsigned char dir,
		unsigned char num, TramaControl& t) {
	char car = 0;

	car = RecibirCaracter(PuertoCOM);

	bool recibido = false;

	if (car) {
		switch (campo) {
		case 1:
			if (car == SYN) {
				campo++;

				t.S = car;
			}

			break;
		case 2:
			if (car == dir) {
				t.D = car;
				campo++;
			}
			break;
		case 3:
			if (car == ACK || car == NACK) {
				campo++;
				t.C = car;

			}

			break;
		case 4:
			if (car == num) {

				if (t.C == ACK) {
					printf("[RECIBIDA] TRAMA ACK - %c", num);
				} else {
					printf("[RECIBIDA] TRAMA NACK - %c", num);

				}

				recibido = true;
			}
			campo = 1;

			break;
		}
	}
	return recibido;
}
