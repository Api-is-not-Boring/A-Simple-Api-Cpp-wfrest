#include <wfrest/HttpServer.h>

#include "models.h"
#include "router.h"

using namespace wfrest;
using namespace model::car;
using ordered_json = nlohmann::ordered_json;

void set_v2_bp(BluePrint& bp)
{
    Db::init();
    bp.GET("/cars", [](const HttpReq* req, HttpResp* resp) {
        auto cars = Db::all();
        ordered_json json = {
            { "total", cars.size() },
            { "cars", cars }
        };
        resp->Json(json.dump());
    });
}
