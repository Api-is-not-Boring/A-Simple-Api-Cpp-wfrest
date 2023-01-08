add_rules("mode.debug", "mode.release")
set_project("A-Simple-Api-Cpp-wfrest")
set_version("0.1.0")

set_allowedplats("linux", "macosx")
set_languages("c23", "cxx23")

package("wfrest")
    set_homepage("https://github.com/wfrest/wfrest")

    set_urls("https://github.com/wfrest/wfrest.git")
    add_urls("https://github.com/sogou/workflow/archive/refs/tags/$(version).tar.gz",
             "https://github.com/sogou/workflow/archive/refs/tags/$(version).zip",
             "https://github.com/wfrest/wfrest.git")

    add_deps("workflow", "brew::spdlog")

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

add_requires("wfrest main", {alias = "wfrest"})

includes("src")