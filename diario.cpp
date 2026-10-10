/**
 * ============================================================================
 * Proyecto: FinCost C++ - Sistema de Contabilidad Financiera y Costos
 * Archivo:  diario.cpp
 * Descripción: Implementación del Módulo 2 - Libro Diario y Registro de Asientos.
 *              Incluye CRUD completo con bajas logicas, verificacion de partida
 *              doble estricta, validacion de fecha y cuentas activas.
 * Paradigma: Programación Estructurada (Estricto sin clases / POO).
 * ============================================================================
 */

#include "diario.h"
#include "catalogo.h"  // Para cuentaActiva()
#include "reportes.h"  // Para imprimirSeparador() y pausarConsola()
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>
#include <cmath>
#include <cctype>
#include <limits>

using namespace std;

// Capacidad maxima de movimientos por partida (arreglo estatico)
static const int MAX_MOVIMIENTOS = 50;

// ============================================================================
// Funciones Auxiliares Locales
// ============================================================================

/**
 * Lee un entero de forma segura protegiendo contra EOF.
 */
static int leerEntero(const char* mensaje) {
    int valor = 0;
    if (cin.eof()) {
        return -1;
    }
    if (mensaje && strlen(mensaje) > 0) {
        cout << mensaje;
    }
    while (!(cin >> valor)) {
        if (cin.eof()) {
            return -1;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << " [!] Entrada invalida. Ingrese un numero entero: ";
    }
    if (!cin.eof()) {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    return valor;
}

/**
 * Lee un double de forma segura protegiendo contra EOF.
 */
static double leerDouble(const char* mensaje) {
    double valor = 0.0;
    if (cin.eof()) {
        return -1.0;
    }
    if (mensaje && strlen(mensaje) > 0) {
        cout << mensaje;
    }
    while (!(cin >> valor)) {
        if (cin.eof()) {
            return -1.0;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << " [!] Entrada invalida. Ingrese un valor numerico: ";
    }
    if (!cin.eof()) {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    return valor;
}

/**
 * Solicita confirmacion mediante (S/N).
 */
static bool confirmarOperacion(const char* mensaje) {
    if (cin.eof()) {
        return false;
    }
    cout << mensaje;
    char resp = 'N';
    if (!(cin >> resp)) {
        return false;
    }
    if (!cin.eof()) {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    return (resp == 'S' || resp == 's');
}

/**
 * Determina si un anio es bisiesto.
 */
static bool esAnioBisiesto(int anio) {
    return (anio % 4 == 0 && (anio % 100 != 0 || anio % 400 == 0));
}

/**
 * Valida que una cadena cumpla con el formato DD/MM/AAAA y fecha valida en calendario.
 * Rango de anio permitido: 2000 a 2100.
 */
static bool validarFormatoFecha(const char* fecha) {
    if (!fecha) {
        return false;
    }
    if (strlen(fecha) != 10) {
        return false;
    }
    if (fecha[2] != '/' || fecha[5] != '/') {
        return false;
    }
    for (int i = 0; i < 10; ++i) {
        if (i == 2 || i == 5) continue;
        if (!isdigit(static_cast<unsigned char>(fecha[i]))) {
            return false;
        }
    }

    int dia = (fecha[0] - '0') * 10 + (fecha[1] - '0');
    int mes = (fecha[3] - '0') * 10 + (fecha[4] - '0');
    int anio = (fecha[6] - '0') * 1000 + (fecha[7] - '0') * 100 + (fecha[8] - '0') * 10 + (fecha[9] - '0');

    if (anio < 2000 || anio > 2100) {
        return false;
    }
    if (mes < 1 || mes > 12) {
        return false;
    }

    int diasPorMes[13];
    diasPorMes[0] = 0;
    diasPorMes[1] = 31;
    diasPorMes[2] = esAnioBisiesto(anio) ? 29 : 28;
    diasPorMes[3] = 31;
    diasPorMes[4] = 30;
    diasPorMes[5] = 31;
    diasPorMes[6] = 30;
    diasPorMes[7] = 31;
    diasPorMes[8] = 31;
    diasPorMes[9] = 30;
    diasPorMes[10] = 31;
    diasPorMes[11] = 30;
    diasPorMes[12] = 31;

    if (dia < 1 || dia > diasPorMes[mes]) {
        return false;
    }

    return true;
}

/**
 * Solicita una fecha hasta que el usuario ingrese una fecha valida o se cierre la entrada.
 */
static bool pedirFechaValida(char* bufferDestino, int tamBuffer) {
    while (!cin.eof()) {
        cout << " Ingrese la fecha (DD/MM/AAAA): ";
        char entrada[30];
        memset(entrada, 0, sizeof(entrada));
        cin.getline(entrada, 30);
        if (cin.eof()) {
            return false;
        }
        if (validarFormatoFecha(entrada)) {
            strncpy(bufferDestino, entrada, tamBuffer - 1);
            bufferDestino[tamBuffer - 1] = '\0';
            return true;
        }
        cout << " [!] ERROR: Fecha invalida (" << entrada << "). Debe ser DD/MM/AAAA y fecha real.\n";
    }
    return false;
}

/**
 * Verifica si ya existe alguna partida activa con ese numero.
 */
static bool existePartidaActiva(int numPartida) {
    ifstream archivo(ARCHIVO_DIARIO, ios::binary | ios::in);
    if (!archivo.is_open()) {
        return false;
    }

    Asiento temp;
    while (archivo.read(reinterpret_cast<char*>(&temp), sizeof(Asiento))) {
        if (temp.numero_partida == numPartida && temp.activo == 1) {
            archivo.close();
            return true;
        }
    }
    archivo.close();
    return false;
}

// ============================================================================
// Implementación: cuentaTieneAsientosActivos()
// ============================================================================

bool cuentaTieneAsientosActivos(int codigoCuenta) {
    ifstream archivo(ARCHIVO_DIARIO, ios::binary | ios::in);
    if (!archivo.is_open()) {
        return false;
    }

    Asiento temp;
    while (archivo.read(reinterpret_cast<char*>(&temp), sizeof(Asiento))) {
        if (temp.codigo_cuenta == codigoCuenta && temp.activo == 1) {
            archivo.close();
            return true;
        }
    }
    archivo.close();
    return false;
}

// ============================================================================
// Implementación: registrarPartida()
// ============================================================================

void registrarPartida() {
    Asiento movimientos[MAX_MOVIMIENTOS];
    int cantidadMovimientos = 0;
    bool partidaCuadrada = false;

    do {
        cantidadMovimientos = 0;
        double sumaDebe = 0.0;
        double sumaHaber = 0.0;

        cout << "\n";
        imprimirSeparador(70, '=');
        cout << "       MODULO 2 :: REGISTRO DE PARTIDA EN LIBRO DIARIO\n";
        imprimirSeparador(70, '=');

        // --- Numero de partida ---
        int numeroPartida = leerEntero(" Ingrese el numero de partida: ");
        if (cin.eof() || numeroPartida <= 0) {
            cout << "\n [!] Numero de partida invalido. Operacion cancelada.\n";
            pausarConsola();
            return;
        }

        // Validacion: No puede repetirse entre partidas activas
        if (existePartidaActiva(numeroPartida)) {
            cout << "\n [!] ERROR: Ya existe una partida activa con el numero #"
                 << numeroPartida << ".\n";
            cout << "     No se permiten partidas duplicadas activas. Operacion cancelada.\n";
            pausarConsola();
            return;
        }

        // --- Fecha de la partida ---
        char fecha[15];
        memset(fecha, 0, sizeof(fecha));
        if (!pedirFechaValida(fecha, sizeof(fecha))) {
            return;
        }

        // --- Captura iterativa de movimientos ---
        cout << "\n";
        imprimirSeparador(70, '-');
        cout << " Ingrese los movimientos de la partida (maximo " << MAX_MOVIMIENTOS << ").\n";
        cout << " Ingrese '0' como codigo de cuenta para finalizar la partida.\n";
        imprimirSeparador(70, '-');

        bool continuarIngreso = true;
        while (continuarIngreso && cantidadMovimientos < MAX_MOVIMIENTOS) {
            if (cin.eof()) {
                return;
            }
            cout << "\n --- Movimiento #" << (cantidadMovimientos + 1) << " ---\n";

            int codigoCuenta = leerEntero("   Codigo de cuenta (0 para terminar): ");
            if (cin.eof()) {
                return;
            }
            if (codigoCuenta == 0) {
                continuarIngreso = false;
                break;
            }

            // Regla: Validar que la cuenta exista y este activa en catalogo
            if (!cuentaActiva(codigoCuenta)) {
                cout << "   [!] ERROR: La cuenta #" << codigoCuenta
                     << " no existe o no esta activa en el catalogo (cuentas.dat).\n";
                cout << "       Verifique el catalogo antes de asentar el movimiento.\n";
                continue; // Reintentar este mismo movimiento
            }

            Asiento& mov = movimientos[cantidadMovimientos];
            memset(&mov, 0, sizeof(Asiento));
            mov.activo = 1;
            mov.numero_partida = numeroPartida;
            mov.codigo_cuenta = codigoCuenta;
            strncpy(mov.fecha, fecha, sizeof(mov.fecha) - 1);

            cout << "   Descripcion: ";
            cin.getline(mov.descripcion, 100);
            if (cin.eof()) {
                return;
            }

            mov.debe = leerDouble("   Monto al DEBE ($): ");
            if (cin.eof()) {
                return;
            }
            if (mov.debe < 0.0) {
                cout << "   [!] Monto negativo no permitido. Asignado a $0.00.\n";
                mov.debe = 0.0;
            }

            mov.haber = leerDouble("   Monto al HABER ($): ");
            if (cin.eof()) {
                return;
            }
            if (mov.haber < 0.0) {
                cout << "   [!] Monto negativo no permitido. Asignado a $0.00.\n";
                mov.haber = 0.0;
            }

            sumaDebe += mov.debe;
            sumaHaber += mov.haber;
            cantidadMovimientos++;
        }

        if (cantidadMovimientos == 0) {
            cout << "\n [!] No se ingresaron movimientos. Operacion cancelada.\n";
            pausarConsola();
            return;
        }

        // --- Resumen y Comprobacion de Partida Doble ---
        cout << "\n";
        imprimirSeparador(70, '=');
        cout << " RESUMEN DE LA PARTIDA #" << numeroPartida << " (" << fecha << ")\n";
        imprimirSeparador(70, '-');
        cout << fixed << setprecision(2);
        cout << left  << setw(8)  << "  #"
             << left  << setw(10) << "CUENTA"
             << left  << setw(28) << "DESCRIPCION"
             << right << setw(12) << "DEBE ($)"
             << right << setw(12) << "HABER ($)"
             << "\n";
        imprimirSeparador(70, '-');

        for (int i = 0; i < cantidadMovimientos; ++i) {
            cout << left  << setw(8)  << (i + 1)
                 << left  << setw(10) << movimientos[i].codigo_cuenta
                 << left  << setw(28) << movimientos[i].descripcion
                 << right << setw(12) << movimientos[i].debe
                 << right << setw(12) << movimientos[i].haber
                 << "\n";
        }

        imprimirSeparador(70, '-');
        cout << left  << setw(46) << "  TOTALES:"
             << right << setw(12) << sumaDebe
             << right << setw(12) << sumaHaber
             << "\n";
        imprimirSeparador(70, '=');

        double diferencia = fabs(sumaDebe - sumaHaber);

        if (diferencia < 0.001) {
            partidaCuadrada = true;

            ofstream archivo(ARCHIVO_DIARIO, ios::binary | ios::app);
            if (!archivo.is_open()) {
                cout << "\n [!] ERROR CRITICO: No se pudo abrir \"" << ARCHIVO_DIARIO << "\".\n";
                pausarConsola();
                return;
            }

            for (int i = 0; i < cantidadMovimientos; ++i) {
                archivo.write(reinterpret_cast<const char*>(&movimientos[i]), sizeof(Asiento));
            }
            archivo.close();

            cout << "\n [OK] PARTIDA DOBLE CUADRADA EXITOSAMENTE.\n";
            cout << "      " << cantidadMovimientos
                 << " movimiento(s) guardado(s) en \"" << ARCHIVO_DIARIO << "\".\n";
            imprimirSeparador(70, '-');

        } else {
            cout << "\n";
            imprimirSeparador(70, '!');
            cout << " [RECHAZADO] LA PARTIDA NO CUADRA.\n";
            cout << "   Suma Debe:  $" << sumaDebe << "\n";
            cout << "   Suma Haber: $" << sumaHaber << "\n";
            cout << "   Diferencia: $" << diferencia << "\n";
            cout << "   Principio violado: La partida doble exige que Debe == Haber.\n";
            cout << "   Los datos NO fueron guardados en el archivo.\n";
            imprimirSeparador(70, '!');

            if (!confirmarOperacion("\n Desea reintentar el ingreso de la partida? (S/N): ")) {
                cout << "\n [i] Operacion descartada por el usuario.\n";
                pausarConsola();
                return;
            }
        }

    } while (!partidaCuadrada && !cin.eof());

    pausarConsola();
}

// ============================================================================
// Implementación: listarAsientos()
// ============================================================================

void listarAsientos() {
    ifstream archivo(ARCHIVO_DIARIO, ios::binary | ios::in);

    if (!archivo.is_open()) {
        cout << "\n";
        imprimirSeparador(65, '!');
        cout << " [!] El archivo \"" << ARCHIVO_DIARIO
             << "\" no existe o esta vacio.\n";
        cout << "     Registre partidas desde la opcion 1 para generar el archivo.\n";
        imprimirSeparador(65, '!');
        pausarConsola();
        return;
    }

    cout << "\n";
    imprimirSeparador(90, '=');
    cout << "               MODULO 2 :: LIBRO DIARIO GENERAL (ASIENTOS ACTIVOS)\n";
    imprimirSeparador(90, '=');

    cout << fixed << setprecision(2);
    cout << left  << setw(8)  << "PARTIDA"
         << left  << setw(14) << "FECHA"
         << left  << setw(10) << "CUENTA"
         << left  << setw(32) << "DESCRIPCION"
         << right << setw(13) << "DEBE ($)"
         << right << setw(13) << "HABER ($)"
         << "\n";
    imprimirSeparador(90, '-');

    Asiento asiento;
    int totalAsientos = 0;
    double granTotalDebe = 0.0;
    double granTotalHaber = 0.0;
    int partidaAnterior = -1;

    while (archivo.read(reinterpret_cast<char*>(&asiento), sizeof(Asiento))) {
        if (asiento.activo == 0) {
            continue; // Ignorar asientos inactivos
        }

        if (partidaAnterior != -1 && asiento.numero_partida != partidaAnterior) {
            imprimirSeparador(90, '.');
        }
        partidaAnterior = asiento.numero_partida;

        totalAsientos++;
        granTotalDebe += asiento.debe;
        granTotalHaber += asiento.haber;

        cout << left  << setw(8)  << asiento.numero_partida
             << left  << setw(14) << asiento.fecha
             << left  << setw(10) << asiento.codigo_cuenta
             << left  << setw(32) << asiento.descripcion
             << right << setw(13) << asiento.debe
             << right << setw(13) << asiento.haber
             << "\n";
    }

    archivo.close();

    if (totalAsientos == 0) {
        cout << "\n [i] El libro diario no contiene asientos activos registrados.\n";
    } else {
        imprimirSeparador(90, '=');
        cout << left  << setw(64) << " TOTALES GENERALES DEL DIARIO:"
             << right << setw(13) << granTotalDebe
             << right << setw(13) << granTotalHaber
             << "\n";
        imprimirSeparador(90, '=');
        cout << " Total de movimientos activos: " << totalAsientos << "\n";
    }
    imprimirSeparador(90, '-');

    pausarConsola();
}

// ============================================================================
// Implementación: consultarPartidaPorNumero()
// ============================================================================

void consultarPartidaPorNumero() {
    cout << "\n";
    imprimirSeparador(70, '=');
    cout << "       MODULO 2 :: CONSULTAR PARTIDA CONTABLE POR NUMERO\n";
    imprimirSeparador(70, '=');

    int numPartida = leerEntero(" Ingrese el numero de partida a consultar: ");
    if (cin.eof() || numPartida <= 0) {
        return;
    }

    ifstream archivo(ARCHIVO_DIARIO, ios::binary | ios::in);
    if (!archivo.is_open()) {
        cout << "\n [!] No se pudo abrir el archivo \"" << ARCHIVO_DIARIO << "\".\n";
        pausarConsola();
        return;
    }

    Asiento asiento;
    int contador = 0;
    double sumaDebe = 0.0;
    double sumaHaber = 0.0;
    char fechaPartida[15];
    memset(fechaPartida, 0, sizeof(fechaPartida));

    while (archivo.read(reinterpret_cast<char*>(&asiento), sizeof(Asiento))) {
        if (asiento.numero_partida == numPartida && asiento.activo == 1) {
            if (contador == 0) {
                strncpy(fechaPartida, asiento.fecha, sizeof(fechaPartida) - 1);
                cout << "\n";
                imprimirSeparador(80, '-');
                cout << " PARTIDA CONTABLE #" << numPartida << " | FECHA: " << fechaPartida << "\n";
                imprimirSeparador(80, '-');
                cout << fixed << setprecision(2);
                cout << left  << setw(6)  << " #"
                     << left  << setw(10) << "CUENTA"
                     << left  << setw(36) << "DESCRIPCION"
                     << right << setw(14) << "DEBE ($)"
                     << right << setw(14) << "HABER ($)"
                     << "\n";
                imprimirSeparador(80, '-');
            }
            contador++;
            sumaDebe += asiento.debe;
            sumaHaber += asiento.haber;

            cout << left  << setw(6)  << contador
                 << left  << setw(10) << asiento.codigo_cuenta
                 << left  << setw(36) << asiento.descripcion
                 << right << setw(14) << asiento.debe
                 << right << setw(14) << asiento.haber
                 << "\n";
        }
    }
    archivo.close();

    if (contador == 0) {
        cout << "\n [!] No se encontro ninguna partida activa con el numero #" << numPartida << ".\n";
    } else {
        imprimirSeparador(80, '-');
        cout << left  << setw(52) << " TOTALES DE LA PARTIDA:"
             << right << setw(14) << sumaDebe
             << right << setw(14) << sumaHaber
             << "\n";
        imprimirSeparador(80, '=');
    }

    pausarConsola();
}

// ============================================================================
// Implementación: consultarPorCuenta()
// ============================================================================

void consultarPorCuenta() {
    cout << "\n";
    imprimirSeparador(70, '=');
    cout << "       MODULO 2 :: CONSULTAR MOVIMIENTOS POR CUENTA CONTABLE\n";
    imprimirSeparador(70, '=');

    int codigoCuenta = leerEntero(" Ingrese el codigo de cuenta: ");
    if (cin.eof() || codigoCuenta <= 0) {
        return;
    }

    ifstream archivo(ARCHIVO_DIARIO, ios::binary | ios::in);
    if (!archivo.is_open()) {
        cout << "\n [!] No se pudo abrir el archivo \"" << ARCHIVO_DIARIO << "\".\n";
        pausarConsola();
        return;
    }

    Asiento asiento;
    int contador = 0;
    double sumaDebe = 0.0;
    double sumaHaber = 0.0;

    cout << fixed << setprecision(2);

    while (archivo.read(reinterpret_cast<char*>(&asiento), sizeof(Asiento))) {
        if (asiento.codigo_cuenta == codigoCuenta && asiento.activo == 1) {
            if (contador == 0) {
                cout << "\n";
                imprimirSeparador(88, '-');
                cout << " MOVIMIENTOS ACTIVOS ASOCIADOS A LA CUENTA #" << codigoCuenta << "\n";
                imprimirSeparador(88, '-');
                cout << left  << setw(10) << "PARTIDA"
                     << left  << setw(14) << "FECHA"
                     << left  << setw(36) << "DESCRIPCION"
                     << right << setw(14) << "DEBE ($)"
                     << right << setw(14) << "HABER ($)"
                     << "\n";
                imprimirSeparador(88, '-');
            }
            contador++;
            sumaDebe += asiento.debe;
            sumaHaber += asiento.haber;

            cout << left  << setw(10) << asiento.numero_partida
                 << left  << setw(14) << asiento.fecha
                 << left  << setw(36) << asiento.descripcion
                 << right << setw(14) << asiento.debe
                 << right << setw(14) << asiento.haber
                 << "\n";
        }
    }
    archivo.close();

    if (contador == 0) {
        cout << "\n [i] No se encontraron movimientos activos para la cuenta #" << codigoCuenta << ".\n";
    } else {
        imprimirSeparador(88, '=');
        cout << left  << setw(60) << " TOTAL ACUMULADO POR LA CUENTA:"
             << right << setw(14) << sumaDebe
             << right << setw(14) << sumaHaber
             << "\n";
        imprimirSeparador(88, '=');
    }

    pausarConsola();
}

// ============================================================================
// Implementación: consultarPorFecha()
// ============================================================================

void consultarPorFecha() {
    cout << "\n";
    imprimirSeparador(70, '=');
    cout << "       MODULO 2 :: CONSULTAR MOVIMIENTOS POR FECHA EXACTA\n";
    imprimirSeparador(70, '=');

    char fechaBuscada[15];
    memset(fechaBuscada, 0, sizeof(fechaBuscada));
    if (!pedirFechaValida(fechaBuscada, sizeof(fechaBuscada))) {
        return;
    }

    ifstream archivo(ARCHIVO_DIARIO, ios::binary | ios::in);
    if (!archivo.is_open()) {
        cout << "\n [!] No se pudo abrir el archivo \"" << ARCHIVO_DIARIO << "\".\n";
        pausarConsola();
        return;
    }

    Asiento asiento;
    int contador = 0;
    double sumaDebe = 0.0;
    double sumaHaber = 0.0;

    cout << fixed << setprecision(2);

    while (archivo.read(reinterpret_cast<char*>(&asiento), sizeof(Asiento))) {
        if (strcmp(asiento.fecha, fechaBuscada) == 0 && asiento.activo == 1) {
            if (contador == 0) {
                cout << "\n";
                imprimirSeparador(88, '-');
                cout << " MOVIMIENTOS ACTIVOS DEL DIA " << fechaBuscada << "\n";
                imprimirSeparador(88, '-');
                cout << left  << setw(10) << "PARTIDA"
                     << left  << setw(10) << "CUENTA"
                     << left  << setw(40) << "DESCRIPCION"
                     << right << setw(14) << "DEBE ($)"
                     << right << setw(14) << "HABER ($)"
                     << "\n";
                imprimirSeparador(88, '-');
            }
            contador++;
            sumaDebe += asiento.debe;
            sumaHaber += asiento.haber;

            cout << left  << setw(10) << asiento.numero_partida
                 << left  << setw(10) << asiento.codigo_cuenta
                 << left  << setw(40) << asiento.descripcion
                 << right << setw(14) << asiento.debe
                 << right << setw(14) << asiento.haber
                 << "\n";
        }
    }
    archivo.close();

    if (contador == 0) {
        cout << "\n [i] No se encontraron movimientos activos en la fecha " << fechaBuscada << ".\n";
    } else {
        imprimirSeparador(88, '=');
        cout << left  << setw(60) << " TOTALES REGISTRADOS EN LA FECHA:"
             << right << setw(14) << sumaDebe
             << right << setw(14) << sumaHaber
             << "\n";
        imprimirSeparador(88, '=');
    }

    pausarConsola();
}

// ============================================================================
// Implementación: modificarPartida()
// ============================================================================

void modificarPartida() {
    cout << "\n";
    imprimirSeparador(70, '=');
    cout << "             MODULO 2 :: MODIFICAR PARTIDA CONTABLE\n";
    imprimirSeparador(70, '=');

    int numPartida = leerEntero(" Ingrese el numero de partida a modificar: ");
    if (cin.eof() || numPartida <= 0) {
        return;
    }

    // Verificar si existe la partida activa
    if (!existePartidaActiva(numPartida)) {
        cout << "\n [!] No se encontro ninguna partida activa con el numero #" << numPartida << ".\n";
        pausarConsola();
        return;
    }

    // Mostrar partida actual
    cout << "\n [i] Movimientos actuales de la partida #" << numPartida << ":\n";
    ifstream archivoLectura(ARCHIVO_DIARIO, ios::binary | ios::in);
    Asiento actual;
    while (archivoLectura.read(reinterpret_cast<char*>(&actual), sizeof(Asiento))) {
        if (actual.numero_partida == numPartida && actual.activo == 1) {
            cout << "     Cuenta: " << setw(6) << actual.codigo_cuenta
                 << " | Debe: $" << setw(10) << fixed << setprecision(2) << actual.debe
                 << " | Haber: $" << setw(10) << actual.haber
                 << " | Desc: " << actual.descripcion << "\n";
        }
    }
    archivoLectura.close();

    cout << "\n [i] Procedera a capturar todos los nuevos movimientos de la partida #" << numPartida << ".\n";
    if (!confirmarOperacion(" Desea continuar con la modificacion? (S/N): ")) {
        cout << "\n [i] Modificacion cancelada.\n";
        pausarConsola();
        return;
    }

    // Solicitar nueva fecha
    char nuevaFecha[15];
    memset(nuevaFecha, 0, sizeof(nuevaFecha));
    if (!pedirFechaValida(nuevaFecha, sizeof(nuevaFecha))) {
        return;
    }

    // Capturar nuevos movimientos en memoria temporal
    Asiento nuevosMovimientos[MAX_MOVIMIENTOS];
    int cantidadNuevos = 0;
    double sumaDebe = 0.0;
    double sumaHaber = 0.0;

    cout << "\n Ingrese los nuevos movimientos (0 en codigo de cuenta para terminar):\n";
    imprimirSeparador(70, '-');

    bool continuar = true;
    while (continuar && cantidadNuevos < MAX_MOVIMIENTOS) {
        if (cin.eof()) {
            return;
        }
        cout << "\n --- Nuevo Movimiento #" << (cantidadNuevos + 1) << " ---\n";
        int cod = leerEntero("   Codigo de cuenta (0 para terminar): ");
        if (cin.eof()) {
            return;
        }
        if (cod == 0) {
            continuar = false;
            break;
        }

        if (!cuentaActiva(cod)) {
            cout << "   [!] ERROR: La cuenta #" << cod << " no existe o no esta activa en cuentas.dat.\n";
            continue;
        }

        Asiento& mov = nuevosMovimientos[cantidadNuevos];
        memset(&mov, 0, sizeof(Asiento));
        mov.activo = 1;
        mov.numero_partida = numPartida;
        mov.codigo_cuenta = cod;
        strncpy(mov.fecha, nuevaFecha, sizeof(mov.fecha) - 1);

        cout << "   Descripcion: ";
        cin.getline(mov.descripcion, 100);
        if (cin.eof()) {
            return;
        }

        mov.debe = leerDouble("   Monto al DEBE ($): ");
        if (cin.eof()) return;
        if (mov.debe < 0.0) mov.debe = 0.0;

        mov.haber = leerDouble("   Monto al HABER ($): ");
        if (cin.eof()) return;
        if (mov.haber < 0.0) mov.haber = 0.0;

        sumaDebe += mov.debe;
        sumaHaber += mov.haber;
        cantidadNuevos++;
    }

    if (cantidadNuevos == 0) {
        cout << "\n [!] No se ingresaron movimientos. La partida original queda intacta.\n";
        pausarConsola();
        return;
    }

    double diferencia = fabs(sumaDebe - sumaHaber);
    if (diferencia >= 0.001) {
        cout << "\n";
        imprimirSeparador(70, '!');
        cout << " [RECHAZADO] LA NUEVA VERSION DE LA PARTIDA NO CUADRA.\n";
        cout << "   Debe: $" << fixed << setprecision(2) << sumaDebe
             << " | Haber: $" << sumaHaber
             << " | Diferencia: $" << diferencia << "\n";
        cout << "   REGLA: Los cambios fueron descartados y la partida original se conserva intacta.\n";
        imprimirSeparador(70, '!');
        pausarConsola();
        return;
    }

    // Pedir confirmacion final antes de aplicar cambios
    if (!confirmarOperacion("\n La partida cuadra. Desea aplicar los cambios a la partida? (S/N): ")) {
        cout << "\n [i] Modificacion cancelada. La partida original se conserva intacta.\n";
        pausarConsola();
        return;
    }

    // Paso 1: Inactivar movimientos viejos
    fstream archivoMod(ARCHIVO_DIARIO, ios::binary | ios::in | ios::out);
    if (!archivoMod.is_open()) {
        cout << "\n [!] Error al acceder a \"" << ARCHIVO_DIARIO << "\".\n";
        pausarConsola();
        return;
    }

    Asiento temp;
    streampos pos = 0;
    while (true) {
        pos = archivoMod.tellg();
        if (!archivoMod.read(reinterpret_cast<char*>(&temp), sizeof(Asiento))) {
            break;
        }
        if (temp.numero_partida == numPartida && temp.activo == 1) {
            temp.activo = 0; // Desactivar version vieja
            archivoMod.seekp(pos);
            archivoMod.write(reinterpret_cast<const char*>(&temp), sizeof(Asiento));
            archivoMod.seekg(pos + static_cast<streampos>(sizeof(Asiento)));
        }
    }
    archivoMod.close();

    // Paso 2: Agregar nuevos movimientos con el mismo numero_partida
    ofstream archivoAppend(ARCHIVO_DIARIO, ios::binary | ios::app);
    for (int i = 0; i < cantidadNuevos; ++i) {
        archivoAppend.write(reinterpret_cast<const char*>(&nuevosMovimientos[i]), sizeof(Asiento));
    }
    archivoAppend.close();

    cout << "\n [OK] Partida #" << numPartida << " modificada exitosamente.\n";
    pausarConsola();
}

// ============================================================================
// Implementación: eliminarPartida()
// ============================================================================

void eliminarPartida() {
    cout << "\n";
    imprimirSeparador(70, '=');
    cout << "       MODULO 2 :: ELIMINAR PARTIDA CONTABLE (BAJA LOGICA)\n";
    imprimirSeparador(70, '=');

    int numPartida = leerEntero(" Ingrese el numero de partida a dar de baja: ");
    if (cin.eof() || numPartida <= 0) {
        return;
    }

    if (!existePartidaActiva(numPartida)) {
        cout << "\n [!] No se encontro ninguna partida activa con el numero #" << numPartida << ".\n";
        pausarConsola();
        return;
    }

    cout << "\n [i] Detalle de la partida a dar de baja:\n";
    ifstream arch(ARCHIVO_DIARIO, ios::binary | ios::in);
    Asiento temp;
    while (arch.read(reinterpret_cast<char*>(&temp), sizeof(Asiento))) {
        if (temp.numero_partida == numPartida && temp.activo == 1) {
            cout << "     Cuenta: " << setw(6) << temp.codigo_cuenta
                 << " | Debe: $" << setw(10) << fixed << setprecision(2) << temp.debe
                 << " | Haber: $" << setw(10) << temp.haber
                 << " | Desc: " << temp.descripcion << "\n";
        }
    }
    arch.close();

    if (!confirmarOperacion("\n Esta seguro de dar de baja logica toda esta partida? (S/N): ")) {
        cout << "\n [i] Eliminacion cancelada por el usuario.\n";
        pausarConsola();
        return;
    }

    fstream archivo(ARCHIVO_DIARIO, ios::binary | ios::in | ios::out);
    if (!archivo.is_open()) {
        cout << "\n [!] No se pudo abrir \"" << ARCHIVO_DIARIO << "\".\n";
        pausarConsola();
        return;
    }

    int movimientosDadosDeBaja = 0;
    streampos pos = 0;
    while (true) {
        pos = archivo.tellg();
        if (!archivo.read(reinterpret_cast<char*>(&temp), sizeof(Asiento))) {
            break;
        }
        if (temp.numero_partida == numPartida && temp.activo == 1) {
            temp.activo = 0;
            archivo.seekp(pos);
            archivo.write(reinterpret_cast<const char*>(&temp), sizeof(Asiento));
            archivo.seekg(pos + static_cast<streampos>(sizeof(Asiento)));
            movimientosDadosDeBaja++;
        }
    }
    archivo.close();

    cout << "\n [OK] Partida #" << numPartida << " eliminada exitosamente ("
         << movimientosDadosDeBaja << " movimiento(s) dados de baja).\n";
    pausarConsola();
}

// ============================================================================
// Submenú del Módulo 2
// ============================================================================

void submenuDiario() {
    int opcion = 0;

    do {
        cout << "\n";
        imprimirSeparador(65, '=');
        cout << "    FINCOST C++ :: MODULO 2 - LIBRO DIARIO Y ASIENTOS CONTABLES\n";
        imprimirSeparador(65, '=');
        cout << " 1. Registrar nueva partida contable\n";
        cout << " 2. Listar todos los asientos del diario\n";
        cout << " 3. Consultar partida por numero\n";
        cout << " 4. Consultar movimientos por codigo de cuenta\n";
        cout << " 5. Consultar movimientos por fecha\n";
        cout << " 6. Modificar partida contable\n";
        cout << " 7. Eliminar partida contable (baja logica)\n";
        cout << " 8. Regresar al Menu Principal\n";
        imprimirSeparador(65, '-');
        cout << " Seleccione una opcion [1-8]: ";

        opcion = leerEntero("");
        if (cin.eof() || opcion == -1) {
            break;
        }

        switch (opcion) {
            case 1:
                registrarPartida();
                break;
            case 2:
                listarAsientos();
                break;
            case 3:
                consultarPartidaPorNumero();
                break;
            case 4:
                consultarPorCuenta();
                break;
            case 5:
                consultarPorFecha();
                break;
            case 6:
                modificarPartida();
                break;
            case 7:
                eliminarPartida();
                break;
            case 8:
                cout << "\n [i] Regresando al Menu Principal...\n";
                break;
            default:
                cout << "\n [!] Opcion invalida. Seleccione entre 1 y 8.\n";
                pausarConsola();
                break;
        }
    } while (opcion != 8 && !cin.eof());
}
