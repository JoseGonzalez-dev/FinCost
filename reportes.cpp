/**
 * ============================================================================
 * Proyecto: FinCost C++ - Sistema de Contabilidad Financiera y Costos
 * Archivo:  reportes.cpp
 * Descripción: Implementación del Módulo de Reportes Automatizados con lectura
 *              de archivos binarios (.dat), filtrado de bajas lógicas y
 *              clasificación contable estandarizada.
 * Paradigma: Programación Estructurada (Estricto sin clases / POO).
 * ============================================================================
 */

#include "reportes.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>
#include <cmath>
#include <cctype>

using namespace std;

// ============================================================================
// Funciones Auxiliares Locales (Ámbito de Traducción)
// ============================================================================

/**
 * Compara dos cadenas de texto de forma insensible a mayusculas/minusculas.
 */
static bool contieneSubcadenaInsensible(const char* fuente, const char* busqueda) {
    if (!fuente || !busqueda) return false;
    
    int lenFuente = strlen(fuente);
    int lenBusqueda = strlen(busqueda);
    if (lenBusqueda == 0) return true;
    if (lenFuente < lenBusqueda) return false;

    for (int i = 0; i <= lenFuente - lenBusqueda; ++i) {
        bool coincidencia = true;
        for (int j = 0; j < lenBusqueda; ++j) {
            if (tolower(static_cast<unsigned char>(fuente[i + j])) != 
                tolower(static_cast<unsigned char>(busqueda[j]))) {
                coincidencia = false;
                break;
            }
        }
        if (coincidencia) return true;
    }
    return false;
}

void imprimirSeparador(int longitud, char caracter) {
    for (int i = 0; i < longitud; ++i) {
        cout << caracter;
    }
    cout << "\n";
}

void pausarConsola() {
    if (cin.eof()) {
        return;
    }
    cout << "\nPresione [ENTER] para continuar...";
    string linea;
    getline(cin, linea);
}

/**
 * Muestra un mensaje amigable cuando un archivo binario no existe o no se puede abrir.
 */
static void mostrarErrorArchivo(const char* nombreArchivo, const char* moduloOrigen) {
    cout << "\n";
    imprimirSeparador(78, '!');
    cout << " [!] AVISO DEL SISTEMA: Archivo no disponible\n";
    imprimirSeparador(78, '-');
    cout << " No se pudo abrir el archivo binario: \"" << nombreArchivo << "\"\n";
    cout << " Causa probable: El archivo aun no ha sido generado o no existe en disco.\n";
    cout << " Solucion: Ingrese datos primero a traves del " << moduloOrigen << ".\n";
    imprimirSeparador(78, '!');
    cout << "\n";
}

// ============================================================================
// 1. Balance de Comprobación
// ============================================================================

void generarBalanceComprobacion() {
    ifstream archivo(ARCHIVO_CUENTAS, ios::binary | ios::in);

    if (!archivo.is_open()) {
        mostrarErrorArchivo(ARCHIVO_CUENTAS, "Modulo 1 (Catalogo de Cuentas)");
        pausarConsola();
        return;
    }

    // Cabecera del Reporte
    cout << "\n";
    imprimirSeparador(88, '=');
    cout << "                         FINCOST C++ - SISTEMA FINANCIERO\n";
    cout << "                             BALANCE DE COMPROBACION\n";
    imprimirSeparador(88, '=');

    cout << left  << setw(8)  << "CODIGO"
         << left  << setw(36) << "NOMBRE DE LA CUENTA"
         << left  << setw(16) << "TIPO"
         << right << setw(14) << "DEBE ($)"
         << right << setw(14) << "HABER ($)"
         << "\n";
    imprimirSeparador(88, '-');

    Cuenta cuenta;
    int totalRegistros = 0;
    double sumaTotalDebe = 0.0;
    double sumaTotalHaber = 0.0;

    cout << fixed << setprecision(2);

    while (archivo.read(reinterpret_cast<char*>(&cuenta), sizeof(Cuenta))) {
        // Ignorar registros eliminados (baja logica)
        if (cuenta.activo == 0) {
            continue;
        }

        totalRegistros++;
        double debe = 0.0;
        double haber = 0.0;

        // Criterio de clasificacion contable estandarizado:
        // Activo, Gasto y Costo -> Debe (Naturaleza Deudora)
        // Pasivo, Capital e Ingreso -> Haber (Naturaleza Acreedora)
        if (contieneSubcadenaInsensible(cuenta.tipo, "Activo") ||
            contieneSubcadenaInsensible(cuenta.tipo, "Gasto")  ||
            contieneSubcadenaInsensible(cuenta.tipo, "Costo")) {
            debe = cuenta.saldo;
        } else {
            haber = cuenta.saldo;
        }

        sumaTotalDebe += debe;
        sumaTotalHaber += haber;

        cout << left  << setw(8)  << cuenta.codigo
             << left  << setw(36) << cuenta.nombre
             << left  << setw(16) << cuenta.tipo
             << right << setw(14) << debe
             << right << setw(14) << haber
             << "\n";
    }

    archivo.close();

    if (totalRegistros == 0) {
        cout << "\n [i] El archivo \"" << ARCHIVO_CUENTAS << "\" no contiene cuentas activas.\n";
        imprimirSeparador(88, '-');
        pausarConsola();
        return;
    }

    imprimirSeparador(88, '=');
    cout << left  << setw(60) << "TOTALES GENERALES:"
         << right << setw(14) << sumaTotalDebe
         << right << setw(14) << sumaTotalHaber
         << "\n";
    imprimirSeparador(88, '=');

    // Validacion de la Partida Doble
    double diferencia = fabs(sumaTotalDebe - sumaTotalHaber);
    if (diferencia < 0.001) {
        cout << " [OK] ESTADO: BALANCE DE COMPROBACION CUADRADO (Diferencia: $0.00)\n";
    } else {
        cout << " [ALERTA] ESTADO: BALANCE DESCUADRADO (Diferencia: $" << diferencia << ")\n";
        cout << " Revise los asientos o ajustes cargados al sistema.\n";
    }
    imprimirSeparador(88, '-');

    pausarConsola();
}

