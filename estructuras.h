#ifndef ESTRUCTURAS_H
#define ESTRUCTURAS_H

/**
 * ============================================================================
 * Proyecto: FinCost C++ - Sistema de Contabilidad Financiera y Costos
 * Archivo:  estructuras.h
 * Descripción: Definición de registros (structs) base para la persistencia en
 *              archivos binarios (.dat) y estandarización entre módulos.
 * Paradigma: Programación Estructurada (Estricto sin clases / POO).
 * ============================================================================
 */

// Nombres estándar de los archivos binarios compartidos por todos los módulos
#define ARCHIVO_CUENTAS "cuentas.dat"
#define ARCHIVO_DIARIO  "diario.dat"
#define ARCHIVO_COSTOS  "costos.dat"

/**
 * Estructura: Cuenta
 * Archivo binario destino: cuentas.dat
 * Descripción: Almacena la información de cada cuenta contable en el catálogo.
 */
struct Cuenta {
    int codigo;         // Código numérico único de la cuenta (ej. 101, 1101)
    char nombre[50];    // Nombre descriptivo (ej. "Caja General", "Ventas")
    char tipo[20];      // Tipo/Naturaleza (ej. "Activo", "Pasivo", "Capital", "Ingreso", "Gasto")
    double saldo;       // Saldo monetario actual de la cuenta
};

/**
 * Estructura: Asiento
 * Archivo binario destino: diario.dat
 * Descripción: Representa una línea o movimiento contable dentro del libro diario.
 */
struct Asiento {
    char fecha[15];         // Fecha de registro en formato DD/MM/AAAA
    int codigo_cuenta;      // Código de la cuenta asociada (referencia a Cuenta.codigo)
    char descripcion[100];  // Detalle o glosa de la transacción
    double debe;            // Monto cargado al Debe
    double haber;           // Monto abonado al Haber
    int numero_partida;     // Número correlativo de la partida o asiento contable
};

/**
 * Estructura: OrdenCosto
 * Archivo binario destino: costos.dat
 * Descripción: Registra los costos de producción acumulados por orden de trabajo.
 */
struct OrdenCosto {
    int numero_orden;           // Identificador único de la orden de producción
    double materiales_directos; // Costo acumulado de Materiales Directos (MD)
    double mano_obra_directa;   // Costo acumulado de Mano de Obra Directa (MOD)
    double cif;                 // Costos Indirectos de Fabricación (CIF)
    int unidades_producidas;    // Cantidad total de unidades terminadas en la orden
    double costo_unitario;      // Costo unitario resultante ((MD + MOD + CIF) / unidades)
};

#endif // ESTRUCTURAS_H
