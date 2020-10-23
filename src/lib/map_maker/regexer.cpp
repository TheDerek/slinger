
#include "regexer.h"

#include <regex>
#include <utility>


std::vector<Match> Regexer::search(const std::string& string) const {
    std::string searchString = string;
    std::vector<Match> matches;

    auto matchResults = std::smatch{};
    while(std::regex_search(searchString, matchResults, regex_))
    {
        matches.emplace_back(Match(matchResults));


        // Remove the captured string from the path and try again
        searchString = matchResults.suffix();
    }

    return matches;
}

Regexer::Regexer(const std::string &regex): regex_(regex) {

}

Match::Match(const std::smatch &results): matchedString_(results.str()) {
    bool firstResult = true;
    for (auto group : results) {
        if (firstResult) {
            firstResult = false;
        } else {
            groups_.emplace_back(group);
        }
    }
}

const std::vector<std::string> Match::groups() const {
    return groups_;
}

const std::string& Match::matchedString() const {
    return matchedString_;
}
