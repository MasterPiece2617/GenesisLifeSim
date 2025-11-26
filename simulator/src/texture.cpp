#include <texture.hpp>

std::unique_ptr<Atlas> Texture::atlas = nullptr;

const sf::Texture& Texture::get_atlas()
{
    if (!atlas)
    {
        atlas = std::make_unique<Atlas>();
    }

    return atlas->get_atlas();
}

sf::IntRect Texture::get_region(const std::string& name)
{
    if (!atlas)
    {
        atlas = std::make_unique<Atlas>();
    }

    auto it = atlas->get_regions().find(name);

    if (it != atlas->get_regions().end())
    {
        return it->second;
    }
    
    return atlas->get_regions().at("error_texture");
}

Atlas::Atlas()
{
    atlas = std::make_unique<sf::Texture>();
    std::vector<std::pair<std::string, sf::Image>> images;
    std::filesystem::path folder = "resources/textures";

    if (std::filesystem::exists(folder) && std::filesystem::is_directory(folder))
    {
        for (const auto& entry : std::filesystem::directory_iterator(folder))
        {
            if (entry.is_directory())
            {
                for (const auto& file : std::filesystem::directory_iterator(entry))
                {
                    if (file.path().extension() == ".png")
                    {
                        sf::Image img;

                        if (img.loadFromFile(file.path().string()))
                        {
                            images.push_back({ file.path().stem().string(), img });
                        }
                        else
                        {
                            std::cerr << "Failed to load image: " << file.path() << std::endl;
                        }
                    }
                }
            }
        }
    }
    else
    {
        std::cerr << "Error, no texture folder.\n";
    }

    sf::Image raw_atlas;
    size_t N = images.size() + 1;
    unsigned cols = std::ceil(std::sqrt(N));
    unsigned rows = std::ceil(float(N) / cols);

    int i = 0;
    int j = 1;

    atlas->create(cols * Constants::px_mt, rows * Constants::px_mt);
    raw_atlas.create(cols * Constants::px_mt, rows * Constants::px_mt);

    regions.insert({ "error_texture", sf::IntRect(0, 0, Constants::px_mt, Constants::px_mt) });

    for (unsigned y = 0; y < Constants::px_mt; ++y)
    {
        for (unsigned x = 0; x < Constants::px_mt; ++x)
        {
            raw_atlas.setPixel(x, y, x < Constants::px_mt / 2 ? (y < Constants::px_mt / 2 ? sf::Color::Black : sf::Color::Magenta) : (y < Constants::px_mt / 2 ? sf::Color::Magenta : sf::Color::Black));
        }
    }

    for (auto& pair : images)
    {
        if (j == cols)
        {
            j = 0;
            ++i;
        }

        unsigned x_offset = j * Constants::px_mt;
        unsigned y_offset = i * Constants::px_mt;

        const sf::Image& img = pair.second;
        const std::string& name = pair.first;

        for (unsigned y = 0; y < Constants::px_mt; ++y)
        {
            for (unsigned x = 0; x < Constants::px_mt; ++x)
            {
                raw_atlas.setPixel(x_offset + x, y_offset + y, img.getPixel(x, y));
            }
        }

        regions.insert({ name, sf::IntRect(x_offset, y_offset, Constants::px_mt, Constants::px_mt) });

        ++j;
    }

    atlas->loadFromImage(raw_atlas);
}

const std::unordered_map<std::string, sf::IntRect>& Atlas::get_regions()
{
    return regions;
}

const sf::Texture& Atlas::get_atlas()
{
    return *atlas;
}
