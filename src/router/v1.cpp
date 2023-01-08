#include <boost/process.hpp>
#include <boost/regex.hpp>
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

void get_connections()
{
    bp::ipstream pipe_stream;
    std::string command = "lsof -a -n -P -p " + std::to_string(process) + " -i tcp";
    bp::system(command, bp::std_out > pipe_stream, bp::std_err > stderr);
    boost::regex expr{R"(^.+\s(\d+u).+(TCP)\s(\*:\d+|(.+:\d+)->(.+:\d+))\s\((\w+)\)$)"};
    boost::smatch match;
    std::string line;
    while (pipe_stream && std::getline(pipe_stream, line) && !line.empty())
        if (boost::regex_search(line, match, expr))
        {
            if (match.size() == 7)
            {
                std::cout << "Connection: " << match[1] << " " << match[2] << " " << match[3] << " " << match[6] << std::endl;
            } else
            {
                std::cout << "Connection: " << match[1] << " " << match[2] << " " << match[3] << " " << match[6] << std::endl;
            }
        }
//        std::cout << line << std::endl;
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
        get_connections();
        ordered_json json = {
                {"connections", {
                        {"pid", process},
                }}
        };
        resp->Json(json.dump());
    });
}
