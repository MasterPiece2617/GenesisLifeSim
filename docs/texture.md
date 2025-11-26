Sistema de Texturas
===================

Este archivo define las clases Atlas y Texture, las cuales implementan un sistema de gestión de recursos gráficos centralizado. Su objetivo principal es optimizar el rendimiento mediante la técnica de **Texture Atlas**, permitiendo que múltiples sprites compartan una única textura en memoria de video (VRAM), lo cual es indispensable para el _Batch Rendering_.

Clase Atlas
-----------

Es el "motor" del sistema. Se encarga de cargar imágenes individuales desde el disco y fusionarlas programáticamente en una única gran textura.

### Constructor Atlas()

Al instanciarse, realiza el siguiente proceso automático:

1.  **Escaneo:** Recorre el directorio resources/textures buscando archivos .png.
    
2.  **Cálculo de Dimensiones:** Determina el tamaño necesario para la textura final basándose en la cantidad de imágenes encontradas y el tamaño de celda estándar (Constants::px\_mt).
    
3.  **Generación de Textura de Error:** Crea una textura de "tablero de ajedrez" (Magenta/Negro) en la posición (0,0) para usarla como respaldo si se solicita una textura inexistente.
    
4.  **Empaquetado (Packing):** Copia píxel por píxel cada imagen cargada en la gran textura (sf::Texture global), organizándolas en una cuadrícula.
    
5.  **Mapeo:** Registra el nombre de cada archivo (sin extensión) asociado a su región rectangular (sf::IntRect) en el mapa regions.
    

### Métodos Públicos

*   **const std::unordered\_map& get\_regions()**
    
    *   **Descripción:** Retorna el mapa que asocia nombres de texturas (IDs) con sus coordenadas dentro del atlas.
        
*   **const sf::Texture& get\_atlas()**
    
    *   **Descripción:** Retorna la textura gigante generada. Esta es la única textura que se necesita vincular (bind) en la GPU para dibujar todo el juego.
        

Clase Texture
-------------

Es una clase estática (Wrapper) que facilita el acceso global al Atlas sin necesidad de pasar punteros por todo el código.

### Métodos Públicos Estáticos

*   **static sf::IntRect get\_region(const std::string& name)**
    
    *   **Descripción:** Busca las coordenadas de textura para un nombre dado (ej. "grass", "organism").
        
    *   **Comportamiento:**
        
        *   Si la textura existe, devuelve su sf::IntRect.
            
        *   Si no existe, devuelve la región de la "textura de error" y muestra un aviso en consola.
            
        *   _Lazy Initialization:_ Si el Atlas no existe aún, lo crea automáticamente.
            
*   **static const sf::Texture& get\_atlas()**
    
    *   **Descripción:** Provee acceso global a la textura maestra del sistema.