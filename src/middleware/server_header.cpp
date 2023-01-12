#include "middleware.h"


bool middleware::ServerHeader::before(const HttpReq* req, HttpResp* resp)
{
    resp->add_header_pair("Server", "wfrest");
    return true;
}

bool middleware::ServerHeader::after(const HttpReq* req, HttpResp* resp) { return true; }