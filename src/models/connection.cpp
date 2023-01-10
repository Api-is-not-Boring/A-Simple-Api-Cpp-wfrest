#include "models.h"
#include <fstream>
#include <regex>
#include <string>
#include <unistd.h>

using namespace model::connection;
using ordered_json = nlohmann::ordered_json;

pid_t process = getpid();

Connections::Connection::Connection(std::string& line)
    : l(line)
    , p()
{
    this->p = std::regex { R"(^.+\s(\d+u).+(TCP)\s(\*:\d+|(.+:\d+)->(.+:\d+))\s\((\w+)\)$)",
        std::regex_constants::multiline };
    std::smatch match;
    if (std::regex_search(line, match, p)) {
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
    : command("lsof -a -n -P -p " + std::to_string(process) + " -i tcp")
    , lsof(::popen(command.c_str(), "r"), &::pclose)
{
    if (lsof) {
        char* buffer = new char[256];
        while (::fgets(buffer, 256, lsof.get()) != nullptr) {
            std::string line(buffer);
            Connection c(line);
            if (!c.empty())
                this->push_back(c);
        }
        delete[] buffer;
    }
}