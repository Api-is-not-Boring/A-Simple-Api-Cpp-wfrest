#include "workflow/WFFacilities.h"
#include "wfrest/HttpServer.h"
#include "wfrest/json.hpp"
#include "spdlog/spdlog.h"
#include "router.h"

using namespace wfrest;

static WFFacilities::WaitGroup wait_group(1);

void sig_handler(int signo)
{
    wait_group.done();
}

int main()
{
    signal(SIGINT, sig_handler);

    HttpServer app;

    app.track([](HttpTask *server_task) {
        HttpResp *resp = server_task->get_resp();
        HttpReq *req = server_task->get_req();
        auto *task = dynamic_cast<HttpServerTask *>(server_task);
        Timestamp current_time = Timestamp::now();
        std::string fmt_time = current_time.to_format_str();
        spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [WFREST] %v");
        spdlog::info("| [{}] {} {} from {}:{}",
                     resp->get_status_code(),
                     req->get_method(),
                     req->current_path().c_str(),
                     task->peer_addr().c_str(),
                     task->peer_port());
    });

    // curl -v http://ip:port/json1
    app.GET("/json1", [](const HttpReq *req, HttpResp *resp)
    {
        Json json;
        json["test"] = 123;
        json["json"] = "test json";
        resp->Json(json);
    });

    // curl -v http://ip:port/json2
    app.GET("/json2", [](const HttpReq *req, HttpResp *resp)
    {
        std::string valid_text = R"(
        {
            "numbers": [1, 2, 3]
        }
        )";
        resp->Json(valid_text);
    });

    // curl -v http://ip:port/json3
    app.GET("/json3", [](const HttpReq *req, HttpResp *resp)
    {
        std::string invalid_text = R"(
        {
            "strings": ["extra", "comma", ]
        }
        )";
        resp->Json(invalid_text);
    });

    // recieve json
    //   curl -X POST http://ip:port/json4
    //   -H 'Content-Type: application/json'
    //   -d '{"login":"my_login","password":"my_password"}'
    app.POST("/json4", [](const HttpReq *req, HttpResp *resp)
    {
        if (req->content_type() != APPLICATION_JSON)
        {
            resp->String("NOT APPLICATION_JSON");
            return;
        }
        fprintf(stderr, "Json : %s", req->json().dump(4).c_str());
    });

    BluePrint admin_bp;
    set_v1_bp(admin_bp);

    app.register_blueprint(admin_bp, "/admin");

    if (app.start(8000) == 0)
    {
        app.list_routes();
        app.print_node_arch();
        wait_group.wait();
        app.stop();
    } else
    {
        fprintf(stderr, "Cannot start server");
        exit(1);
    }
    return 0;
}
