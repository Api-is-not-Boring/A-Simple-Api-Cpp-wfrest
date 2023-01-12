add_rules("mode.debug", "mode.release")
set_project("A-Simple-Api-Cpp-wfrest")
set_version("0.1.0")

set_allowedplats("linux", "macosx")
set_languages("c17", "cxx17")

includes("packages")

add_requires("wfrest main", {alias = "wfrest"})
add_requires("sql_orm v1.8", {alias = "sql_orm"})
add_requires("bcrypt", {alias = "bcrypt"})
add_requires("nanoid-cpp", {alias = "nanoid-cpp"})
add_requires("jwt-cpp", {alias = "jwt-cpp"})

includes("src")