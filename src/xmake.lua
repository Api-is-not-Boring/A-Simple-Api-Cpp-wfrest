includes("router")

target("A-Simple-Api-Cpp-wfrest")
    add_links("router")
    set_kind("binary")
    add_packages("wfrest")
    add_files("main.cpp")
