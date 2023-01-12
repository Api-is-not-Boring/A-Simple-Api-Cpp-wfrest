#include "middleware.h"
#include "models.h"
#include "router.h"

using namespace wfrest;
using namespace model::user;
using ordered_json = nlohmann::ordered_json;

void router::ApiRoutes::set_v3_bp(BluePrint& bp)
{
    Db::init();
    bp.GET("/auth", [](const HttpReq* req, HttpResp* resp) {
        resp->Json({ { "message", "[v3] -> 200 Login with Post Request" } });
    });

    bp.POST("/auth", [](const HttpReq* req, HttpResp* resp) {
        if (Db::authenticate(req->form()))
            resp->Json({ { "message", "Login Successful !!!" },
                { "token", Db::generate_token(::getenv("APP_SECRET")) } });
        else {
            resp->set_status(401);
            resp->Json({ { "message", "[v3] -> 401 Login Failed" } });
        }
    });

    bp.GET(
        "/check", [](const HttpReq* req, HttpResp* resp) {
            resp->Json({ { "message", "[v3] -> 200 Token Valid" } });
        },
        JwtAuth());

    bp.GET(
        "/secure", [](const HttpReq* req, HttpResp* resp) {
            resp->Json({ { "s3c5et", ::getenv("APP_SECRET") } });
        },
        JwtAuth());
}