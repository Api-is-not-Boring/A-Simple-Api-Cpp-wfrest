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

std::regex pattern(
    R"(^.+\s(\d+u).+(TCP)\s(\*:\d+|(.+:\d+)->(.+:\d+))\s\((\w+)\)$)",
    std::regex_constants::multiline);

std::vector<ordered_json> get_cons()
{
    std::vector<ordered_json> connections;
    std::string command = "lsof -a -n -P -p " + std::to_string(process) + " -i tcp";
    std::unique_ptr<::FILE, decltype(&::pclose)> pipe(::popen(command.c_str(), "r"), &::pclose);
    if (pipe) {
        char buffer[256];
        while (::fgets(buffer, sizeof(buffer), pipe.get()) != nullptr) {
            std::string line(buffer);
            std::smatch match;
            if (std::regex_search(line, match, pattern, std::regex_constants::match_not_eol)) {
                ordered_json connection;
                connection["id"] = std::stoi(match[1].str());
                connection["protocol"] = match[2].str();
                connection["type"] = match[6] == "LISTEN" ? "LISTENING" : match[6].str();
                connection["local"] = std::regex_replace(
                    match[4].str().empty() ? match[3].str() : match[4].str(),
                    std::regex("\\*"),
                    "0.0.0.0");
                connection["remote"] = match[5].str().empty() ? "0.0.0.0:0" : match[5].str();
                connections.push_back(connection);
            }
        }
    }
    return connections;
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
        ordered_json json = { { "project",
            {
                { "name", NAME },
                { "description", DESCRIPTION },
                { "language", LANGUAGE },
                { "version", VERSION },
                { "url", URL },
                { "git hash", GIT_COMMIT },
                { "version", VERSION },
            } } };
        resp->Json(json.dump());
    });

    bp.GET("/connections", [](const HttpReq* req, HttpResp* resp) {
        ordered_json json = { { "connections", get_cons() } };
        resp->Json(json.dump());
    });
}
