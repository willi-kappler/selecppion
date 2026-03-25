set_project("selecppion")
    set_languages("c++23")
    set_version("0.1.0")
    set_optimize("fastest")
    -- set_defaultmode("debug")
    set_defaultmode("release")
    add_rules("mode.debug", "mode.release")

-- Clean configuration:
-- xmake f -c

-- Set configuration:
-- xmake f -m release
-- xmake f -m debug

-- Set clang:
-- xmake f --toolchain=clang --runtimes=c++_shared
-- xmake f --toolchain=clang --runtimes=stdc++_shared

-- Force rebuild:
-- rm -rf build/ .xmake ~/.xmake/
-- xmake p
-- xmake -a -r

-- Just clean cache and update:
-- xrepo clean
-- xrepo update-repo

-- For all compilers:
set_warnings("all", "extra", "pedantic", "error")

-- For GCC and Clang only:
if is_kind("gcc", "clang") then
    -- For GCC only:
    if is_kind("gcc") then
        add_cxxflags("-Walloca", "-Wcast-align=strict", "-Wimplicit-fallthrough=5")
    elseif is_kind("clang") then
        -- Clang prefers the boolean flag without the numeric level
        add_cxxflags("-Wimplicit-fallthrough")
    end
--    add_cxxflags("-Wnull-dereference", "-Wswitch-enum")
    add_cxxflags("-Wconversion", "-Wshadow", "-Wsign-conversion", "-Wdouble-promotion", "-Wformat=2")
    add_cxxflags("-Wundef", "-Wcast-qual", "-Wnon-virtual-dtor", "-Wold-style-cast")
    add_cxxflags("-Woverloaded-virtual", "-Wunused", "-Wuninitialized", "-Winit-self")
    add_cxxflags("-Wredundant-decls", "-Wsuggest-override")
end

-- For MSVC only:
if is_kind("cl") then
    -- add_cxxflags("/Wall")
    add_cxxflags("/W4") -- Level 4 is the standard "Strict" for MSVC
    add_cxxflags("/w14242") -- 'identifier': conversion from 'type1' to 'type1', possible loss of data
    add_cxxflags("/w14265") -- 'class': class has virtual functions, but destructor is not virtual
    add_cxxflags("/w14287") -- 'operator': unsigned/negative constant mismatch
    add_cxxflags("/we4289") -- nonstandard extension used: 'variable': control variable declared in the for-loop is used outside the for-loop scope
    add_cxxflags("/w14296") -- 'operator': expression is always false
    add_cxxflags("/w14311") -- 'variable' : pointer truncation from 'type' to 'type'
    add_cxxflags("/wd4068") -- disable "unknown pragma" (useful if you use GCC pragmas)
    add_cxxflags("/utf-8")
end

package("node_crunch2")
    set_homepage("https://github.com/willi-kappler/node_crunch2")
    set_description("A C++ library for distributed computing.")
    add_urls("https://github.com/willi-kappler/node_crunch2.git")
    add_versions("latest", "main")

    add_deps("asio")

    on_install(function (package)
        import("package.tools.xmake").install(package)
    end)
package_end()


add_requires("taocpp-json 2025.03.11")
add_requires("snitch")
add_requires("spdlog", {configs = {header_only = false}})
add_requires("argparse")
add_requires("node_crunch2")

target("selecppion")
    set_kind("shared")
    if is_plat("windows") then
        add_rules("utils.symbols.export_all", {export_classes = true})
    end
    add_files("src/secpion/*.cpp")
    add_packages("node_crunch2", {public = true})
    add_packages("taocpp-json")
    add_packages("spdlog")
    -- For spdlog, so that every object file sees the global logger:
    add_defines("SPDLOG_COMPILED_LIB", {public = true})
    -- Tell xmake which headers to give to the user when installing it:
    add_headerfiles("src/(secpion/*.hpp)")

-- Test cases:
target("se_test")
    set_kind("binary")
    add_files("tests/test_all.cpp")
    add_packages("node_crunch2")
    add_packages("snitch")
    add_packages("spdlog")
    add_packages("taocpp-json")
    add_deps("selecppion")
    add_includedirs("src")
    set_default(false) -- Don't build by default

-- All examples:
target("se_example_tsp")
    set_kind("binary")
    add_files("examples/tsp/src/*.cpp")
    add_packages("node_crunch2")
    add_packages("spdlog")
    add_packages("taocpp-json")
    add_packages("argparse")
    add_deps("selecppion")
    add_includedirs("src", "examples/utils")
    set_default(false) -- Don't build by default

target("se_example_bin_packing")
    set_kind("binary")
    add_files("examples/bin_packing/src/*.cpp")
    add_packages("node_crunch2")
    add_packages("spdlog")
    add_packages("taocpp-json")
    add_packages("argparse")
    add_deps("selecppion")
    add_includedirs("src", "examples/utils")
    set_default(false) -- Don't build by default

target("se_example_knapsack")
    set_kind("binary")
    add_files("examples/knapsack/src/*.cpp")
    add_packages("node_crunch2")
    add_packages("spdlog")
    add_packages("taocpp-json")
    add_packages("argparse")
    add_deps("selecppion")
    add_includedirs("src", "examples/utils")
    set_default(false) -- Don't build by default

target("se_example_queens")
    set_kind("binary")
    add_files("examples/queens/src/*.cpp")
    add_packages("node_crunch2")
    add_packages("spdlog")
    add_packages("taocpp-json")
    add_packages("argparse")
    add_deps("selecppion")
    add_includedirs("src", "examples/utils")
    set_default(false) -- Don't build by default

-- Test package locally in xmake repo:
-- xmake l scripts/test.lua --shallow -vD selecppion
-- xmake l scripts/test.lua --shallow -vD -k shared -m debug selecppion
-- xmake l scripts/test.lua --shallow -vD --runtimes=MD selecppion
