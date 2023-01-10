#include <wfrest/HttpServer.h>

#include <fstream>

#include "models.h"
#include "project.h"
#include "router.h"

using namespace wfrest;
using namespace model::connection;
using ordered_json = nlohmann::ordered_json;

void get_cons(HttpResp* resp)
{
    ordered_json json = {
        { "connections", Connections() }
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
