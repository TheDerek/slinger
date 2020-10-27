#include "animation.h"

Animation::Animation(
    std::shared_ptr<sf::Texture> texture,
    uint fps,
    std::vector<sf::IntRect> frames
):
    texture_(texture),
    fps_(fps),
    frames_(std::move(frames))
{

}

sf::IntRect Animation::getFrame(size_t frame) {
    return frames_.at(frame);
}

const std::shared_ptr<sf::Texture> &Animation::getTexture() {
    return texture_;
}

AnimationManager::AnimationManager(std::map<std::string, Animation> animations):
    animations_(std::move(animations))
{

}

std::vector<sf::IntRect> getFrames(const sf::Texture &texture, unsigned int horizontalFrames) {
    unsigned int frameWidth = texture.getSize().x / horizontalFrames;
    unsigned int frameHeight = texture.getSize().y;

    std::vector<sf::IntRect> frames;
    for (int i = 0; i < horizontalFrames; i++) {
        frames.emplace_back(sf::IntRect(
            frameWidth * i, 0, frameWidth, frameHeight
        ));
    }

    return frames;
}
