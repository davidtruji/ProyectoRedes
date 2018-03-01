/*
 *  Creado el: 28/2/2018
 *      Curso: 2�
 *      Autor: David Trujillo Torres
 *		Autor: Alberto Diaz Martin
 */

//FUNDAMENTOS DE REDES Y COMUNICACIONES - CURSO 2016/17
//LIBRERÃ�A QUE CONTROLA EL PUERTO SERIE
#include "PuertoSerie.h"
#include <stdio.h>
#include <iostream>
using namespace std;

//Abre el puerto serie indicado en "NombrePuerto, con parÃ¡metros de configuraciÃ³n y timeouts por defecto.
HANDLE AbrirPuerto(LPCSTR NombrePuerto, DWORD Velocidad, BYTE NumBitsXByte,
		BYTE Paridad, BYTE BitsParada) {
	HANDLE COMAux = NULL;        // Manejador local de puerto serie
	//DWORD Error;
	DCB PuertoDCB; // Estructura para almacenar la configuraciÃ³n del puerto serie
	COMMTIMEOUTS CommTimeouts; // Estructura para almacenar los timeouts de las operaciones de I/O
	COMMPROP PropiedadesPuerto;  // Propiedades del puerto
	COMSTAT EstadoPuerto;  // Estado del puerto
	DWORD EstadoError;   // Estado del error
	// Abrir el puerto serie:
	COMAux = CreateFile(NombrePuerto, // Puerto serie a abrir
			GENERIC_READ | GENERIC_WRITE, // Se accederÃ¡ en modo lectura/escritura
			0,              // Abrir en modo compartido (siempre a 0)
			NULL, // ParÃ¡metro relacionado con la seguridad de las operaciones de I/O
			OPEN_EXISTING, // El puerto serie debe existir previamente en el SO (fÃ­sico o virtual)
			0,              // Atributos extra (siempre a 0)
			NULL);          // Manejo sÃ­ncrono del puerto
	SetupComm(COMAux, 0, 0);
	// Inicializa la estructura de configuraciÃ³n del puerto serie:
	PuertoDCB.DCBlength = sizeof(DCB);

	// Obtiene los parÃ¡metros actuales de configuraciÃ³n del puerto serie y los almacena en "PuertoDCB":
	GetCommState(COMAux, &PuertoDCB);

	// Modifica los parÃ¡metros de configuraciÃ³n en la estructura "PuertoDCB":
	PuertoDCB.BaudRate = Velocidad;              // Velocidad
	PuertoDCB.fBinary = TRUE; // Transferencia en modo binario (no se comprueba EOF)
	PuertoDCB.fParity = TRUE;             // ComprobaciÃ³n de paridad activada
	PuertoDCB.fOutxCtsFlow = FALSE;         // Control de flujo CTS desactivado
	PuertoDCB.fOutxDsrFlow = FALSE;         // Control de flujo DSR desactivado
	PuertoDCB.fDtrControl = DTR_CONTROL_ENABLE; // Control de flujo DTR activado
	PuertoDCB.fDsrSensitivity = FALSE; // Driver de comunicaciones NO sensible a DSR
	PuertoDCB.fTXContinueOnXoff = TRUE; // La transferencia continÃºa aunque el buffer de entrada se llene
	PuertoDCB.fOutX = FALSE; // Control de flujo XON/XOFF desactivado durante el envÃ­o
	PuertoDCB.fInX = FALSE; // Control de flujo XON/XOFF desactivado durante la recepciÃ³n
	PuertoDCB.fErrorChar = FALSE; // SustituciÃ³n de bytes errÃ³neos por el byte "ErrorChar" desactivada
	PuertoDCB.fNull = FALSE;               // Descarte de bytes NULL desactivado
	PuertoDCB.fRtsControl = RTS_CONTROL_ENABLE; // Control de flujo RTS activado
	PuertoDCB.fAbortOnError = FALSE; // FinalizaciÃ³n de la comunicaciÃ³n ante un error desactivada
	PuertoDCB.ByteSize = NumBitsXByte; // NÃºmero de bits en cada byte enviado o recibido (4..8)
	PuertoDCB.Parity = Paridad; // Paridad (0=sin paridad, 1=impar, 2=par, 3=marca, 4=espacio)
	PuertoDCB.StopBits = BitsParada; // Bits de stop (0 = 1 bit, 1 = 1.5 bits, 2 = 2 bits)

	// Establece en el puerto serie la configuraciÃ³n especificada en PuertoDCB:
	if (!SetCommState(COMAux, &PuertoDCB)) {
		printf("Error al configurar el puerto serie\n");
		return NULL;
	}

	// Obtiene los parÃ¡metros de timeout actuales del puerto serie y los almacena en "CommTimeouts":
	GetCommTimeouts(COMAux, &CommTimeouts);

	// Modifica los parÃ¡metros de timeout en la estructura "CommTimeouts".
	CommTimeouts.ReadIntervalTimeout = MAXDWORD; // Para conseguir lecturas no bloqueantes
	CommTimeouts.ReadTotalTimeoutMultiplier = 0;
	CommTimeouts.ReadTotalTimeoutConstant = 0;
	CommTimeouts.WriteTotalTimeoutMultiplier = 0;
	CommTimeouts.WriteTotalTimeoutConstant = 0;

	// Establece en el puerto serie los parÃ¡metros de timeout especificados en CommTimeouts:
	if (!SetCommTimeouts(COMAux, &CommTimeouts)) {
		printf("Error al configurar timeouts en el puerto serie\n");
		return NULL;
	}
	return (COMAux);
}

