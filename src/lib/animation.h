#ifndef SLINGER_ANIMATION_H
#define SLINGER_ANIMATION_H

#include <memory>
#include <vector>
#include <SFML/Graphics/Texture.hpp>
#include <map>

std::vector<sf::IntRect> getFrames(const sf::Texture &texture, unsigned int horizontalFrames);

class Animation {
    std::shared_ptr<sf::Texture> texture_;
    std::vector<sf::IntRect> frames_;
    int fps_;

public:
    explicit Animation(
        std::shared_ptr<sf::Texture> texture,
        uint fps,
        std::vector<sf::IntRect> frames
    );

    sf::IntRect getFrame(size_t frame);
    const std::shared_ptr<sf::Texture>& getTexture();
};


class AnimationManager {
    std::map<std::string, Animation> animations_;

    explicit AnimationManager(std::map<std::string, Animation> animations);
};


#endif //SLINGER_ANIMATION_H
