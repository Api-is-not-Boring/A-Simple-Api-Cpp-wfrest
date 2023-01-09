#include <boost/process.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <string>
#include <iostream>
#include "router.h"
#include "wfrest/HttpServer.h"
#include "wfrest/json.hpp"
#include "project.h"

using namespace wfrest;
using ordered_json = nlohmann::ordered_json;
namespace bp = boost::process;

pid_t process = getpid();

std::vector<ordered_json> get_connections()
{
    std::vector<ordered_json> connections;
    bp::ipstream pipe_stream;
    std::string command = "lsof -a -n -P -p " + std::to_string(process) + " -i tcp";
    bp::child c(command, bp::std_out > pipe_stream, bp::std_err > stderr);
    boost::regex expr{R"(^.+\s(\d+u).+(TCP)\s(\*:\d+|(.+:\d+)->(.+:\d+))\s\((\w+)\)$)"};
    boost::smatch match;
    std::string line;
    while (pipe_stream && std::getline(pipe_stream, line) && !line.empty())
        if (boost::regex_search(line, match, expr))
        {
            std::string id = match[1].str(); id.pop_back();
            std::string protocol = match[2].str();
            std::string type = match[6].str() == "LISTEN" ? "LISTENING" : match[6].str();
            std::string local = match[4].str().empty() ? match[3].str() : match[4].str();
            boost::replace_first(local, "*", "0.0.0.0");
            std::string remote = match[5].str().empty() ? "0.0.0.0:0" : match[5].str();
            ordered_json connection {
                    {"id", boost::lexical_cast<int>(id)},
                    {"protocol", protocol},
                    {"type", type},
                    {"local", local},
                    {"remote", remote}
            };
            connections.push_back(connection);
        }
    c.wait();
    return connections;
}

void set_v1_bp(BluePrint &bp)
{
    bp.GET("/ping", [](const HttpReq *req, HttpResp *resp)
    {
        ordered_json json = {
                {"agent", req->header("User-Agent")},
                {"datetime",  Timestamp::now().to_format_str("%a, %d %b %Y %T GMT")},
                {"message", "pong"}
        };
        resp->Json(json.dump());
    });

    bp.GET("/info", [](const HttpReq *req, HttpResp *resp)
    {
        ordered_json json = {
                {"project", {
                        {"name", NAME},
                        {"description", DESCRIPTION},
                        {"language", LANGUAGE},
                        {"version", VERSION},
                        {"url", URL},
                        {"git hash", GIT_COMMIT},
                        {"version", VERSION},
                }}
        };
        resp->Json(json.dump());
    });
    //TODO: get client ip
    bp.GET("/connections", [](const HttpReq *req, HttpResp *resp)
    {
        ordered_json json = {
                {"connections", get_connections()}
        };
        resp->Json(json.dump());
    });
}
