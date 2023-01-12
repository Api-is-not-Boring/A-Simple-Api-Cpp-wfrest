includes("**/xmake.lua")

target("main")
    add_packages("wfrest")
    add_packages("nanoid-cpp")
    add_deps("router", "middleware")
    set_kind("binary")
    add_files("main.cpp")
