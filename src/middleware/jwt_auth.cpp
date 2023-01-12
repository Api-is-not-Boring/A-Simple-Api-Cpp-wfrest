#include "middleware.h"
#include <cstdlib>
#include <jwt-cpp/jwt.h>
#include <wfrest/json.hpp>

bool token_valid(std::string const &token, std::string s, wfrest::HttpResp* resp)
{
    try {
        auto decoded = jwt::decode(token);
        auto verifier = jwt::verify()
                            .allow_algorithm(jwt::algorithm::hs512(std::move(s)));
        verifier.verify(decoded);
        return true;
    } catch (const std::exception& e) {
        resp->set_status(401);
        std::string msg = std::string("[v3] -> 401 ") + e.what();
        resp->Json({ { "message", msg } });
        std::cout << e.what() << std::endl;
        return false;
    }
}

bool middleware::JwtAuth::before(const HttpReq* req, HttpResp* resp)
{
    if (req->has_header("Authorization")) {
        std::string token = req->header("Authorization");
        token.find("Bearer ") != std::string::npos ? token.erase(0, 7) : token;
        return token_valid(token, ::getenv("APP_SECRET"), resp);
    } else {
        resp->set_status(401);
        resp->Json({ { "message", "[v3] -> 401 No Authorization Header" } });
        return false;
    }
}

bool middleware::JwtAuth::after(const HttpReq* req, HttpResp* resp) { return true; }
