#include <fmt/core.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <wfrest/HttpServer.h>
#include <workflow/WFFacilities.h>

#include <string>

#include "middleware.h"
#include "router.h"

using namespace wfrest;

static WFFacilities::WaitGroup wait_group(1);

void sig_handler(int signo) { wait_group.done(); }

auto logger = spdlog::stdout_color_mt("wfrest");

int main()
{
    signal(SIGINT, sig_handler);

    HttpServer app;

    logger->set_pattern("[%Y-%m-%d %T] [ %n - %P ] [%^%l%$] %v");

    app.track([](HttpTask* server_task) {
        HttpResp* resp = server_task->get_resp();
        int code = std::stoi(resp->get_status_code());
        HttpReq* req = server_task->get_req();
        auto* task = dynamic_cast<HttpServerTask*>(server_task);
        Timestamp current_time = Timestamp::now();
        std::string fmt_time = current_time.to_format_str();
        std::string msg = fmt::format("| [{}] {} {} <- {}:{}", resp->get_status_code(),
            req->get_method(), req->current_path().c_str(),
            task->peer_addr().c_str(), task->peer_port());
        if (code == 500)
            logger->critical(msg);
        else if (code >= 400)
            logger->error(msg);
        else if (code >= 300)
            logger->warn(msg);
        else
            logger->info(msg);
    });

    app.Use(ServerHeader());

    BluePrint v1;
    set_v1_bp(v1);

    app.register_blueprint(v1, "/api/v1");

    if (app.start(8000) == 0) {
        app.list_routes();
        wait_group.wait();
        getchar();
        app.stop();
    } else {
        logger->error("Cannot start server");
        exit(1);
    }
    return 0;
}
