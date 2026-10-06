# 🧾 FinCost C++

**Sistema Unificado de Contabilidad Financiera y Costos**

![C++](https://img.shields.io/badge/C%2B%2B-00599C?style=flat&logo=c%2B%2B&logoColor=white)
![Status](https://img.shields.io/badge/estado-en%20pruebas-yellow)
![Curso](https://img.shields.io/badge/curso-Algoritmos%20UMG-1F3864)

Proyecto Fase I–II del curso de Algoritmos (UMG) — Grupo 7.
Un sistema de consola en C++ que integra la **Contabilidad Financiera** (catálogo de cuentas, libro diario, mayor, balances) con la **Contabilidad de Costos Industriales** (materia prima, mano de obra, CIF), usando registros (`struct`) y archivos binarios como almacenamiento permanente.

## Tabla de contenidos

- [¿Qué hace este sistema?](#qué-hace-este-sistema)
- [Arquitectura modular](#arquitectura-modular)
- [Cómo se conecta todo](#cómo-se-conecta-todo)
- [Requisitos](#requisitos)
- [Cómo compilar y ejecutar](#cómo-compilar-y-ejecutar)
- [Estructura del proyecto](#estructura-del-proyecto)
- [Uso](#uso)
- [Equipo y asignaciones](#equipo-y-asignaciones)
- [Flujo de trabajo en Git](#flujo-de-trabajo-en-git)
- [Estado del proyecto](#estado-del-proyecto)

## ¿Qué hace este sistema?

Simula la operación contable de una empresa: registra la compra de materia prima, calcula el costo de producción (comparando PEPS vs. UEPS), y genera automáticamente los asientos contables correspondientes — sin que el usuario tenga que capturar todo dos veces.

## Arquitectura modular

| Módulo | Descripción | Archivo binario |
|---|---|---|
| **Catálogo Contable** | CRUD del plan de cuentas (Activo, Pasivo, Capital, Costos, Gastos) | `cuentas.dat` |
| **Libro Diario** | Asientos contables con validación de partida doble (ΣDebe = ΣHaber) | `diario.dat` |
| **Costos Industriales** | Órdenes de producción: materia prima + mano de obra + CIF → costo unitario | `costos.dat` |
| **Reportes Automatizados** | Balance de Comprobación, Estado de Resultados, Hoja de Costos | *(solo lectura)* |

Cada registro se define como un `struct` (`Cuenta`, `Asiento`, `OrdenCosto`) y vive en su propio archivo binario, relacionado con los demás por código de cuenta.

## Cómo se conecta todo

~~~text
Compra de Materia Prima
        │
        ▼
Inventario / Kardex (PEPS y UEPS)
        │
        ▼
Consumo en Producción (+ Mano de Obra + CIF)
        │
        ▼
Costo de Producción  ──────►  Diario (automático) ──────►  Mayor ──────►  Balance
~~~

## Requisitos

- Compilador C++ (`g++`) compatible con C++11 o superior
- Sin dependencias externas — solo librería estándar

## Cómo compilar y ejecutar

### Linux
~~~bash
g++ -Wall -Wextra -std=c++11 main.cpp reportes.cpp -o fincost
./fincost
~~~

### Windows
Si tienes Dev-C++ instalado en Windows, puedes compilar y ejecutar el proyecto desde el IDE abriendo los archivos fuente.

Si prefieres usar la terminal (mediante MinGW, WSL o WSL2), ejecuta:
~~~bash
g++ -Wall -Wextra -std=c++11 main.cpp reportes.cpp -o fincost
fincost.exe
~~~

## Estructura del proyecto

~~~text
FinCost-CPP/
├── estructuras.h              # Registros base y estandarización (Núcleo)
├── main.cpp                   # Menú principal e integración
├── catalogo.h/.cpp            # Módulo 1 
├── diario.h/.cpp              # Módulo 2 
├── costos.h/.cpp              # Módulo 3 
├── reportes.h/.cpp            # Módulo 4
├── cuentas.dat                # Se genera al ejecutar
├── diario.dat                 # Se genera al ejecutar
├── costos.dat                 # Se genera al ejecutar
└── README.md
~~~

## Uso

Al ejecutar, el menú principal ofrece:

~~~text
1) Catálogo Contable
2) Libro Diario
3) Costos Industriales
4) Reportes Automatizados
5) Salir
~~~

Cada opción abre un submenú con las operaciones correspondientes.

## Equipo y asignaciones

| Integrante | Módulo Asignado | Carné |
|---|---|---|
| Nelson Alberto Arevalo Guardado | Catálogo Contable | 0900-26-969 |
| Clisman Emanuel López Lajpop | Libro Diario | 0900-26-21859 |
| Joseph Alain Mendez Mendez | Costos Industriales | 0900-26-1674 |
| José Francisco González Ordoñez | Reportes, Estructura e Integración | 0900-26-562 |
| *Todo el equipo* | Pruebas de escritorio y depuración | — |

## Flujo de trabajo en Git

1. Clonar el repositorio.
2. Crear una rama por módulo a partir de `dev` (ej. `git checkout -b ft/jgonzalez`).
3. Hacer commits pequeños y frecuentes, con mensajes claros.
4. Subir la rama (`git push origin ft/nombre-rama`) y avisar al equipo cuando el módulo esté listo.
5. Crear un **Pull Request (PR)** hacia la rama `dev` para pruebas de integración. Las ramas `main` y `dev` están protegidas contra subidas directas.

## Estado del proyecto

- [x] Definición de estructuras y arquitectura binaria.
- [x] Estructura inicial, menú principal y submódulo de reportes automatizados.
- [ ] Desarrollo de módulos CRUD (Catálogo, Diario, Costos).
- [ ] Pruebas de escritorio e integración de los 4 módulos en `dev`.
- [ ] Fusión final a `main` y documentación técnica (manual de usuario).

---
*Proyecto académico — Curso de Algoritmos, Universidad Mariano Gálvez de Guatemala.*