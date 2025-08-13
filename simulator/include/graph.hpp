#include <entity.hpp>

class Hierarchy
{

protected:
	// Esto se va a cambiar por un grafo/árbol. Asignación de José...
	std::vector<std::shared_ptr<Entity>> entities;

public:

	Hierarchy() = default; // Provisional
	bool add_entity(const std::shared_ptr<Entity> entity);
	std::vector<std::shared_ptr<Entity>> get_entities(); // Provisional
	~Hierarchy() = default;
};