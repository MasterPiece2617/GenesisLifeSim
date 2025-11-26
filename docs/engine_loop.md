Engine
------

Este archivo define la clase Engine, que actúa como el núcleo central de la aplicación. Es responsable de orquestar el ciclo de vida del juego, la gestión de ventanas, la entrada del usuario y la actualización de la simulación.

Clase Engine
------------

Controlador principal que inicializa y mantiene el bucle de juego.

### Métodos Públicos

*   **Engine()**
    
    *   **Descripción:** Constructor. Inicializa la ventana de renderizado (sf::RenderWindow), configura ImGui, suscribe los eventos principales (cerrar ventana, clics del mouse) e inicializa los gestores de estadísticas y entrada.
        
*   **~Engine()**
    
    *   **Descripción:** Destructor. Se encarga de la limpieza de recursos al cerrar la aplicación.
        
*   **void run()**
    
    *   **Descripción:** Punto de entrada del bucle principal del juego.
        
    *   **Funcionalidad:** Mantiene la ventana abierta, gestiona el tiempo (Time::update), procesa la lógica de actualización (update) y renderizado (render) en cada frame. También coordina la transición entre el menú de selección de mapa y la simulación activa.
        
*   **void update()**
    
    *   **Descripción:** Ejecuta la lógica de actualización de cada fotograma.
        
    *   **Funcionalidad:** Procesa la entrada del usuario (InputManager), gestiona eventos de ventana (poll events), actualiza todas las entidades activas en la escena y sincroniza la cámara.
        
*   **void render()**
    
    *   **Descripción:** Se encarga de dibujar todos los elementos visuales en la ventana.
        
    *   **Funcionalidad:** Limpia la pantalla, dibuja el terreno, renderiza las entidades por capas (optimizando con batching), dibuja la interfaz de usuario (ImGui) y finalmente muestra el frame (display).
        
*   **void reset\_simulation(std::shared\_ptr& terrain, bool& map\_loaded, std::string& selected\_map, bool& centered)**
    
    *   **Descripción:** Restablece la simulación a su estado inicial.
        
    *   **Funcionalidad:** Limpia la escena actual, resetea las estadísticas de población, descarga el terreno y restablece la cámara a su posición por defecto, permitiendo al usuario cargar un nuevo mapa desde cero.