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
#### Métodos de establecimiento de transformaciones
Estos métodos establecen las transformaciones absolutas de la entidad, es decir, su posición, escala y rotación en el espacio global.
	void set_position(const sf::Vector2f& pos);
	sf::Vector2f get_position() const;
	void set_scale(const sf::Vector2f& _scale);
	sf::Vector2f get_scale() const;
	void set_rotation(float _angle);
	float get_rotation() const;
#### Métodos de establecimiento de transformaciones relativas
Estos métodos establecen las transformaciones relativas de la entidad, es decir, su posición, escala y rotación en relación a su padre.
	void set_local_position(const sf::Vector2f& pos);
	sf::Vector2f get_local_position() const;
	void set_local_scale(const sf::Vector2f& _scale);
	sf::Vector2f get_local_scale() const;
	void set_local_rotation(float _angle);
	float get_local_rotation() const;
#### Métodos de transformación
Estos métodos aplican transformaciones a la entidad, modificando su posición, escala o rotación en relación a su estado actual.
	void translate(const sf::Vector2f& offset);
	void rotate(float _angle);
	void scale(const sf::Vector2f& offsset);
#### Métodos de vector de dirección
```cpp
	sf::Vector2f get_forward() const;
```
Función que devuelve un vector `sf::Vector2f` que representa la dirección normalizada hacia adelante de la entidad, calculada a partir de su rotación.

```cpp
	void look_at(const sf::Vector2f& target);
```
Fucción que establece la dirección de la entidad hacia un punto específico en el espacio 2D.
### Destructor
```cpp
	~Transform() override = default;
```
Destructor por defecto heredado de [`Component`](component.md).