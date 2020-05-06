#!python3

_ccflags = '-std=c++11 -Wall -Wextra -pedantic '
# if ARGUMENTS.get('debug', 0):
if not BUILD_TARGETS:
    print("Building Release version")
    _ccflags += '-D NDEBUG '
if 'debug' in BUILD_TARGETS:
    print("Building Debug version")
    _ccflags += '-D DEBUG '
if 'tracer' in BUILD_TARGETS:
    print("Building Tracer version")
    _ccflags += '-D TRACER '
if 'gdb' in BUILD_TARGETS:
    print("Building Gdb version")
    _ccflags += '-g '

env = Environment(CCFLAGS = _ccflags, CC = 'g++')

"""
SConscript('src/SConscript', variant_dir='#/build', src_dir='#/src', duplicate=0,
      exports='env')
"""

# building release version
env.VariantDir('build/release', 'src', duplicate=0)
release = env.Program('build/release/luky', Glob('build/release/*.cpp'))
env.Alias('release', 'build/release/luky')
Default(release)

# building debug version
env.VariantDir('build/debug/', 'src', duplicate=0)
debug = env.Program('build/debug/luky_debug', Glob('build/debug/*.cpp'))
env.Alias('debug', 'build/debug/luky_debug')

# building tracer version
tracer = env.Program('build/debug/luky_debug', Glob('build/debug/*.cpp'))
env.Alias('tracer', 'build/debug/luky_debug')

gdb = env.Program('build/debug/luky_debug', Glob('build/debug/*.cpp'))
env.Alias('gdb', 'build/debug/luky_debug')

from subprocess import call
if 'run' in COMMAND_LINE_TARGETS:
    run_alias = Alias('run', call(['rlwrap', release[0].abspath]))
elif 'rundbg' in COMMAND_LINE_TARGETS:
    rundbg_alias = Alias('rundbg', call(['rlwrap', debug[0].abspath]))

