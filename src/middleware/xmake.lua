target("middleware")
    set_kind("static")
    add_packages("wfrest")
    add_packages("jwt-cpp")
    add_files("*.cpp")
    add_includedirs("include", {public = true})