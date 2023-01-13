package("wfrest")
    set_homepage("https://github.com/wfrest/wfrest")

    set_urls("https://github.com/wfrest/wfrest.git")
    add_urls("https://github.com/sogou/workflow/archive/refs/tags/$(version).tar.gz",
            "https://github.com/wfrest/wfrest.git")

    add_deps("workflow", "zlib")

    if is_plat("macosx") then
        add_deps("brew::spdlog")
    elseif is_plat("linux") then
        add_deps("apt::libspdlog-dev")
    end

    on_install("macosx", "linux", function (package)
        local configs = {kind = "static", plat = os.host(), arch = os.arch()}
        local options = {wfrest_lib = package:installdir("lib"), wfrest_inc = package:installdir("include")}
        import("package.tools.xmake").install(package, configs, options)
    end)
package_end()

package("sql_orm")
    set_homepage("https://github.com/fnc12/sqlite_orm")

    set_urls("https://github.com/fnc12/sqlite_orm.git")
    add_urls("https://github.com/fnc12/sqlite_orm/archive/refs/tags/$(version).tar.gz",
            "https://github.com/fnc12/sqlite_orm.git")

    add_deps("cmake", "sqlite3")

    on_install("macosx", "linux", function (package)
        import("package.tools.cmake").install(package , {"-DSQLITE_ORM_ENABLE_CXX_17=ON", "-DBUILD_TESTING=OFF"})
    end)

    --on_test(function (package)
    --    assert(package:has_cxxfuncs("sqlite_orm::make_storage", {configs = {languages = "c++17"}, includes = "sqlite_orm/sqlite_orm.h"}))
    --end)

package_end()

package("bcrypt")
    set_homepage("https://github.com/hilch/Bcrypt.cpp")

    set_urls("https://github.com/hilch/Bcrypt.cpp.git")

    add_deps("cmake")

    on_install("macosx", "linux", function (package)
        io.writefile("xmake.lua", [[
            add_rules("mode.debug", "mode.release")
            set_languages("c11", "cxx11")
            target("bcrypt")
                set_kind("static")
                add_files("src/*.cpp")
                add_includedirs("include")
                add_headerfiles("include/*.h")
        ]])
        import("package.tools.xmake").install(package)
    end)

    on_test(function (package)
        assert(package:check_cxxsnippets({test = [[
            void test() {
                std::string password = "top_secret";

                std::string hash = bcrypt::generateHash(password);

                std::cout << "Hash: " << hash << std::endl;

                std::cout << "\"" << password << "\" : " << bcrypt::validatePassword(password,hash) << std::endl;
                std::cout << "\"wrong\" : " << bcrypt::validatePassword("wrong",hash) << std::endl;
            }
        ]]}, {configs = {languages = "c++17"}, includes = { "bcrypt.h", "iostream", "string" }}))
    end)

package_end()

package("nanoid-cpp")
    set_homepage("https://github.com/mcmikecreations/nanoid_cpp")

    set_urls("https://github.com/mcmikecreations/nanoid_cpp.git")

    add_deps("cmake")

    on_install("macosx", "linux", function (package)
        import("package.tools.cmake").install(package, {"-DBUILD_EXAMPLES=OFF", "-DBUILD_TESTS=OFF", "-DCMAKE_CXX_STANDARD=17"})
    end)

    on_test(function (package)
        assert(package:check_cxxsnippets({test = [[
            void test() {
                std::string id = nanoid::generate(36);
                std::cout << id << std::endl;
            }
        ]]}, {configs = {languages = "c++17"}, includes = { "nanoid/nanoid.h", "iostream", "string" }}))
    end)

package_end()

package("jwt-cpp")
    set_homepage("https://github.com/Thalhammer/jwt-cpp")

    set_urls("https://github.com/Thalhammer/jwt-cpp.git")

    add_deps("openssl", "gtest")

    on_install("macosx", "linux", function (package)
        os.cp("include/*", package:installdir("include"))
    end)

    on_test(function (package)
        assert(package:check_cxxsnippets({test = [[
            #include <jwt-cpp/jwt.h>
            void test() {
                std::string token = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9.eyJpc3MiOiJhdXRoMCJ9.AbIJTDMFc7yUa5MhvcP03nJPyCPzZtQcGEp-zWfOkEE";
                auto decoded = jwt::decode(token);

                for(auto& e : decoded.get_payload_json())
                    std::cout << e.first << " = " << e.second << std::endl;
            }
        ]]}, {configs = {languages = "c++17"}, includes = "jwt-cpp/jwt.h"}))
    end)

package_end()