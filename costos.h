#ifndef COSTOS_H
#define COSTOS_H

/**
 * ============================================================================
 * Proyecto: FinCost C++ - Sistema de Contabilidad Financiera y Costos
 * Archivo:  costos.h
 * Descripción: Declaraciones del Módulo 3 - Contabilidad de Costos Industriales.
 *              Gestiona órdenes de producción con cálculo automático de
 *              costo unitario sobre el archivo binario costos.dat.
 * Paradigma: Programación Estructurada (Estricto sin clases / POO).
 * ============================================================================
 */

#include "estructuras.h"

// ============================================================================
// Prototipos de Funciones del Módulo de Costos Industriales
// ============================================================================

/**
 * Solicita los datos de una nueva orden de producción.
 * Calcula automáticamente el costo unitario con la fórmula:
 *   Costo Unitario = (MD + MOD + CIF) / Unidades Producidas
 * antes de guardar el registro en costos.dat.
 */
void crearOrden();

/**
 * Lee e imprime en consola todas las órdenes de costos almacenadas
 * en costos.dat con formato tabulado y totales acumulados.
 */
void listarOrdenes();

/**
 * Muestra el submenú del Módulo 3 con las opciones de crear y listar
 * órdenes de costos. Cicla hasta que el usuario regrese al menú principal.
 */
void submenuCostos();

#endif // COSTOS_H
