/**
 * ============================================================================
 * Proyecto: FinCost C++ - Sistema de Contabilidad Financiera y Costos
 * Archivo:  costos.cpp
 * Descripción: Implementación del Módulo 3 - Contabilidad de Costos Industriales.
 *              Permite crear órdenes de producción con cálculo automático del
 *              costo unitario y listar todas las órdenes de costos.dat.
 * Paradigma: Programación Estructurada (Estricto sin clases / POO).
 * ============================================================================
 */

#include "costos.h"
#include "reportes.h"  // Para imprimirSeparador() y pausarConsola()
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>
#include <limits>

using namespace std;

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

/**
 * Verifica si un número de orden ya existe en costos.dat.
 * @param numOrden Número de orden a buscar.
 * @return true si ya existe, false en caso contrario.
 */
static bool existeNumeroOrden(int numOrden) {
    ifstream archivo(ARCHIVO_COSTOS, ios::binary | ios::in);
    if (!archivo.is_open()) {
        return false;
    }

    OrdenCosto temp;
    while (archivo.read(reinterpret_cast<char*>(&temp), sizeof(OrdenCosto))) {
        if (temp.numero_orden == numOrden) {
            archivo.close();
            return true;
        }
    }
    archivo.close();
    return false;
}

// ============================================================================
// Implementación: crearOrden()
// ============================================================================

void crearOrden() {
    cout << "\n";
    imprimirSeparador(70, '=');
    cout << "       MODULO 3 :: CREAR NUEVA ORDEN DE COSTO DE PRODUCCION\n";
    imprimirSeparador(70, '=');

    OrdenCosto nueva;
    // Inicializar a ceros para evitar basura en el archivo binario
    memset(&nueva, 0, sizeof(OrdenCosto));

    // --- Número de orden ---
    nueva.numero_orden = leerEntero(" Ingrese el numero de orden: ");

    if (nueva.numero_orden <= 0) {
        cout << "\n [!] ERROR: El numero de orden debe ser positivo. Operacion cancelada.\n";
        pausarConsola();
        return;
    }

    // Validar que no exista previamente
    if (existeNumeroOrden(nueva.numero_orden)) {
        cout << "\n [!] ERROR: La orden #" << nueva.numero_orden
             << " ya existe en el sistema de costos.\n";
        cout << "     No se permiten ordenes duplicadas. Operacion cancelada.\n";
        pausarConsola();
        return;
    }

    // --- Materiales Directos (MD) ---
    nueva.materiales_directos = leerDouble(" Materiales Directos (MD) ($): ");
    if (nueva.materiales_directos < 0.0) {
        cout << "\n [!] ERROR: Los materiales directos no pueden ser negativos.\n";
        pausarConsola();
        return;
    }

    // --- Mano de Obra Directa (MOD) ---
    nueva.mano_obra_directa = leerDouble(" Mano de Obra Directa (MOD) ($): ");
    if (nueva.mano_obra_directa < 0.0) {
        cout << "\n [!] ERROR: La mano de obra directa no puede ser negativa.\n";
        pausarConsola();
        return;
    }

    // --- Costos Indirectos de Fabricación (CIF) ---
    nueva.cif = leerDouble(" Costos Indirectos de Fabricacion (CIF) ($): ");
    if (nueva.cif < 0.0) {
        cout << "\n [!] ERROR: Los CIF no pueden ser negativos.\n";
        pausarConsola();
        return;
    }

    // --- Unidades Producidas ---
    nueva.unidades_producidas = leerEntero(" Unidades Producidas: ");
    if (nueva.unidades_producidas <= 0) {
        cout << "\n [!] ERROR: Las unidades producidas deben ser mayores a cero.\n";
        cout << "     No se puede calcular el costo unitario con cero unidades.\n";
        pausarConsola();
        return;
    }

    // --- REGLA DE NEGOCIO CRÍTICA: Cálculo Automático del Costo Unitario ---
    double costoTotal = nueva.materiales_directos + nueva.mano_obra_directa + nueva.cif;
    nueva.costo_unitario = costoTotal / nueva.unidades_producidas;

    // --- Confirmación visual antes de guardar ---
    cout << "\n";
    imprimirSeparador(70, '-');
    cout << " RESUMEN DE LA ORDEN DE PRODUCCION #" << nueva.numero_orden << "\n";
    imprimirSeparador(70, '-');
    cout << fixed << setprecision(2);
    cout << "   Materiales Directos (MD):       $" << setw(14) << nueva.materiales_directos << "\n";
    cout << "   Mano de Obra Directa (MOD):     $" << setw(14) << nueva.mano_obra_directa << "\n";
    cout << "   Costos Ind. de Fabricacion (CIF):$" << setw(13) << nueva.cif << "\n";
    imprimirSeparador(70, '-');
    cout << "   COSTO TOTAL DE PRODUCCION:      $" << setw(14) << costoTotal << "\n";
    cout << "   Unidades Producidas:              " << setw(14) << nueva.unidades_producidas << "\n";
    imprimirSeparador(70, '=');
    cout << "   COSTO UNITARIO CALCULADO:       $" << setw(14) << nueva.costo_unitario << "\n";
    cout << "   Formula: ($" << costoTotal << " / " << nueva.unidades_producidas
         << " uds) = $" << nueva.costo_unitario << " c/u\n";
    imprimirSeparador(70, '=');

    // --- Guardar en archivo binario ---
    ofstream archivo(ARCHIVO_COSTOS, ios::binary | ios::app);
    if (!archivo.is_open()) {
        cout << "\n [!] ERROR CRITICO: No se pudo abrir/crear el archivo \""
             << ARCHIVO_COSTOS << "\".\n";
        pausarConsola();
        return;
    }

    archivo.write(reinterpret_cast<const char*>(&nueva), sizeof(OrdenCosto));
    archivo.close();

    cout << "\n [OK] Orden de costo #" << nueva.numero_orden
         << " guardada exitosamente en \"" << ARCHIVO_COSTOS << "\".\n";
    imprimirSeparador(70, '-');

    pausarConsola();
}

