#ifndef A_SIMPLE_API_CPP_WFREST_MIDDLEWARE_H
#define A_SIMPLE_API_CPP_WFREST_MIDDLEWARE_H

#include <wfrest/HttpServer.h>
#include <wfrest/Aspect.h>

namespace middleware {
    using Aspect = wfrest::Aspect;
    using HttpReq = wfrest::HttpReq;
    using HttpResp = wfrest::HttpResp;

    class ServerHeader : public Aspect
    {
    public:
        bool before(const HttpReq *req, HttpResp *resp) override;

        bool after(const HttpReq *req, HttpResp *resp) override;

    };

    class JwtAuth : public Aspect
    {
    public:
        bool before(const HttpReq *req, HttpResp *resp) override;

        bool after(const HttpReq *req, HttpResp *resp) override;

    };
}


#endif //A_SIMPLE_API_CPP_WFREST_MIDDLEWARE_H
