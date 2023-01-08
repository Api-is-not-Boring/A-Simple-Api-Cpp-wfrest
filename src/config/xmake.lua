target("config")
    set_kind("headeronly")
    add_includedirs("$(buildir)/config", {public = true})
    set_configdir("$(buildir)/config")
    add_configfiles("project.h.in", {variables = {
        NAME = "A-Simple-Api-Cpp-wfrest",
        DESCRIPTION = "A simple API build with C++ and wfrest framework",
        URL = "https://github.com/Api-is-not-Boring/A-Simple-Api-Cpp-wfrest",
    }})