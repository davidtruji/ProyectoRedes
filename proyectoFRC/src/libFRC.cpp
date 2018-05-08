/*
 *  Creado el: 8/5/2018
 *      Curso: 2º
 *      Autor: David Trujillo Torres
 *		Autor: Alberto Diaz Martin
 */

#include "libFRC.h"

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
			EnviarCaracter(PuertoCOM, 'T'); //Direccion=(En principio fijo a ’T’)
			EnviarCaracter(PuertoCOM, STX); //Control = STX = 02;
			EnviarCaracter(PuertoCOM, '0'); //NumTrama = (En principio fijo a ‘0’);

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
				printf("\nRecibiendo fichero\n");

			} else if (car == '#') {
				esFichero = false;
				flujoFichero.close();
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
								//enviarConfirmacion(PuertoCOM, 'T', '0');
							} else {
								printf("\n[RECIBIDA] TRAMA DE DATOS - 1\n");
								enviarConfirmacion(PuertoCOM, 'R', '1');
								//enviarConfirmacion(PuertoCOM, 'T', '1');

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

					} else
						printf("\nError al recibir trama BCE incorrecto...\n");

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
		cout << "\n===== PROTOCOLO Maestro-Esclavo =====\n";
		cout << "(1) - Maestro\n";
		cout << "(2) - Esclavo\n";

		cin >> x;
		switch (x) {
		case 1:
			cout << "(1) - Maestro\n";
			flag = false;
			maestro = true;
			esclavo = false;
			seleccionMaestro(PuertoCOM);
			break;
		case 2:
			cout << "(2) - Esclavo\n";
			flag = false;
			esclavo = true;
			maestro = false;
			break;
		default:
			cout << "Elija una opcion valida\n";
		}

	}
}

void seleccionMaestro(HANDLE PuertoCOM) {
	bool flag = true;
	int x;
	while (flag) {
		cout << "\n===== MAESTRO: Elija una opcion; =====\n";
		cout << "(1) - Seleccion\n";
		cout << "(2) - Sondeo\n";

		cin >> x;
		switch (x) {
		case 1:
			cout << "(1) - Seleccion\n";
			seleccion(PuertoCOM); //Establecimiento
			enviarFicheroME(PuertoCOM, 'R'); //Tranferencia
			liberacionSeleccion(PuertoCOM); //Liberacion
			maestro = false;
			flag = false;
			break;
		case 2:
			cout << "(2) - Sondeo\n";
			sondeo(PuertoCOM); //Establecimiento
			//TODO:Liberacion
			//maestro = false;
			flag = false;

			break;
		default:
			cout << "Elija una opcion valida\n";
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
	EnviarCaracter(PuertoCOM, direccion); //Direccion=(’R’ indica seleccion )
	EnviarCaracter(PuertoCOM, ENQ); //Control = (05 (ENQ), 04 (EOT), 06 (ACK), 21 (NACK))
	EnviarCaracter(PuertoCOM, '0'); //Numero de Trama = (En principio fijo a ‘0’)
	printf("\n[ENVIADA] TRAMA ENQ\n");
}

void enviarConfirmacion(HANDLE PuertoCOM, unsigned char direccion,
		unsigned char num) {
	EnviarCaracter(PuertoCOM, SYN); //Sincronismo = SYN =22
	EnviarCaracter(PuertoCOM, direccion); //Direccion=(En principio fijo a ’T’)
	EnviarCaracter(PuertoCOM, ACK); //Control = (05 (ENQ), 04 (EOT), 06 (ACK), 21 (NACK))
	EnviarCaracter(PuertoCOM, num); //Numero de Trama = (En principio fijo a ‘0’)
	cout << "[ENVIADA] TRAMA ACK - " << num << endl;
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
				cout << "[RECIBIDA] TRAMA ACK - " << num << endl;

			}
			campo = 1;

			break;
		}
	}
	return ack;

}

