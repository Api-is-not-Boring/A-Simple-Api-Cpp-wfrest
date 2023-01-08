#include "include/router.h"
#include "wfrest/json.hpp"
#include "project.h"

using namespace wfrest;
using ordered_json = nlohmann::ordered_json;

void set_v1_bp(BluePrint &bp)
{
    bp.GET("/ping", [](const HttpReq *req, HttpResp *resp)
    {
        ordered_json json = {
                {"agent", req->header("User-Agent")},
                {"time",  Timestamp::now().to_format_str("%a, %d %b %Y %T GMT")},
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
}
