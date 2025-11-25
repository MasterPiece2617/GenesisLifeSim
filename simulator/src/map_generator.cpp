#include <map_generator.hpp>

void MapGenerator::generate_map(const Config& config)
{
    std::vector<CellData> cells(static_cast<size_t>(config.width * config.height));

    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noise.SetSeed(config.seed);
    noise.SetFrequency(config.frequency);
    noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    noise.SetFractalOctaves(config.octaves);

    FastNoiseLite noiseDetail;
    noiseDetail.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noiseDetail.SetSeed(config.seed + 1);
    noiseDetail.SetFrequency(config.detail_freq);

    std::cout << "Generando terreno con Rough Sea en los bordes..." << std::endl;

    for (int y = 0; y < config.height; ++y) {
        for (int x = 0; x < config.width; ++x) {
            
            if (y < 8 || y >= config.height - 8 || x < 8 || x >= config.width - 8) {
                cells[y * config.width + x] = {7, 7, 255, false, false, false}; // ID 7: Rough Sea
                continue;
            }

            float heightValue = noise.GetNoise((float)x, (float)y);
            float detailValue = noiseDetail.GetNoise((float)x, (float)y);

            CellData cell;

            // 1. Normal Water (< -0.2)
            if (heightValue < -0.2f) {
                cell = {2, 2, 7, false, true, false}; // Water
            }
            // 2. Sand (-0.2 a -0.05)
            else if (heightValue < -0.05f) {
                cell = {3, 3, 3, true, false, false}; // Sand
            }
            // 3. Rohan (0.0 a 0.5)
            else if (heightValue < 0.5f) {
                if (detailValue > 0.2f) {
                    cell = {6, 6, 1, true, false, true}; // Dark Grass
                } else {
                    cell = {1, 1, 1, true, false, true}; // Grass
                }
            }
            // 4. Swamp (0.5 a 0.7)
            else if (heightValue < 0.7f) {
                cell = {5, 5, 10, false, true, false}; // Swamp (Lento)
            }
            // 5. Mountain (> 0.7)
            else {
                cell = {4, 4, 9, true, false, false}; // Stone
            }

            cells[y * config.width + x] = cell;
        }
    }

    std::string path = "resources/maps/" + std::string(config.filename);
    #ifdef PROJECT_SOURCE_DIR
        std::string base_path = PROJECT_SOURCE_DIR;
        std::string path2 = base_path + "/resources/maps/" + std::string(config.filename);
        std::ofstream file2(path2, std::ios::binary); // idk this, will be ignored on build systems that don't define PROJECT_SOURCE_DIR, its focking amazing
    #endif
    std::ofstream file(path, std::ios::binary);

    if (!file) {
        std::cerr << "Error opening file for writing." << std::endl;
        return;
    }

    file.write(reinterpret_cast<const char*>(&config.width), sizeof(config.width));
    file.write(reinterpret_cast<const char*>(&config.height), sizeof(config.height));
    file.write(reinterpret_cast<const char*>(cells.data()), cells.size() * sizeof(CellData));
    file.close();

    std::cout << "Mapa generado! Bordes seguros establecidos con textura 7." << std::endl;
}