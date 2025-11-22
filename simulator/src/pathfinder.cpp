#include "pathfinder.hpp"
#include <iostream>

// 1. HEURÍSTICA
float PathFinder::calculate_h_cost(sf::Vector2i from, sf::Vector2i to) {
    float dx = static_cast<float>(from.x - to.x);
    float dy = static_cast<float>(from.y - to.y);
    // Distancia directa sin escalar
    return std::sqrt(dx * dx + dy * dy) * 1.001f;
}

// 2. RECONSTRUCCIÓN
std::vector<sf::Vector2f> PathFinder::reconstruct_path(Node* end_node) {
    std::vector<sf::Vector2f> path;
    Node* current = end_node;
    
    while (current != nullptr) {
        if (current->parent != nullptr) // Ignorar inicio
        {
            // ESCALA 1:1
            // La coordenada de mundo es el índice de la celda + 0.5 (centro)
            float world_x = static_cast<float>(current->position.x) + 0.5f;
            float world_y = static_cast<float>(current->position.y) + 0.5f;
            
            path.push_back(sf::Vector2f(world_x, world_y));
        }
        current = current->parent;
    }
    
    std::reverse(path.begin(), path.end());
    return path;
}

// 3. ALGORITMO A*
std::vector<sf::Vector2f> PathFinder::find_path(const sf::Vector2f& start_f, const sf::Vector2f& end_f, const EntityTerrain& terrain) {
    
    // ESCALA 1:1 (METROS -> CELDAS)
    // Si estás en 74.5m, estás en la celda 74. No dividimos por 32, dios mio yo no sabia eso me mato la vaina.
    sf::Vector2i start_grid(static_cast<int>(start_f.x), static_cast<int>(start_f.y));
    sf::Vector2i end_grid(static_cast<int>(end_f.x), static_cast<int>(end_f.y));

    // Caso: Misma Celda
    if (start_grid == end_grid) return { end_f };

    // Validaciones
    if (end_grid.x < 0 || end_grid.x >= terrain.get_width() ||
        end_grid.y < 0 || end_grid.y >= terrain.get_height()) 
    {
        return {}; 
    }

    // Estructuras A*
    auto compare_nodes = [](Node* a, Node* b) { return a->f_cost > b->f_cost; };
    std::priority_queue<Node*, std::vector<Node*>, decltype(compare_nodes)> open_list(compare_nodes);
    std::unordered_map<int, std::unique_ptr<Node>> all_nodes;

    auto pos_to_key = [&](sf::Vector2i pos) {
        return (pos.y << 16) | (pos.x & 0xFFFF);
    };

    auto start_node = std::make_unique<Node>(start_grid);
    start_node->h_cost = calculate_h_cost(start_grid, end_grid);
    start_node->f_cost = start_node->h_cost;
    
    int start_key = pos_to_key(start_grid);
    all_nodes[start_key] = std::move(start_node);
    open_list.push(all_nodes[start_key].get());

    while (!open_list.empty()) {
        Node* current_node = open_list.top();
        open_list.pop();

        if (current_node->position == end_grid) {
            std::vector<sf::Vector2f> final_path = reconstruct_path(current_node);
            final_path.push_back(end_f); 
            return final_path;
        }

        for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
                if (dx == 0 && dy == 0) continue;

                sf::Vector2i neighbor_pos = current_node->position + sf::Vector2i(dx, dy);

                if (neighbor_pos.x < 0 || neighbor_pos.x >= terrain.get_width() ||
                    neighbor_pos.y < 0 || neighbor_pos.y >= terrain.get_height()) continue;

                // Obtener Esfuerzo
                float raw_effort = terrain.get_effort(static_cast<uint16_t>(neighbor_pos.x), static_cast<uint16_t>(neighbor_pos.y));
                
                // Penalización Cuadrática, recordar que la vaina camina mas lento a mayor esfuerzo, simulamos eso en el pathfinder
                float penalty = raw_effort * raw_effort;

                float dist_cost = (dx == 0 || dy == 0) ? 1.0f : 1.414f;
                float traversal_cost = dist_cost * penalty;

                float g_cost_new = current_node->g_cost + traversal_cost;

                int neighbor_key = pos_to_key(neighbor_pos);
                bool is_new_node = (all_nodes.find(neighbor_key) == all_nodes.end());
                Node* neighbor_node = nullptr;

                if (is_new_node) {
                    auto new_node = std::make_unique<Node>(neighbor_pos, current_node);
                    new_node->h_cost = calculate_h_cost(neighbor_pos, end_grid);
                    all_nodes[neighbor_key] = std::move(new_node);
                    neighbor_node = all_nodes[neighbor_key].get();
                } else {
                    neighbor_node = all_nodes[neighbor_key].get();
                }

                if (is_new_node || g_cost_new < neighbor_node->g_cost) {
                    neighbor_node->g_cost = g_cost_new;
                    neighbor_node->f_cost = g_cost_new + neighbor_node->h_cost;
                    neighbor_node->parent = current_node;
                    open_list.push(neighbor_node);
                }
            }
        }
    }

    return {}; 
}