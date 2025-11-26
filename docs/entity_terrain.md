Entity Terrain
==============

Este archivo define la clase EntityTerrain, la cual actúa como la manifestación física y visual del mapa dentro del motor de juego. Hereda de Entity, lo que le permite existir dentro de la Scene, pero encapsula una instancia de Terrain para gestionar los datos lógicos subyacentes.

Clase EntityTerrain
-------------------

Es el componente encargado de transformar la cuadrícula de datos abstractos en una malla gráfica renderizable. Utiliza técnicas de _Batch Rendering_ para dibujar miles de celdas de manera eficiente.

### Constructor

*   **EntityTerrain(const std::string& map\_from\_file, const std::shared\_ptr& \_atlas)**
    
    *   **Descripción:** Inicializa la entidad cargando los datos binarios del mapa especificado.
        
    *   **Flujo:**
        
        1.  Llama a terrain.load\_from\_file() para llenar la memoria con los datos de las celdas.
            
        2.  Si la carga es exitosa, llama automáticamente a generate\_vertices() para construir la representación visual.
            
        3.  Si falla, lanza una excepción crítica para detener la ejecución segura.
            

### Sistema de Renderizado y Geometría

*   **void generate\_vertices()**
    
    *   **Descripción:** Construye un sf::VertexArray único compuesto por _Quads_ (cuadrados de 4 vértices) para representar todo el mapa.
        
    *   **Mapeo de Texturas:**
        
        *   Itera sobre cada celda lógica del Terrain.
            
        *   Utiliza el mapa interno texture\_id\_map para traducir el ID numérico de la celda (ej. 1) a un nombre de recurso (ej. "grass\_floor").
            
        *   Consulta al Atlas para obtener las coordenadas UV (sf::IntRect) de esa textura.
            
        *   Asigna la posición en píxeles (mundo) y las coordenadas de textura a los 4 vértices correspondientes.
            
*   **void draw(sf::RenderTarget& target, sf::RenderStates states) const**
    
    *   **Descripción:** Sobrescribe el método de dibujo de SFML. Vincula la textura gigante del Atlas y envía el array de vértices completo a la GPU en una sola llamada (_Draw Call_), optimizando drásticamente el rendimiento.
        

### Manipulación Dinámica

*   **void set\_cell(uint16\_t x, uint16\_t y, const CellData& cell)**
    
    *   **Descripción:** Actualiza los datos lógicos de una celda específica y, crucialmente, **recalcula solo los 4 vértices visuales** correspondientes a esa posición. Esto permite modificar el terreno en tiempo real (ej. modo editor) sin regenerar toda la malla.
        

### Métodos Proxy (Acceso a Datos)

La clase expone métodos que redirigen directamente a la instancia interna de Terrain, permitiendo que sistemas externos (como el Pathfinder o la Física) consulten propiedades del mapa a través de la entidad.

*   **walkable(...) / navigable(...) / plantable(...)**: Consultas booleanas de estado.
    
*   **get\_effort(...)**: Consulta de costo de movimiento.
    
*   **get\_width() / get\_height()**: Dimensiones del mapa en celdas.
    

### Utilidades Estáticas

*   **static std::vector get\_map\_files(...)**
    
    *   **Descripción:** Utilidad del sistema de archivos que escanea un directorio y devuelve una lista de todos los archivos de mapa disponibles (por extensión .zadat), usada para poblar menús de selección.