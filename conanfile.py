from conans import ConanFile, CMake

class DrawFractals(ConanFile):
	name = "draw_fractals"
	version = "0.1"
	settings = "os", "compiler", "build_type", "arch"
	requires = "sdl2/2.0.14@bincrafters/stable"
	generators = "cmake"

	def build(self):
		cmake = CMake(self)
		cmake.configure()
		cmake.build()