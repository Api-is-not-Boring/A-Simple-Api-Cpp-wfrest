target("models")
    set_kind("static")
    add_packages("wfrest")
    add_packages("sql_orm", {public = true})
    add_packages("bcrypt")
    add_packages("jwt-cpp", {public = true})
    add_files("*.cpp")
    add_includedirs("include", {public = true})