//Libera el manejador de puerto serie.
void CerrarPuerto(HANDLE &PuertoCOM) {
	CloseHandle(PuertoCOM);
	return;
}

//EnvÃ­a el caracter "CarAEnviar" a travÃ©s de un puerto serie que debe estar abierto previamente.
//Devuelve un booleano con el resultado de la operaciÃ³n de envÃ­o.
BOOL EnviarCaracter(HANDLE &PuertoCOM, char CarAEnviar) {
	DWORD NumCarAEnviar = 1; // NÃºmero de caracteres que se van a enviar a travÃ©s del puerto serie
	DWORD NumCarEnviados = 0;  // NÃºmero de caracteres correctamente enviados
	if (WriteFile(PuertoCOM, (LPCVOID) &CarAEnviar, NumCarAEnviar,
			&NumCarEnviados, NULL) == 0)
		return FALSE;
	else
		return TRUE;
}

//EnvÃ­a la cadena de caracteres "CadenaAEnviar" a travÃ©s del puerto serie. El parÃ¡metro "longitud"
//es el nÃºmero de caracteres de la cadena. Devuelve un booleano con el resultado de la operaciÃ³n.
BOOL EnviarCadena(HANDLE &PuertoCOM, const void *CadenaAEnviar, int longitud) {
	int i;
	char *cadaux = (char *) CadenaAEnviar;
	for (i = 0; i < longitud; ++i)
		if (EnviarCaracter(PuertoCOM, cadaux[i]) == FALSE)
			return FALSE;
	return TRUE;
}

//Recibe un caracter por el puerto serie (si el caracter ya estÃ¡ esperando en el buffer de entrada).
//Devuelve el caracter recibido o el valor 0 (si se produjo algÃºn error o si no habÃ­a caracter
//en el buffer de entrada).
char RecibirCaracter(HANDLE &PuertoCOM)

{
	char CaracterRecibido = 0;  // Caracter recibido del puerto serie
	int NumCarARecibir = 1; // NÃºmero de caracteres que se espera recibir a travÃ©s del puerto serie
	DWORD NumCarRecibidos = 0; // NÃºmero de caracteres correctamente recibidos
	if (ReadFile(PuertoCOM, &CaracterRecibido, NumCarARecibir, &NumCarRecibidos,
			0) > 0) {
		if (NumCarRecibidos == NumCarARecibir)
			return (CaracterRecibido);
		else
			return (0);
	} else
		return (0);
}

//Lee del puerto serie hasta que consigue "NumCaracteres" caracteres.
BOOL RecibirCadena(HANDLE &PuertoCOM, char *CadenaRecibida, int NumCaracteres) {
	char caraux = 0;
	char cadaux[4096] = { 0 };
	int i = 0;

	while ((i < NumCaracteres)) {
		caraux = RecibirCaracter(PuertoCOM);
		if (caraux)
			cadaux[i++] = caraux;
	}
	strcpy(CadenaRecibida, cadaux);
	return TRUE;
}

//Modifica el circuito de control DTR.
void SetDTR(HANDLE &PuertoCOM, int Estado) {
	if (Estado)
		EscapeCommFunction(PuertoCOM, SETDTR);
	else
		EscapeCommFunction(PuertoCOM, CLRDTR);
}

