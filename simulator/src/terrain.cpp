#include <terrain.hpp>

#include <iostream>
#include <fstream>

// Load and save terrain data from a binary file
bool Terrain::load_from_file(const std::string &filename)
{
	std::ifstream file(filename, std::ios::binary);

	if (!file.is_open())
	{
		std::cerr << "Error opening file for reading: " << filename << std::endl;
		cells.clear();
		width = 0;
		height = 0;
		return false;
	}

	file.read(reinterpret_cast<char*>(&width), sizeof(width));
	file.read(reinterpret_cast<char*>(&height), sizeof(height));

	if (width == 0 || height == 0)
	{
		std::cerr << "Error: El archivo de mapa est� vac�o o es inv�lido." << std::endl;
		cells.clear();
		width = 0;
		height = 0;
		file.close();
		return false;
	}

	cells.resize(static_cast<size_t>(width) * height);
	file.read(reinterpret_cast<char*>(cells.data()), cells.size() * sizeof(CellData));

	if (file.gcount() != static_cast<std::streamsize>(cells.size() * sizeof(CellData))) {
		std::cerr << "Error: El archivo no contiene la cantidad de datos esperada." << std::endl;
		cells.clear();
		width = 0;
		height = 0;
		file.close();
		return false;
	}

	file.close();
	std::cout << "Mapa cargado exitosamente. Dimensiones: " << width << "x" << height << std::endl;
	return true;
}


// modifiers
void Terrain::set_cell(uint16_t x, uint16_t y, const CellData& cell)
{
	if (x >= width || y >= height)
	{
		throw std::out_of_range("Cell coordinates out of range");
	}
	cells[y * width + x] = cell;
}

CellData Terrain::get_cell(uint16_t x, uint16_t y) const
{
	if (x >= width || y >= height)
	{
		throw std::out_of_range("Cell coordinates out of range");
	}
	return cells[y * width + x];
}

// getters
uint16_t Terrain::get_width() const
{ 
	return width; 
}

uint16_t Terrain::get_height() const
{ 
	return height; 
}

float Terrain::get_effort(uint16_t x, uint16_t y) const
{
	if (x >= width || y >= height)
	{
		return 100.0f;
	}
	return static_cast<float>(get_cell(x, y).effort);
}

bool Terrain::walkable(uint16_t x, uint16_t y) const
{
	if (x >= width || y >= height)
	{
		return false;
	}
	return get_cell(x, y).is_walkable;
}