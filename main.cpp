/**
 * ============================================================================
 * Proyecto: FinCost C++ - Sistema de Contabilidad Financiera y Costos
 * Archivo:  main.cpp
 * Descripción: Menu principal, integracion de modulos y submenu de reportes.
 * Paradigma: Programacion Estructurada (Estricto sin clases / POO).
 * ============================================================================
 */

#include <iostream>
#include <limits>
#include "estructuras.h"
#include "reportes.h"
#include "catalogo.h"
#include "diario.h"
#include "costos.h"

using namespace std;

// ============================================================================
// Funciones de Lectura y Limpieza de Entrada
// ============================================================================

/**
 * Lee un entero desde el teclado con validacion contra entradas no numericas y EOF.
 */
static int leerOpcion() {
    int opcion = 0;
    if (cin.eof()) {
        return -1;
    }
    while (!(cin >> opcion)) {
        if (cin.eof()) {
            return -1; // Fin de entrada (EOF)
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << " [!] Entrada no valida. Ingrese un numero de opcion: ";
    }
    if (!cin.eof()) {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    return opcion;
}

// ============================================================================
// Submenú: Módulo 4 - Reportes Automatizados
// ============================================================================

void submenuReportes() {
    int opcionSubmenu = 0;

    do {
        cout << "\n";
        imprimirSeparador(65, '=');
        cout << "         FINCOST C++ :: MODULO 4 - REPORTES AUTOMATIZADOS\n";
        imprimirSeparador(65, '=');
        cout << " 1. Balance de Comprobacion (cuentas.dat)\n";
        cout << " 2. Estado de Resultados / Perdidas y Ganancias (cuentas.dat)\n";
        cout << " 3. Hoja de Costos por Ordenes de Produccion (costos.dat)\n";
        cout << " 4. Regresar al Menu Principal\n";
        imprimirSeparador(65, '-');
        cout << " Seleccione una opcion [1-4]: ";

        opcionSubmenu = leerOpcion();
        if (cin.eof() || opcionSubmenu == -1) {
            break;
        }

        switch (opcionSubmenu) {
            case 1:
                generarBalanceComprobacion();
                break;
            case 2:
                generarEstadoResultados();
                break;
            case 3:
                generarHojaCostos();
                break;
            case 4:
                cout << "\n [i] Regresando al Menu Principal...\n";
                break;
            default:
                cout << "\n [!] Opcion invalida. Por favor seleccione entre 1 y 4.\n";
                pausarConsola();
                break;
        }
    } while (opcionSubmenu != 4 && !cin.eof());
}

// ============================================================================
// Función Principal
// ============================================================================

int main() {
    int opcionPrincipal = 0;

    do {
        cout << "\n";
        imprimirSeparador(65, '=');
        cout << "            SISTEMA FINANCIERO Y DE COSTOS (FINCOST C++)\n";
        cout << "                       MENU PRINCIPAL\n";
        imprimirSeparador(65, '=');
        cout << " 1. Modulo 1: Catalogo de Cuentas Contables\n";
        cout << " 2. Modulo 2: Libro Diario y Registro de Asientos\n";
        cout << " 3. Modulo 3: Contabilidad de Costos (Ordenes de Produccion)\n";
        cout << " 4. Modulo 4: Reportes Financieros y de Costos Automatizados\n";
        cout << " 5. Salir del Sistema\n";
        imprimirSeparador(65, '-');
        cout << " Seleccione una opcion [1-5]: ";

        opcionPrincipal = leerOpcion();
        if (cin.eof() || opcionPrincipal == -1) {
            opcionPrincipal = 5;
        }

        switch (opcionPrincipal) {
            case 1:
                submenuCatalogo();
                break;

            case 2:
                submenuDiario();
                break;

            case 3:
                submenuCostos();
                break;

            case 4:
                submenuReportes();
                break;

            case 5:
                cout << "\n";
                imprimirSeparador(65, '=');
                cout << "   Gracias por utilizar FinCost C++. Cerrando sesion segura...\n";
                imprimirSeparador(65, '=');
                cout << "\n";
                break;

            default:
                cout << "\n [!] Opcion invalida. Por favor seleccione una opcion entre 1 y 5.\n";
                pausarConsola();
                break;
        }

    } while (opcionPrincipal != 5 && !cin.eof());

    return 0;
}
