#include <string>
#include <regex>
#include "models.h"


using namespace model::connection;
using ordered_json = nlohmann::ordered_json;

Connections::Connection::Connection(const std::shared_ptr<std::array<char, 256>>& b)
    : l(b->begin(), b->end())
{
    this->p = std::regex { R"(^.+\s(\d+u).+(TCP)\s(\*:\d+|(.+:\d+)->(.+:\d+))\s\((\w+)\)$)",
        std::regex_constants::multiline };
    std::smatch match;
    if (std::regex_search(l, match, p)) {
        (*this)["id"] = std::stoi(match[1].str());
        (*this)["protocol"] = match[2].str();
        (*this)["type"] = match[6] == "LISTEN" ? "LISTENING" : match[6].str();
        (*this)["local"] = std::regex_replace(
            match[4].str().empty() ? match[3].str() : match[4].str(),
            std::regex("\\*"),
            "0.0.0.0");
        (*this)["remote"] = match[5].str().empty() ? "0.0.0.0:0" : match[5].str();
    }
}

Connections::Connections()
    : lsof(::popen(command.c_str(), "r"), &::pclose)
{
    if (lsof) {
        while (::fgets(b->data(), int(b->size()), lsof.get()) != nullptr) {
            if (std::string(b->begin(), b->end()).find("COMMAND") == std::string::npos) {
                this->push_back(Connection(b));
            }
        }
    }
}