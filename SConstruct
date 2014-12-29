import os
env = Environment(ENV = {'PATH' : os.environ['PATH']})

ccflags = ['-fms-extensions', '-Wno-microsoft']
libs = ['sodium', 'jansson']

VariantDir('build', 'src')

Program(
    target='bin/icecore', 
    source=Glob('build/*.c'),
    CCFLAGS=ccflags,
    LIBS=libs,
)

if 0:
    Program(
        zarget='bin/test-icecore',
        source=['src/tests/main.c'],
        CCFLAGS=ccflags,
        LIBS=libs,
        CPPPATH=['lib/sput-1.3.0'],
    )