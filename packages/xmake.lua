package("wfrest")
set_homepage("https://github.com/wfrest/wfrest")

set_urls("https://github.com/wfrest/wfrest.git")
add_urls("https://github.com/sogou/workflow/archive/refs/tags/$(version).tar.gz",
        "https://github.com/sogou/workflow/archive/refs/tags/$(version).zip",
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

add_deps("cmake", "sqlite3")

local unit_tests

on_install("macosx", "linux", function (package)
    import("package.tools.cmake").install(package)
    unit_tests = path.join(os.curdir(), package:buildir(), "tests/unit_tests")
end)

on_test(function (package)
    os.exec(unit_tests)
    os.isfile(path.join(package:installdir(), "/include/sql_orm/sqlite_orm.h"))
end)

package_end()