import glob
import re
import sys


with open('shaders.hpp', 'w') as target_file:
    target_file.write('#ifndef _SHADERS_H_\n#define _SHADERS_H_\n')
    for filename in glob.glob("./shaders/*", recursive=True):
        if sys.platform == 'win32':
            filename2 = filename.split('\\')[-1]
        else:
            filename2 = filename.split('/')[-1]
        tok = filename2.split('.')
        s_type = tok.pop()
        shader_src_name = '_'.join(''.join(tok).split('-'))
        shader_name = shader_src_name + '_shader'
        shader_src_name += '_shader_source'
        with open(filename, 'r') as f2:
            shader_contents = f2.read()
            shader_contents = re.sub('#[ ]*version[ ]+150[ ]+core', '', shader_contents)
            target_file.write(f'const char *{shader_src_name} = R\"({shader_contents})\";\n')
    target_file.write('#endif')
