#ifndef REPORTES_H
#define REPORTES_H

/**
 * ============================================================================
 * Proyecto: FinCost C++ - Sistema de Contabilidad Financiera y Costos
 * Archivo:  reportes.h
 * Descripción: Declaraciones de funciones del Módulo de Reportes Automatizados.
 * Paradigma: Programación Estructurada (Estricto sin clases / POO).
 * ============================================================================
 */

#include "estructuras.h"

// ============================================================================
// Prototipos de Funciones de Reportes
// ============================================================================

/**
 * Genera e imprime en consola el Balance de Comprobación a partir de cuentas.dat.
 * Agrupa las cuentas según su naturaleza (Debe/Haber), totaliza ambos saldos
 * y verifica si la partida doble se encuentra cuadrada.
 */
void generarBalanceComprobacion();

/**
 * Genera e imprime en consola el Estado de Resultados a partir de cuentas.dat.
 * Filtra cuentas de ingresos y gastos, totaliza cada sección y calcula
 * la Utilidad o Pérdida Neta del ejercicio contable.
 */
void generarEstadoResultados();

/**
 * Genera e imprime en consola la Hoja de Costos por Órdenes de Producción
 * a partir de costos.dat. Detalla MD, MOD, CIF, Costo Total y Costo Unitario.
 */
void generarHojaCostos();

// ============================================================================
// Funciones Auxiliares de Interfaz y Utilidad
// ============================================================================

/**
 * Pausa la consola y espera a que el usuario presione una tecla para continuar.
 */
void pausarConsola();

/**
 * Imprime una línea divisoria decorativa con caracteres ASCII estándar.
 * @param longitud Cantidad de caracteres de la línea.
 * @param caracter Carácter a repetir (por defecto '-').
 */
void imprimirSeparador(int longitud, char caracter = '-');

#endif // REPORTES_H
