#ifndef A_SIMPLE_API_CPP_WFREST_MODELS_H
#define A_SIMPLE_API_CPP_WFREST_MODELS_H

#include <regex>
#include <utility>
#include <unistd.h>
#include <wfrest/json.hpp>


using ordered_json = nlohmann::ordered_json;

const pid_t process = getpid();

namespace model::connection {
    class Connections : public std::vector<ordered_json> {
        using buffer = std::shared_ptr<std::array<char, 256>>; // 256
        std::string command ="lsof -a -n -P -p " + std::to_string(process) + " -i tcp";
        std::unique_ptr<::FILE, decltype(&::pclose)> lsof;
        buffer b = std::make_shared<std::array<char, 256>>();
        class Connection : public ordered_json {
        private:
            std::string l;
            std::regex p;
        public:
            explicit Connection(const std::shared_ptr<std::array<char, 256>>& b);
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
            ordered_json get_car(int id);
            bool car_exists(int id);
            int count();
            void car_add(const Car& car);
            int last_insert_id();
            void car_update(const Car& car);
            void car_delete(int id);
        };
    }



#endif //A_SIMPLE_API_CPP_WFREST_MODELS_H
