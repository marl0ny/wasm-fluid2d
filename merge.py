#!/usr/bin/python3
"""
Merge C++ .cpp and .h files into a single large one.
This assumes that a .h file's corresponding .cpp file
is in the same directory.
"""
import glob
import re
import os


with open('shaders.h', 'w') as target_file:
    target_file.write('#ifndef _SHADERS_H_\n#define _SHADERS_H_\n')
    # target_file.write('#include "gl_wrappers2d/gl_wrappers.h"\n\n')
    for filename in glob.glob("./shaders/*", recursive=True):
        filename2 = filename.split('/')[-1]
        tok = filename2.split('.')
        s_type = tok.pop()
        shader_src_name = '_'.join(''.join(tok).split('-'))
        shader_name = shader_src_name + '_shader'
        shader_src_name += '_shader_source'
        with open(filename, 'r') as f2:
            shader_contents = f2.read()
            shader_contents = re.sub('#[ ]*version[ ]+150[ ]+core', '', shader_contents)
            # precision highp float;
            # shader_contents = re.sub('precision highp float;', '', shader_contents)
            target_file.write(f'const char *{shader_src_name} = R\"({shader_contents})\";\n')
            if s_type == 'vert':
                target_file.write(f'GLuint {shader_name}')
                target_file.write(f' = make_vertex_shader({shader_src_name});\n\n')
            elif s_type == 'frag':
                target_file.write(f'GLuint {shader_name}')
                target_file.write(f' = make_fragment_shader({shader_src_name});\n\n')
    target_file.write('#endif')


main_src = 'fluid2d.cpp'
srcs = {}
headers = {}

for src in glob.glob("**/*.cpp", recursive=True):
    with open(src, 'r') as f:
        srcs[src] = f.read()

for h in glob.glob("**/*.h", recursive=True):
    with open(h, 'r') as f:
        headers[h] = f.read()

main_file = srcs[main_src]
src_incs = []
for h in headers.keys():
    h_contents = headers[h]
    src_inc = ''.join(h.split('.')[0:-1]) + '.cpp'
    h = h.replace('.', '\\.').split('/')[-1]
    p = f'#[ ]*include[ ]+"[^"]*{h}"'
    if src_inc in srcs:
        src_incs.append(re.sub(p, '', srcs[src_inc]))
    main_file = re.sub(p, h_contents, main_file)

with open("main.cpp", 'w') as f:
    f.write(main_file)

with open("main.cpp", "a") as f:
    for src in src_incs:
        f.write(src)

os.remove('shaders.h')
