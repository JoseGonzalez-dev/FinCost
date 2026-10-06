#ifndef CATALOGO_H
#define CATALOGO_H

/**
 * ============================================================================
 * Proyecto: FinCost C++ - Sistema de Contabilidad Financiera y Costos
 * Archivo:  catalogo.h
 * Descripción: Declaraciones del Módulo 1 - Catálogo de Cuentas Contables.
 *              Gestiona altas y consultas sobre el archivo binario cuentas.dat.
 * Paradigma: Programación Estructurada (Estricto sin clases / POO).
 * ============================================================================
 */

#include "estructuras.h"

// ============================================================================
// Prototipos de Funciones del Módulo de Catálogo Contable
// ============================================================================

/**
 * Solicita los datos de una nueva cuenta contable al usuario.
 * Valida que el código no exista previamente en cuentas.dat y que el saldo
 * no sea negativo. Si todo es correcto, agrega el registro al archivo binario.
 */
void agregarCuenta();

/**
 * Lee e imprime en consola todas las cuentas almacenadas en cuentas.dat
 * con formato tabulado y montos formateados a 2 decimales.
 */
void listarCuentas();

/**
 * Muestra el submenú del Módulo 1 con las opciones de agregar y listar cuentas.
 * Cicla hasta que el usuario elija regresar al menú principal.
 */
void submenuCatalogo();

#endif // CATALOGO_H
