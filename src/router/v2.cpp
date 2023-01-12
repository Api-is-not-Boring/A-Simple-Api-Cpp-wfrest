#include "models.h"
#include "router.h"

using namespace wfrest;
using namespace model::car;
using ordered_json = nlohmann::ordered_json;

static void not_found(HttpResp* resp)
{
    resp->set_status(404);
    resp->Json(ordered_json {
        { "message", "[v2] -> Car not found" } }
                   .dump());
}

static void missing_id(HttpResp* resp)
{
    resp->set_status(400);
    resp->Json(ordered_json {
        { "message", "[v2] -> Missing id parameter" } }
                   .dump());
}

static void find_car(const HttpReq* req, HttpResp* resp, int id)
{
    if (Db::car_exists(id))
        resp->Json(Db::get_car(id).dump());
    else
        not_found(resp);
}

static void update_car(HttpResp* resp, Car car, std::string const& method)
{
    Db::car_update(car);
    resp->Json(ordered_json {
        { "method", "[v2] -> " + method },
        { "car", car } }
                   .dump());
}

static void delete_car(HttpResp* resp, int id)
{
    Db::car_delete(id);
    resp->set_status(200);
    resp->Json(ordered_json {
        { "method", "[v2] -> DELETE with Path Parameter" },
        { "deleted_id", id },
        { "total", Db::count() } }
                   .dump());
}

void router::ApiRoutes::set_v2_bp(BluePrint& bp)
{
    Db::init();
    bp.GET("/cars/{id}", [](const HttpReq* req, HttpResp* resp) {
        if (req->has_param("id")) {
            find_car(req, resp, req_id(req->param("id")));
        } else
            missing_id(resp);
    });

    bp.PUT("/cars/{id}", [](const HttpReq* req, HttpResp* resp) {
        if (req->has_param("id")) {
            int id = req_id(req->param("id"));
            if (!Db::car_exists(id))
                not_found(resp);
            else {
                Car car = req->json().get<Car>();
                car.id = id;
                update_car(resp, car, "PUT with JSON and Query Parameter");
            }
        } else
            missing_id(resp);
    });

    bp.DELETE("/cars/{id}", [](const HttpReq* req, HttpResp* resp) {
        if (req->has_param("id")) {
            int id = req_id(req->param("id"));
            if (Db::car_exists(id))
                delete_car(resp, id);
            else
                not_found(resp);
        } else
            missing_id(resp);
    });
}

void router::ApiRoutes::fix_v2_route(HttpServer& server)
{
    server.GET("/api/v2/cars", [](const HttpReq* req, HttpResp* resp) {
        if (req->has_query("id")) {
            find_car(req, resp, req_id(req->query("id")));
        } else {
            std::vector<ordered_json> cars = Db::all();
            resp->Json(ordered_json {
                { "total", cars.size() },
                { "cars", cars } }
                           .dump());
        }
    });

    server.POST("/api/v2/cars", [](const HttpReq* req, HttpResp* resp) {
        Car car = req->json().get<Car>();
        Db::car_add(car);
        resp->set_status(201);
        resp->Json(ordered_json {
            { "method", "[v2] -> POST with JSON" },
            { "car", Db::get_car(Db::last_insert_id()) } }
                       .dump());
    });

    server.PUT("/api/v2/cars", [](const HttpReq* req, HttpResp* resp) {
        if (req->has_query("id")) {
            int id = req_id(req->query("id"));
            if (Db::car_exists(id)) {
                Car car = req->json().get<Car>();
                car.id = id;
                update_car(resp, car, "PUT with JSON and Query Parameter");
            } else
                not_found(resp);
        } else if (req->json().contains("id")) {
            Car car = req->json().get<Car>();
            if (Db::car_exists(car.id)) {
                update_car(resp, car, "PUT with JSON");
            } else
                not_found(resp);
        } else
            missing_id(resp);
    });

    server.DELETE("/api/v2/cars", [](const HttpReq* req, HttpResp* resp) {
        if (req->has_query("id")) {
            int id = req_id(req->query("id"));
            if (Db::car_exists(id))
                delete_car(resp, id);
            else
                not_found(resp);
        } else
            missing_id(resp);
    });
}
