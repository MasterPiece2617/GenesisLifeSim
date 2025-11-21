#pragma once

#include <vector>
#include <queue>
#include <unordered_map>
#include <cmath>
#include <algorithm>
#include <memory> 

#include <SFML/System/Vector2.hpp>

#include "entity_terrain.hpp"
#include "utils.hpp" 

class PathFinder
{
private:
    // Estructura de Nodo para A*
    struct Node {
        sf::Vector2i position; // Posición en la grilla (x, y)
        float g_cost;          // Costo desde el inicio
        float h_cost;          // Heurística (distancia estimada al final)
        float f_cost;          // Costo total (g + h)
        Node* parent;          // Para reconstruir el camino

        Node(sf::Vector2i pos, Node* p = nullptr) 
            : position(pos), g_cost(0.f), h_cost(0.f), f_cost(0.f), parent(p) {}

        // Operador para la cola de prioridad (el menor F tiene prioridad)
        bool operator>(const Node& other) const {
            return f_cost > other.f_cost;
        }
    };

    // Calcula la distancia estimada (Euclidiana)
    float calculate_h_cost(sf::Vector2i from, sf::Vector2i to);
    
    // Reconstruye la ruta desde el nodo final hasta el inicio
    std::vector<sf::Vector2f> reconstruct_path(Node* end_node);

public:
    PathFinder() = default;
    ~PathFinder() = default;

    std::vector<sf::Vector2f> find_path(const sf::Vector2f& start, const sf::Vector2f& end, const EntityTerrain& terrain);
};