#ifndef A_SIMPLE_API_CPP_WFREST_MODELS_H
#define A_SIMPLE_API_CPP_WFREST_MODELS_H

#include <regex>
#include <utility>
#include <sqlite_orm/sqlite_orm.h>
#include <wfrest/json.hpp>

using namespace sqlite_orm;
using ordered_json = nlohmann::ordered_json;

namespace model::connection {
    class Connections : public std::vector<ordered_json> {
        std::string command;
        std::unique_ptr<::FILE, decltype(&::pclose)> lsof;
        class Connection : public ordered_json {
        private:
            std::string l;
            std::regex p;
        public:
            explicit Connection(std::string &line);
        };
    public:
        explicit Connections();
    };
}


namespace model::car {
        struct Car {
            int id;
            std::string name;
            int price;
        };

        void to_json(ordered_json& j, const Car& c);
        void from_json(const ordered_json& j, Car& c);

        namespace Db {
            void init();
            void db_reset();
            std::vector<ordered_json> all();
            void car_add(const Car& car);
            void car_update(const Car& car);
            void car_delete(int id);
        };
    }



#endif //A_SIMPLE_API_CPP_WFREST_MODELS_H
