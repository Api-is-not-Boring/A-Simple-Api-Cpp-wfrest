#include "router.h"
#include "wfrest/HttpServer.h"
#include "wfrest/json.hpp"
#include "project.h"

using namespace wfrest;
using ordered_json = nlohmann::ordered_json;

static ordered_json get_gonnections(HttpServer *server, HttpServerTask *task)
{
    ordered_json json;
    json["connections"] = server->get_conn_count();
    return json;
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
                {"connections", {
                        {"total", 0},
                        {"active", 0},
                        {"idle", 0},
                        {"waiting", 0},
                }}
        };
        resp->Json(json.dump());
    });
}
