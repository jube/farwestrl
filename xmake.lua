set_project("farwestrl")
set_version("0.1.0")

add_repositories("gf-repo https://github.com/GamedevFramework/xmake-repo")

add_requires("gamedevframework2", "nlohmann_json")

add_rules("mode.debug", "mode.releasedbg", "mode.release")
add_rules("plugin.compile_commands.autoupdate", {outputdir = "$(builddir)"})

if is_mode("sanitizers") then
    set_symbols("debug")
    set_optimize("none")
    set_policy("build.sanitizer.address", true)
    set_policy("build.sanitizer.undefined", true)
end

set_policy("build.warning", true)
set_warnings("allextra")
set_languages("cxx20")
set_encodings("utf-8")

if is_plat("windows") then
  add_cxflags("/wd4251") -- Disable warning: class needs to have dll-interface to be used by clients of class blah blah blah
  add_defines("NOMINMAX", "_CRT_SECURE_NO_WARNINGS")
end

set_configdir("$(builddir)/config")
set_configvar("FARWESTRL_DATADIR", "$(projectdir)/data/farwestrl")
add_configfiles("code/config.h.in", {pattern = "@(.-)@"})

target("farwestrl0")
    set_kind("static")
    add_files("code/bits/*.cc")
    add_packages("gamedevframework2", "nlohmann_json", { public = true })

target("farwestrl")
    set_kind("binary")
    add_files("code/farwestrl.cc")
    add_includedirs("$(builddir)/config")
    add_deps("farwestrl0")
    set_rundir("$(projectdir)/run")

target("world-generation")
    set_kind("binary")
    add_files("code/world-generation.cc")
    add_deps("farwestrl0")
    set_rundir("$(projectdir)/run")

target("name-generation")
    set_kind("binary")
    add_files("code/name-generation.cc")
    add_deps("farwestrl0")
