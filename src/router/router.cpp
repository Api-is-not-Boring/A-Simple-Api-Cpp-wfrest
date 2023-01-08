#include "include/router.h"
#include "wfrest/json.hpp"

using namespace wfrest;

void set_v1_bp(BluePrint &bp)
{
    bp.GET("/ping", [](const HttpReq *req, HttpResp *resp)
    {
        Json json = {
                {"agent", req->header("User-Agent")},
                {"time",  Timestamp::now().to_format_str("%Y-%m-%d %H:%M:%S.%e")},
                {"message", "pong"}
        };
        resp->Json(json);
    });
}
