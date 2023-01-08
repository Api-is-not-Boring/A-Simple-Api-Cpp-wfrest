target("router")
    add_headerfiles("router.h", {prefixdir = "include"})
    add_files("*.cpp")
    add_packages("wfrest")