// ============================================================================
// 2. Estado de Resultados
// ============================================================================

void generarEstadoResultados() {
    ifstream archivo(ARCHIVO_CUENTAS, ios::binary | ios::in);

    if (!archivo.is_open()) {
        mostrarErrorArchivo(ARCHIVO_CUENTAS, "Modulo 1 (Catalogo de Cuentas)");
        pausarConsola();
        return;
    }

    cout << "\n";
    imprimirSeparador(75, '=');
    cout << "                     FINCOST C++ - SISTEMA FINANCIERO\n";
    cout << "                    ESTADO DE RESULTADOS (PERDIDAS Y GANANCIAS)\n";
    imprimirSeparador(75, '=');

    cout << fixed << setprecision(2);

    Cuenta cuenta;
    double totalIngresos = 0.0;
    double totalGastos = 0.0;
    int cantIngresos = 0;
    int cantGastos = 0;

    // --- SECCION 1: INGRESOS OPERACIONALES ---
    cout << "\n [1] INGRESOS OPERACIONALES\n";
    imprimirSeparador(75, '-');
    cout << left  << setw(10) << "CODIGO"
         << left  << setw(45) << "CUENTA DE INGRESO"
         << right << setw(20) << "MONTO ($)"
         << "\n";
    imprimirSeparador(75, '-');

    while (archivo.read(reinterpret_cast<char*>(&cuenta), sizeof(Cuenta))) {
        if (cuenta.activo == 0) {
            continue; // Ignorar cuentas inactivas
        }
        if (contieneSubcadenaInsensible(cuenta.tipo, "Ingreso") ||
            contieneSubcadenaInsensible(cuenta.tipo, "Venta")) {
            cantIngresos++;
            totalIngresos += cuenta.saldo;
            cout << left  << setw(10) << cuenta.codigo
                 << left  << setw(45) << cuenta.nombre
                 << right << setw(20) << cuenta.saldo
                 << "\n";
        }
    }

    if (cantIngresos == 0) {
        cout << "    (No se encontraron cuentas activas de tipo Ingreso)\n";
    }
    imprimirSeparador(75, '-');
    cout << left  << setw(55) << "TOTAL INGRESOS:"
         << right << setw(20) << totalIngresos
         << "\n";
    imprimirSeparador(75, '=');

    // --- SECCION 2: COSTOS Y GASTOS OPERACIONALES ---
    archivo.clear();
    archivo.seekg(0, ios::beg);

    cout << "\n [2] COSTOS Y GASTOS OPERACIONALES\n";
    imprimirSeparador(75, '-');
    cout << left  << setw(10) << "CODIGO"
         << left  << setw(45) << "CUENTA DE GASTO / COSTO"
         << right << setw(20) << "MONTO ($)"
         << "\n";
    imprimirSeparador(75, '-');

    while (archivo.read(reinterpret_cast<char*>(&cuenta), sizeof(Cuenta))) {
        if (cuenta.activo == 0) {
            continue; // Ignorar cuentas inactivas
        }
        if (contieneSubcadenaInsensible(cuenta.tipo, "Gasto") ||
            contieneSubcadenaInsensible(cuenta.tipo, "Costo")) {
            cantGastos++;
            totalGastos += cuenta.saldo;
            cout << left  << setw(10) << cuenta.codigo
                 << left  << setw(45) << cuenta.nombre
                 << right << setw(20) << cuenta.saldo
                 << "\n";
        }
    }

    if (cantGastos == 0) {
        cout << "    (No se encontraron cuentas activas de tipo Gasto/Costo)\n";
    }
    imprimirSeparador(75, '-');
    cout << left  << setw(55) << "TOTAL GASTOS Y COSTOS:"
         << right << setw(20) << totalGastos
         << "\n";
    imprimirSeparador(75, '=');

    archivo.close();

    // --- RESUMEN FINAL: UTILIDAD O PERDIDA NETA ---
    double resultadoNeto = totalIngresos - totalGastos;

    cout << "\n";
    imprimirSeparador(75, '*');
    if (resultadoNeto >= 0.0) {
        cout << left  << setw(55) << " ( + ) UTILIDAD NETA DEL EJERCICIO:"
             << right << setw(20) << resultadoNeto
             << "\n";
        cout << "       Resultado: Rentabilidad favorable en el periodo analizado.\n";
    } else {
        cout << left  << setw(55) << " ( - ) PERDIDA NETA DEL EJERCICIO:"
             << right << setw(20) << resultadoNeto
             << "\n";
        cout << "       Alerta: Los costos y gastos superaron los ingresos del periodo.\n";
    }
    imprimirSeparador(75, '*');

    pausarConsola();
}

