/**
 * ============================================================================
 * Proyecto: FinCost C++ - Sistema de Contabilidad Financiera y Costos
 * Archivo:  catalogo.cpp
 * Descripción: Implementación del Módulo 1 - Catálogo de Cuentas Contables.
 *              Permite agregar cuentas con validación de unicidad y saldo,
 *              y listar todas las cuentas almacenadas en cuentas.dat.
 * Paradigma: Programación Estructurada (Estricto sin clases / POO).
 * ============================================================================
 */

#include "catalogo.h"
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
 * Busca si un código de cuenta ya existe en cuentas.dat.
 * @param codigo Código a buscar.
 * @return true si el código ya fue registrado, false en caso contrario.
 */
static bool existeCodigoCuenta(int codigo) {
    ifstream archivo(ARCHIVO_CUENTAS, ios::binary | ios::in);
    if (!archivo.is_open()) {
        // Si el archivo no existe, ningún código puede estar duplicado
        return false;
    }

    Cuenta temp;
    while (archivo.read(reinterpret_cast<char*>(&temp), sizeof(Cuenta))) {
        if (temp.codigo == codigo) {
            archivo.close();
            return true;
        }
    }
    archivo.close();
    return false;
}

/**
 * Lee un entero con validación robusta contra entradas no numéricas.
 * Repite la solicitud hasta obtener un número válido.
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
 * Lee un double con validación robusta contra entradas no numéricas.
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
// Implementación: agregarCuenta()
// ============================================================================

void agregarCuenta() {
    cout << "\n";
    imprimirSeparador(65, '=');
    cout << "       MODULO 1 :: AGREGAR NUEVA CUENTA AL CATALOGO CONTABLE\n";
    imprimirSeparador(65, '=');

    Cuenta nueva;
    // Inicializar la estructura a ceros para evitar basura en el archivo binario
    memset(&nueva, 0, sizeof(Cuenta));

    // --- Código de cuenta ---
    nueva.codigo = leerEntero(" Ingrese el codigo de la cuenta: ");

    // Validar que el código no exista previamente
    if (existeCodigoCuenta(nueva.codigo)) {
        cout << "\n [!] ERROR: El codigo " << nueva.codigo
             << " ya existe en el catalogo de cuentas.\n";
        cout << "     No se permiten codigos duplicados. Operacion cancelada.\n";
        pausarConsola();
        return;
    }

    // --- Nombre de la cuenta ---
    cout << " Ingrese el nombre de la cuenta: ";
    cin.getline(nueva.nombre, 50);

    // Validar que no esté vacío
    if (strlen(nueva.nombre) == 0) {
        cout << "\n [!] ERROR: El nombre de la cuenta no puede estar vacio.\n";
        pausarConsola();
        return;
    }

    // --- Tipo de cuenta ---
    cout << " Tipos validos: Activo, Pasivo, Capital, Ingreso, Gasto, Costo\n";
    cout << " Ingrese el tipo de la cuenta: ";
    cin.getline(nueva.tipo, 20);

    // Validar que no esté vacío
    if (strlen(nueva.tipo) == 0) {
        cout << "\n [!] ERROR: El tipo de la cuenta no puede estar vacio.\n";
        pausarConsola();
        return;
    }

    // --- Saldo inicial ---
    nueva.saldo = leerDouble(" Ingrese el saldo inicial ($): ");

    // Validar que el saldo no sea negativo
    if (nueva.saldo < 0.0) {
        cout << "\n [!] ERROR: El saldo no puede ser negativo ($"
             << fixed << setprecision(2) << nueva.saldo << ").\n";
        cout << "     Ingrese un valor mayor o igual a cero. Operacion cancelada.\n";
        pausarConsola();
        return;
    }

    // --- Guardar en archivo binario ---
    ofstream archivo(ARCHIVO_CUENTAS, ios::binary | ios::app);
    if (!archivo.is_open()) {
        cout << "\n [!] ERROR CRITICO: No se pudo abrir/crear el archivo \""
             << ARCHIVO_CUENTAS << "\".\n";
        pausarConsola();
        return;
    }

    archivo.write(reinterpret_cast<const char*>(&nueva), sizeof(Cuenta));
    archivo.close();

    // Confirmación visual
    cout << "\n";
    imprimirSeparador(65, '-');
    cout << " [OK] Cuenta registrada exitosamente en el catalogo.\n";
    cout << fixed << setprecision(2);
    cout << "      Codigo: " << nueva.codigo << "\n";
    cout << "      Nombre: " << nueva.nombre << "\n";
    cout << "      Tipo:   " << nueva.tipo << "\n";
    cout << "      Saldo:  $" << nueva.saldo << "\n";
    imprimirSeparador(65, '-');

    pausarConsola();
}

// ============================================================================
// Implementación: listarCuentas()
// ============================================================================

void listarCuentas() {
    ifstream archivo(ARCHIVO_CUENTAS, ios::binary | ios::in);

    if (!archivo.is_open()) {
        cout << "\n";
        imprimirSeparador(65, '!');
        cout << " [!] El archivo \"" << ARCHIVO_CUENTAS
             << "\" no existe o esta vacio.\n";
        cout << "     Agregue cuentas desde la opcion 1 para generar el archivo.\n";
        imprimirSeparador(65, '!');
        pausarConsola();
        return;
    }

    cout << "\n";
    imprimirSeparador(80, '=');
    cout << "              MODULO 1 :: CATALOGO DE CUENTAS CONTABLES\n";
    imprimirSeparador(80, '=');

    cout << fixed << setprecision(2);
    cout << left  << setw(10) << "CODIGO"
         << left  << setw(34) << "NOMBRE"
         << left  << setw(16) << "TIPO"
         << right << setw(18) << "SALDO ($)"
         << "\n";
    imprimirSeparador(80, '-');

    Cuenta cuenta;
    int totalCuentas = 0;

    while (archivo.read(reinterpret_cast<char*>(&cuenta), sizeof(Cuenta))) {
        totalCuentas++;
        cout << left  << setw(10) << cuenta.codigo
             << left  << setw(34) << cuenta.nombre
             << left  << setw(16) << cuenta.tipo
             << right << setw(18) << cuenta.saldo
             << "\n";
    }

    archivo.close();

    if (totalCuentas == 0) {
        cout << "\n [i] El catalogo esta vacio. No hay cuentas registradas.\n";
    } else {
        imprimirSeparador(80, '=');
        cout << " Total de cuentas registradas: " << totalCuentas << "\n";
    }
    imprimirSeparador(80, '-');

    pausarConsola();
}

// ============================================================================
// Submenú del Módulo 1
// ============================================================================

void submenuCatalogo() {
    int opcion = 0;

    do {
        cout << "\n";
        imprimirSeparador(65, '=');
        cout << "      FINCOST C++ :: MODULO 1 - CATALOGO DE CUENTAS CONTABLES\n";
        imprimirSeparador(65, '=');
        cout << " 1. Agregar nueva cuenta\n";
        cout << " 2. Listar todas las cuentas\n";
        cout << " 3. Regresar al Menu Principal\n";
        imprimirSeparador(65, '-');
        cout << " Seleccione una opcion [1-3]: ";

        opcion = leerEntero("");

        switch (opcion) {
            case 1:
                agregarCuenta();
                break;
            case 2:
                listarCuentas();
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