//Modifica el circuito de control RTS.
void SetRTS(HANDLE &PuertoCOM, int Estado) {
	if (Estado)
		EscapeCommFunction(PuertoCOM, SETRTS);
	else
		EscapeCommFunction(PuertoCOM, CLRRTS);
}

//Devuelve el estado de CTS.
BOOL GetCTS(HANDLE &PuertoCOM) {
	DWORD EstadoCircuitosControl;
	BOOL _CTS;
	if (!GetCommModemStatus(PuertoCOM, &EstadoCircuitosControl)) {
		printf("Error al obtener estado de CTS");
		return (-1);
	} else
		_CTS = MS_CTS_ON & EstadoCircuitosControl;
	return (_CTS);
}

//Devuelve el estado de DCD.
BOOL GetDCD(HANDLE &PuertoCOM) {
	DWORD EstadoCircuitosControl;
	BOOL _DCD;
	if (!GetCommModemStatus(PuertoCOM, &EstadoCircuitosControl)) {
		printf("Error al obtener estado de DCD");
		return (-1);
	} else
		_DCD = MS_RLSD_ON & EstadoCircuitosControl;
	return (_DCD);
}

//Devuelve el estado de DSR
BOOL GetDSR(HANDLE &PuertoCOM) {
	DWORD EstadoCircuitosControl;
	BOOL _DSR;
	if (!GetCommModemStatus(PuertoCOM, &EstadoCircuitosControl)) {
		printf("Error al obtener estado de DSR");
		return (-1);
	} else
		_DSR = MS_DSR_ON & EstadoCircuitosControl;
	return (_DSR);
}

//Devuelve el estado de RI
BOOL GetRI(HANDLE &PuertoCOM) {
	DWORD EstadoCircuitosControl;
	BOOL _RI;
	if (!GetCommModemStatus(PuertoCOM, &EstadoCircuitosControl)) {
		printf("Error al obtener estado de RI");
		return (-1);
	} else
		_RI = MS_RING_ON & EstadoCircuitosControl;
	return (_RI);
}
void ComprobarTamanoBuffers(HANDLE &PuertoCOM) {
	COMMPROP PropiedadesPuerto;
	COMSTAT EstadoPuerto;
	DWORD EstadoError;
	GetCommProperties(PuertoCOM, &PropiedadesPuerto);
	//Muestra el tamaÃ±o de los Buffers
	printf("\n TamaÃ±o BUFFER TX: %lu", PropiedadesPuerto.dwCurrentTxQueue);
	printf("\n TamaÃ±o BUFFER RX: %lu", PropiedadesPuerto.dwCurrentRxQueue);

	// Leer estado del puerto
	ClearCommError(PuertoCOM, &EstadoError, &EstadoPuerto);

	//Mostrar tamaÃ±o ocupado
	cout << "\n Ocupado BUFFER TX:" << EstadoPuerto.cbInQue;
	cout << "\n Ocupado BUFFER RX:" << EstadoPuerto.cbOutQue;
}

//Cambiar el tamaÃ±o de los buffers
int CambiarTamanoBuffers(HANDLE &PuertoCOM, int TamBufferEntrada,
		int TamBufferSalida) {
	int sal = 0;
	COMSTAT EstadoPuerto;
	sal = SetupComm(PuertoCOM, TamBufferEntrada, TamBufferSalida);
	return (sal);
}

//VacÃ­a el buffer de entrada.
int VaciarBufferEntrada(HANDLE &PuertoCOM) {
	if (!PurgeComm(PuertoCOM, PURGE_RXCLEAR)) {
		printf("Error al vaciar buffer de entrada");
		return (-1);
	}
	return (0);
}

//VacÃ­a el buffer de salida.
int VaciarBufferSalida(HANDLE &PuertoCOM) {
	if (!PurgeComm(PuertoCOM, PURGE_TXCLEAR)) {
		printf("Error al vaciar buffer de salida");
		return (-1);
	}
	return (0);
}

//VacÃ­a los buffers de entrada y de salida.
int VaciarBuffers(HANDLE &PuertoCOM) {
	if (!PurgeComm(PuertoCOM, PURGE_RXCLEAR)) {
		printf("Error al vaciar buffer de entrada");
		return (-1);
	}
	if (!PurgeComm(PuertoCOM, PURGE_TXCLEAR)) {
		printf("Error al vaciar buffer de salida");
		return (-1);
	}
	return (0);
}
