## Estructura básica del programa

## loop

El programa usa un loop principal que se ejecuta continuamente hasta que se cierra la ventana. Dentro de este bucle, se manejan los eventos, se actualiza el estado del juego y se renderiza la pantalla,
este bucle principal se encuentra en los archivos [engine_loop.hpp/engine_loop.cpp](engine_loop.md), los cuales definen la clase `Engine`. 

En resumen esta clase tiene los atributos y métodos principales: 

- [`scenario`](scenario.md), el cual es el contenedor de todas las entidades y componentes que definen un contexto activo, puede ser la simulación en sí o un menú principal, solo un `Scenario` puede estar activo a la vez.
- `window`, que es la ventana donde se renderiza la simulación. 
- El bucle principal se ejecuta en el método `run()` de la clase `Engine`, este método se llama desde [main.cpp](main.md).

## Componentes y entidades

El programa utiliza un sistema de componentes y entidades para definir el comportamiento de los objetos en la simulación. Cada entidad puede tener múltiples componentes que definen su apariencia, comportamiento y estado.
Las entidades son representadas por la clase `Entity`, que se encuentra en [entity.hpp/entity.cpp](entity.md) y los componentes por la clase `Component`, alojada en [component.hpp/component.cpp](component.md).
Ambas clases están diseñadas para ser heredadas, lo que permite crear entidades y componentes personalizados según las necesidades la simulación.

## Componentes básicos

## Transform

[`Transform`](transform.md) es un componente que define la posición, rotación y escala de una entidad en el espacio 2D. Este componente está presente en todas las entidades, ya que es escencial para representar la jerarquía de objetos, incluso si no se renderiza en pantalla.

## Render (no ha sido creado)
[`Render`](render.md) es un componente que define cómo se renderiza una entidad en la pantalla. Este componente puede contener información sobre la textura, el color y otros aspectos visuales de la entidad. Es opcional, ya que algunas entidades pueden no necesitar ser renderizadas.

## Manejo de eventos (Aún por decidir)