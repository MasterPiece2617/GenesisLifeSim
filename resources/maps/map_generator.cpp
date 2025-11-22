#include <iostream>
#include <fstream>
#include <cstdint>
#include <vector>

#define FASTNOISE_LITE_IMPLEMENTATION
#include "FastNoiseLite.h"

struct CellData
{
	uint8_t typeCell;
	uint8_t textureCell;
	uint8_t effort;
	bool is_walkable;
};

int main()
{
    const uint16_t width = 200; 
    const uint16_t height = 200;
    const int seed = 33; // Puedes cambiar la seed

    std::vector<CellData> cells(static_cast<size_t>(width) * height);

    // 1. Configuración de Ruido
    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noise.SetSeed(seed);
    noise.SetFrequency(0.02f);
    noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    noise.SetFractalOctaves(5);

    FastNoiseLite noiseDetail;
    noiseDetail.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noiseDetail.SetSeed(seed + 1);
    noiseDetail.SetFrequency(0.05f);

    std::cout << "Generando terreno con Rough Sea en los bordes..." << std::endl;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            
            // --- CAMBIO AQUÍ: BORDES CON 'ROUGH SEA' (ID 7) ---
            // Aumenté el borde a 8 celdas para que se note bien la "zona prohibida"
            if (y < 8 || y >= height - 8 || x < 8 || x >= width - 8) {
                cells[y * width + x] = {7, 7, 255, false}; // ID 7: Rough Sea
                continue;
            }

            float heightValue = noise.GetNoise((float)x, (float)y);
            float detailValue = noiseDetail.GetNoise((float)x, (float)y);

            CellData cell;

            // 1. AGUA NORMAL (< -0.2)
            if (heightValue < -0.2f) {
                // Opcional: Si el agua es MUY profunda (ej. < -0.7), 
                // también podrías poner Rough Sea (7) aquí para que se vea natural
                // mezclado con el borde. Por ahora lo dejo en agua normal (2).
                cell = {2, 2, 7, false}; 
            }
            // 2. ARENA (-0.2 a -0.05)
            else if (heightValue < -0.05f) {
                cell = {3, 3, 3, true}; // Sand
            }
            // 3. LLANURAS (0.0 a 0.5)
            else if (heightValue < 0.5f) {
                if (detailValue > 0.2f) {
                    cell = {6, 6, 1, true}; // Dark Grass
                } else {
                    cell = {1, 1, 1, true}; // Grass
                }
            }
            // 4. PANTANO (0.5 a 0.7)
            else if (heightValue < 0.7f) {
                cell = {5, 5, 10, true}; // Swamp (Lento)
            }
            // 5. MONTAÑA (> 0.7)
            else {
                cell = {4, 4, 9, true}; // Stone
            }

            cells[y * width + x] = cell;
        }
    }

    std::ofstream file("map_noise_1_v3.zadat", std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file for writing." << std::endl;
        return -1;
    }

    file.write(reinterpret_cast<const char*>(&width), sizeof(width));
    file.write(reinterpret_cast<const char*>(&height), sizeof(height));
    file.write(reinterpret_cast<const char*>(cells.data()), cells.size() * sizeof(CellData));
    file.close();

    std::cout << "Mapa generado! Bordes seguros establecidos con textura 7." << std::endl;
    return 0;
}