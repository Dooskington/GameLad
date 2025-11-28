gb_emu_lib_sources = ['APU.cpp',
	 'CPU.cpp',
	 'Cartridge.cpp',
	 'Emulator.cpp',
	 'GPU.cpp',
	 'Joypad.cpp',
	 'Logger.cpp',
	 'MBC.cpp',
	 'MMU.cpp',
	 'Serial.cpp',
	 'Timer.cpp',
	 'pch.cpp']

gb_emu_lib_inc = include_directories('.')

gb_emu_lib = library('gb-emu', gb_emu_lib_sources, dependencies:[sdl2])

gb_emu_lib_dep = declare_dependency( include_directories:gb_emu_lib_inc,
	link_with: gb_emu_lib,
	dependencies: sdl2
	)