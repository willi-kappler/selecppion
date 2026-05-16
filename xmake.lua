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
    -- Optimization:
    add_cxflags("-march=native")
    add_cxflags("-Ofast")

    -- Warnings:
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

includes("src")
includes("tests")
includes("examples")

-- Test package locally in xmake repo:
-- xmake l scripts/test.lua --shallow -vD selecppion
-- xmake l scripts/test.lua --shallow -vD -k shared -m debug selecppion
-- xmake l scripts/test.lua --shallow -vD --runtimes=MD selecppion
