add_rules("mode.debug", "mode.release")
set_project("A-Simple-Api-Cpp-wfrest")
set_version("0.1.0")

set_allowedplats("linux", "macosx")
set_languages("c23", "cxx23")

includes("packages")

add_requires("wfrest main", {alias = "wfrest"})
add_requires("sql_orm")

includes("src")