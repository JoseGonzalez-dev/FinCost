/**
 * ============================================================================
 * Proyecto: FinCost C++ - Sistema de Contabilidad Financiera y Costos
 * Archivo:  catalogo.cpp
 * Descripción: Implementación del Módulo 1 - Catálogo de Cuentas Contables.
 *              Incluye CRUD completo con bajas logicas, verificacion de
 *              integridad referencial con diario.dat y tipos normalizados.
 * Paradigma: Programación Estructurada (Estricto sin clases / POO).
 * ============================================================================
 */

#include "catalogo.h"
#include "diario.h"    // Para cuentaTieneAsientosActivos()
#include "reportes.h"  // Para imprimirSeparador() y pausarConsola()
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>
#include <cctype>
#include <cstdlib>
#include <limits>

using namespace std;

// ============================================================================
// Funciones Auxiliares Locales
// ============================================================================

/**
 * Lee un entero de la entrada estandar con proteccion contra fallos y EOF.
 * Si se detecta fin de entrada (EOF), retorna -1 sin entrar en bucle infinito.
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
 * Lee un double de la entrada estandar con proteccion contra fallos y EOF.
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
 * Solicita confirmacion al usuario mediante (S/N).
 * Retorna true unicamente si la respuesta es 'S' o 's'.
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
 * Verifica si un codigo de cuenta ya existe en cuentas.dat (activo o inactivo).
 * Regla de negocio: Los codigos dados de baja no pueden reutilizarse.
 */
