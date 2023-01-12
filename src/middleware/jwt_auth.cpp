#include "middleware.h"
#include <cstdlib>
#include <jwt-cpp/jwt.h>
#include <wfrest/json.hpp>

using namespace wfrest;

bool token_valid(std::string token, std::string s)
{
    try {
        auto decoded = jwt::decode(token);
        auto verifier = jwt::verify()
                            .allow_algorithm(jwt::algorithm::hs512(std::move(s)));
        verifier.verify(decoded);
        return true;
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return false;
    }
}

bool JwtAuth::before(const HttpReq* req, HttpResp* resp)
{
    if (req->has_header("Authorization")) {
        std::string token = req->header("Authorization");
        token.find("Bearer ") != std::string::npos ? token.erase(0, 7) : token;
        if (token_valid(token, ::getenv("APP_SECRET")))
            return true;
        else {
            resp->set_status(401);
            resp->Json({ { "message", "[v3] -> 401 Token Invalid" } });
            return false;
        }
    }
}

bool JwtAuth::after(const HttpReq* req, HttpResp* resp) { return true; }
