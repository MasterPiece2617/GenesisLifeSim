# Transform

`Transform` es un [`Component`](component.md) que representa la posición, rotación y escala de una entidad en el espacio 2D, además de manejar la jerarquía. Todas las [`Entity`](entity.md) tienen un `Transform` por defecto.

## Atributos de Transform

Además de los atributos heredados de [`Component`](component.md), `Transform` tiene 3 vectores `sf::Vector2f` de la librería SFML, un vector de posición, un vector de escala y un vector de rotación (en grados). 
También tiene un `weak_ptr` a su entidad padre y un vector  de `shared_ptr` a sus entidades hijas.

```cpp
	sf::Vector2f position;
	sf::Vector2f scale;
	sf::Vector2f rotation; // Grades
	std::weak_ptr<Entity> parent;
	std::vector<std::shared_ptr<Entity>> children;
```

## Métodos de Transform

### Constructor
```cpp
	Transform(std::weak_ptr<Entity> _owner);
```
Constructor que recibe un `weak_ptr` a la entidad dueña del componente.
### Metodos de jerarquía
#### `set_parent`
```cpp
	bool set_parent(std::shared_ptr<Entity> _parent);
```
Función que asigna un padre a la entidad dueña del componente. Devuelve `true` si se asignó correctamente, `false` si no se pudo asignar. No admite ciclos en la jerarquía.
#### `get_parent`
```cpp
	std::shared_ptr<Entity> get_parent() const;
```
Función que devuelve un `shared_ptr` a la entidad padre. Si no tiene padre, devuelve un `shared_ptr` nulo.
#### `add_child`
```cpp
	bool add_child(std::shared_ptr<Entity> child);
```
Función que añade un hijo a la entidad dueña del componente. Devuelve `true` si se añadió correctamente, `false` si no se pudo añadir. No admite ciclos en la jerarquía.
#### `remove_child`
```cpp
	bool remove_child(std::shared_ptr<Entity> child);
```
Función que elimina un hijo de la entidad dueña del componente. Devuelve `true` si se eliminó correctamente, `false` si no se pudo eliminar.
#### `get_children`
```cpp
	std::vector<std::shared_ptr<Entity>> get_children() const;
```
Función que devuelve un vector de `shared_ptr` a las entidades hijas.
### Destructor
```cpp
	~Transform() override = default;
```
Destructor por defecto heredado de [`Component`](component.md).