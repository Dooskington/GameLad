project('GameLad', 'cpp', default_options : ['cpp_std=c++14'])

sdl2 = dependency('SDL2', fallback:['sdl2', 'sdl2_dep'])

subdir('gb-emu-lib')
subdir('gb-emu-tests')
subdir('gb-emu')