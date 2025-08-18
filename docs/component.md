# Component

`Component` es la clase base de todos los componentes del programa, desde los componentes de posición hasta los de renderizado, son derivados de `Component`. Está pensada para ser una clase base que se puede heredar, 
por lo que no tiene un comportamiento por defecto.

## Atributos de Component

Cada `Component` cuenta con una referencia a la [`Entity`](entity.md) a la que pertenece y un `bool` que indica si está activo o no.

```cpp
	std::weak_ptr<Entity> owner;
	bool is_active = true;
```
## Métodos de Component

### Constructor
```cpp
	Component(std::weak_ptr<Entity> _owner);
```
Funciona como constructor de la clase `Component`, recibe un `std::weak_ptr<Entity>` que representa la entidad propietaria del componente.
### Métodos de acceso a la entidad propietaria
#### `get_owner`
```cpp
	std::weak_ptr<Entity> get_owner() const;
```
Devuelve un `std::weak_ptr<Entity>` que representa la entidad propietaria del componente.
#### `set_owner`
```cpp
	bool set_owner(std::weak_ptr<Entity> _owner);
```
Establece la entidad propietaria del componente. Devuelve `true` si se ha establecido correctamente, `false` si no se ha podido establecer.
### Métodos de activación y desactivación
#### `get_is_active`
```cpp
	bool get_is_active() const;
```
Devuelve un `bool` que indica si el componente está activo o no.
#### `deactivate`
```cpp
	void deactivate();
```
Desactiva el componente, estableciendo `is_active` a `false`.
#### `activate`
```cpp
	void activate();
```
Activa el componente, estableciendo `is_active` a `true`.
### Metodos de loop
#### `start`
```cpp
	virtual void start();
```
Inicia el componente, se llama una vez al inicio del programa. Este método es virtual y puede ser sobreescrito por las clases derivadas para implementar su propia lógica de inicio.
#### `update`
```cpp
	virtual void update();
```
Actualiza el componente, se llama en cada frame del programa. Este método es virtual y puede ser sobreescrito por las clases derivadas para implementar su propia lógica de actualización.
### Destructor
```cpp
	virtual ~Component() = default;
```
Destruye el `Component`. Este método es virtual y puede ser sobreescrito por las clases derivadas para implementar su propia lógica de destrucción.

## Cómo crear una clase derivada de Component?

Para crear una clase derivada de `Component` debes sobreescribir los métodos `start()` y `update()` los cuales contienen la lógica que se ejecutará al inicio de la [`Scene`](scene.md) (o al ser iniciada una [`Entity`](entity.md) durante la ejecución)
y en cada frame respectivamente. Aquí tienes un ejemplo de `TestComponent`:
`component_test.hpp`:
```cpp
	#pragma once

	#include <component.hpp>

	class ComponentTest : public Component
	{
	public:
	
		ComponentTest(std::weak_ptr<Entity> _owner) : Component(_owner) {}

		void start() override
		{
			std::cout << "ComponentTest from " << owner.lock()->get_name() << " started." << std::endl;
		}

		void update() override
		{
			std::cout << "ComponentTest from " << owner.lock()->get_name() << " updated." << std::endl;
		}

		~ComponentTest() override = default;
	};
```
Agregar componenten creado a una [`Entity`](entity.md), es este caso lo agregaremos a una cláse derivada `EntityTest`.
`entity_test.cpp`:
```cpp
	void init() override
    {
        add_component(std::make_shared<ComponentTest>());
    }
```
Ejemplo al usar `EntityTest`:
```cpp
	auto entity = EntityFactory<EntityTest>::create("Test1");
	entity->start();
	entity->update();
```
Resultando en la salida:
```terminal
	ComponentTest from Test1 started.
	ComponentTest from Test1 updated.
```