// ============================================================================
// Implementación: listarOrdenes()
// ============================================================================

void listarOrdenes() {
    ifstream archivo(ARCHIVO_COSTOS, ios::binary | ios::in);

    if (!archivo.is_open()) {
        cout << "\n";
        imprimirSeparador(65, '!');
        cout << " [!] El archivo \"" << ARCHIVO_COSTOS
             << "\" no existe o esta vacio.\n";
        cout << "     Cree ordenes desde la opcion 1 para generar el archivo.\n";
        imprimirSeparador(65, '!');
        pausarConsola();
        return;
    }

    cout << "\n";
    imprimirSeparador(104, '=');
    cout << "                      MODULO 3 :: ORDENES DE COSTO DE PRODUCCION\n";
    imprimirSeparador(104, '=');

    cout << fixed << setprecision(2);
    cout << left  << setw(10) << "N. ORDEN"
         << right << setw(16) << "MAT. DIR. ($)"
         << right << setw(16) << "M.O. DIR. ($)"
         << right << setw(15) << "C.I.F. ($)"
         << right << setw(16) << "COSTO TOT. ($)"
         << right << setw(13) << "UNIDADES"
         << right << setw(18) << "COSTO UNIT. ($)"
         << "\n";
    imprimirSeparador(104, '-');

    OrdenCosto orden;
    int totalOrdenes = 0;
    double acumMD = 0.0, acumMOD = 0.0, acumCIF = 0.0, acumTotal = 0.0;
    int acumUnidades = 0;

    while (archivo.read(reinterpret_cast<char*>(&orden), sizeof(OrdenCosto))) {
        totalOrdenes++;
        double costoTotal = orden.materiales_directos + orden.mano_obra_directa + orden.cif;

        acumMD += orden.materiales_directos;
        acumMOD += orden.mano_obra_directa;
        acumCIF += orden.cif;
        acumTotal += costoTotal;
        acumUnidades += orden.unidades_producidas;

        cout << left  << setw(10) << orden.numero_orden
             << right << setw(16) << orden.materiales_directos
             << right << setw(16) << orden.mano_obra_directa
             << right << setw(15) << orden.cif
             << right << setw(16) << costoTotal
             << right << setw(13) << orden.unidades_producidas
             << right << setw(18) << orden.costo_unitario
             << "\n";
    }

    archivo.close();

    if (totalOrdenes == 0) {
        cout << "\n [i] El archivo de costos esta vacio. No hay ordenes registradas.\n";
    } else {
        imprimirSeparador(104, '=');
        double promedioGlobal = (acumUnidades > 0) ? (acumTotal / acumUnidades) : 0.0;

        cout << left  << setw(10) << "TOTALES:"
             << right << setw(16) << acumMD
             << right << setw(16) << acumMOD
             << right << setw(15) << acumCIF
             << right << setw(16) << acumTotal
             << right << setw(13) << acumUnidades
             << right << setw(18) << promedioGlobal
             << "\n";
        imprimirSeparador(104, '=');
        cout << " Total de ordenes registradas: " << totalOrdenes << "\n";
    }
    imprimirSeparador(104, '-');

    pausarConsola();
}

// ============================================================================
// Submenú del Módulo 3
// ============================================================================

void submenuCostos() {
    int opcion = 0;

    do {
        cout << "\n";
        imprimirSeparador(65, '=');
        cout << "   FINCOST C++ :: MODULO 3 - COSTOS INDUSTRIALES (POR ORDENES)\n";
        imprimirSeparador(65, '=');
        cout << " 1. Crear nueva orden de produccion\n";
        cout << " 2. Listar todas las ordenes de costo\n";
        cout << " 3. Regresar al Menu Principal\n";
        imprimirSeparador(65, '-');
        cout << " Seleccione una opcion [1-3]: ";

        opcion = leerEntero("");

        switch (opcion) {
            case 1:
                crearOrden();
                break;
            case 2:
                listarOrdenes();
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
