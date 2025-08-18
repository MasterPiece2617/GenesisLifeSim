// Texto provcional
En resumen esta clase tiene los atributos y métodos principales: 

- [`scene`](scene.md), el cual es el contenedor de todas las entidades y componentes que definen un contexto activo, puede ser la simulación en sí o un menú principal, solo una [`Scene`](scene.md) puede estar activa a la vez.
- `window`, que es la ventana donde se renderiza la simulación. 
- El bucle principal se ejecuta en el método `run()` de la clase `Engine`, este método se llama desde [main.cpp](main.md).