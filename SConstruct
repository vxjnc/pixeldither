import os
import sys

from SCons.Script import ARGUMENTS, Environment, SConscript, Variables

libname = "pixeldither"
projectdir = "project"

localEnv = Environment(tools=["default"], PLATFORM="")

customs = ["custom.py"]
customs = [os.path.abspath(path) for path in customs]
opts = Variables(customs, ARGUMENTS)
opts.Update(localEnv)
Help(opts.GenerateHelpText(localEnv))

env = localEnv.Clone()

if not (os.path.isdir("godot-cpp") and os.listdir("godot-cpp")):
    print(
        "godot-cpp is not available within this folder, as Git submodules haven't been initialized."
    )
    print("Run: git submodule update --init --recursive")
    sys.exit(1)

env = SConscript("godot-cpp/SConstruct", {"env": env, "customs": customs})

if env.WhereIs("ccache"):
    env["CXXCOM"] = "ccache " + env["CXXCOM"]
    env["CCCOM"] = "ccache " + env["CCCOM"]

if env.WhereIs("mold"):
    env.Append(LINKFLAGS=["-fuse-ld=mold"])

if env["platform"] == "windows" and env.get("use_mingw", False) == False:
    env["CXXFLAGS"] = [f for f in env["CXXFLAGS"] if not f.startswith("/std:")] + [
        "/std:c++20"
    ]
else:
    env["CXXFLAGS"] = [f for f in env["CXXFLAGS"] if not f.startswith("-std=")] + [
        "-std=c++20"
    ]

env.Append(CPPPATH=["src/"])
sources = Glob("src/*.cpp")

env.Tool("compilation_db")
cdb = env.CompilationDatabase("compile_commands.json")
Alias("cdb", cdb)

suffix = env['suffix'].replace(".dev", "").replace(".universal", "")
lib_filename = "{}{}{}{}".format(
    env.subst('$SHLIBPREFIX'), libname, suffix, env.subst('$SHLIBSUFFIX')
)

library = env.SharedLibrary(
    "bin/{}/{}".format(env['platform'], lib_filename),
    source=sources,
)

copy = env.Install("{}/bin/{}/".format(projectdir, env["platform"]), library)

Default(library, copy)
