#include "middleware.h"

using namespace wfrest;

bool ServerHeader::before(const HttpReq* req, HttpResp* resp)
{
    resp->add_header_pair("Server", "wfrest");
    return true;
}

bool ServerHeader::after(const HttpReq* req, HttpResp* resp) { return true; }