// ============================================================================
// 3. Hoja de Costos por Órdenes de Producción
// ============================================================================

void generarHojaCostos() {
    ifstream archivo(ARCHIVO_COSTOS, ios::binary | ios::in);

    if (!archivo.is_open()) {
        mostrarErrorArchivo(ARCHIVO_COSTOS, "Modulo 3 (Contabilidad de Costos)");
        pausarConsola();
        return;
    }

    cout << "\n";
    imprimirSeparador(104, '=');
    cout << "                              FINCOST C++ - MODULO DE COSTOS\n";
    cout << "                    HOJA DE COSTOS POR ORDENES DE PRODUCCION (ACTIVAS)\n";
    imprimirSeparador(104, '=');

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
    double acumMateriales = 0.0;
    double acumManoObra = 0.0;
    double acumCif = 0.0;
    double acumCostoTotal = 0.0;
    int acumUnidades = 0;

    cout << fixed << setprecision(2);

    while (archivo.read(reinterpret_cast<char*>(&orden), sizeof(OrdenCosto))) {
        if (orden.activo == 0) {
            continue; // Ignorar ordenes inactivas
        }

        totalOrdenes++;
        double costoTotalOrden = orden.materiales_directos + orden.mano_obra_directa + orden.cif;
        
        double costoUnitarioCalculado = 0.0;
        if (orden.unidades_producidas > 0) {
            costoUnitarioCalculado = costoTotalOrden / orden.unidades_producidas;
        } else if (orden.costo_unitario > 0.0) {
            costoUnitarioCalculado = orden.costo_unitario;
        }

        acumMateriales += orden.materiales_directos;
        acumManoObra += orden.mano_obra_directa;
        acumCif += orden.cif;
        acumCostoTotal += costoTotalOrden;
        acumUnidades += orden.unidades_producidas;

        cout << left  << setw(10) << orden.numero_orden
             << right << setw(16) << orden.materiales_directos
             << right << setw(16) << orden.mano_obra_directa
             << right << setw(15) << orden.cif
             << right << setw(16) << costoTotalOrden
             << right << setw(13) << orden.unidades_producidas
             << right << setw(18) << costoUnitarioCalculado
             << "\n";
    }

    archivo.close();

    if (totalOrdenes == 0) {
        cout << "\n [i] El archivo \"" << ARCHIVO_COSTOS << "\" no contiene ordenes activas registradas.\n";
        imprimirSeparador(104, '-');
        pausarConsola();
        return;
    }

    // Totales acumulados
    imprimirSeparador(104, '=');
    double costoUnitarioPromedioGlobal = (acumUnidades > 0) ? (acumCostoTotal / acumUnidades) : 0.0;

    cout << left  << setw(10) << "TOTALES:"
         << right << setw(16) << acumMateriales
         << right << setw(16) << acumManoObra
         << right << setw(15) << acumCif
         << right << setw(16) << acumCostoTotal
         << right << setw(13) << acumUnidades
         << right << setw(18) << costoUnitarioPromedioGlobal
         << "\n";
    imprimirSeparador(104, '=');

    cout << " Resumen: " << totalOrdenes << " orden(es) activa(s) procesada(s) | Total de unidades fabricadas: " 
         << acumUnidades << "\n";
    imprimirSeparador(104, '-');

    pausarConsola();
}
