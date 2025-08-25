# Transform

`Transform` es un [`Component`](component.md) que representa la posición, rotación y escala de una entidad en el espacio 2D, además de manejar la jerarquía. Todas las [`Entity`](entity.md) tienen un `Transform` por defecto.

## Atributos de Transform

Además de los atributos heredados de [`Component`](component.md), `Transform` tiene 3 vectores `sf::Vector2f` de la librería SFML, un vector de posición, un vector de escala y un vector de rotación (en grados). 
También tiene un `weak_ptr` a su entidad padre y un vector  de `shared_ptr` a sus entidades hijas.

```cpp
	sf::Vector2f position;
	sf::Vector2f scale;
	float angle; 
	std::weak_ptr<Entity> parent;
	std::vector<std::shared_ptr<Entity>> children;
```

## Métodos de Transform

### Constructor
```cpp
	Transform(std::weak_ptr<Entity> _owner);
```
Constructor que recibe un `weak_ptr` a la entidad dueña del componente.

---

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

---

### Métodos de establecimiento de transformaciones
Estos métodos establecen las transformaciones absolutas de la entidad, es decir, su posición, escala y rotación en el espacio global.

#### `set_position`
```cpp
	void set_position(const sf::Vector2f& pos);
```
Esta función establece la posición absoluta de la entidad en el espacio 2D.
#### `get_position`
```cpp
	sf::Vector2f get_position() const;
```
Esta función devuelve un vector `sf::Vector2f` que representa la posición absoluta de la entidad en el espacio 2D.
#### `set_scale`
```cpp
	void set_scale(const sf::Vector2f& _scale);
```
Esta función establece la escala absoluta de la entidad en el espacio 2D.
#### `get_scale`
```cpp
	sf::Vector2f get_scale() const;
```
Esta función devuelve un vector `sf::Vector2f` que representa la escala absoluta de la entidad en el espacio 2D.
#### `set_rotation`
```cpp
	void set_rotation(float _angle);
```
Esta función establece la rotación absoluta de la entidad en el espacio 2D, en grados.
#### `get_rotation`
```cpp
	float get_rotation() const;
```
Esta función devuelve un `float` que representa la rotación absoluta de la entidad en el espacio 2D, en grados.

---

### Métodos de establecimiento de transformaciones relativas
Estos métodos establecen las transformaciones relativas de la entidad, es decir, su posición, escala y rotación en relación a su padre.
#### `set_local_position`
```cpp
	void set_local_position(const sf::Vector2f& pos);
```
Esta función establece la posición relativa de la entidad en relación a su padre en el espacio 2D.
#### `get_local_position`
```cpp
	sf::Vector2f get_local_position() const;
```
Esta función devuelve un vector `sf::Vector2f` que representa la posición relativa de la entidad en relación a su padre en el espacio 2D.
#### `set_local_scale`
```cpp
	void set_local_scale(const sf::Vector2f& _scale);
```
Esta función establece la escala relativa de la entidad en relación a su padre en el espacio 2D.
#### `get_local_scale`
```cpp
	sf::Vector2f get_local_scale() const;
```
Esta función devuelve un vector `sf::Vector2f` que representa la escala relativa de la entidad en relación a su padre en el espacio 2D.
#### `set_local_rotation`
```cpp
	void set_local_rotation(float _angle);
```
Esta función establece la rotación relativa de la entidad en relación a su padre en el espacio 2D, en grados.
#### `get_local_rotation`
```cpp
	float get_local_rotation() const;
```
Esta función devuelve un `float` que representa la rotación relativa de la entidad en relación a su padre en el espacio 2D, en grados.

---

### Métodos de transformación
Estos métodos aplican transformaciones a la entidad, modificando su posición, escala o rotación en relación a su estado actual.
#### `translate_mod`
```cpp
	void translate(const sf::Vector2f& offset);
```
Función que aplica una transformación de traslación a la entidad, modificando su posición actual en el espacio 2D. El parámetro `offset` es un vector `sf::Vector2f` que indica cuánto se debe desplazar la entidad.
#### `rotate_mod`
```cpp
	void rotate(float _angle);
```	
Función que aplica una transformación de rotación a la entidad, modificando su ángulo actual en grados. El parámetro `_angle` es un `float` que indica cuánto se debe rotar la entidad.
#### `scale_mod`
```cpp
	void add__scale(const sf::Vector2f& offsset);
```
Función que aplica una transformación de escala a la entidad, modificando su escala actual en el espacio 2D. El parámetro `offset` es un vector `sf::Vector2f` que indica cuánto se debe escalar la entidad.

---

### Métodos de vector de dirección
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
	~Transform() override;
```
Destructor encargado de eliminar el transform de la entidad y todos los hijos (referencias incluidas).