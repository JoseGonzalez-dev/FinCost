#ifndef DIARIO_H
#define DIARIO_H

/**
 * ============================================================================
 * Proyecto: FinCost C++ - Sistema de Contabilidad Financiera y Costos
 * Archivo:  diario.h
 * Descripción: Declaraciones del Módulo 2 - Libro Diario y Registro de Asientos.
 *              Gestiona la creación de partidas contables con validación de
 *              partida doble sobre el archivo binario diario.dat.
 * Paradigma: Programación Estructurada (Estricto sin clases / POO).
 * ============================================================================
 */

#include "estructuras.h"

// ============================================================================
// Prototipos de Funciones del Módulo de Libro Diario
// ============================================================================

/**
 * Proceso iterativo para registrar una partida contable completa.
 * 1. Solicita número de partida y fecha.
 * 2. En un bucle, solicita movimientos (código cuenta, descripción, debe, haber).
 * 3. Al finalizar, valida la partida doble (Suma Debe == Suma Haber).
 *    - Si cuadra: guarda todos los movimientos en diario.dat.
 *    - Si no cuadra: rechaza el guardado y ofrece reintentar.
 */
void registrarPartida();

/**
 * Lee e imprime en consola todos los asientos almacenados en diario.dat,
 * agrupados visualmente por número de partida.
 */
void listarAsientos();

/**
 * Muestra el submenú del Módulo 2 con las opciones de registrar partidas
 * y listar asientos. Cicla hasta que el usuario regrese al menú principal.
 */
void submenuDiario();

#endif // DIARIO_H
