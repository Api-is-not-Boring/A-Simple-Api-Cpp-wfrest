#ifndef A_SIMPLE_API_CPP_WFREST_ROUTER_H
#define A_SIMPLE_API_CPP_WFREST_ROUTER_H

#include <wfrest/HttpServer.h>
#include <wfrest/BluePrint.h>
#include "models.h"

namespace router {
    using BluePrint = wfrest::BluePrint;
    using HttpServer = wfrest::HttpServer;
    class ApiRoutes : public BluePrint {
        BluePrint v1;
        BluePrint v2;
        static void set_v1_bp(BluePrint& bp);
        static void set_v2_bp(BluePrint& bp);
        static void fix_v2_route(HttpServer& server);
    public:
        explicit ApiRoutes(HttpServer& server) {
            set_v1_bp(this->v1);
            this->add_blueprint(this->v1, "/v1");
            set_v2_bp(this->v2);
            this->add_blueprint(this->v2, "/v2");
            fix_v2_route(server);
        }
        static int req_id(std::string const& id) { return std::stoi(id); }
    };
}

#endif //A_SIMPLE_API_CPP_WFREST_ROUTER_H
