#include "models.h"

using model::car::Car;
auto car_storage = make_storage(":memory:",
    make_table("cars",
        make_column("id", &Car::id, primary_key().autoincrement()),
        make_column("name", &Car::name),
        make_column("price", &Car::price)));

std::vector<Car> init_cars = {
    Car { -1, "Audi", 52642 },
    Car { -1, "Mercedes", 57127 },
    Car { -1, "Skoda", 9000 },
    Car { -1, "Volvo", 29000 },
    Car { -1, "Bentley", 350000 },
    Car { -1, "Citroen", 21000 },
    Car { -1, "Hummer", 41400 },
    Car { -1, "Volkswagen", 21600 }
};

void model::car::to_json(ordered_json& j, const Car& c)
{
    j = ordered_json {
        { "id", c.id },
        { "name", c.name },
        { "price", c.price }
    };
}

void model::car::Db::init()
{
    car_storage.sync_schema();
    car_storage.remove_all<Car>();
    car_storage.transaction([&] {
        for (auto& car : init_cars) {
            car_storage.insert(car);
        }
        return true;
    });
}

void model::car::Db::db_reset()
{
    car_storage.remove_all<Car>();
    car_storage.transaction([&] {
        for (auto& car : init_cars) {
            car_storage.insert(car);
        }
        return true;
    });
}

std::vector<ordered_json> model::car::Db::all()
{
    std::vector<Car> cars = car_storage.get_all<Car>();
    std::vector<ordered_json> json_cars;
    for (auto& car : cars) {
        json_cars.emplace_back(car);
    }
    return json_cars;
}

void model::car::Db::car_add(const Car& car)
{
    car_storage.insert(car);
}