static bool existeCodigoCuentaFisico(int codigo) {
    ifstream archivo(ARCHIVO_CUENTAS, ios::binary | ios::in);
    if (!archivo.is_open()) {
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
 * Valida y normaliza el tipo de cuenta contable.
 * Permite los 6 tipos contables estandar: Activo, Pasivo, Capital, Ingreso, Gasto, Costo.
 * Compara de manera insensible a mayusculas y escribe el resultado estandarizado
 * con la primera letra en mayuscula y las demas en minuscula.
 */
static bool normalizarTipoCuenta(const char* entrada, char* salida, int tamSalida) {
    if (!entrada || strlen(entrada) == 0) {
        return false;
    }

    const char* tiposValidos[6];
    tiposValidos[0] = "Activo";
    tiposValidos[1] = "Pasivo";
    tiposValidos[2] = "Capital";
    tiposValidos[3] = "Ingreso";
    tiposValidos[4] = "Gasto";
    tiposValidos[5] = "Costo";

    for (int i = 0; i < 6; ++i) {
        const char* tv = tiposValidos[i];
        int lenE = strlen(entrada);
        int lenT = strlen(tv);
        if (lenE != lenT) {
            continue;
        }

        bool coincide = true;
        for (int k = 0; k < lenE; ++k) {
            if (tolower(static_cast<unsigned char>(entrada[k])) !=
                tolower(static_cast<unsigned char>(tv[k]))) {
                coincide = false;
                break;
            }
        }

        if (coincide) {
            strncpy(salida, tv, tamSalida - 1);
            salida[tamSalida - 1] = '\0';
            return true;
        }
    }
    return false;
}

// ============================================================================
// Implementación: cuentaActiva()
// ============================================================================

bool cuentaActiva(int codigo) {
    ifstream archivo(ARCHIVO_CUENTAS, ios::binary | ios::in);
    if (!archivo.is_open()) {
        return false;
    }

    Cuenta temp;
    while (archivo.read(reinterpret_cast<char*>(&temp), sizeof(Cuenta))) {
        if (temp.codigo == codigo && temp.activo == 1) {
            archivo.close();
            return true;
        }
    }
    archivo.close();
    return false;
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
    memset(&nueva, 0, sizeof(Cuenta));
    nueva.activo = 1;

    // --- Codigo de cuenta ---
    nueva.codigo = leerEntero(" Ingrese el codigo de la cuenta: ");
    if (cin.eof() || nueva.codigo <= 0) {
        cout << "\n [!] Codigo invalido o cancelado. Operacion terminada.\n";
        pausarConsola();
        return;
    }

    // Validar unicidad contra todo el archivo (activos e inactivos)
    if (existeCodigoCuentaFisico(nueva.codigo)) {
        cout << "\n [!] ERROR: El codigo " << nueva.codigo
             << " ya existe en el catalogo de cuentas (activo o historico).\n";
        cout << "     No se permite reutilizar codigos. Operacion cancelada.\n";
        pausarConsola();
        return;
    }

    // --- Nombre de la cuenta ---
    cout << " Ingrese el nombre de la cuenta: ";
    cin.getline(nueva.nombre, 50);
    if (cin.eof()) {
        return;
    }

    if (strlen(nueva.nombre) == 0) {
        cout << "\n [!] ERROR: El nombre de la cuenta no puede estar vacio.\n";
        pausarConsola();
        return;
    }

    // --- Tipo de cuenta ---
    cout << " Tipos validos: Activo, Pasivo, Capital, Ingreso, Gasto, Costo\n";
    cout << " Ingrese el tipo de la cuenta: ";
    char tipoIngresado[50];
    memset(tipoIngresado, 0, sizeof(tipoIngresado));
    cin.getline(tipoIngresado, 50);
    if (cin.eof()) {
        return;
    }

    char tipoNormalizado[20];
    memset(tipoNormalizado, 0, sizeof(tipoNormalizado));
    if (!normalizarTipoCuenta(tipoIngresado, tipoNormalizado, sizeof(tipoNormalizado))) {
        cout << "\n [!] ERROR: El tipo \"" << tipoIngresado << "\" no es valido.\n";
        cout << "     Debe ser: Activo, Pasivo, Capital, Ingreso, Gasto o Costo.\n";
        pausarConsola();
        return;
    }
    strncpy(nueva.tipo, tipoNormalizado, sizeof(nueva.tipo) - 1);

    // --- Saldo inicial ---
    nueva.saldo = leerDouble(" Ingrese el saldo inicial ($): ");
    if (cin.eof()) {
        return;
    }

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
        cout << "\n [!] ERROR CRITICO: No se pudo abrir el archivo \""
             << ARCHIVO_CUENTAS << "\".\n";
        pausarConsola();
        return;
    }

    archivo.write(reinterpret_cast<const char*>(&nueva), sizeof(Cuenta));
    archivo.close();

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
    cout << "              MODULO 1 :: CATALOGO DE CUENTAS CONTABLES ACTIVAS\n";
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
    double totalSaldos = 0.0;

    while (archivo.read(reinterpret_cast<char*>(&cuenta), sizeof(Cuenta))) {
        if (cuenta.activo == 0) {
            continue; // Ignorar cuentas dadas de baja logica
        }
        totalCuentas++;
        totalSaldos += cuenta.saldo;
        cout << left  << setw(10) << cuenta.codigo
             << left  << setw(34) << cuenta.nombre
             << left  << setw(16) << cuenta.tipo
             << right << setw(18) << cuenta.saldo
             << "\n";
    }

    archivo.close();

    if (totalCuentas == 0) {
        cout << "\n [i] No hay cuentas activas registradas en el catalogo.\n";
    } else {
        imprimirSeparador(80, '=');
        cout << left  << setw(60) << " SUMATORIA TOTAL DE SALDOS:"
             << right << setw(18) << totalSaldos << "\n";
        imprimirSeparador(80, '=');
        cout << " Total de cuentas activas: " << totalCuentas << "\n";
    }
    imprimirSeparador(80, '-');

    pausarConsola();
}

// ============================================================================
// Implementación: consultarCuenta()
// ============================================================================

void consultarCuenta() {
    cout << "\n";
    imprimirSeparador(65, '=');
    cout << "       MODULO 1 :: CONSULTAR CUENTA CONTABLE POR CODIGO\n";
    imprimirSeparador(65, '=');

    int codigo = leerEntero(" Ingrese el codigo de cuenta a consultar: ");
    if (cin.eof() || codigo <= 0) {
        return;
    }

    ifstream archivo(ARCHIVO_CUENTAS, ios::binary | ios::in);
    if (!archivo.is_open()) {
        cout << "\n [!] No se pudo abrir el archivo \"" << ARCHIVO_CUENTAS << "\".\n";
        pausarConsola();
        return;
    }

    Cuenta cuenta;
    bool encontrada = false;

    while (archivo.read(reinterpret_cast<char*>(&cuenta), sizeof(Cuenta))) {
        if (cuenta.codigo == codigo && cuenta.activo == 1) {
            encontrada = true;
            break;
        }
    }
    archivo.close();

    if (!encontrada) {
        cout << "\n [!] No se encontro ninguna cuenta activa con el codigo " << codigo << ".\n";
    } else {
        cout << "\n";
        imprimirSeparador(65, '-');
        cout << " [i] DATOS DE LA CUENTA CONTABLE\n";
        imprimirSeparador(65, '-');
        cout << fixed << setprecision(2);
        cout << " Codigo:  " << cuenta.codigo << "\n";
        cout << " Nombre:  " << cuenta.nombre << "\n";
        cout << " Tipo:    " << cuenta.tipo << "\n";
        cout << " Saldo:   $" << cuenta.saldo << "\n";
        cout << " Estado:  Vigente (Activa)\n";
        imprimirSeparador(65, '-');
    }

    pausarConsola();
}

// ============================================================================
// Implementación: modificarCuenta()
// ============================================================================

void modificarCuenta() {
    cout << "\n";
    imprimirSeparador(65, '=');
    cout << "             MODULO 1 :: MODIFICAR CUENTA CONTABLE\n";
    imprimirSeparador(65, '=');

    int codigo = leerEntero(" Ingrese el codigo de la cuenta a modificar: ");
    if (cin.eof() || codigo <= 0) {
        return;
    }

    fstream archivo(ARCHIVO_CUENTAS, ios::binary | ios::in | ios::out);
    if (!archivo.is_open()) {
        cout << "\n [!] No se pudo abrir el archivo \"" << ARCHIVO_CUENTAS << "\".\n";
        pausarConsola();
        return;
    }

    Cuenta cuenta;
    bool encontrada = false;
    streampos posicionRegistro = 0;

    while (true) {
        posicionRegistro = archivo.tellg();
        if (!archivo.read(reinterpret_cast<char*>(&cuenta), sizeof(Cuenta))) {
            break;
        }
        if (cuenta.codigo == codigo && cuenta.activo == 1) {
            encontrada = true;
            break;
        }
    }

    if (!encontrada) {
        archivo.close();
        cout << "\n [!] No se encontro ninguna cuenta activa con el codigo " << codigo << ".\n";
        pausarConsola();
        return;
    }

    cout << "\n";
    imprimirSeparador(65, '-');
    cout << " DATOS ACTUALES (Codigo: " << cuenta.codigo << " [No modificable])\n";
    cout << "  1. Nombre actual: " << cuenta.nombre << "\n";
    cout << "  2. Tipo actual:   " << cuenta.tipo << "\n";
    cout << fixed << setprecision(2);
    cout << "  3. Saldo actual:  $" << cuenta.saldo << "\n";
    imprimirSeparador(65, '-');
    cout << " (Presione ENTER vacio en cualquier campo para conservar el valor actual)\n\n";

    // Modificar Nombre
    char bufferNombre[50];
    memset(bufferNombre, 0, sizeof(bufferNombre));
    cout << " Nuevo nombre: ";
    cin.getline(bufferNombre, 50);
    if (cin.eof()) {
        archivo.close();
        return;
    }

    // Modificar Tipo
    char bufferTipo[50];
    memset(bufferTipo, 0, sizeof(bufferTipo));
    cout << " Nuevo tipo (Activo, Pasivo, Capital, Ingreso, Gasto, Costo): ";
    cin.getline(bufferTipo, 50);
    if (cin.eof()) {
        archivo.close();
        return;
    }

    // Modificar Saldo
    char bufferSaldo[50];
    memset(bufferSaldo, 0, sizeof(bufferSaldo));
    cout << " Nuevo saldo ($): ";
    cin.getline(bufferSaldo, 50);
    if (cin.eof()) {
        archivo.close();
        return;
    }

    // Aplicar o conservar cambios en una copia de trabajo
    Cuenta cuentaModificada = cuenta;

    if (strlen(bufferNombre) > 0) {
        strncpy(cuentaModificada.nombre, bufferNombre, sizeof(cuentaModificada.nombre) - 1);
        cuentaModificada.nombre[sizeof(cuentaModificada.nombre) - 1] = '\0';
    }

    if (strlen(bufferTipo) > 0) {
        char tipoNormalizado[20];
        memset(tipoNormalizado, 0, sizeof(tipoNormalizado));
        if (!normalizarTipoCuenta(bufferTipo, tipoNormalizado, sizeof(tipoNormalizado))) {
            archivo.close();
            cout << "\n [!] ERROR: El tipo ingresado \"" << bufferTipo << "\" no es valido.\n";
            cout << "     Modificacion cancelada.\n";
            pausarConsola();
            return;
        }
        strncpy(cuentaModificada.tipo, tipoNormalizado, sizeof(cuentaModificada.tipo) - 1);
        cuentaModificada.tipo[sizeof(cuentaModificada.tipo) - 1] = '\0';
    }

    if (strlen(bufferSaldo) > 0) {
        char* finPtr = 0;
        double nuevoSaldo = strtod(bufferSaldo, &finPtr);
        if (finPtr == bufferSaldo || nuevoSaldo < 0.0) {
            archivo.close();
            cout << "\n [!] ERROR: El saldo ingresado no es valido o es negativo.\n";
            cout << "     Modificacion cancelada.\n";
            pausarConsola();
            return;
        }
        cuentaModificada.saldo = nuevoSaldo;
    }

    // Resumen previo a confirmar
    cout << "\n";
    imprimirSeparador(65, '-');
    cout << " RESUMEN DE CAMBIOS A APLICAR:\n";
    cout << "  Codigo: " << cuentaModificada.codigo << "\n";
    cout << "  Nombre: " << cuentaModificada.nombre << "\n";
    cout << "  Tipo:   " << cuentaModificada.tipo << "\n";
    cout << "  Saldo:  $" << cuentaModificada.saldo << "\n";
    imprimirSeparador(65, '-');

    if (!confirmarOperacion(" Desea guardar los cambios? (S/N): ")) {
        archivo.close();
        cout << "\n [i] Modificacion cancelada por el usuario.\n";
        pausarConsola();
        return;
    }

    archivo.seekp(posicionRegistro);
    archivo.write(reinterpret_cast<const char*>(&cuentaModificada), sizeof(Cuenta));
    archivo.close();

    cout << "\n [OK] Cuenta #" << codigo << " modificada exitosamente.\n";
    pausarConsola();
}

// ============================================================================
// Implementación: eliminarCuenta()
// ============================================================================

void eliminarCuenta() {
    cout << "\n";
    imprimirSeparador(65, '=');
    cout << "       MODULO 1 :: ELIMINAR CUENTA CONTABLE (BAJA LOGICA)\n";
    imprimirSeparador(65, '=');

    int codigo = leerEntero(" Ingrese el codigo de la cuenta a dar de baja: ");
    if (cin.eof() || codigo <= 0) {
        return;
    }

    fstream archivo(ARCHIVO_CUENTAS, ios::binary | ios::in | ios::out);
    if (!archivo.is_open()) {
        cout << "\n [!] No se pudo abrir el archivo \"" << ARCHIVO_CUENTAS << "\".\n";
        pausarConsola();
        return;
    }

    Cuenta cuenta;
    bool encontrada = false;
    streampos posicionRegistro = 0;

    while (true) {
        posicionRegistro = archivo.tellg();
        if (!archivo.read(reinterpret_cast<char*>(&cuenta), sizeof(Cuenta))) {
            break;
        }
        if (cuenta.codigo == codigo && cuenta.activo == 1) {
            encontrada = true;
            break;
        }
    }

    if (!encontrada) {
        archivo.close();
        cout << "\n [!] No se encontro ninguna cuenta activa con el codigo " << codigo << ".\n";
        pausarConsola();
        return;
    }

    // Regla de Integridad Referencial: Verificar si tiene asientos activos en diario.dat
    if (cuentaTieneAsientosActivos(codigo)) {
        archivo.close();
        cout << "\n";
        imprimirSeparador(70, '!');
        cout << " [!] ERROR DE INTEGRIDAD REFERENCIAL:\n";
        cout << "     No se puede eliminar la cuenta #" << codigo << " (" << cuenta.nombre << ").\n";
        cout << "     Motivo: Existen asientos contables activos en \"" << ARCHIVO_DIARIO
             << "\" que registran movimientos para esta cuenta.\n";
        cout << "     Accion: Debe dar de baja primero las partidas correspondientes\n";
        cout << "             antes de poder eliminar esta cuenta del catalogo.\n";
        imprimirSeparador(70, '!');
        pausarConsola();
        return;
    }

    cout << "\n";
    imprimirSeparador(65, '-');
    cout << " CUENTA A DAR DE BAJA:\n";
    cout << "  Codigo: " << cuenta.codigo << "\n";
    cout << "  Nombre: " << cuenta.nombre << "\n";
    cout << "  Tipo:   " << cuenta.tipo << "\n";
    cout << fixed << setprecision(2);
    cout << "  Saldo:  $" << cuenta.saldo << "\n";
    imprimirSeparador(65, '-');

    if (!confirmarOperacion(" Esta seguro de dar de baja logica esta cuenta? (S/N): ")) {
        archivo.close();
        cout << "\n [i] Eliminacion cancelada por el usuario.\n";
        pausarConsola();
        return;
    }

    cuenta.activo = 0; // Baja logica
    archivo.seekp(posicionRegistro);
    archivo.write(reinterpret_cast<const char*>(&cuenta), sizeof(Cuenta));
    archivo.close();

    cout << "\n [OK] Cuenta #" << codigo << " dada de baja exitosamente.\n";
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
        cout << " 2. Listar todas las cuentas activas\n";
        cout << " 3. Consultar cuenta por codigo\n";
        cout << " 4. Modificar cuenta\n";
        cout << " 5. Eliminar cuenta (baja logica)\n";
        cout << " 6. Regresar al Menu Principal\n";
        imprimirSeparador(65, '-');
        cout << " Seleccione una opcion [1-6]: ";

        opcion = leerEntero("");
        if (cin.eof() || opcion == -1) {
            break;
        }

        switch (opcion) {
            case 1:
                agregarCuenta();
                break;
            case 2:
                listarCuentas();
                break;
            case 3:
                consultarCuenta();
                break;
            case 4:
                modificarCuenta();
                break;
            case 5:
                eliminarCuenta();
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
