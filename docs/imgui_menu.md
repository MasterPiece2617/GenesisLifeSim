ImGui Menu
==========

Este archivo define las clases estáticas ImGuiMenu e ImPlotMenu, encargadas de centralizar toda la lógica de la interfaz de usuario (UI) y la visualización de datos mediante gráficos en tiempo real.

Clase ImGuiMenu
---------------

Gestor principal de las ventanas de configuración, control y menús contextuales de la simulación.

### Métodos Públicos Estáticos

*   **static void show\_menu(...)**
    
    *   **Descripción:** Punto de entrada principal ("Hub") de la interfaz durante la simulación activa. Orquesta la renderización de sub-menús y herramientas según el estado actual del motor.
        
*   **static void show\_select\_map\_window(...)**
    
    *   **Descripción:** Renderiza la ventana inicial de selección de escenario. Permite al usuario elegir un archivo de mapa (.zadat) existente o acceder al creador de mapas.
        
*   **static void show\_map\_creator()**
    
    *   **Descripción:** Muestra la interfaz del Generador Procedural de Terrenos. Ofrece controles para ajustar parámetros de ruido (seed, frecuencia, octavas) y guardar el resultado en disco.
        
*   **static void show\_organism\_config\_window(...)**
    
    *   **Descripción:** Panel de configuración biológica. Permite ajustar los atributos base (velocidad, visión, color) para los nuevos organismos y activar/desactivar un modo para colocarlos con click izquierdo.
        
*   **static void call\_reset\_simulation(...)**
    
    *   **Descripción:** Función auxiliar que conecta el botón de reinicio de la UI con la lógica interna del motor para limpiar y reiniciar el estado global.
        

Clase ImPlotMenu
----------------

Clase especializada (hereda de ImGuiMenu) para la renderización de gráficos estadísticos de alto rendimiento utilizando la librería **ImPlot**. Se conecta directamente con PopulationStats para obtener los datos.

### Métodos Públicos Estáticos

*   **static void organism\_population\_plot()**
    
    *   **Descripción:** Dibuja el gráfico lineal de la población total a lo largo del tiempo.
        
*   **static void carnivore\_herbivore\_plot()**
    
    *   **Descripción:** Dibuja un gráfico comparativo con múltiples líneas (Carnívoros vs Herbívoros vs Total) para visualizar la dinámica depredador-presa.
        
*   **static void organism\_stats\_plot()**
    
    *   **Descripción:** Renderiza gráficos evolutivos que muestran el promedio de los atributos genéticos (Velocidad, Visión) de la población viva, permitiendo observar la adaptación de las especies en tiempo real.