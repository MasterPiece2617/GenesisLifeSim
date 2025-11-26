Population Stats
================

Este archivo define la clase estática PopulationStats, que actúa como el "cerebro de datos" de la simulación. Su responsabilidad principal es centralizar, procesar y almacenar todas las estadísticas biológicas y demográficas de los organismos en tiempo real para su posterior visualización.

Clase PopulationStats
---------------------

Clase estática que mantiene el estado global de la población (conteos, promedios de atributos) y su evolución histórica.

### Métodos Públicos Estáticos

*   **static void init(std::shared\_ptr window)**
    
    *   **Descripción:** Inicializa el sistema de estadísticas suscribiéndose a los eventos relevantes del motor (como ORGANISM\_BORN y ORGANISM\_DIED) para mantener los contadores actualizados automáticamente.
        
*   **static void reset()**
    
    *   **Descripción:** Reinicia toda la simulación estadística. Pone a cero todos los contadores (población, tiempo) y limpia todos los vectores de historial, dejando el sistema listo para una nueva ejecución.
        
*   **static void update\_history(float delta)**
    
    *   **Descripción:** **Método crítico de sincronización.**
        
    *   **Funcionalidad:** Se llama una vez por ciclo de actualización de datos (ej. cada 0.1s). Su función es tomar una snapshot de **todas** las variables actuales (número de organismos, promedios de velocidad, visión, tamaño, etc.) y guardarlas simultáneamente en sus respectivos vectores históricos.
        
    *   **Importancia:** Al actualizar todos los vectores en el mismo instante y avanzar el reloj maestro, garantiza que todas las gráficas (Población, Velocidad, Visión) estén perfectamente alineadas en el tiempo, evitando desincronizaciones visuales.
        
*   **static float get\_time()**
    
    *   **Descripción:** Retorna el tiempo actual acumulado de la simulación (el reloj maestro).
        

### Getters de Datos (Para ImPlot)

Estos métodos devuelven referencias constantes a los vectores de datos históricos, listos para ser graficados.

*   **get\_time\_history()**: Vector del eje X (Tiempo) compartido por todas las gráficas.
    
*   **get\_total\_organisms\_history()**: Historial de población total.
    
*   **get\_carnivores\_history() / get\_herbivores\_history()**: Historiales de población por categoría.
    
*   **get\_organisms\_vision\_data() / get\_organisms\_speed\_data()**: Historiales de promedios globales de atributos.
    
*   **get\_carnivores\_... / get\_herbivores\_...**: Historiales de promedios de atributos filtrados por tipo de alimentación.