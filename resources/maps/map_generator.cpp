#include <iostream>
#include <fstream>
#include <cstdint>
#include <vector>

struct CellData
{
	uint8_t typeCell;
	uint8_t textureCell;
	bool is_walkable;
};

int main()
{
	// Just a test file
	const uint16_t width = 100;
	const uint16_t height = 100;

	std::vector<CellData> cells(static_cast<size_t>(width) * height);

	// Initialize cells with some data
	for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (y < 10 || y >= height - 10 ||
                x < 10 || x >= width - 10) {
                cells[y * width + x] = {2, 2, false}; // Agua en los bordes
            } else {
                cells[y * width + x] = {1, 1, true}; // Tierra en el centro
            }
        }
    }

	// Write to binary file
	std::ofstream file("map_2.zadat", std::ios::binary);

	if (!file)
	{
		std::cerr << "Error opening file for writing." << std::endl;
		return -1;
	}

	file.write(reinterpret_cast<const char*>(&width), sizeof(width));
	file.write(reinterpret_cast<const char*>(&height), sizeof(height));

	file.write(reinterpret_cast<const char*>(cells.data()), cells.size() * sizeof(CellData));

	file.close();

	std::cout << "Map file 'map_2.zadat' generado chevere." << std::endl;
	return 0;
}