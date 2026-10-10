/**
 * ============================================================================
 * Proyecto: FinCost C++ - Sistema de Contabilidad Financiera y Costos
 * Archivo:  costos.cpp
 * Descripción: Implementación del Módulo 3 - Contabilidad de Costos Industriales.
 *              Incluye CRUD completo para órdenes de producción, recálculo
 *              automático del costo unitario, bajas lógicas y validación EOF.
 * Paradigma: Programación Estructurada (Estricto sin clases / POO).
 * ============================================================================
 */

#include "costos.h"
#include "reportes.h"  // Para imprimirSeparador() y pausarConsola()
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>
#include <cstdlib>
#include <limits>

using namespace std;

// ============================================================================
// Funciones Auxiliares Locales
// ============================================================================

/**
 * Lee un entero de forma segura con proteccion ante EOF.
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
 * Lee un double de forma segura con proteccion ante EOF.
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
 * Verifica si un numero de orden ya existe fisicamente en costos.dat (activo o no).
 * Regla de negocio: Los numeros de orden dados de baja no pueden reutilizarse.
 */
static bool existeNumeroOrdenFisico(int numOrden) {
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
    memset(&nueva, 0, sizeof(OrdenCosto));
    nueva.activo = 1;

    // --- Numero de orden ---
    nueva.numero_orden = leerEntero(" Ingrese el numero de orden: ");
    if (cin.eof() || nueva.numero_orden <= 0) {
        cout << "\n [!] Numero de orden invalido. Operacion cancelada.\n";
        pausarConsola();
        return;
    }

    // Validacion de unicidad contra todo el archivo (activos o inactivos)
    if (existeNumeroOrdenFisico(nueva.numero_orden)) {
        cout << "\n [!] ERROR: La orden #" << nueva.numero_orden
             << " ya existe en el sistema (activa o historica).\n";
        cout << "     No se permite reutilizar numeros de orden. Operacion cancelada.\n";
        pausarConsola();
        return;
    }

    // --- Materiales Directos (MD) ---
    nueva.materiales_directos = leerDouble(" Materiales Directos (MD) ($): ");
    if (cin.eof()) return;
    if (nueva.materiales_directos < 0.0) {
        cout << "\n [!] ERROR: Los materiales directos no pueden ser negativos.\n";
        pausarConsola();
        return;
    }

    // --- Mano de Obra Directa (MOD) ---
    nueva.mano_obra_directa = leerDouble(" Mano de Obra Directa (MOD) ($): ");
    if (cin.eof()) return;
    if (nueva.mano_obra_directa < 0.0) {
        cout << "\n [!] ERROR: La mano de obra directa no puede ser negativa.\n";
        pausarConsola();
        return;
    }

    // --- Costos Indirectos de Fabricacion (CIF) ---
    nueva.cif = leerDouble(" Costos Indirectos de Fabricacion (CIF) ($): ");
    if (cin.eof()) return;
    if (nueva.cif < 0.0) {
        cout << "\n [!] ERROR: Los CIF no pueden ser negativos.\n";
        pausarConsola();
        return;
    }

    // --- Unidades Producidas ---
    nueva.unidades_producidas = leerEntero(" Unidades Producidas: ");
    if (cin.eof()) return;
    if (nueva.unidades_producidas <= 0) {
        cout << "\n [!] ERROR: Las unidades producidas deben ser mayores a cero.\n";
        pausarConsola();
        return;
    }

    // --- Calculo del Costo Unitario ---
    double costoTotal = nueva.materiales_directos + nueva.mano_obra_directa + nueva.cif;
    nueva.costo_unitario = costoTotal / nueva.unidades_producidas;

    // Resumen visual
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
    imprimirSeparador(70, '=');

    ofstream archivo(ARCHIVO_COSTOS, ios::binary | ios::app);
    if (!archivo.is_open()) {
        cout << "\n [!] ERROR CRITICO: No se pudo abrir \"" << ARCHIVO_COSTOS << "\".\n";
        pausarConsola();
        return;
    }

    archivo.write(reinterpret_cast<const char*>(&nueva), sizeof(OrdenCosto));
    archivo.close();

    cout << "\n [OK] Orden #" << nueva.numero_orden << " guardada exitosamente en \""
         << ARCHIVO_COSTOS << "\".\n";
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
    cout << "               MODULO 3 :: ORDENES DE COSTO DE PRODUCCION (ACTIVAS)\n";
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
        if (orden.activo == 0) {
            continue; // Ignorar ordenes dadas de baja
        }

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
        cout << "\n [i] No hay ordenes de costos activas registradas.\n";
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
        cout << " Total de ordenes activas: " << totalOrdenes << "\n";
    }
    imprimirSeparador(104, '-');

    pausarConsola();
}

