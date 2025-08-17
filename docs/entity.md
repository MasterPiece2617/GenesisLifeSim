# Entity

`Entity` es la clase base de todas las entidades del programa, desde las plantas hasta la cámara que renderiza la escena, son derivados de `Entity`. Está pensada para ser una clase base que se puede heredar, por lo que no tiene [`Component`](component.md) 
por defecto, a excepción del [`Transform`](transform.md), que es obligatorio para todas las entidades ya que define la jerarquía de `Entity` en la [`Scene`](scene.md), esto es importante para la propagación de cambios en casos donde la lógica lo requiera.
A pesar de no tener [`Component`](component.md) que definan su comportamiento o apariencia, puede ser útil para crear entidades vacías a las que se le pueden agregar y eliminar [`Component`](component.md) de forma dinámica en casos donde sea más conveniente en vez de
crear una clase heredera nueva.

Cada `Entity` cuenta con un nombre, `tags`, una lista de [`Component`](component.md), un `bool` que indica si está activo o no y un acceso directo al [`Component`](component.md) [`transform`](transform.md).

```cpp
    std::string name;
    std::vector<std::shared_ptr<Component>> components;
    std::vector<std::string> tags;
    std::shared_ptr<Transform> transform;
    bool is_active = true;
```

Sus métodos son:

```cpp
    Entity(const std::string& _name);
```
Constructor que recibe un nombre para la entidad. <span style="color:red">NOTA</span>: no usar este constructor directamente, en vez usar [`EntityFactory`](#entityfactory), pero si aún así decides usarla, no olvides inicializarla con `init_transform_()` y luego con
`init()`.
```cpp
    void init_transform();
```
Función que inicializa el [`transform`](transform.md) de la `Entity` con un apuntador de sí mismo `shared_from_this()`, esto es necesario debido a que no es posible usar `shared_from_this()` en el constructor porque el apuntador inteligente no ha sido creado aún.
```cpp
    virtual void init();
```
Función virtual cuya función principal es inicializar los [`Component`](component.md) de la `Entity`, es necesario sobreescribir esta función para agregar los [`Component`](component.md) en las clases derivadas.
```cpp
    const std::string& get_name() const;
```
Función que devuelve el nombre de la entidad.
```cpp
    void set_name(const std::string& _name);
```
Función que establece el nombre de la entidad.
```cpp
    bool add_component(std::shared_ptr<Component> component);
```
Función que añade un componente a la entidad. Devuelve `true` si se ha añadido correctamente, `false` si ya existe un componente con el mismo tipo.
```cpp
    virtual void print() const;
```
Función que imprime información de la entidad, como su nombre y sus [`Component`](component.md). Esta función es virtual, por lo que puede ser sobreescrita en clases derivadas.
```cpp
    void print_family() const;
```
Función que imprime el nombre de la entidad, así como los nombres de las entidades padre e hijo.
```cpp
    bool get_is_active() const;
```
Función que devuelve si la entidad está activa o no.
```cpp
    void deactivate();
```
Función que desactiva la entidad, estableciendo `is_active` a `false`.
```cpp
    void activate();
```
Función que activa la entidad, estableciendo `is_active` a `true`.
```cpp
    Transform& get_transform() const;
```
Función que devuelve el componente [`Transform`](transform.md) de la entidad. Este componente es especial, ya que todas las entidades deben tenerlo, se usa para manejar la posición, rotación y escala de la entidad en el mundo, y también para manejar la jerarquía.
```cpp
    bool add_tag(const std::string& tag);
```
Función que añade una etiqueta a la entidad. Devuelve `true` si se ha añadido correctamente, `false` si ya existe una etiqueta con el mismo nombre.
```cpp
    bool has_tag(const std::string& tag) const;
```
Función que comprueba si la entidad tiene una etiqueta con el nombre dado. Devuelve `true` si la etiqueta existe, `false` en caso contrario.
```cpp
    bool remove_tag(const std::string& tag);
```
Función que elimina una etiqueta de la entidad. Devuelve `true` si se ha eliminado correctamente, `false` si no existe una etiqueta con el nombre dado.
```cpp
    bool set_parent(std::shared_ptr<Entity> _parent);
```
Función que establece la entidad padre de la entidad actual. Devuelve `true` si se ha establecido correctamente, `false` si no ha sido posible.
```cpp
    std::shared_ptr<Entity> get_parent() const;
```
Función que devuelve la entidad padre de la entidad actual. Devuelve un `shared_ptr` a la entidad padre, o `nullptr` si no tiene padre.
```cpp
    std::vector<std::shared_ptr<Entity>> get_children() const;
```
Función que devuelve un vector de `shared_ptr` a las entidades hijas de la entidad actual. Si no tiene hijas, devuelve un vector vacío.
```cpp
    bool add_child(std::shared_ptr<Entity> child);
```
Función que añade una entidad hija a la entidad actual. Devuelve `true` si se ha añadido correctamente, `false` si no ha sido posible.
```cpp
    bool remove_child(std::shared_ptr<Entity> child);
```
Función que elimina una entidad hija de la entidad actual. Devuelve `true` si se ha eliminado correctamente, `false` si no existe la entidad hija.
```cpp
    void start();
```
Función que se llama al iniciar la entidad. Se usa para inicializar los [`Component`](component.md).
```cpp
    void update();
```
Función que se llama en cada frame para actualizar la `Entity`, es decir, sus [`Component`](component.md).
```cpp
    virtual ~Entity() = default;
```
estructor virtual que se encarga de liberar los recursos de la entidad. Es importante que sea virtual para que se llame al destructor de las clases derivadas cuando se destruya una entidad.

# EntityFactory

`EntityFactory` es una clase que se encarga de crear e inicializar automáticamente [`Entity`](#entity) del tipo `T` derivado de [`Entity`](#entity).

```cpp
    static std::shared_ptr<T> create(const std::string& name);
```
Función `static` que crea una nueva entidad del tipo `T` con el nombre dado. Inicializa el [`transform`](transform.md) de la entidad y llama a `init()` para inicializar los componentes de la entidad. Devuelve el `shared_ptr` de la entidad creada.
