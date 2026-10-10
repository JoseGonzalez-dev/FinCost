#ifndef ESTRUCTURAS_H
#define ESTRUCTURAS_H

/**
 * ============================================================================
 * Proyecto: FinCost C++ - Sistema de Contabilidad Financiera y Costos
 * Archivo:  estructuras.h
 * Descripción: Definicion de registros (structs) base para la persistencia en
 *              archivos binarios (.dat) y estandarizacion entre modulos.
 * Paradigma: Programacion Estructurada (Estricto sin clases / POO).
 * ============================================================================
 */

// Nombres estandar de los archivos binarios compartidos por todos los modulos
#define ARCHIVO_CUENTAS "cuentas.dat"
#define ARCHIVO_DIARIO  "diario.dat"
#define ARCHIVO_COSTOS  "costos.dat"

/**
 * Estructura: Cuenta
 * Archivo binario destino: cuentas.dat
 * Descripcion: Almacena la informacion de cada cuenta contable en el catalogo.
 */
struct Cuenta {
    int codigo;         // Codigo numerico unico de la cuenta (ej. 101, 1101)
    char nombre[50];    // Nombre descriptivo (ej. "Caja General", "Ventas")
    char tipo[20];      // Tipo/Naturaleza (ej. "Activo", "Pasivo", "Capital", "Ingreso", "Gasto", "Costo")
    double saldo;       // Saldo monetario actual de la cuenta
    int activo;         // Estado logico: 1 = vigente, 0 = eliminado (baja logica)
};

/**
 * Estructura: Asiento
 * Archivo binario destino: diario.dat
 * Descripcion: Representa una linea o movimiento contable dentro del libro diario.
 */
struct Asiento {
    char fecha[15];         // Fecha de registro en formato DD/MM/AAAA
    int codigo_cuenta;      // Codigo de la cuenta asociada (referencia a Cuenta.codigo)
    char descripcion[100];  // Detalle o glosa de la transaccion
    double debe;            // Monto cargado al Debe
    double haber;           // Monto abonado al Haber
    int numero_partida;     // Numero correlativo de la partida o asiento contable
    int activo;             // Estado logico: 1 = vigente, 0 = eliminado (baja logica)
};

/**
 * Estructura: OrdenCosto
 * Archivo binario destino: costos.dat
 * Descripcion: Registra los costos de produccion acumulados por orden de trabajo.
 */
struct OrdenCosto {
    int numero_orden;           // Identificador unico de la orden de produccion
    double materiales_directos; // Costo acumulado de Materiales Directos (MD)
    double mano_obra_directa;   // Costo acumulado de Mano de Obra Directa (MOD)
    double cif;                 // Costos Indirectos de Fabricacion (CIF)
    int unidades_producidas;    // Cantidad total de unidades terminadas en la orden
    double costo_unitario;      // Costo unitario resultante ((MD + MOD + CIF) / unidades)
    int activo;                 // Estado logico: 1 = vigente, 0 = eliminado (baja logica)
};

#endif // ESTRUCTURAS_H