// ============================================================================
// Implementación: consultarOrden()
// ============================================================================

void consultarOrden() {
    cout << "\n";
    imprimirSeparador(70, '=');
    cout << "       MODULO 3 :: CONSULTAR ORDEN DE PRODUCCION POR NUMERO\n";
    imprimirSeparador(70, '=');

    int numOrden = leerEntero(" Ingrese el numero de orden a consultar: ");
    if (cin.eof() || numOrden <= 0) {
        return;
    }

    ifstream archivo(ARCHIVO_COSTOS, ios::binary | ios::in);
    if (!archivo.is_open()) {
        cout << "\n [!] No se pudo abrir \"" << ARCHIVO_COSTOS << "\".\n";
        pausarConsola();
        return;
    }

    OrdenCosto orden;
    bool encontrada = false;

    while (archivo.read(reinterpret_cast<char*>(&orden), sizeof(OrdenCosto))) {
        if (orden.numero_orden == numOrden && orden.activo == 1) {
            encontrada = true;
            break;
        }
    }
    archivo.close();

    if (!encontrada) {
        cout << "\n [!] No se encontro ninguna orden activa con el numero #" << numOrden << ".\n";
    } else {
        double costoTotal = orden.materiales_directos + orden.mano_obra_directa + orden.cif;
        cout << "\n";
        imprimirSeparador(70, '-');
        cout << " [i] DETALLE DE LA ORDEN DE PRODUCCION #" << orden.numero_orden << "\n";
        imprimirSeparador(70, '-');
        cout << fixed << setprecision(2);
        cout << " Numero de Orden:                 " << orden.numero_orden << "\n";
        cout << " Materiales Directos (MD):        $" << setw(12) << orden.materiales_directos << "\n";
        cout << " Mano de Obra Directa (MOD):      $" << setw(12) << orden.mano_obra_directa << "\n";
        cout << " Costos Ind. Fabricacion (CIF):   $" << setw(12) << orden.cif << "\n";
        imprimirSeparador(70, '-');
        cout << " COSTO TOTAL DE PRODUCCION:       $" << setw(12) << costoTotal << "\n";
        cout << " Unidades Fabricadas:               " << setw(12) << orden.unidades_producidas << "\n";
        cout << " COSTO UNITARIO:                  $" << setw(12) << orden.costo_unitario << " c/u\n";
        cout << " Estado:                          Vigente (Activa)\n";
        imprimirSeparador(70, '-');
    }

    pausarConsola();
}

// ============================================================================
// Implementación: modificarOrden()
// ============================================================================