void enviarFicheroME(HANDLE PuertoCOM, unsigned char direccion) {

	printf("\nEnviando fichero\n");
	int c;
	char datos[255];
	ifstream flujoFicheroLectura;
	int longitudFichero = 0, trama = 0, campo = 1;
	char fichero[255];
	flujoFicheroLectura.open("FRC-E.txt");
	bool tramaRecibida = false;
	ofstream flujoFicheroEscritura;

	if (flujoFicheroLectura.is_open()) {

		//Enviamos caracter fichero '$' antes de la primera trama de datos
		EnviarCaracter(PuertoCOM, '$');

		while (!flujoFicheroLectura.eof()) {

			flujoFicheroLectura.read(fichero, 254);
			longitudFichero = flujoFicheroLectura.gcount();
			fichero[longitudFichero] = '\0';

			EnviarCaracter(PuertoCOM, SYN); //Sincronismo = SYN =22
			EnviarCaracter(PuertoCOM, direccion); //Direccion=(En principio fijo a ’T’)
			EnviarCaracter(PuertoCOM, STX); //Control = STX = 02;
			if (trama % 2 == 0)
				EnviarCaracter(PuertoCOM, '0'); //NumTrama = (0 en pares);
			else
				EnviarCaracter(PuertoCOM, '1'); //NumTrama = (1 en impares);

			//Enviamos el campo LONG
			EnviarCaracter(PuertoCOM, longitudFichero);

			//Enviamos Campo de datos
			for (c = 0; c < longitudFichero && c < 254; c++) {

				EnviarCaracter(PuertoCOM, fichero[c]);
				datos[c] = fichero[c];
			}

			EnviarCaracter(PuertoCOM, calcularBCE(datos, c)); //Calculo y envio del BCE

			if (trama % 2 == 0)
				printf("\n[ENVIADA] TRAMA DE DATOS - 0\n");
			else
				printf("\n[ENVIADA] TRAMA DE DATOS - 1\n");

			if (direccion == 'R') {
				while (!tramaRecibida) {
					if (trama % 2 == 0)
						tramaRecibida = recibirConfirmacionSeleccion(PuertoCOM,
								campo, '0');
					else
						tramaRecibida = recibirConfirmacionSeleccion(PuertoCOM,
								campo, '1');

				}
			} else {
				while (!tramaRecibida) {
					if (trama % 2 == 0)
						tramaRecibida = recibirConfirmacionSondeo(PuertoCOM,
								campo, '0');
					else
						tramaRecibida = recibirConfirmacionSondeo(PuertoCOM,
								campo, '1');

				}
			}

			tramaRecibida = false;
			trama++;
		}
		//Enviamos caracter fichero '#' despues de la ultima trama
		EnviarCaracter(PuertoCOM, '#');

		flujoFicheroLectura.close();
		printf("\nFichero enviado\n");
	} else
		printf("\nError al intentar abrir el fichero...\n");

}

void enviarEOT(HANDLE PuertoCOM, unsigned char direccion, unsigned char num) {
	EnviarCaracter(PuertoCOM, SYN); //Sincronismo = SYN =22
	EnviarCaracter(PuertoCOM, direccion); //Direccion=(En principio fijo a ’T’ o 'R')
	EnviarCaracter(PuertoCOM, EOT); //Control = (05 (ENQ), 04 (EOT), 06 (ACK), 21 (NACK))
	EnviarCaracter(PuertoCOM, num); //Numero de Trama = (En principio fijo a ‘0’)
	cout << "[ENVIADA] TRAMA EOT - " << num << endl;

}

void sondeo(HANDLE PuertoCOM) {
	bool sondeo = false;
	int campo = 1;
	//Enviar llamada de sondeo
	enviarLlamada(PuertoCOM, 'T');
	//Espera de confirmacion
	while (!sondeo)
		sondeo = recibirConfirmacionSondeo(PuertoCOM, campo, '0');

	cout << "SONDEO ESTABLeCIDO" << endl;

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
				cout << "[RECIBIDA] TRAMA ACK - " << num << endl;

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
					cout << "[RECIBIDA] TRAMA ACK - " << num << endl;
				} else {
					cout << "[RECIBIDA] TRAMA NACK - " << num << endl;
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
	bool flag = true;
	int x;
	while (flag) {
		cout << "\n===== CONFIRMACION DE CIERRE: Elija una opcion; =====\n";
		cout << "(1) - ACEPTAR\n";
		cout << "(2) - RECHAZAR\n";

		cin >> x;
		switch (x) {
		case 1:
			cout << "ACEPTADO\n";
			enviarConfirmacion(PuertoCOM, 'T', num);
			flag = false;
			break;
		case 2:
			cout << "RECHAZADO\n";
			enviarRechazo(PuertoCOM, num);
			flag = false;
			break;
		default:
			cout << "Elija una opcion valida\n";
		}

	}

}

void enviarRechazo(HANDLE PuertoCOM, unsigned char num) {
	EnviarCaracter(PuertoCOM, SYN); //Sincronismo = SYN =22
	EnviarCaracter(PuertoCOM, 'T'); //Direccion=(En principio fijo a ’T’)
	EnviarCaracter(PuertoCOM, NACK); //Control = (05 (ENQ), 04 (EOT), 06 (ACK), 21 (NACK))
	EnviarCaracter(PuertoCOM, num); //Numero de Trama = (En principio fijo a ‘0’)
	cout << "[ENVIADA] TRAMA NACK - " << num << endl;
}
