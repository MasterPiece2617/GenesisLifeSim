Behaviour Tree
==============

Este archivo implementa un sistema de **Árbol de Comportamiento (Behaviour Tree)** modular y extensible para gestionar la inteligencia artificial (IA) de las entidades. Permite definir comportamientos complejos combinando nodos simples de control y ejecución.

Conceptos Básicos
-----------------

*   **Nodos (Nodes):** Unidades básicas de lógica. Cada nodo procesa una tarea y devuelve un estado (BTStatus).
    
*   **Estados:**
    
    *   SUCCESS: La tarea se completó correctamente.
        
    *   FAILURE: La tarea falló.
        
    *   RUNNING: La tarea está en proceso (necesita más ticks para completarse).
        

Clases Principales
------------------

### Node (Clase Base)

Clase abstracta que define la interfaz común para todos los nodos del árbol.

*   **Métodos:**
    
    *   tick(): Ejecuta la lógica del nodo (se debe sobrescribir).
        
    *   add\_child(...): Agrega sub-nodos hijos.
        

### ControlNode (Clase Base Abstracta)

Nodo intermedio diseñado para gestionar el flujo de ejecución de sus hijos (ej. Secuencias, Selectores). Mantiene un iterador interno para recordar qué hijo se está ejecutando.

### ExecutionNode (Hoja)

Nodo final que ejecuta una acción concreta (definida mediante una std::function). Es el "músculo" del árbol.

*   **Uso:** Moverse, Comer, Atacar.
    

Tipos de Nodos de Control
-------------------------

### SequenceNode (AND)

Ejecuta sus hijos en orden secuencial.

*   **Éxito:** Si **todos** los hijos devuelven SUCCESS.
    
*   **Fallo:** Si **uno** de los hijos devuelve FAILURE (se detiene inmediatamente).
    
*   **Running:** Si el hijo actual devuelve RUNNING.
    

### FallbackNode (Selector / OR)

Ejecuta sus hijos en orden hasta que uno tenga éxito.

*   **Éxito:** Si **uno** de los hijos devuelve SUCCESS (se detiene inmediatamente).
    
*   **Fallo:** Si **todos** los hijos devuelven FAILURE.
    
*   **Uso:** Prioridades (ej. Si hay peligro -> Huir; Si no -> Comer).
    

### PararellNode

Ejecuta todos sus hijos simultáneamente (en el mismo tick).

*   **Éxito:** Si un número determinado de hijos (success\_threshold) devuelve SUCCESS.
    

### DecoratorNode

Nodo especial que tiene un solo hijo y modifica su resultado.

*   **Políticas:**
    
    *   INVERT: Convierte SUCCESS en FAILURE y viceversa.
        
    *   REPEAT: Repite la ejecución del hijo (ej. hasta que falle).
        

Clase BehaviourTree
-------------------

Es el motor que ejecuta la estructura.

*   **Método tick():** Recorre el árbol desde la raíz utilizando una **pila (stack)** en lugar de recursión. Esto optimiza el rendimiento y evita desbordamientos de pila en árboles muy profundos. Gestiona el estado de los nodos de control (running, reset) automáticamente.