void modificarOrden() {
    cout << "\n";
    imprimirSeparador(70, '=');
    cout << "             MODULO 3 :: MODIFICAR ORDEN DE PRODUCCION\n";
    imprimirSeparador(70, '=');

    int numOrden = leerEntero(" Ingrese el numero de orden a modificar: ");
    if (cin.eof() || numOrden <= 0) {
        return;
    }

    fstream archivo(ARCHIVO_COSTOS, ios::binary | ios::in | ios::out);
    if (!archivo.is_open()) {
        cout << "\n [!] No se pudo abrir \"" << ARCHIVO_COSTOS << "\".\n";
        pausarConsola();
        return;
    }

    OrdenCosto orden;
    bool encontrada = false;
    streampos pos = 0;

    while (true) {
        pos = archivo.tellg();
        if (!archivo.read(reinterpret_cast<char*>(&orden), sizeof(OrdenCosto))) {
            break;
        }
        if (orden.numero_orden == numOrden && orden.activo == 1) {
            encontrada = true;
            break;
        }
    }

    if (!encontrada) {
        archivo.close();
        cout << "\n [!] No se encontro ninguna orden activa con el numero #" << numOrden << ".\n";
        pausarConsola();
        return;
    }

    double costoTotalActual = orden.materiales_directos + orden.mano_obra_directa + orden.cif;
    cout << "\n";
    imprimirSeparador(70, '-');
    cout << " DATOS ACTUALES (Orden #" << orden.numero_orden << " [Numero no modificable]):\n";
    cout << fixed << setprecision(2);
    cout << "  1. Materiales Directos actual:   $" << orden.materiales_directos << "\n";
    cout << "  2. Mano de Obra Directa actual:  $" << orden.mano_obra_directa << "\n";
    cout << "  3. CIF actual:                   $" << orden.cif << "\n";
    cout << "  4. Costo Total actual:           $" << costoTotalActual << "\n";
    cout << "  5. Unidades producidas actual:    " << orden.unidades_producidas << "\n";
    cout << "  6. Costo unitario actual:        $" << orden.costo_unitario << "\n";
    imprimirSeparador(70, '-');
    cout << " (Presione ENTER vacio en cualquier campo para conservar el valor actual)\n\n";

    OrdenCosto modificada = orden;

    // Modificar MD
    char bufferMD[50];
    memset(bufferMD, 0, sizeof(bufferMD));
    cout << " Nuevos Materiales Directos ($): ";
    cin.getline(bufferMD, 50);
    if (cin.eof()) {
        archivo.close();
        return;
    }
    if (strlen(bufferMD) > 0) {
        char* finPtr = 0;
        double val = strtod(bufferMD, &finPtr);
        if (finPtr == bufferMD || val < 0.0) {
            archivo.close();
            cout << "\n [!] Valor de MD invalido o negativo. Modificacion cancelada.\n";
            pausarConsola();
            return;
        }
        modificada.materiales_directos = val;
    }

    // Modificar MOD
    char bufferMOD[50];
    memset(bufferMOD, 0, sizeof(bufferMOD));
    cout << " Nueva Mano de Obra Directa ($): ";
    cin.getline(bufferMOD, 50);
    if (cin.eof()) {
        archivo.close();
        return;
    }
    if (strlen(bufferMOD) > 0) {
        char* finPtr = 0;
        double val = strtod(bufferMOD, &finPtr);
        if (finPtr == bufferMOD || val < 0.0) {
            archivo.close();
            cout << "\n [!] Valor de MOD invalido o negativo. Modificacion cancelada.\n";
            pausarConsola();
            return;
        }
        modificada.mano_obra_directa = val;
    }

    // Modificar CIF
    char bufferCIF[50];
    memset(bufferCIF, 0, sizeof(bufferCIF));
    cout << " Nuevos Costos Ind. Fabricacion ($): ";
    cin.getline(bufferCIF, 50);
    if (cin.eof()) {
        archivo.close();
        return;
    }
    if (strlen(bufferCIF) > 0) {
        char* finPtr = 0;
        double val = strtod(bufferCIF, &finPtr);
        if (finPtr == bufferCIF || val < 0.0) {
            archivo.close();
            cout << "\n [!] Valor de CIF invalido o negativo. Modificacion cancelada.\n";
            pausarConsola();
            return;
        }
        modificada.cif = val;
    }

    // Modificar Unidades
    char bufferUds[50];
    memset(bufferUds, 0, sizeof(bufferUds));
    cout << " Nuevas Unidades Producidas: ";
    cin.getline(bufferUds, 50);
    if (cin.eof()) {
        archivo.close();
        return;
    }
    if (strlen(bufferUds) > 0) {
        char* finPtr = 0;
        long val = strtol(bufferUds, &finPtr, 10);
        if (finPtr == bufferUds || val <= 0) {
            archivo.close();
            cout << "\n [!] Las unidades producidas deben ser un entero positivo (> 0).\n";
            pausarConsola();
            return;
        }
        modificada.unidades_producidas = static_cast<int>(val);
    }

    // RECALCULO AUTOMATICO DEL COSTO UNITARIO
    double nuevoTotal = modificada.materiales_directos + modificada.mano_obra_directa + modificada.cif;
    modificada.costo_unitario = nuevoTotal / modificada.unidades_producidas;

    cout << "\n";
    imprimirSeparador(70, '-');
    cout << " RESUMEN DE CAMBIOS PARA ORDEN #" << modificada.numero_orden << ":\n";
    cout << "  MD:             $" << modificada.materiales_directos << "\n";
    cout << "  MOD:            $" << modificada.mano_obra_directa << "\n";
    cout << "  CIF:            $" << modificada.cif << "\n";
    cout << "  COSTO TOTAL:    $" << nuevoTotal << "\n";
    cout << "  UNIDADES:        " << modificada.unidades_producidas << "\n";
    cout << "  COSTO UNITARIO: $" << modificada.costo_unitario << " c/u\n";
    imprimirSeparador(70, '-');

    if (!confirmarOperacion(" Desea confirmar y guardar los cambios? (S/N): ")) {
        archivo.close();
        cout << "\n [i] Modificacion cancelada por el usuario.\n";
        pausarConsola();
        return;
    }

    archivo.seekp(pos);
    archivo.write(reinterpret_cast<const char*>(&modificada), sizeof(OrdenCosto));
    archivo.close();

    cout << "\n [OK] Orden #" << numOrden << " modificada exitosamente.\n";
    pausarConsola();
}

