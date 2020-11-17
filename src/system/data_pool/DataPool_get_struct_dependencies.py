'''
Small script that gets the dependencies (included .h files) of DataPool_struct.h
'''

import os
import re
from pathlib import Path

def main():
    # Get the root dir of OBC-Firmware
    root_dir = Path('/')
    current_path = root_dir
    for part in Path(os.getcwd()).parts:
        current_path = current_path.joinpath(part)
        if part == 'obc-firmware':
            root_dir = current_path

    # Check root dir was found
    if root_dir == Path('/'):
        raise RuntimeError('Could not find the obc-firmware directory')

    # Change into src
    src_dir = root_dir.joinpath('src')
    os.chdir(src_dir)

    # Open the struct file
    with open('system/data_pool/DataPool_struct.h') as dp_struct_f:

        # Get list of all include files
        include_regex = re.compile('#include \"(.*?)\"')
        includes = [
            match.group(1) for match in include_regex.finditer(dp_struct_f.read())
        ]

    # Print out the dependencies as absolute paths so CMake doesn't get confused.
    print(' '.join([str(src_dir.joinpath(include)) for include in includes]))
    
    # for include in includes:
    #     print(src_dir.joinpath(include))

    return 0

if __name__ == '__main__':
    main()