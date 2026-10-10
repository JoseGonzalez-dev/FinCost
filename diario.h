#ifndef DIARIO_H
#define DIARIO_H

/**
 * ============================================================================
 * Proyecto: FinCost C++ - Sistema de Contabilidad Financiera y Costos
 * Archivo:  diario.h
 * Descripción: Declaraciones del Módulo 2 - Libro Diario y Registro de Asientos.
 *              Gestiona registro, consultas multifiltro, modificacion y baja
 *              logica de partidas contables con validacion de partida doble.
 * Paradigma: Programación Estructurada (Estricto sin clases / POO).
 * ============================================================================
 */

#include "estructuras.h"

// ============================================================================
// Prototipos de Funciones del Módulo de Libro Diario
// ============================================================================

/**
 * Proceso iterativo para registrar una partida contable completa.
 * Valida numero de partida unico entre partidas activas, fecha valida DD/MM/AAAA,
 * cuentas activas en catalogo y cuadre exacto de partida doble (Debe == Haber).
 */
void registrarPartida();

/**
 * Lee e imprime en consola todos los asientos activos en diario.dat,
 * agrupados visualmente por numero de partida con totales generales.
 */
void listarAsientos();

/**
 * Consulta y muestra todos los movimientos activos de una partida por su numero.
 */
void consultarPartidaPorNumero();

/**
 * Consulta y muestra todos los movimientos activos que afectan a una cuenta contable.
 */
void consultarPorCuenta();

/**
 * Consulta y muestra todos los movimientos activos registrados en una fecha exacta.
 */
void consultarPorFecha();

/**
 * Modifica una partida contable existente: solicita nuevos movimientos y,
 * solo si cuadran, desactiva los movimientos anteriores y registra los nuevos.
 */
void modificarPartida();

/**
 * Realiza la baja logica (activo = 0) de todos los movimientos de una partida.
 * Solicita confirmacion (S/N) previa.
 */
void eliminarPartida();

/**
 * Verifica si existen asientos contables activos que utilicen un codigo de cuenta.
 * Permite garantizar la integridad referencial antes de eliminar cuentas.
 * @param codigoCuenta Codigo numerico de la cuenta.
 * @return true si existen asientos activos con dicha cuenta, false en caso contrario.
 */
bool cuentaTieneAsientosActivos(int codigoCuenta);

/**
 * Muestra el submenú del Módulo 2 con las opciones de operacion del libro diario.
 */
void submenuDiario();

#endif // DIARIO_H
