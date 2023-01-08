#include "boost/process/system.hpp"
#include "boost/process/io.hpp"
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
    std::string command = "lsof -a -n -P -p " + std::to_string(process) + " -i tcp";
    bp::system(command, bp::std_out > stdout, bp::std_err > stderr);
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
