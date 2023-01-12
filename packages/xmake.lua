package("wfrest")
    set_homepage("https://github.com/wfrest/wfrest")

    set_urls("https://github.com/wfrest/wfrest.git")
    add_urls("https://github.com/sogou/workflow/archive/refs/tags/$(version).tar.gz",
            "https://github.com/wfrest/wfrest.git")

    add_deps("workflow", "brew::spdlog", "fmt")

    if is_plat("macosx") then
        add_deps("brew::spdlog")
    elseif is_plat("linux") then
        add_deps("apt::spdlog")
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

    local unit_tests

    on_install("macosx", "linux", function (package)
        import("package.tools.cmake").install(package , {"-DSQLITE_ORM_ENABLE_CXX_17=ON"})
        unit_tests = path.join(os.curdir(), package:buildir(), "tests/unit_tests")
    end)

    on_test(function (package)
        os.exec(unit_tests)
        os.isfile(path.join(package:installdir(), "/include/sql_orm/sqlite_orm.h"))
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
        os.vrun("echo 'int main(){}' > test.cpp")
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