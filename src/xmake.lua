target("selecppion")
    set_kind("shared")
    if is_plat("windows") then
        add_rules("utils.symbols.export_all", {export_classes = true})
    end
    add_files("secpion/*.cpp")
    add_packages("node_crunch2", {public = true})
    add_packages("taocpp-json")
    add_packages("spdlog")
    -- For spdlog, so that every object file sees the global logger:
    add_defines("SPDLOG_COMPILED_LIB", {public = true})
    -- Tell xmake which headers to give to the user when installing it:
    add_headerfiles("(secpion/*.hpp)")
