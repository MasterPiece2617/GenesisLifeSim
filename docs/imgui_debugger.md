ImGui Debugger
==============

Este archivo define la clase estática Debugger, la cual actúa como un contenedor de herramientas visuales construidas sobre la librería **Dear ImGui**. Su propósito es proporcionar información en tiempo real sobre el estado interno de la simulación, el terreno y las entidades, facilitando el diagnóstico de errores y el monitoreo de variables.

Clase Debugger
--------------

Clase utilitaria que encapsula funciones estáticas para renderizar ventanas y superposiciones (overlays) de depuración en la interfaz de usuario. Ayuda a comprender "qué está pasando" detrás de la lógica del código.

### Métodos Públicos Estáticos

*   **static void imgui\_terrain(std::shared\_ptr t, std::shared\_ptr window)**
    
    *   **Descripción:** Renderiza una ventana o overlay con información detallada sobre el terreno. Generalmente muestra datos de la celda bajo el cursor del mouse.
        
    *   **Parámetros:**
        
        *   t: Puntero inteligente al terreno actual para acceder a los datos de las celdas (CellData, effort).
            
        *   window: Ventana de renderizado para calcular la posición relativa del mouse.
            
    *   **Uso:** Permite inspeccionar por qué un organismo no cruza cierta zona (verificando si es caminable o su costo de esfuerzo por ejemplo).
        
*   **static void imgui\_scene(float& fps\_display, sf::Vector2f& mouse\_world\_pos)**
    
    *   **Descripción:** Muestra estadísticas generales de la escena y del motor en una ventana flotante.
        
    *   **Parámetros:**
        
        *   fps\_display: Referencia al valor actual de cuadros por segundo (FPS).
            
        *   mouse\_world\_pos: Coordenadas actuales del mouse convertidas al sistema de mundo (Metros/Píxeles).
            
    *   **Uso:** Útil para monitorear el rendimiento y verificar que la conversión de coordenadas del mouse sea correcta.
        
*   **static void show\_selected\_entity(std::shared\_ptr& selected\_entity)**
    
    *   **Descripción:** Renderiza un inspector de propiedades para una entidad específica seleccionada previamente (por ejemplo, mediante un clic).
        
    *   **Parámetros:**
        
        *   selected\_entity: Puntero al organismo seleccionado. Si es nullptr, la función no dibuja nada o muestra "Ninguna selección".
            
    *   **Uso:** Permite ver en tiempo real el estado interno de un organismo: nivel de hambre, estado actual del _Behaviour Tree_ (Deambulando, Comiendo, Persiguiendo), posición exacta, velocidad, etc.