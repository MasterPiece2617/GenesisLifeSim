Organism
========

Este archivo define la lógica biológica y de comportamiento de las entidades vivas en la simulación. Es el núcleo de la IA y la gestión de estados vitales.

Clase Organism
--------------

Representa la entidad biológica en sí misma. Hereda de Entity y actúa principalmente como un contenedor de datos y estado.

### Características Principales

*   **Stats (Estructura de Datos):** Almacena todos los atributos genéticos y variables del organismo:
    
    *   vision: Radio de percepción en metros.
        
    *   speed: Velocidad de movimiento en m/s.
        
    *   hunger / max\_hunger: Energía metabólica.
        
    *   size / weight: Determinan el consumo de energía.
        
    *   color: Representación visual genética.
        
*   **Gestión de Estado:** Mantiene un string descriptivo (state) útil para depuración (ej. "Wandering", "Eating", "Mating") y flags booleanos como is\_resting.
    
*   **Inicialización:** En init(), configura automáticamente su representación visual (SpriteRenderer) y su cerebro (Behaviour).
    

Clase Behaviour (Componente de IA)
----------------------------------

Este componente contiene la lógica de toma de decisiones utilizando un **Behaviour Tree (Árbol de Comportamiento)**. Define _qué_ hace el organismo y _cómo_ lo hace.

### Lógica del Ciclo de Vida (update)

1.  **Tick del Árbol:** Ejecuta la lógica de decisión en cada frame.
    
2.  **Metabolismo:** Reduce el nivel de hambre basándose en una tasa basal (k\_basal), el tamaño del organismo y la velocidad de simulación.
    
3.  **Muerte:** Si hunger <= 0, el organismo es eliminado de la escena.
    

### Acciones del Árbol de Comportamiento

Las acciones se definen como funciones lambda encapsuladas en nodos del árbol:

#### 1\. move (Navegación y Física)

Es el motor de movimiento.

*   **Pathfinding Híbrido:**
    
    *   Si existe una ruta A\* (path), se mueve nodo a nodo hacia el objetivo.
        
    *   Si no (deambular), se mueve vectorialmente hacia goal.
        
*   **Consumo de Stamina:** Moverse cuesta energía (stamina). Si se agota, el organismo entra en estado de reposo forzado hasta recuperarse.
    
*   **Interacción con Terreno:** Verifica colisiones y ajusta la velocidad según el esfuerzo del terreno (lodo, agua, pasto).
    

#### 2\. hunger (Condición)

Nodo de control que actúa como "gatillo". Verifica si el organismo tiene suficiente hambre para justificar la búsqueda de comida. Si está satisfecho, devuelve SUCCESS y evita que se ejecute la búsqueda de alimento.

#### 3\. look\_food (Sensores y Búsqueda)

Gestiona la supervivencia alimentaria.

*   **Persecución:** Si ya tiene un objetivo (fixed\_entity), verifica si está a rango de interacción (1 metro) para comer. Si está lejos, calcula/recalcula la ruta A\*.
    
*   **Escaneo:** Utiliza el sistema de Chunks para buscar entidades de tipo Food dentro del radio de visión.
    
*   **Deambular (Wandering):** Si no hay comida ni objetivos, selecciona un punto aleatorio válido en el mapa (respetando límites y rebotes en bordes) para explorar.
    

#### 4\. reproduction (Genética y Evolución)

Gestiona la continuidad de la especie.

*   **Criterios:** Requiere que el organismo esté bien alimentado, tenga un compañero de la misma especie cerca y que ambos hayan superado su tiempo de espera (cooldown).
    
*   **Herencia Genética con Mutación:** Al nacer un hijo, sus estadísticas son el promedio de los padres más un factor de variación aleatoria.
    
    *   _Tamaño, Velocidad y Visión:_ Mutan ligeramente, permitiendo la selección natural (los más aptos sobreviven).
        
    *   _Color:_ Se mezcla el RGB de los padres.
        
*   **Coste:** La reproducción consume una cantidad significativa de hambre de los padres.
    

Elementos del Entorno
---------------------

Clases auxiliares para el sustento de los organismos:

*   **Food**: Entidad pasiva que aporta valor nutricional (nu) al ser consumida.
    
*   **Tree**: Entidad estática que actúa como punto de generación de recursos.
    
*   **FoodSpawner**: Componente lógico (usado en árboles) que genera instancias de Food periódicamente en áreas plantables.