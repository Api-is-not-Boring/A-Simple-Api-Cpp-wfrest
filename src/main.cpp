#include <csignal>
#include <cstdlib>
#include <fmt/core.h>
#include <nanoid/nanoid.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <wfrest/HttpServer.h>
#include <workflow/WFFacilities.h>

#include <string>

#include "middleware.h"
#include "router.h"

using namespace wfrest;

using wait_group = WFFacilities::WaitGroup ;

void sig_handler(int signum) {
    if (signum == SIGINT) {
        spdlog::warn("SIGINT received. Exiting...");
        exit(0);
    } else if (signum == SIGTERM) {
        spdlog::warn("SIGTERM received. Exiting...");
        exit(0);
    }
}

int main()
{
    wait_group wg(1);
    signal(SIGINT, sig_handler);

    HttpServer app;

    auto logger = spdlog::stdout_color_mt("wfrest");
    logger->set_pattern("[%Y-%m-%d %T] [ %n - %P ] [%^%l%$] %v");

    app.track([&logger](HttpTask* server_task) {
        HttpResp const* resp = server_task->get_resp();
        int code = std::stoi(resp->get_status_code());
        HttpReq const* req = server_task->get_req();
        auto const* task = dynamic_cast<HttpServerTask*>(server_task);
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

    app.Use(middleware::ServerHeader());

    std::string app_secret = nanoid::generate(32);
    setenv("APP_SECRET", app_secret.c_str(), 1);
    router::ApiRoutes api_routes(app);

    app.register_blueprint(api_routes, "/api");

    if (app.start(8000) == 0) {
        app.print_node_arch();
        wg.wait();
        getchar();
        app.stop();
    } else {
        logger->error("Cannot start server");
        wg.done();
        exit(1);
    }
    return 0;
}
