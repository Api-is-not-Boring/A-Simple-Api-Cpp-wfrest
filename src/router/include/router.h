#ifndef A_SIMPLE_API_CPP_WFREST_ROUTER_H
#define A_SIMPLE_API_CPP_WFREST_ROUTER_H

#include <wfrest/BluePrint.h>

using namespace wfrest;
using ordered_json = nlohmann::ordered_json;

void set_v1_bp(BluePrint &bp);

void set_v2_bp(BluePrint &bp);

void set_v3_bp(BluePrint &bp);

#endif //A_SIMPLE_API_CPP_WFREST_ROUTER_H
