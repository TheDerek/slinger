//
// Created by derek on 22/10/20.
//

#ifndef SLINGER_PATH_BUILDER_H
#define SLINGER_PATH_BUILDER_H

#include <variant>
#include <map>

#include <entt/entity/registry.hpp>
#include <SFML/Main.hpp>
#include <SFML/System/Vector2.hpp>

#include "regexer.h"


namespace {
    using PointList = std::vector<sf::Vector2f>;
}

/**
 * Fetches a list of vertices from an svg path
 */
class PathBuilder {
    enum class Command: char {
        MoveTo, RelativeMoveTo, LineTo, HorizontalLineTo, VerticalLineTo,
        RelativeLineTo, RelativeHorizontalLineTo, RelativeVerticalLineTo, ClosePath, RelativeClosePath
    };

    const static inline std::map<std::string, Command> COMMAND_NAMES = {
        {"M", Command::MoveTo},
        {"m", Command::RelativeMoveTo},
        {"L", Command::LineTo},
        {"H", Command::HorizontalLineTo},
        {"V", Command::VerticalLineTo},
        {"l", Command::RelativeLineTo},
        {"h", Command::RelativeHorizontalLineTo},
        {"v", Command::RelativeVerticalLineTo},
        {"Z", Command::ClosePath},
        {"z", Command::RelativeClosePath}
    };

    using Arguments = std::variant<std::monostate, std::vector<float>, std::vector<sf::Vector2f>>;
    using CommandList = std::vector<std::pair<Command, Arguments>>;

    static CommandList getCommands(const std::string& svgPath);
    static PointList getPoints(const CommandList& list);
    static bool isPointList(Command command, const std::string &arguments);

public:
    const static Regexer COMMAND_REGEX;
    const static Regexer COORDINATE_REGEX;

    static PointList build(const std::string& string);
};


#endif //SLINGER_PATH_BUILDER_H
