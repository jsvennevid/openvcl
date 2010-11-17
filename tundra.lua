local common = {
	Env = {
		CCOPTS = { "-ansi", "-pedantic", "-Werror", "-g", Config = "*-gcc-*" },
		LIBS = { "stdc++", Config = "*-gcc-*" }
	}
}

Build {
	Units = function()
		Program {
			Name = "openvcl",
			Sources = { Glob { Dir = "src", Extensions = { ".c", ".cpp", ".h" } } }
		}

		Default "openvcl"
	end,
	SyntaxExtensions = { "tundra.syntax.glob" },
	Configs = {
		Config { Name = "macosx-clang", Inherit = common, Tools = { "clang-osx" }, DefaultOnHost = "macosx" },
		Config { Name = "macosx-gcc", Inherit = common, Tools = { "gcc-osx" } },
		Config { Name = "win32-msvc", Inherit = common, Tools = { { "msvc-winsdk"; TargetArch = "x86" } } },
		Config { Name = "win64-msvc", Inherit = common, Tools = { { "msvc-winsdk"; TargetArch = "x64" } } },
		Config { Name = "linux-gcc", Inherit = common, Tools = { "gcc" } },
	},
}