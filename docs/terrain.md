Terrain
=======

Este archivo define la clase Terrain, que se encarga de la gestión de los datos geográficos del mundo simulado. Funciona como una estructura de datos de bajo nivel que almacena información sobre cada celda del mapa.

Clase Terrain
-------------

Clase base que gestiona la cuadrícula de celdas (CellData). Es responsable de cargar mapas desde archivos binarios y proveer acceso rápido a las propiedades de cada ubicación.

### Métodos de Carga

*   **bool load\_from\_file(const std::string &filename)**
    
    *   **Descripción:** Carga un mapa previamente generado (formato .zadat) desde el disco.
        
    *   **Proceso:**
        
        1.  Abre el archivo binario.
            
        2.  Lee las dimensiones (width, height).
            
        3.  Redimensiona el vector interno cells.
            
        4.  Lee el bloque de memoria cruda de las celdas directamente al vector.
            
    *   **Retorno:** true si la carga fue exitosa, false si hubo errores (archivo no encontrado, corrupto o vacío).
        

### Métodos de Acceso y Modificación

*   **CellData get\_cell(uint16\_t x, uint16\_t y) const**
    
    *   **Descripción:** Retorna la estructura de datos completa de una celda específica. Lanza una excepción si las coordenadas están fuera de rango.
        
*   **void set\_cell(uint16\_t x, uint16\_t y, const CellData& cell)**
    
    *   **Descripción:** Modifica los datos de una celda específica.
        

### Consultas de Propiedades (Helpers)

Estos métodos facilitan la consulta de atributos específicos sin tener que extraer toda la estructura CellData. Todos incluyen validación de límites (retornan valores seguros si se consulta fuera del mapa).

*   **float get\_effort(uint16\_t x, uint16\_t y) const**
    
    *   **Descripción:** Retorna el costo de movimiento (peso) de la celda.
        
    *   **Uso:** Utilizado por el Pathfinder para calcular la ruta más eficiente.
        
*   **bool walkable(uint16\_t x, uint16\_t y) const**
    
    *   **Descripción:** Indica si una entidad terrestre puede caminar sobre la celda.
        
*   **bool navigable(uint16\_t x, uint16\_t y) const**
    
    *   **Descripción:** Indica si la celda es agua navegable (útil para futuras entidades acuáticas).
        
*   **bool plantable(uint16\_t x, uint16\_t y) const**
    
    *   **Descripción:** Indica si el terreno es fértil para el crecimiento de vegetación (árboles, comida).
        

### Getters de Dimensiones

*   **uint16\_t get\_width() const**
    
*   **uint16\_t get\_height() const**