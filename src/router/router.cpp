#include "include/router.h"
#include "wfrest/json.hpp"

inline void set_v1_bp(BluePrint &bp)
{
    bp.GET("/ping", [](const HttpReq *req, HttpResp *resp)
    {
        Json json = {
                {"message", "pong"}
        };
    });
}