// ============================================================================
// Implementación: eliminarOrden()
// ============================================================================

void eliminarOrden() {
    cout << "\n";
    imprimirSeparador(70, '=');
    cout << "       MODULO 3 :: ELIMINAR ORDEN DE PRODUCCION (BAJA LOGICA)\n";
    imprimirSeparador(70, '=');

    int numOrden = leerEntero(" Ingrese el numero de orden a dar de baja: ");
    if (cin.eof() || numOrden <= 0) {
        return;
    }

    fstream archivo(ARCHIVO_COSTOS, ios::binary | ios::in | ios::out);
    if (!archivo.is_open()) {
        cout << "\n [!] No se pudo abrir \"" << ARCHIVO_COSTOS << "\".\n";
        pausarConsola();
        return;
    }

    OrdenCosto orden;
    bool encontrada = false;
    streampos pos = 0;

    while (true) {
        pos = archivo.tellg();
        if (!archivo.read(reinterpret_cast<char*>(&orden), sizeof(OrdenCosto))) {
            break;
        }
        if (orden.numero_orden == numOrden && orden.activo == 1) {
            encontrada = true;
            break;
        }
    }

    if (!encontrada) {
        archivo.close();
        cout << "\n [!] No se encontro ninguna orden activa con el numero #" << numOrden << ".\n";
        pausarConsola();
        return;
    }

    double total = orden.materiales_directos + orden.mano_obra_directa + orden.cif;
    cout << "\n";
    imprimirSeparador(65, '-');
    cout << " ORDEN A DAR DE BAJA:\n";
    cout << "  Numero:          " << orden.numero_orden << "\n";
    cout << fixed << setprecision(2);
    cout << "  Costo Total:    $" << total << "\n";
    cout << "  Unidades:        " << orden.unidades_producidas << "\n";
    cout << "  Costo Unitario: $" << orden.costo_unitario << "\n";
    imprimirSeparador(65, '-');

    if (!confirmarOperacion(" Esta seguro de dar de baja logica esta orden? (S/N): ")) {
        archivo.close();
        cout << "\n [i] Eliminacion cancelada por el usuario.\n";
        pausarConsola();
        return;
    }

    orden.activo = 0; // Baja logica
    archivo.seekp(pos);
    archivo.write(reinterpret_cast<const char*>(&orden), sizeof(OrdenCosto));
    archivo.close();

    cout << "\n [OK] Orden #" << numOrden << " dada de baja exitosamente.\n";
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
        cout << " 3. Consultar orden por numero\n";
        cout << " 4. Modificar orden de produccion\n";
        cout << " 5. Eliminar orden de produccion (baja logica)\n";
        cout << " 6. Regresar al Menu Principal\n";
        imprimirSeparador(65, '-');
        cout << " Seleccione una opcion [1-6]: ";

        opcion = leerEntero("");
        if (cin.eof() || opcion == -1) {
            break;
        }

        switch (opcion) {
            case 1:
                crearOrden();
                break;
            case 2:
                listarOrdenes();
                break;
            case 3:
                consultarOrden();
                break;
            case 4:
                modificarOrden();
                break;
            case 5:
                eliminarOrden();
                break;
            case 6:
                cout << "\n [i] Regresando al Menu Principal...\n";
                break;
            default:
                cout << "\n [!] Opcion invalida. Seleccione entre 1 y 6.\n";
                pausarConsola();
                break;
        }
    } while (opcion != 6 && !cin.eof());
}
