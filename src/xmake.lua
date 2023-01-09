includes("**/xmake.lua")

target("main")
    add_packages("wfrest", "boost")
    add_deps("router", "middleware")
    set_kind("binary")
    add_files("main.cpp")
