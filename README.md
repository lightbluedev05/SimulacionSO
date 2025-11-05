# 🧠 Simulación de Planificación de Procesos – Sistemas Operativos

## 👥 Equipo de Desarrollo
| Integrante |
|-------------|
| **Solis Cunza, Miguel Alonso** |
| **Matamoros Laura, Ricardo José** |
| **Cristobal Rojas, Mihael Jhire** |
| **Montes Ramos, Carol Sofía** |
| **Arroyo Tapia, Luis Enrique** |

---

## 📘 Descripción General
Este proyecto implementa un **simulador de planificación de procesos**, cuyo objetivo es analizar cómo diferentes **algoritmos de planificación** impactan el rendimiento y la equidad del sistema operativo.  
A través de múltiples escenarios y métricas, se busca comprender el comportamiento de cada algoritmo frente a diversas cargas de trabajo.

---

## 🎯 Objetivo
Comprender cómo los distintos **algoritmos de planificación** afectan el rendimiento global y la equidad en la ejecución de procesos dentro de un sistema operativo simulado.

---

## ⚙️ Algoritmos Implementados
El simulador incluye los siguientes algoritmos de planificación:

| Algoritmo | Descripción |
|------------|-------------|
| **FCFS (First Come, First Served)** | Los procesos se ejecutan en el orden en que llegan. |
| **SJF (Shortest Job First)** | Se priorizan los procesos con menor tiempo de ráfaga. |
| **Round Robin (RR)** | Cada proceso recibe una fracción de CPU (quantum) de forma cíclica. |
| **Prioridad Estática** | Los procesos se ordenan según una prioridad fija asignada al inicio. |
| **Prioridad Dinámica** | La prioridad puede variar según el comportamiento del proceso (por ejemplo, penalizando a los CPU-bound). |

---

## 🧩 Implementación Técnica
El simulador fue desarrollado en **C++**, empleando una arquitectura modular que separa los distintos algoritmos y componentes del sistema:

```markdown

include/
├── fcfs.h
├── sjf.h
├── rr.h
├── s_priority.h
├── d_priority.h
└── scheduler.h

src/
├── fcfs.cpp
├── sjf.cpp
├── rr.cpp
├── s_priority.cpp
├── d_priority.cpp
└── scheduler.cpp

```

Cada algoritmo maneja su propia **cola de procesos**, los cuales pueden llegar en distintos tiempos y con diferentes duraciones.  
Los resultados se registran en archivos `.csv` para su posterior análisis.

---

## 🧪 Escenarios de Prueba
Para evaluar el rendimiento de los algoritmos, se consideraron los siguientes escenarios:

1. 🔹 **Mezcla de procesos cortos y largos.**  
2. 🔹 **Procesos CPU-bound vs I/O-bound.**  
3. 🔹 **Alta concurrencia vs baja concurrencia.**

---

## 📊 Métricas de Evaluación
El desempeño de cada algoritmo se mide utilizando las siguientes métricas:

| Métrica | Descripción |
|----------|--------------|
| **Turnaround Time** | Tiempo total desde la llegada hasta la finalización del proceso. |
| **Tiempo de Espera** | Tiempo que el proceso pasa esperando en la cola. |
| **Tiempo de Respuesta** | Tiempo desde la llegada hasta la primera ejecución. |
| **Throughput** | Número de procesos completados por unidad de tiempo. |
| **Fairness** | Medida de equidad en la asignación de CPU entre procesos. |

---

## 📁 Estructura del Proyecto
```

SimulacionSO/
├── data/
│   ├── input.csv
│   ├── results.csv
│   ├── FCFS_*.csv
│   ├── SJF_*.csv
│   └── ...
├── include/
├── src/
├── output/
├── dependencies/
│   └── SFML/
└── Playground.sln

````

---

## 🧠 Requisitos
- **Lenguaje:** C++
- **Entorno:** Visual Studio / g++  
- **Librerías:** SFML (para visualización)

---

## ▶️ Ejecución
1. Compilar el proyecto (por ejemplo, en Visual Studio o desde terminal con `g++`).
2. Ejecutar el programa principal:
   ```bash
   ./SFMLPractice

3. Los resultados de cada algoritmo se generarán automáticamente en la carpeta `output/` como archivos `.csv`.

---

## 🧾 Licencia

Este proyecto fue desarrollado con fines **académicos** como parte del curso de **Sistemas Operativos**.

---

## 🌟 Créditos

Proyecto académico 2025 – Facultad de Ingeniería de Sistemas e Informática
Universidad Nacional Mayor de San Marcos 🇵🇪
