Pathfinder
==========

Este archivo define la clase PathFinder, responsable de calcular rutas óptimas entre dos puntos en el mundo del juego utilizando el algoritmo A\* (A-Star).

Clase PathFinder
----------------

Clase utilitaria para la navegación inteligente de entidades. Utiliza un sistema de coordenadas basado en metros (1:1 con las celdas del terreno) pero opera con coordenadas flotantes para permitir precisión de movimiento en las celdas, basicamente, no dar saltos extraños

### Estructuras Privadas

*   **struct Node**: Representa un nodo en el grafo de búsqueda (una celda del mapa). Almacena su posición, costos (g, h, f) y un puntero al nodo padre para reconstruir el camino.
    

### Métodos Privados

*   **float calculate\_h\_cost(sf::Vector2i from, sf::Vector2i to)**
    
    *   **Descripción:** Calcula la heurística (H) utilizando la distancia Euclidiana entre dos puntos. Sirve para estimar el costo restante hasta la meta.
        
*   **std::vector reconstruct\_path(Node\* end\_node)**
    
    *   **Descripción:** Toma el nodo final alcanzado y rastrea los punteros parent hacia atrás hasta el inicio para construir el camino. Convierte las coordenadas de grilla a coordenadas de mundo (centradas en la celda) y devuelve el vector ordenado desde el inicio hasta el fin.
        

### Métodos Públicos

*   **std::vector find\_path(const sf::Vector2f& start, const sf::Vector2f& end, const EntityTerrain& terrain)**
    
    *   **Descripción:** _Núcleo del algoritmo A._\*
        
    *   **Funcionalidad:** Recibe coordenadas de inicio y fin en el mundo. Ejecuta la búsqueda A\* considerando los costos de movimiento (effort) definidos en el terrain. Devuelve una lista de puntos (waypoints) que el organismo debe seguir para llegar al destino evitando obstáculos costosos.