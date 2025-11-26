Scene
=====

Este archivo define la clase Scene, que actúa como el contenedor principal del mundo del juego. Es responsable de gestionar el ciclo de vida de todas las entidades (Entity), la cámara principal y la optimización espacial mediante un sistema de **Chunks** (cuadrantes).

Clase Scene
-----------

Gestor central de objetos en el mundo. Implementa un patrón Singleton (Scene::instance()) para ser accesible globalmente desde cualquier sistema.

### Gestión Espacial (Spatial Partitioning)

Para evitar iterar sobre todas las entidades en cada cálculo (colisiones, clics), la escena divide el mundo en una cuadrícula de "Chunks".

*   **sf::Vector2i quantize(sf::Vector2f pos)**
    
    *   **Descripción:** Convierte una posición de mundo (Metros) a coordenadas de índice de Chunk.
        
*   **std::vector get\_covered\_chunks(...)**
    
    *   **Descripción:** Calcula qué chunks están siendo ocupados por una entidad basándose en su posición y escala (Bounding Box). Una entidad grande o en un borde puede ocupar múltiples chunks.
        
*   **void update\_entity\_grid(...)**
    
    *   **Descripción:** Actualiza la posición de una entidad en el sistema de chunks. Si una entidad se mueve, este método la elimina de los chunks viejos y la inserta en los nuevos. Se llama automáticamente cuando un Transform se marca como _dirty_.
        
*   **std::vector\> get\_chunk\_entities(sf::Vector2f coords)**
    
    *   **Descripción:** Retorna todas las entidades que residen en el chunk correspondiente a las coordenadas dadas. Vital para el _Mouse Picking_ y optimización de renderizado.
        

### Gestión de Entidades

*   **void add\_entity(std::shared\_ptr entity)**
    
    *   **Descripción:** Registra una nueva entidad en la simulación. La añade a la lista global, la inserta en el sistema de chunks y llama a su método start().
        
*   **bool remove\_entity(std::shared\_ptr entity)**
    
    *   **Descripción:** Elimina una entidad de la simulación.
        
    *   **Efectos colaterales:**
        
        *   Si la entidad es un Organism, publica automáticamente el evento ORGANISM\_DIED para las estadísticas.
            
        *   Limpia las referencias en los chunks y marca la entidad para borrado seguro.
            
*   **void spawn\_organisms(sf::Vector2f position\_meters, OrganismConfig& config)**
    
    *   **Descripción:** Generador de organismos en lote. Crea múltiples entidades alrededor de un punto específico con una pequeña dispersión aleatoria.
        

### Ciclo de Vida de la Escena

*   **Scene()**
    
    *   **Descripción:** Constructor. Inicializa la cámara principal.
        
*   **void load(const OrganismConfig& config)**
    
    *   **Descripción:** Inicializa el escenario. Actualmente se encarga de la generación procedimental de vegetación (árboles) verificando las zonas plantables del terreno.
        
*   **void clear()**
    
    *   **Descripción:** Reinicia la escena por completo.
        
    *   **Acciones:** Borra todas las entidades y chunks, reinicia contadores de IDs y restablece la velocidad del tiempo. Recrea la cámara principal para evitar referencias nulas.