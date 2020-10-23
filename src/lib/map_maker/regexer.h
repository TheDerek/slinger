#ifndef SLINGER_REGEXER_H
#define SLINGER_REGEXER_H

#include <vector>
#include <string>
#include <regex>

class Match {
    std::vector<std::string> groups_;
    std::string matchedString_;
public:
    explicit Match(const std::smatch& results);
    const std::string& matchedString() const;
    const std::vector<std::string> groups() const;
};

class Regexer {
    const std::regex regex_;

public:
    explicit Regexer(const std::string& regex);
    std::vector<Match> search(const std::string& searchString) const;
};
#endif //SLINGER_REGEXER_H
