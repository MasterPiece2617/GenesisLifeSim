# Estructura básica del programa

Este progma está diseñado para simular un entorno 2D utillizando la biblioteca SFML. Usa una arquitectura ECS (Entity-Component Sistem) para organizar el código, facilitar la extensión y mantenimiento del mismo.

## loop

El programa se estrucutura en un loop principal que se ejecuta continuamente hasta que se cierra la ventana. Dentro de este bucle, se manejan los eventos, se actualiza el estado del juego y se renderiza la pantalla, este loop se define en la clase [`Engine`](engine_loop.md). 

## Componentes y entidades

El programa utiliza un sistema de componentes y entidades para definir el comportamiento de los objetos en la simulación. Cada entidad puede tener múltiples componentes que definen su apariencia, comportamiento y estado.
Las entidades son representadas por la clase [`Entity`](entity.md) y los componentes por la clase [`Component`](component.md).
Ambas clases están diseñadas para ser heredadas, lo que permite crear entidades y componentes personalizados según las necesidades la simulaci�n.

## Componentes básicos

### Transform

[`Transform`](transform.md) es un componente que define la posición, rotación y escala de una entidad en el espacio 2D. Este componente está presente en todas las entidades, ya que es escencial para representar la jerarquía de objetos, incluso si no se renderiza en pantalla.

### Render
[`Render`](render.md) es un componente que define cómo se renderiza una entidad en la pantalla. Este componente puede contener información sobre la textura, el color y otros aspectos visuales de la entidad. Es opcional, ya que algunas entidades pueden no necesitar ser renderizadas.

## Manejo de eventos

El motor utiliza un sistema EDA (Event-Driven Architecture). El manejo de eventos se realiza a través de la clase [`EventManager`](event_manager.md), que captura y procesa los eventos generados por la ventana SFML y por las entidades. Los eventos pueden incluir entradas del teclado, 
movimientos del ratón y otros eventos del sistema cómo interacciones entre entidades de la simulación. El [`EventManager`](event_manager.md) distribuye estos eventos a las entidades y componentes que los necesitan para actualizar su estado.