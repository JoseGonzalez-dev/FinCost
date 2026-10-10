# 🧾 FinCost C++

**Sistema Unificado de Contabilidad Financiera y Costos**

![C++](https://img.shields.io/badge/C%2B%2B-00599C?style=flat&logo=c%2B%2B&logoColor=white)
![Status](https://img.shields.io/badge/estado-completado-brightgreen)
![Curso](https://img.shields.io/badge/curso-Algoritmos%20UMG-1F3864)

Proyecto de Algoritmos (UMG) — Grupo 7.
Un sistema de consola en C++ que integra la **Contabilidad Financiera** (catálogo de cuentas, libro diario con partida doble, reportes contables) con la **Contabilidad de Costos Industriales** (órdenes de producción con cálculo de costo unitario), implementado bajo el paradigma de **Programación Estructurada clásica** (estricto cero clases / POO), empleando registros (`struct`) y archivos binarios (`.dat`) con soporte para operaciones CRUD completas, bajas lógicas y validación de integridad referencial.

## Tabla de contenidos

- [Arquitectura y Módulos](#arquitectura-y-módulos)
- [Funcionalidades por Módulo](#funcionalidades-por-módulo)
- [Requisitos](#requisitos)
- [Cómo compilar y ejecutar](#cómo-compilar-y-ejecutar)
- [Estructura del proyecto](#estructura-del-proyecto)
- [Reglas de Negocio y Bajas Lógicas](#reglas-de-negocio-y-bajas-lógicas)
- [Equipo y asignaciones](#equipo-y-asignaciones)
- [Estado del proyecto](#estado-del-proyecto)

## Arquitectura y Módulos

| Módulo | Descripción | Archivo binario | Registro (`struct`) |
|---|---|---|---|
| **Catálogo Contable** | CRUD completo de cuentas con validación de tipos, saldos e integridad referencial | `cuentas.dat` | `Cuenta` |
| **Libro Diario** | Registro de partidas con validación estricta de partida doble ($\sum Debe = \sum Haber$), consultas multifiltro, modificación y eliminación | `diario.dat` | `Asiento` |
| **Costos Industriales** | CRUD de órdenes de producción: materia prima (MD) + mano de obra (MOD) + CIF $\to$ recálculo de costo unitario | `costos.dat` | `OrdenCosto` |
| **Reportes Automatizados** | Balance de Comprobación, Estado de Resultados y Hoja de Costos por Órdenes | *(solo lectura)* | — |

## Funcionalidades por Módulo

### 1. Módulo de Catálogo Contable (`catalogo.h` / `catalogo.cpp`)
- `agregarCuenta()`: Alta de cuentas con código numérico único (validado contra activos e inactivos), validación de nombre no vacío, tipo contable normalizado y saldo inicial no negativo.
- `listarCuentas()`: Listado tabular de todas las cuentas activas en `cuentas.dat` con sumatoria acumulada de saldos.
- `consultarCuenta()`: Búsqueda y presentación detallada de una cuenta contable activa por su código.
- `modificarCuenta()`: Modificación in-place de nombre, tipo normalizado o saldo (soporta ENTER vacío para conservar valor actual). Código no modificable. Requiere confirmación (S/N).
- `eliminarCuenta()`: Baja lógica (`activo = 0`). Impide la eliminación si existen asientos contables activos en `diario.dat` asociados a la cuenta (integridad referencial). Requiere confirmación (S/N).
- `cuentaActiva(int codigo)`: Función pública auxiliar para verificar la existencia y estado activo de una cuenta.
- `submenuCatalogo()`: Menú interactivo con navegación protegida ante EOF.

### 2. Módulo de Libro Diario (`diario.h` / `diario.cpp`)
- `registrarPartida()`: Registro de partidas contables de hasta 50 movimientos con validación de:
  - Número de partida único entre partidas activas.
  - Formato estricto de fecha `DD/MM/AAAA` (comprobación de días según mes, rango 2000-2100 y años bisiestos).
  - Verificación de que cada cuenta exista y esté activa en el catálogo (`cuentaActiva`).
  - Validación matemática de partida doble ($|\sum Debe - \sum Haber| < 0.001$). Rechazo total si descuadra.
- `listarAsientos()`: Listado general de todos los movimientos activos en `diario.dat` agrupados por partida con totales de Debe y Haber.
- `consultarPartidaPorNumero()`: Muestra todos los movimientos activos de una partida específica y sus sumatorias.
- `consultarPorCuenta()`: Filtra y muestra todos los movimientos activos que afectan a un código de cuenta.
- `consultarPorFecha()`: Filtra y muestra todos los movimientos activos asentados en una fecha exacta.
- `modificarPartida()`: Muestra la partida actual y permite recapturar todos sus movimientos; solo si la nueva versión cuadra y se confirma (S/N), desactiva los movimientos anteriores y guarda los nuevos con el mismo número de partida.
- `eliminarPartida()`: Baja lógica (`activo = 0`) in-place de todos los movimientos de una partida. Requiere confirmación (S/N).
- `cuentaTieneAsientosActivos(int codigoCuenta)`: Función pública de integridad para comprobar si una cuenta tiene movimientos vigentes.
- `submenuDiario()`: Menú interactivo del módulo.

### 3. Módulo de Costos Industriales (`costos.h` / `costos.cpp`)
- `crearOrden()`: Alta de órdenes de producción con número de orden único (validado contra activos e inactivos), costos (MD, MOD, CIF) no negativos y unidades producidas $> 0$. Cálculo automático del costo unitario.
- `listarOrdenes()`: Tabla detallada de órdenes activas con totales acumulados de cada elemento de costo y costo unitario promedio ponderado.
- `consultarOrden()`: Detalle completo de una orden activa por su número.
- `modificarOrden()`: Modificación in-place de MD, MOD, CIF y unidades con recálculo automático del costo unitario. Requiere confirmación (S/N).
- `eliminarOrden()`: Baja lógica (`activo = 0`) in-place de una orden de producción. Requiere confirmación (S/N).
- `submenuCostos()`: Menú interactivo del módulo.

### 4. Módulo de Reportes Automatizados (`reportes.h` / `reportes.cpp`)
- `generarBalanceComprobacion()`: Clasificación automática según tipo normalizado (Activo, Gasto, Costo $\to$ Debe; Pasivo, Capital, Ingreso $\to$ Haber), ignorando registros inactivos y verificando el cuadre general.
- `generarEstadoResultados()`: Filtrado de ingresos operacionales contra costos y gastos operacionales, calculando la Utilidad o Pérdida Neta del período.
- `generarHojaCostos()`: Detalle de elementos de costos y unidades producidas por orden activa.

## Requisitos

- Compilador C++ (`g++`) compatible con C++98, C++11, C++14 o C++17.
- Sin dependencias externas — solo biblioteca estándar de C++.

## Cómo compilar y ejecutar

### Linux
~~~bash
g++ -Wall -Wextra -pedantic -std=c++11 main.cpp catalogo.cpp diario.cpp costos.cpp reportes.cpp -o fincost
./fincost
~~~

*También es compatible con estándares anteriores y posteriores:*
~~~bash
# C++98
g++ -Wall -Wextra -pedantic -std=c++98 main.cpp catalogo.cpp diario.cpp costos.cpp reportes.cpp -o fincost

# C++17
g++ -Wall -Wextra -pedantic -std=c++17 main.cpp catalogo.cpp diario.cpp costos.cpp reportes.cpp -o fincost
~~~

### Windows

#### Opción 1: En Dev-C++ (Recomendado para entorno gráfico)
1. Abre el archivo de proyecto [`FinCost.dev`](FinCost.dev) directamente en Dev-C++ (`Archivo -> Abrir Proyecto o Archivo...`).
2. Presiona **F11** (*Compilar y Ejecutar*) o ve al menú `Ejecutar -> Compilar y Ejecutar`.
> **Nota importante:** En Dev-C++ **NO** abras únicamente `main.cpp` con F9, ya que al ser un sistema modular de 5 archivos `.cpp`, requiere compilarse como **Proyecto** para que el enlazador vincule todos los módulos.

#### Opción 2: Mediante consola (MinGW, MSYS2 o CMD)
~~~cmd
g++ -Wall -Wextra -pedantic -std=c++11 main.cpp catalogo.cpp diario.cpp costos.cpp reportes.cpp -o fincost.exe
fincost.exe
~~~

## Estructura del proyecto

~~~text
FinCost-CPP/
├── FinCost.dev                # Archivo de proyecto listo para Dev-C++ (Windows)
├── estructuras.h              # Registros base (Cuenta, Asiento, OrdenCosto) con campo activo
├── main.cpp                   # Menú principal e integración segura con EOF
├── catalogo.h/.cpp            # Módulo 1 - Catálogo de Cuentas (CRUD + integridad)
├── diario.h/.cpp              # Módulo 2 - Libro Diario (CRUD + partida doble)
├── costos.h/.cpp              # Módulo 3 - Costos Industriales (CRUD + costo unitario)
├── reportes.h/.cpp            # Módulo 4 - Reportes Financieros y Costos
├── cuentas.dat                # Archivo binario de cuentas
├── diario.dat                 # Archivo binario de libro diario
├── costos.dat                 # Archivo binario de órdenes de costo
└── README.md
~~~

## Reglas de Negocio y Bajas Lógicas

1. **Baja lógica:** La eliminación de registros nunca destruye físicamente bytes del archivo; actualiza el campo `activo = 0` directamente en su posición de disco mediante `fstream` en modo lectura/escritura (`ios::in | ios::out | ios::binary`) con posicionamiento por `seekp()`.
2. **Consultas y reportes:** Ignoran sistemáticamente cualquier registro con `activo == 0`.
3. **Unicidad histórica:** Un código de cuenta o número de orden que haya sido dado de baja lógica no puede ser reutilizado, previniendo colisiones de auditoría histórica.
4. **Integridad referencial:** Una cuenta contable no puede eliminarse si tiene asientos contables activos registrados en `diario.dat`.

## Equipo y asignaciones

| Integrante | Módulo Asignado | Carné |
|---|---|---|
| Nelson Alberto Arevalo Guardado | Catálogo Contable | 0900-26-969 |
| Clisman Emanuel López Lajpop | Libro Diario | 0900-26-21859 |
| Joseph Alain Mendez Mendez | Costos Industriales | 0900-26-1674 |
| José Francisco González Ordoñez | Reportes, Estructura e Integración | 0900-26-562 |

## Estado del proyecto

- [x] Definición de estructuras y arquitectura binaria con soporte para baja lógica (`int activo`).
- [x] CRUD completo del Módulo 1 (Catálogo Contable) con integridad referencial.
- [x] CRUD completo del Módulo 2 (Libro Diario) con validación estricta de partida doble y fecha.
- [x] CRUD completo del Módulo 3 (Costos Industriales) con recálculo de costo unitario.
- [x] Módulo 4 (Reportes Automatizados) con filtrado de registros inactivos y clasificación contable corregida.
- [x] Cero advertencias con `-Wall -Wextra -pedantic` en C++98, C++11 y C++17.
- [x] Protección completa contra bucle infinito ante EOF (Ctrl+D / pipe cerrado).

---
*Proyecto académico — Curso de Algoritmos, Universidad Mariano Gálvez de Guatemala.*