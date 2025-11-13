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
	const uint16_t width = 10;
	const uint16_t height = 10;

	std::vector<CellData> cells(static_cast<size_t>(width) * height);

	// Initialize cells with some data
	for (uint16_t y = 0; y < height; ++y)
	{
		for (uint16_t x = 0; x < width; ++x)
		{
			if (x == 0 || x == width - 1 || y == 0 || y == height - 1)
			{
				cells[y * width + x] = { 2, 2, false }; // Agua (tipo 2, textura 2)
			}
			else
			{
				cells[y * width + x] = { 1, 1, true }; // Pasto (tipo 1, textura 1)
			}
		}
	}

	// Write to binary file
	std::ofstream file("map.zadat", std::ios::binary);

	if (!file)
	{
		std::cerr << "Error opening file for writing." << std::endl;
		return -1;
	}

	file.write(reinterpret_cast<const char*>(&width), sizeof(width));
	file.write(reinterpret_cast<const char*>(&height), sizeof(height));

	file.write(reinterpret_cast<const char*>(cells.data()), cells.size() * sizeof(CellData));

	file.close();

	std::cout << "Map file 'map.zadat' generado chevere." << std::endl;
	return 0;
}