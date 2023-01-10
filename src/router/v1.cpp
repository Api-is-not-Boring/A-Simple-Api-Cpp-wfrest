#include <wfrest/HttpServer.h>

#include <cstdio>
#include <fstream>
#include <regex>
#include <string>
#include <wfrest/json.hpp>

#include "project.h"
#include "router.h"

using namespace wfrest;
using ordered_json = nlohmann::ordered_json;

pid_t process = getpid();

class Connection : public ordered_json {
    std::string l;
    std::regex p = std::regex { R"(^.+\s(\d+u).+(TCP)\s(\*:\d+|(.+:\d+)->(.+:\d+))\s\((\w+)\)$)",
        std::regex_constants::multiline };

public:
    explicit Connection(const std::string& line)
        : l(line)
    {
        std::smatch match;
        if (std::regex_search(line, match, p)) {
            (*this)["id"] = std::stoi(match[1].str());
            (*this)["protocol"] = match[2].str();
            (*this)["local"] = match[6] == "LISTEN" ? "LISTENING" : match[6].str();
            (*this)["remote"] = std::regex_replace(
                match[4].str().empty() ? match[3].str() : match[4].str(),
                std::regex("\\*"),
                "0.0.0.0");
            (*this)["state"] = match[5].str().empty() ? "0.0.0.0:0" : match[5].str();
        }
    }
};

void get_cons(HttpResp* resp)
{
    std::vector<ordered_json> connections;
    std::string command = "lsof -a -n -P -p " + std::to_string(process) + " -i tcp";
    std::unique_ptr<::FILE, decltype(&::pclose)> lsof(::popen(command.c_str(), "r"), &::pclose);
    if (lsof) {
        char* buffer = new char[256];
        while (::fgets(buffer, 256, lsof.get()) != nullptr) {
            std::string line(buffer);
            Connection c(line);
            if (!c.empty())
                connections.push_back(c);
        }
        delete[] buffer;
    }
    ordered_json json = {
        { "connections", connections }
    };
    resp->Json(json.dump());
}

void set_v1_bp(BluePrint& bp)
{
    bp.GET("/ping", [](const HttpReq* req, HttpResp* resp) {
        ordered_json json = {
            { "agent", req->header("User-Agent") },
            { "datetime", Timestamp::now().to_format_str("%a, %d %b %Y %T GMT") },
            { "message", "pong" }
        };
        resp->Json(json.dump());
    });

    bp.GET("/info", [](const HttpReq* req, HttpResp* resp) {
        ordered_json json = {
            { "project",
                {
                    { "name", NAME },
                    { "description", DESCRIPTION },
                    { "language", LANGUAGE },
                    { "version", VERSION },
                    { "url", URL },
                    { "git hash", GIT_COMMIT },
                    { "version", VERSION },
                } }
        };
        resp->Json(json.dump());
    });

    bp.GET("/connections", 1, [](const HttpReq* req, HttpResp* resp) {
        get_cons(resp);
    });
}
