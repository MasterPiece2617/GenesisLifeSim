Camera
======

Este archivo define las clases Camera y CameraController, encargadas de gestionar la vista (View) en la simulación. La cámara hereda de Entity, permitiendo que tenga una posición (Transform) dentro de la escena.

Clase Camera
------------

Es la representación de la "ventana al mundo". Gestiona internamente un sf::View de SFML, del cual se aprovecha para hacerse más modular.

### Métodos Públicos

*   **Camera(std::string \_name)**
    
    *   **Descripción:** Constructor. Inicializa la vista con el tamaño de la ventana definido en Config.
        
*   **void init()**
    
    *   **Descripción:** Método de inicialización que añade automáticamente el componente CameraController a la entidad.
        
*   **void set\_zoom(float new\_zoom)**
    
    *   **Descripción:** Aplica un factor de zoom multiplicativo al nivel de zoom actual.
        
    *   **Parámetros:** new\_zoom (Factor de escala, ej. 0.9 para acercar, 1.1 para alejar).
        
*   **float get\_zoom() const**
    
    *   **Descripción:** Retorna el nivel de zoom acumulado actual.
        
*   **sf::View& get\_view()**
    
    *   **Descripción:** Retorna una referencia al objeto sf::View interno para ser usado por la ventana de renderizado (window->setView()).
        

Clase CameraController
----------------------

Componente encargado de la lógica de movimiento y control de entrada para la cámara.

### Métodos Públicos

*   **CameraController(std::weak\_ptr \_owner, float \_speed = 200)**
    
    *   **Descripción:** Constructor. Configura la velocidad base de movimiento.
        
*   **void update()**
    
    *   **Descripción:** Se ejecuta en cada frame.
        
    *   **Funcionalidades:**
        
        1.  **Zoom:** Lee el scroll del mouse desde InputManager y ajusta el zoom.
            
        2.  **Movimiento (WASD):** Lee el teclado y traslada la posición de la entidad cámara. La velocidad se ajusta dinámicamente según el nivel de zoom para mantener la sensación de control constante.