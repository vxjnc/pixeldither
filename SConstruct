env = SConscript("godot-cpp/SConstruct")

if env.WhereIs("ccache"):
    env["CXXCOM"] = "ccache " + env["CXXCOM"]
    env["CCCOM"] = "ccache " + env["CCCOM"]

if env.WhereIs("mold"):
    env.Append(LINKFLAGS=["-fuse-ld=mold"])

env.Append(CPPPATH=["src/"])
sources = Glob("src/*.cpp")

env.Tool("compilation_db")
cdb = env.CompilationDatabase("compile_commands.json")
Alias("cdb", cdb)

if env["platform"] == "macos":
    library = env.SharedLibrary(
        "bin/libpixeldither.{}.{}.framework/libpixeldither.{}.{}".format(
            env["platform"], env["target"], env["platform"], env["target"]
        ),
        source=sources,
    )
else:
    library = env.SharedLibrary(
        "bin/libpixeldither{}{}".format(env["suffix"], env["SHLIBSUFFIX"]),
        source=sources,
    )

Default(library)
