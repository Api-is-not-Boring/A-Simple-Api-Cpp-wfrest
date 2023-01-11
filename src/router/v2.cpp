#include "models.h"
#include "router.h"

using namespace wfrest;
using namespace model::car;
using ordered_json = nlohmann::ordered_json;

void router::ApiRoutes::set_v2_bp(BluePrint& bp)
{
    Db::init();
    bp.GET("/cars/{id}", [](const HttpReq* req, HttpResp* resp) {
        if (req->has_param("id")) {
            int id = std::stoi(req->param("id"));
            auto car = Db::get_car(id);
            if (!car.empty()) {
                resp->Json(car.dump());
            } else {
                resp->Json(ordered_json {
                        { "error", "Car not found" }
                }.dump());
            }
        } else {
            resp->set_status(400);
            resp->Json(ordered_json {
                    { "error", "Missing id parameter" }
            }.dump());
        }
    });

    bp.PUT("/cars/{id}", [](const HttpReq* req, HttpResp* resp) {
        if (req->has_param("id")) {
            int id = std::stoi(req->param("id"));
            auto body = req->json();
            auto car = body.get<Car>();
            car.id = id;
            Db::car_update(car);
            resp->Json(ordered_json {
                    { "message", "Car updated" },
                    { "car", car }
            }.dump());
        } else {
            resp->set_status(400);
            resp->Json(ordered_json {
                    { "error", "Missing id parameter" }
            }.dump());
        }
    });
}

void router::ApiRoutes::fix_v2_route(HttpServer &server) {
    server.GET("/api/v2/cars", [](const HttpReq* req, HttpResp* resp) {
        if (req->has_query("id")) {
            auto id = req->query("id");
            auto car = Db::get_car(std::stoi(id));
            if (!car.empty()) {
                resp->Json(car.dump());
            } else {
                resp->Json(ordered_json {
                        { "error", "Car not found" }
                }.dump());
            }
        } else {
            auto cars = Db::all();
            ordered_json json = {
                    { "total", cars.size() },
                    { "cars", cars }
            };
            resp->Json(json.dump());
        }
    });

    server.POST("/api/v2/cars", [](const HttpReq* req, HttpResp* resp) {
        auto body = req->json();
        auto car = body.get<Car>();
        car = Db::car_add(car);
        resp->set_status(201);
        resp->Json(ordered_json {
                { "message", "Car added" },
                { "car", car }
        }.dump());
    });

    server.PUT("/api/v2/cars", [](const HttpReq* req, HttpResp* resp) {
        if (req->has_query("id")) {
            auto id = req->query("id");
            auto body = req->json();
            auto car = body.get<Car>();
            car.id = std::stoi(id);
            Db::car_update(car);
            resp->Json(ordered_json {
                    { "message", "Car updated" },
                    { "car", car }
            }.dump());
        } else if (req->json().contains("id")) {
            auto body = req->json();
            auto car = body.get<Car>();
            Db::car_update(car);
            resp->Json(ordered_json {
                    { "message", "Car updated" },
                    { "car", car }
            }.dump());
        } else {
            resp->set_status(400);
            resp->Json(ordered_json {
                    { "error", "Missing id parameter" }
            }.dump());
        }
    });
}