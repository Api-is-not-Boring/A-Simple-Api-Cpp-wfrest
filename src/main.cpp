#include "workflow/WFFacilities.h"
#include "wfrest/HttpServer.h"
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

    BluePrint v1;
    set_v1_bp(v1);

    app.register_blueprint(v1, "/api/v1");

    if (app.start(8000) == 0)
    {
        app.list_routes();
        wait_group.wait();
        app.stop();
    } else
    {
        fprintf(stderr, "Cannot start server");
        exit(1);
    }
    return 0;
}
