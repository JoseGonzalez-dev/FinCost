#ifndef CATALOGO_H
#define CATALOGO_H

/**
 * ============================================================================
 * Proyecto: FinCost C++ - Sistema de Contabilidad Financiera y Costos
 * Archivo:  catalogo.h
 * Descripción: Declaraciones del Módulo 1 - Catálogo de Cuentas Contables.
 *              Gestiona altas, bajas logicas, modificaciones y consultas sobre
 *              el archivo binario cuentas.dat.
 * Paradigma: Programación Estructurada (Estricto sin clases / POO).
 * ============================================================================
 */

#include "estructuras.h"

// ============================================================================
// Prototipos de Funciones del Módulo de Catálogo Contable
// ============================================================================

/**
 * Solicita los datos de una nueva cuenta contable al usuario.
 * Valida que el código no exista previamente en cuentas.dat (activa o no),
 * que el tipo corresponda a los tipos permitidos y que el saldo no sea negativo.
 */
void agregarCuenta();

/**
 * Lee e imprime en consola todas las cuentas activas (activo == 1)
 * almacenadas en cuentas.dat con formato tabulado y totales.
 */
void listarCuentas();

/**
 * Consulta y muestra la ficha completa de una cuenta activa por su codigo.
 */
void consultarCuenta();

/**
 * Modifica nombre, tipo o saldo de una cuenta existente activa.
 * El codigo no se modifica. Permite conservar valores con ENTER vacio.
 * Solicita confirmacion (S/N) antes de aplicar cambios in-place.
 */
void modificarCuenta();

/**
 * Realiza la baja logica (activo = 0) de una cuenta contable.
 * Impide la eliminacion si la cuenta posee asientos activos en diario.dat.
 * Solicita confirmacion (S/N) antes de aplicar la baja.
 */
void eliminarCuenta();

/**
 * Verifica si una cuenta con el codigo especificado existe y esta activa.
 * @param codigo Codigo de la cuenta a verificar.
 * @return true si la cuenta existe y esta activa (activo == 1), false en caso contrario.
 */
bool cuentaActiva(int codigo);

/**
 * Muestra el submenú del Módulo 1 con las opciones CRUD del catálogo.
 * Cicla hasta que el usuario elija regresar al menú principal o fin de entrada.
 */
void submenuCatalogo();

#endif // CATALOGO_H
