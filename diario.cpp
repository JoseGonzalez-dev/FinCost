/**
 * ============================================================================
 * Proyecto: FinCost C++ - Sistema de Contabilidad Financiera y Costos
 * Archivo:  diario.cpp
 * Descripción: Implementación del Módulo 2 - Libro Diario y Registro de Asientos.
 *              Implementa el registro de partidas con validación estricta del
 *              principio de partida doble (Debe == Haber) antes de persistir.
 * Paradigma: Programación Estructurada (Estricto sin clases / POO).
 * ============================================================================
 */

#include "diario.h"
#include "reportes.h"  // Para imprimirSeparador() y pausarConsola()
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>
#include <cmath>
#include <limits>

using namespace std;

// Capacidad máxima de movimientos por partida (arreglo estático)
static const int MAX_MOVIMIENTOS = 50;

// ============================================================================
// Funciones Auxiliares Locales
// ============================================================================

/**
 * Lee un entero con validación robusta.
 */
static int leerEntero(const char* mensaje) {
    int valor;
    cout << mensaje;
    while (!(cin >> valor)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << " [!] Entrada invalida. Ingrese un numero.\n";
        cout << mensaje;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return valor;
}

/**
 * Lee un double con validación robusta.
 */
static double leerDouble(const char* mensaje) {
    double valor;
    cout << mensaje;
    while (!(cin >> valor)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << " [!] Entrada invalida. Ingrese un valor numerico.\n";
        cout << mensaje;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return valor;
}

// ============================================================================
// Implementación: registrarPartida()
// ============================================================================

void registrarPartida() {
    // Arreglo temporal para almacenar los movimientos antes de validar
    Asiento movimientos[MAX_MOVIMIENTOS];
    int cantidadMovimientos = 0;
    bool partidaCuadrada = false;

    // Bucle exterior: reintento hasta que la partida cuadre o el usuario cancele
    do {
        cantidadMovimientos = 0;
        double sumaDebe = 0.0;
        double sumaHaber = 0.0;

        cout << "\n";
        imprimirSeparador(70, '=');
        cout << "       MODULO 2 :: REGISTRO DE PARTIDA EN LIBRO DIARIO\n";
        imprimirSeparador(70, '=');

        // --- Datos de cabecera de la partida ---
        int numeroPartida = leerEntero(" Ingrese el numero de partida: ");

        cout << " Ingrese la fecha (DD/MM/AAAA): ";
        char fecha[15];
        memset(fecha, 0, sizeof(fecha));
        cin.getline(fecha, 15);

        if (strlen(fecha) == 0) {
            cout << "\n [!] ERROR: La fecha no puede estar vacia. Operacion cancelada.\n";
            pausarConsola();
            return;
        }

        // --- Bucle de ingreso de movimientos ---
        cout << "\n";
        imprimirSeparador(70, '-');
        cout << " Ingrese los movimientos de la partida (maximo "
             << MAX_MOVIMIENTOS << ").\n";
        cout << " Escriba '0' como codigo de cuenta para finalizar el ingreso.\n";
        imprimirSeparador(70, '-');

        bool seguirIngresando = true;
        while (seguirIngresando && cantidadMovimientos < MAX_MOVIMIENTOS) {
            cout << "\n --- Movimiento #" << (cantidadMovimientos + 1) << " ---\n";

            int codigoCuenta = leerEntero("   Codigo de cuenta (0 para terminar): ");
            if (codigoCuenta == 0) {
                seguirIngresando = false;
                break;
            }

            // Inicializar el registro del movimiento
            Asiento& mov = movimientos[cantidadMovimientos];
            memset(&mov, 0, sizeof(Asiento));

            // Copiar datos de cabecera
            strncpy(mov.fecha, fecha, 14);
            mov.fecha[14] = '\0';
            mov.codigo_cuenta = codigoCuenta;
            mov.numero_partida = numeroPartida;

            // Descripción del movimiento
            cout << "   Descripcion: ";
            cin.getline(mov.descripcion, 100);

            // Montos
            mov.debe = leerDouble("   Monto al DEBE ($): ");
            if (mov.debe < 0.0) {
                cout << "   [!] El monto al Debe no puede ser negativo. Se asignara $0.00.\n";
                mov.debe = 0.0;
            }

            mov.haber = leerDouble("   Monto al HABER ($): ");
            if (mov.haber < 0.0) {
                cout << "   [!] El monto al Haber no puede ser negativo. Se asignara $0.00.\n";
                mov.haber = 0.0;
            }

            sumaDebe += mov.debe;
            sumaHaber += mov.haber;
            cantidadMovimientos++;
        }

        // --- Verificar que se hayan ingresado movimientos ---
        if (cantidadMovimientos == 0) {
            cout << "\n [!] No se ingresaron movimientos. Operacion cancelada.\n";
            pausarConsola();
            return;
        }

        // --- Resumen y validación de partida doble ---
        cout << "\n";
        imprimirSeparador(70, '=');
        cout << " RESUMEN DE LA PARTIDA #" << movimientos[0].numero_partida << "\n";
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

        // --- REGLA DE NEGOCIO CRÍTICA: Validación de Partida Doble ---
        double diferencia = fabs(sumaDebe - sumaHaber);

        if (diferencia < 0.001) {
            // Partida cuadrada: proceder a guardar
            partidaCuadrada = true;

            ofstream archivo(ARCHIVO_DIARIO, ios::binary | ios::app);
            if (!archivo.is_open()) {
                cout << "\n [!] ERROR CRITICO: No se pudo abrir/crear el archivo \""
                     << ARCHIVO_DIARIO << "\".\n";
                pausarConsola();
                return;
            }

            for (int i = 0; i < cantidadMovimientos; ++i) {
                archivo.write(reinterpret_cast<const char*>(&movimientos[i]), sizeof(Asiento));
            }
            archivo.close();

            cout << "\n [OK] PARTIDA DOBLE VALIDADA CORRECTAMENTE.\n";
            cout << "      " << cantidadMovimientos
                 << " movimiento(s) guardado(s) exitosamente en \"" << ARCHIVO_DIARIO << "\".\n";
            imprimirSeparador(70, '-');

        } else {
            // Partida descuadrada: RECHAZAR el guardado
            cout << "\n";
            imprimirSeparador(70, '!');
            cout << " [RECHAZADO] LA PARTIDA NO CUADRA.\n";
            cout << "   Suma Debe:  $" << sumaDebe << "\n";
            cout << "   Suma Haber: $" << sumaHaber << "\n";
            cout << "   Diferencia: $" << diferencia << "\n";
            cout << "\n   La partida doble exige que Debe == Haber.\n";
            cout << "   Los datos NO fueron guardados en el archivo.\n";
            imprimirSeparador(70, '!');

            char respuesta;
            cout << "\n Desea reintentar el ingreso de la partida? (S/N): ";
            cin >> respuesta;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (respuesta != 'S' && respuesta != 's') {
                cout << "\n [i] Operacion cancelada por el usuario.\n";
                pausarConsola();
                return;
            }
            // Si responde S, el do-while vuelve a iterar
        }

    } while (!partidaCuadrada);

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
    cout << "                   MODULO 2 :: LIBRO DIARIO GENERAL\n";
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
        // Separador visual entre partidas distintas
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
        cout << "\n [i] El libro diario esta vacio. No hay asientos registrados.\n";
    } else {
        imprimirSeparador(90, '=');
        cout << left  << setw(64) << " TOTALES GENERALES DEL DIARIO:"
             << right << setw(13) << granTotalDebe
             << right << setw(13) << granTotalHaber
             << "\n";
        imprimirSeparador(90, '=');
        cout << " Total de movimientos registrados: " << totalAsientos << "\n";
    }
    imprimirSeparador(90, '-');

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
        cout << " 3. Regresar al Menu Principal\n";
        imprimirSeparador(65, '-');
        cout << " Seleccione una opcion [1-3]: ";

        opcion = leerEntero("");

        switch (opcion) {
            case 1:
                registrarPartida();
                break;
            case 2:
                listarAsientos();
                break;
            case 3:
                cout << "\n [i] Regresando al Menu Principal...\n";
                break;
            default:
                cout << "\n [!] Opcion invalida. Seleccione entre 1 y 3.\n";
                pausarConsola();
                break;
        }
    } while (opcion != 3);
}
