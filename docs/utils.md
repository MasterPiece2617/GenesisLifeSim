Utils
=====

En este archivo se encuentran clases estáticas utilitarias fundamentales para el funcionamiento del motor. Se encarga de la gestión del tiempo (delta time), la velocidad de la simulación y define las constantes globales para la conversión de unidades y dimensionamiento del mundo.

Clase Time
----------

Clase estática encargada de gestionar el reloj del juego. Es vital para lograr un movimiento fluido e independiente de los fotogramas por segundo (FPS).

### Métodos Públicos

*   **static void update()**
    
    *   **Descripción:** Actualiza el reloj interno. Calcula el tiempo transcurrido desde el último frame y lo almacena en delta\_time.
        
    *   **Uso:** Debe llamarse una vez por iteración en el bucle principal (Engine::run), justo antes de la lógica de actualización (Update).
        
*   **static float get\_delta()**
    
    *   **Descripción:** Retorna el tiempo transcurrido entre el frame actual y el anterior (Delta Time) en segundos.
        
    *   **Retorno:** float (segundos).
        
    *   **Uso:** Multiplicar cualquier movimiento o cálculo físico por este valor para asegurar consistencia temporal.
        
*   **static void set\_simulation\_speed(float speed)**
    
    *   **Descripción:** Define un multiplicador global para la velocidad de la simulación.
        
    *   **Parámetros:** speed (1.0 = tiempo real, >1.0 = cámara rápida, <1.0 = cámara lenta).
        
*   **static float get\_simulation\_speed()**
    
    *   **Descripción:** Retorna el multiplicador actual de velocidad de la simulación.
        

### Miembros Protegidos

*   static float simulation\_speed: Almacena el factor de velocidad actual (Default: 1.0).
    
*   static sf::Time delta\_time: Almacena la duración del último frame.
    
*   static sf::Clock clock: Objeto de SFML utilizado para medir el tiempo real.
    

Clase Constants
---------------

Contenedor de valores constantes inmutables utilizados en todo el proyecto para mantener la coherencia en las escalas y dimensiones.

### Constantes Definidas

*   **px\_mt (32)**
    
    *   **Tipo:** int
        
    *   **Significado:** _Pixels per Meter_ (Píxeles por Metro).
        
    *   **Propósito:** Factor de conversión entre el sistema lógico (Metros) y el sistema de renderizado (Píxeles).
        
        *   _Fórmula:_ Pixeles = Metros \* px\_mt.
            
*   **map\_size (4096)**
    
    *   **Tipo:** int
        
    *   **Propósito:** Define el límite máximo o tamaño de referencia para el mundo del juego.
        
*   **chunk\_size (4)**
    
    *   **Tipo:** int
        
    *   **Propósito:** Define el tamaño de los "Chunks" (fragmentos de mapa) para la optimización espacial. Indica cuántas celdas/metros de ancho y alto tiene cada chunk.
        
*   **pi\_val (3.1415927f)**
    
    *   **Tipo:** float
        
    *   **Propósito:** Valor de PI para cálculos trigonométricos (rotaciones, ángulos de visión, etc.).