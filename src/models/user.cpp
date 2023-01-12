#include "models.h"
#include <bcrypt.h>
#include <jwt-cpp/jwt.h>
#include <random>
#include <sqlite_orm/sqlite_orm.h>
#include <utility>


using namespace sqlite_orm;
using model::user::User;

auto user_storage = make_storage(":memory:",
    make_table("users",
        make_column("id", &User::id, primary_key().autoincrement()),
        make_column("username", &User::username),
        make_column("password", &User::password)));

void model::user::to_json(ordered_json& j, const User& u)
{
    j = ordered_json {
        { "id", std::isnan(u.id) ? -1 : u.id },
        { "username", u.username },
        { "password", u.password }
    };
}

void model::user::from_json(const ordered_json& j, User& u)
{
    u.id = !j.contains("id") ? -1 : j.at("id").get<int>();
    u.username = j.at("username").get<std::string>();
    u.password = j.at("password").get<std::string>();
}

void model::user::Db::init()
{
    user_storage.sync_schema();
    user_storage.remove_all<User>();
    user_storage.transaction([] {
        user_storage.insert(User { -1, "admin", bcrypt::generateHash("password") });
        return true;
    });
}

void model::user::Db::db_reset()
{
    user_storage.drop_table("users");
    init();
}

bool model::user::Db::authenticate(std::map<std::string, std::pair<std::string, std::string>> login)
{
    auto username = login["username"].second;
    auto password = login["password"].second;
    if (auto user = user_storage.get_all<User>(where(c(&User::username) == username)); user.size() == 1)
        return bcrypt::validatePassword(password, user[0].password);
    else
        return false;
}

std::string model::user::Db::generate_token(std::string s)
{
    auto now = std::chrono::system_clock::now();
    auto exp = now + std::chrono::seconds(60);
    auto iat = now;
    auto payload = jwt::create()
                       .set_type("JWT")
                       .set_issued_at(iat)
                       .set_expires_at(exp)
                       .sign(jwt::algorithm::hs512(std::move(s)));
    return payload;
}
