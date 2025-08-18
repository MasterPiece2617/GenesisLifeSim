# Entity

`Entity` es la clase base de todas las entidades del programa, desde las plantas hasta la cámara que renderiza la escena, son derivados de `Entity`. Está pensada para ser una clase base que se puede heredar, por lo que no tiene [`Component`](component.md) 
por defecto, a excepción del [`Transform`](transform.md), que es obligatorio para todas las entidades ya que define la jerarquía de `Entity` en la [`Scene`](scene.md), esto es importante para la propagación de cambios en casos donde la lógica lo requiera.
A pesar de no tener [`Component`](component.md) que definan su comportamiento o apariencia, puede ser útil para crear entidades vacías a las que se le pueden agregar y eliminar [`Component`](component.md) de forma dinámica en casos donde sea más conveniente en vez de
crear una clase heredera nueva.

## Atributos de Entity

Cada `Entity` cuenta con un nombre, `tags`, una lista de [`Component`](component.md), un `bool` que indica si está activo o no y un acceso directo al [`Component`](component.md) [`transform`](transform.md).

```cpp
    std::string name;
    std::vector<std::shared_ptr<Component>> components;
    std::vector<std::string> tags;
    std::shared_ptr<Transform> transform;
    bool is_active = true;
```

## Métodos de Entity

Sus métodos son:

