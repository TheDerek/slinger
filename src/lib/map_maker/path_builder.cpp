//
// Created by derek on 22/10/20.
//

#include "path_builder.h"

const Regexer PathBuilder::COMMAND_REGEX = Regexer(R"(\s?([a-zA-Z])(?:\s([^a-zA-Z]+))?)");
const Regexer PathBuilder::COORDINATE_REGEX = Regexer(R"((-?\d+\.?\d*))");

PathBuilder::CommandList PathBuilder::getCommands(const std::string &svgPath) {
    auto list = PathBuilder::CommandList();

    for (const auto& commandStr : COMMAND_REGEX.search(svgPath)) {
        if (commandStr.groups().size() != 2) {
            throw std::runtime_error("Could not parse svg path: " + svgPath);
        }

        auto commandName = commandStr.groups().at(0);
        auto numbersStr = commandStr.groups().at(1);

        if (!COMMAND_NAMES.contains(commandName)) {
            throw std::runtime_error("Could not find svg path command: " + commandName);
        }

        auto command = COMMAND_NAMES.at(commandName);
        Arguments arguments;

        if (numbersStr.empty()) {
            arguments = std::monostate {};
        }
        else {
            const auto& numbers = COORDINATE_REGEX.search(numbersStr);

            // String contains a list of coordinates
            if (numbersStr.find(',') != std::string::npos) {
                PointList list;
                std::optional<float> lastCoord;
                for (const auto& x : numbers) {
                    auto number = std::atof(x.matchedString().c_str());

                    if (!lastCoord) {
                        lastCoord = number;
                    } else {
                        list.push_back(sf::Vector2f(lastCoord.value(), number));
                        lastCoord.reset();
                    }
                }
                arguments = list;
            } else {
                std::vector<float> floatList;
                for (const auto& x : numbers) {
                    auto number = std::atof(x.matchedString().c_str());
                    floatList.emplace_back(number);
                }
                arguments = floatList;
            }
        }

        list.emplace_back(command, arguments);
    }

    return list;
}

PointList PathBuilder::getPoints(const PathBuilder::CommandList &commands) {
    auto list = PointList();

    for (const auto& commandPair : commands) {
        Command command = commandPair.first;
        Arguments arguments = commandPair.second;
        //m 18.35551,3.2577749 v 3.621956 l -5.472546,0.019407 1.727002,-2.6997475 z

        if (command == Command::MoveTo || command == Command::RelativeMoveTo) {
            if (!std::holds_alternative<PointList>(arguments)) {
                throw std::runtime_error("Unsupported arguments for MoveTo");
            }

            auto points = std::get<PointList>(arguments);
            auto point = points.at(0);
            point.y *= -1.f;
            list.emplace_back(point);

            if (points.size() > 1) {
                command = command == Command::MoveTo ? Command::LineTo : Command::RelativeLineTo;
                arguments = PointList(points.begin() + 1, points.end());
            }
        }

        if (command == Command::LineTo) {
            if (!std::holds_alternative<PointList>(arguments)) {
                throw std::runtime_error("Unsupported arguments for LineTo");
            }

            auto points = std::get<PointList>(arguments);
            for (auto point : points)
            {
                point.y *= -1.f;
                list.emplace_back(point);
            }
        }

        if (command == Command::RelativeLineTo) {
            if (!std::holds_alternative<PointList>(arguments)) {
                throw std::runtime_error("Unsupported arguments for RelativeLineTo");
            }

            auto points = std::get<PointList>(arguments);
            for (auto point : points)
            {
                point.y *= -1.f;
                point = list.back() + point;
                list.emplace_back(point);
            }
        }

        if (command == Command::HorizontalLineTo) {
            if (!std::holds_alternative<std::vector<float>>(arguments)) {
                throw std::runtime_error("Unsupported arguments for HorizontalLineTo");
            }

            for (float x : std::get<std::vector<float>>(arguments)) {
                auto point = sf::Vector2f(x, list.back().y);
                list.emplace_back(point);
            }
        }

        if (command == Command::RelativeHorizontalLineTo) {
            if (!std::holds_alternative<std::vector<float>>(arguments)) {
                throw std::runtime_error("Unsupported arguments for RelativeHorizontalLineTo");
            }

            for (float x : std::get<std::vector<float>>(arguments)) {
                auto point = list.back() + sf::Vector2f(x, 0);
                list.emplace_back(point);
            }
        }

        if (command == Command::VerticalLineTo) {
            if (!std::holds_alternative<std::vector<float>>(arguments)) {
                throw std::runtime_error("Unsupported arguments for VerticalLineTo");
            }

            for (float x : std::get<std::vector<float>>(arguments)) {
                auto point = sf::Vector2f(list.back().x, -x);
                list.emplace_back(point);
            }
        }

        if (command == Command::RelativeVerticalLineTo) {
            if (!std::holds_alternative<std::vector<float>>(arguments)) {
                throw std::runtime_error("Unsupported arguments for RelativeVerticalLineTo");
            }

            for (float x : std::get<std::vector<float>>(arguments)) {
                auto point =  list.back() + sf::Vector2f(0, -x);
                list.emplace_back(point);
            }
        }

        if (command == Command::ClosePath || command == Command::RelativeClosePath) {
            if (!std::holds_alternative<std::monostate>(arguments)) {
                throw std::runtime_error("Unsupported arguments for ClosePath");
            }

            list.emplace_back(list.at(0));
        }
    }

    return list;
}

PointList PathBuilder::build(const std::string &string) {
    auto commands = getCommands(string);

    return getPoints(commands);
}