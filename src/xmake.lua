includes("**/xmake.lua")

target("main")
    add_packages("wfrest")
    add_deps("router")
    set_kind("binary")
    add_files("main.cpp")