### Constructor
```cpp
    Entity(const std::string& _name);
```
Constructor que recibe un nombre para la entidad. <span style="color:red">NOTA</span>: no usar este constructor directamente, en vez usar [`EntityFactory`](#entityfactory), pero si aún así decides usarla, no olvides inicializarla con `init_transform_()` y luego con
`init()`.
### Inicialización
Estos métodos son necesarios para inicializar la entidad, [`EntityFactory`](#entityfactory) se encarga de llamarlos automáticamente al crear una entidad, pero si decides crear una entidad directamente, debes llamarlos manualmente.
#### `init_transform`
```cpp
    void init_transform();
```
Función que inicializa el [`transform`](transform.md) de la `Entity` con un apuntador de sí mismo `shared_from_this()`, esto es necesario debido a que no es posible usar `shared_from_this()` en el constructor porque el apuntador inteligente no ha sido creado aún.
#### `init`
```cpp
    virtual void init();
```
Función virtual cuya función principal es inicializar los [`Component`](component.md) de la `Entity`, es necesario sobreescribir esta función para agregar los [`Component`](component.md) en las clases derivadas.
### Métodos de name
#### `get_name`
```cpp
    const std::string& get_name() const;
```
Función que devuelve el nombre de la entidad.
#### `set_name`
```cpp
    void set_name(const std::string& _name);
```
Función que establece el nombre de la entidad.
### Métodos de componentes
#### `add_component`
```cpp
    bool add_component(std::shared_ptr<Component> component);
```
Función que añade un componente a la entidad. Devuelve `true` si se ha añadido correctamente, `false` si ya existe un componente con el mismo tipo.
#### `get_component`
```cpp
    template <typename T>
    std::shared_ptr<T> get_component() const;
```
Función que devuelve un componente de tipo `T` de la entidad. Si no existe un componente de ese tipo, devuelve `nullptr`.
#### `get_components`
```cpp
    template <typename T>
    bool remove_component();
```
Función que elimina un componente de tipo `T` de la entidad. Devuelve `true` si se ha eliminado correctamente, `false` si no existe un componente de ese tipo.
### Métodos de impresión
#### `print`
```cpp
    virtual void print() const;
```
Función que imprime información de la entidad, como su nombre y sus [`Component`](component.md). Esta función es virtual, por lo que puede ser sobreescrita en clases derivadas.
#### `print_family`
```cpp
    void print_family() const;
```
Ejemplo:
```terminal
    Entity Name: Test1
    Parent: Padre de test1
    Children:
    Hijo de Test1
```
Función que imprime el nombre de la entidad, así como los nombres de las entidades padre e hijo.
### Métodos de estado
#### `get_is_active`
```cpp
    bool get_is_active() const;
```
Función que devuelve si la entidad está activa o no.
#### `deactivate`
```cpp
    void deactivate();
```
Función que desactiva la entidad, estableciendo `is_active` a `false`.
#### `activate`
```cpp
    void activate();
```
Función que activa la entidad, estableciendo `is_active` a `true`.
### Métodos de transform y jerarquía
#### `get_transform`
```cpp
    Transform& get_transform() const;
```
Función que devuelve el componente [`Transform`](transform.md) de la entidad. Este componente es especial, ya que todas las entidades deben tenerlo, se usa para manejar la posición, rotación y escala de la entidad en el mundo, y también para manejar la jerarquía.
#### `set_parent`
```cpp
    bool set_parent(std::shared_ptr<Entity> _parent);
```
Función que establece la entidad padre de la entidad actual. Devuelve `true` si se ha establecido correctamente, `false` si no ha sido posible.
#### `get_parent`
```cpp
    std::shared_ptr<Entity> get_parent() const;
```
Función que devuelve la entidad padre de la entidad actual. Devuelve un `shared_ptr` a la entidad padre, o `nullptr` si no tiene padre.
#### `get_children`
```cpp
    std::vector<std::shared_ptr<Entity>> get_children() const;
```
Función que devuelve un vector de `shared_ptr` a las entidades hijas de la entidad actual. Si no tiene hijas, devuelve un vector vacío.
#### `add_child`
```cpp
    bool add_child(std::shared_ptr<Entity> child);
```
Función que añade una entidad hija a la entidad actual. Devuelve `true` si se ha añadido correctamente, `false` si no ha sido posible.
#### `remove_child`
```cpp
    bool remove_child(std::shared_ptr<Entity> child);
```
Función que elimina una entidad hija de la entidad actual. Devuelve `true` si se ha eliminado correctamente, `false` si no existe la entidad hija.
### Métodos de etiquetas
#### `add_tag`
```cpp
    bool add_tag(const std::string& tag);
```
Función que añade una etiqueta a la entidad. Devuelve `true` si se ha añadido correctamente, `false` si ya existe una etiqueta con el mismo nombre.
#### `get_tags`
```cpp
    bool has_tag(const std::string& tag) const;
```
Función que comprueba si la entidad tiene una etiqueta con el nombre dado. Devuelve `true` si la etiqueta existe, `false` en caso contrario.
#### `remove_tag`
```cpp
    bool remove_tag(const std::string& tag);
```
Función que elimina una etiqueta de la entidad. Devuelve `true` si se ha eliminado correctamente, `false` si no existe una etiqueta con el nombre dado.
### Métodos de loop
#### `start`
```cpp
    void start();
```
Función que se llama al iniciar la entidad en la [`Scene`](scene.md). Se usa para iniciar los [`Component`](component.md) al inicio de la [`Scene`](scene.md) o cuando se crea una `Entity` durante la ejecución.
#### `update`

```cpp
    void update();
```
### Destuctor
Función que se llama en cada frame para actualizar la `Entity`, es decir, sus [`Component`](component.md).
```cpp
    virtual ~Entity() = default;
```
Destructor virtual que se encarga de liberar los recursos de la entidad. Es importante que sea virtual para que se llame al destructor de las clases derivadas cuando se destruya una entidad.

## Cómo crear una clase derivada de Entity?

Para crear una clase derivada de `Entity`, debes sobreescribir el método `init()` para inicializar los [`Component`](component.md) que necesites. Aquí tienes un ejemplo de `EntityTest`:

`entity_test.hpp`:
```cpp
    #pragma once

    #include <entity.hpp>
    #include <component_test.hpp>

    class EntityTest : public Entity
    {
    public:

        EntityTest(const std::string& name) : Entity(name) {}

        void init() override
        {
            // Añadir componentes a la entidad
            add_component(std::make_shared<ComponentTest>());
        }

        ~EntityTest() override = default;
    };
```
La clase `EntityTest` hereda de `Entity` y sobreescribe el método `init()` para añadir un componente de prueba `ComponentTest`. Puedes añadir tantos componentes como necesites en este método.

<span style="color:red">NOTA</span>: No se pueden añadir componentes en el constructor, ya que el `shared_from_this()` no está disponible en ese momento, por lo que debes usar el método `init()` para añadir los componentes.
<span style="color:red">NOTA</span>: No sepueden agregar dos componentes del mismo tipo.

También puedes sobreescribir el método `print()` para imprimir información específica de la entidad:

`entity_test.hpp`:
```cpp
    void print() const override
    {
        std::cout << "EntityTest: " << get_name() << std::endl;
    }
```
Resultado:
```cpp
    std::shared_ptr<Entity> entity = std::EntityFactory<EntityTest>::create("Test1");
    entity->print();
```
Terminal:
```terminal
    EntityTest: Test1
```

Es posible acceder al método `print()` de la clase base `Entity` usando `Entity::print()` dentro del método `print()` de la clase derivada, si se desea imprimir también la información de la clase base, aplica para clases derivadas de la clase derivada de `Entity`.

`entity.cpp`:
```cpp
    void Entity::print() const
    {
	    std::cout << "Entity Name: " << name << std::endl;
    }
```
`entity_test.hpp`:
```cpp
    void print() const override
    {
        Entity::print();
        std::cout << "This is an test" << std::endl;
    }
```
Resultado:
```cpp
    std::shared_ptr<Entity> entity = std::EntityFactory<EntityTest>::create("Test1");
    entity->print();
```
Terminal:
```terminal
    Entity name: Test1
    This is an test
```

# EntityFactory

`EntityFactory` es una clase que se encarga de crear e inicializar automáticamente [`Entity`](#entity) del tipo `T` derivado de [`Entity`](#entity).

#### create
```cpp
    static std::shared_ptr<T> create(const std::string& name);
```
Función `static` que crea una nueva entidad del tipo `T` con el nombre dado. Inicializa el [`transform`](transform.md) de la entidad y llama a `init()` para inicializar los componentes de la entidad. Devuelve el `shared_ptr` de la entidad creada.

Ejemplo de uso:
```cpp
    std::shared_ptr<Entity> entity = EntityFactory<EntityTest>::create("Test1");
```