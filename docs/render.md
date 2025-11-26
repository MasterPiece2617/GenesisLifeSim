Renderer
========

Este archivo define la clase SpriteRenderer, un componente esencial para la representación visual de las entidades dinámicas (organismos, comida, objetos) en la simulación. Se encarga de transformar la información lógica de la entidad (posición en metros, rotación) en vértices gráficos (píxeles) listos para ser renderizados por SFML.

Clase SpriteRenderer
--------------------

Componente que gestiona la visualización de una entidad mediante un _Sprite_ (quad con textura). Está diseñado para trabajar con un **Texture Atlas** global para optimizar el rendimiento mediante _Batch Rendering_.

### Constructores

*   **SpriteRenderer(...)**: Inicializa el componente asignando la textura desde el atlas global y construyendo la geometría inicial. Permite definir un color de tinte opcional (útil para diferenciar organismos genéticamente distintos).
    

### Métodos Principales

*   **void start()**
    
    *   **Descripción:** Publica el evento RENDERER\_CREATED. Esto notifica al sistema principal que hay un nuevo elemento visible que debe ser rastreado para el renderizado.
        
*   **void set\_texture(std::string \_texture\_id)**
    
    *   **Descripción:** Busca la región específica (sf::IntRect) dentro del Atlas de texturas usando el ID (nombre). Si la textura no existe, asigna una textura de error ("error\_texture") para facilitar la depuración visual.
        
*   **void build\_batch()**
    
    *   **Descripción:** **Método crítico de transformación.** Construye o actualiza el sf::VertexArray (un cuadrado de 4 vértices) que representa a la entidad.
        
    *   **Proceso:**
        
        1.  **Obtención de Transform:** Lee la posición, rotación y escala de la entidad dueña.
            
        2.  **Conversión de Unidades:** Transforma la posición lógica (Metros) a posición de pantalla (Píxeles) multiplicando por Constants::px\_mt.
            
        3.  **Geometría Local:** Crea un quad centrado localmente.
            
        4.  **Transformación de Matriz:** Aplica la rotación y escala de la entidad a los vértices locales para situarlos en el mundo.
            
        5.  **Mapeo UV:** Asigna las coordenadas de textura del Atlas a cada vértice para que el sprite muestre la imagen correcta.
            
*   **sf::VertexArray& get\_batch()**
    
    *   **Descripción:** Devuelve la geometría cruda (vértices) de la entidad.
        
    *   **Uso:** El Engine o Scene llama a este método para recolectar todos los quads de una zona (chunk), combinarlos en un solo buffer y dibujarlos en una sola llamada a la GPU, lo cual es vital para mantener altos FPS con miles de entidades.
        
*   **uint8\_t get\_layer()**
    
    *   **Descripción:** Devuelve el índice de capa (Layer) para asegurar que los objetos se dibujen en el orden correcto (ej. Frutas sobre el suelo, Organismos sobre las frutas).