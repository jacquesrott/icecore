import os
import operator

env = Environment(
    ENV={'PATH' : os.environ['PATH']},
    CCFLAGS=['-Wall', '-Werror', '-fms-extensions', '-Wno-microsoft', '-fcolor-diagnostics'],
    CPPPATH=['lib/sput-1.3.0'],
    LIBS=['sodium', 'jansson'],
)

env.VariantDir('build', 'src')
env.Program(
    target='bin/icecore', 
    source=env.Glob('build/*.c'),
)


test_env = env.Clone()
test_env.VariantDir('test-build', 'src')
test_env.Program(
    target='bin/test-icecore',
    source=['test-build/tests/main.c'] + [f for f in test_env.Glob('test-build/*.c') if os.path.basename(f.path) != 'main.c'],
)

