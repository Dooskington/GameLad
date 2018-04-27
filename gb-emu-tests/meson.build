gb_emu_tests_srcs = ['CPUTests.cpp',
	 'GPUTests.cpp',
	 'JoypadTests.cpp',
	 'MBCTests.cpp',
	 'TestMain.cpp',
	 'stdafx.cpp']

gb_emu_tests = executable('gb-emu-tests', gb_emu_tests_srcs, dependencies:gb_emu_lib_dep)

test('tests', gb_emu_tests)