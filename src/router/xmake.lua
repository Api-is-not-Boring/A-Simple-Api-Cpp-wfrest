target("router")
    set_kind("static")
    add_deps("config")
    add_packages("wfrest", "boost")
    add_files("*.cpp")
    add_includedirs("include", {public = true})