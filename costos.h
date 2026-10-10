#ifndef COSTOS_H
#define COSTOS_H

/**
 * ============================================================================
 * Proyecto: FinCost C++ - Sistema de Contabilidad Financiera y Costos
 * Archivo:  costos.h
 * Descripción: Declaraciones del Módulo 3 - Contabilidad de Costos Industriales.
 *              Gestiona altas, consultas, modificaciones y baja logica de ordenes
 *              de produccion sobre el archivo binario costos.dat.
 * Paradigma: Programación Estructurada (Estricto sin clases / POO).
 * ============================================================================
 */

#include "estructuras.h"

// ============================================================================
// Prototipos de Funciones del Módulo de Costos Industriales
// ============================================================================

/**
 * Solicita los datos de una nueva orden de produccion.
 * Valida numero de orden unico (contra activos o no), costos no negativos
 * y unidades mayores a cero. Calcula automaticamente el costo unitario.
 */
void crearOrden();

/**
 * Lee e imprime en consola todas las ordenes de costo activas (activo == 1)
 * almacenadas en costos.dat con formato tabulado y totales acumulados.
 */
void listarOrdenes();

/**
 * Consulta y muestra el detalle completo de una orden activa por su numero.
 */
void consultarOrden();

/**
 * Modifica los elementos de costo (MD, MOD, CIF) o las unidades producidas
 * de una orden activa existente. Recalcula automaticamente el costo unitario.
 * Solicita confirmacion (S/N) previa.
 */
void modificarOrden();

/**
 * Realiza la baja logica (activo = 0) de una orden de produccion.
 * Solicita confirmacion (S/N) antes de aplicar la baja.
 */
void eliminarOrden();

/**
 * Muestra el submenú del Módulo 3 con las opciones de gestion de costos.
 */
void submenuCostos();

#endif // COSTOS_H
