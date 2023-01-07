add_rules("mode.debug", "mode.release")
set_project("A-Simple-Api-Cpp-wfrest")
set_version("0.1.0")

package("wfrest")
    set_homepage("https://github.com/wfrest/wfrest")

    set_kind("static")
    set_urls("https://github.com/wfrest/wfrest.git")
    add_urls("https://github.com/sogou/workflow/archive/refs/tags/$(version).tar.gz",
             "https://github.com/sogou/workflow/archive/refs/tags/$(version).zip",
             "https://github.com/wfrest/wfrest.git")

    on_load(function (package)
        print("Install Location:", package:installdir())
        print("Cache Location:", package:cachedir())
    end)

    on_install("macosx", "linux", function (package)

        import("package.tools.xmake").install(package)
        print(import("package.tools.xmake").buildenvs(package))
        --os.cp("libwfrest.a", package:installdir("lib"))
    end)

package_end()

add_requires("wfrest main", {alias = "wfrest"})

target("A-Simple-Api-Cpp-wfrest")
    set_kind("binary")
    add_files("src/*.cpp")
    add_packages("wfrest")