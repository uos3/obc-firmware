'''
This file generates the const_db.json file, a database of all constant values,
their names and descriptions in the codebase.

The constants included in this database are:
 - `ErrorCode`s
 - `Event`s
 - `DataPoolId`s

This database is used for documentation and decode purposes. It is generated as
a part of the build process.

Parsing of these starts with the module IDs file Kernel_module_ids.h, which
generates the table containing unshifted and shifted module IDs, along with
their descriptions.

Next all error and event files (*_errors.h and *_events.h) are read and parsed
in similar ways. #defines are read and names/ID values stored, along with
descriptions.

Finally DataPool IDs are parsed using the DataPool_generated.json file, which
is build during the DataPool compilation process.

The results are output in this directory for later copying into the build dir.
'''

import os
import re
from pathlib import Path
from tinydb import TinyDB, where
import json

def gen_const_db():
    '''
    Run the generation process for the database
    '''

    # Get the root directory
    root_dir = Path(__file__).parent.parent.parent.absolute()

    # Create the database
    db, db_path = create_empty_db(root_dir)

    # Generate the module IDs table
    db = gen_module_ids_table(root_dir, db)

    # Generate the errors table
    db = gen_errors_table(root_dir, db)

    # Generate the events table
    db = gen_events_table(root_dir, db)

    # Generate the datapool table
    db = gen_datapool_table(root_dir, db)

    return db_path

def create_empty_db(root_dir):
    '''
    Create an empty TinyDB database in the src/tools dir
    '''

    # Database file path
    db_path = root_dir.joinpath('src', 'tools', 'const_db.json')

    # Remove the database file if it exists - it is an old one and we will
    # generate from new
    if db_path.exists():
        os.remove(db_path)

    # Create new db
    return TinyDB(str(db_path)), db_path

def gen_module_ids_table(root_dir, db):
    '''
    Generate the module IDs table in the database by reading the contents of
    Kernel_module_ids.h.

    The module IDs table has the following columns:
        - symbol
        - module_name
        - group_name
        - id
        - shifted_id
    '''

    # Path to the module_ids file
    file_path = root_dir.joinpath('src', 'system', 'kernel', 'Kernel_module_ids.h')
    
    # Read the module_ids file
    with open(file_path) as module_ids_file:
        module_ids_str = module_ids_file.read()

    # Create the table
    table = db.table('modules')

    # Extract the number of bits to shift the module IDs by for the shifted ID
    shift_regex = re.compile(
        r'#define KERNEL_MOD_ID_SHIFT\s\((\d+)\)',
        flags=re.M|re.S
    )
    shift = int(shift_regex.search(module_ids_str).group(1))

    # Regex for matching the defines in the module file. This is a little
    # hacky, however the file is actually generated so it's format is
    # guarenteed to be regular, hense using a regex is acceptable here.
    regex = re.compile(
        r'(\S+)\smodule\s\((\S+)\sgroup\)[^\#]+#define\s(\S+)\s[^0]+(0x[0-9A-Fa-f]{2})',
        flags=re.M|re.S
    )
    
    # For each match in the file insert a new row into the table
    for match in regex.finditer(module_ids_str):
        table.insert({
            'symbol': match.group(3).strip(),
            'module_name': match.group(1).strip(),
            'group_name': match.group(2).strip(),
            'id': int(match.group(4), 16),
            'shifted_id': int(match.group(4), 16) << shift
        })

    return db

def gen_errors_table(root_dir, db):
    '''
    Generate the errors table of the database by parsing all files with an
    ending of _errors.h.

    The format of the errors table is:
        - symbol
        - module
        - description
        - value
    '''

    # Get the glob of all errors files
    glob = root_dir.joinpath('src').glob('**/*_errors.h')

    # The use of a regex here is very hacky, and is likely to be a major source
    # of errors, however I have yet to find a better way to do this.
    regex = re.compile(
        r'@brief((?:(?!\*\/).)*)[^#]+#define\s(\S+)\s+\({2}ErrorCode\)\(?(\S+)\s*\|\s*((?:0x)?\d+)\)',
        flags=re.M|re.S
    )

    # Create new table
    table = db.table('errors')
    modules_table = db.table('modules')

    # For each error file
    for path in glob:
        # Read the file
        with open(path, 'r') as file:
            string = file.read()

        # Run the regex on the source
        for match in regex.finditer(string):

            # Get the module info from the modules table
            module = modules_table.get(where('symbol') == match.group(3))

            if module is None:
                raise RuntimeError(
                    f'Could not find module with symbol "{match.group(3)}"'
                )

            # Remove newlines and " * " from the description
            desc = match.group(1).strip().replace('\n', '').replace(' * ', ' ')

            # Insert a new row in the table
            table.insert({
                'symbol': match.group(2).strip(),
                'module': module['module_name'],
                'description': desc,
                'value': module['shifted_id'] | int(match.group(4), 0)
            })

    return db

def gen_events_table(root_dir, db):
    '''
    Generate the events table of the database by parsing all files with an
    ending of _events.h.

    The format of the events table is:
        - symbol
        - module
        - description
        - value
    '''

    # Get the glob of all events files
    glob = root_dir.joinpath('src').glob('**/*_events.h')

    # The use of a regex here is very hacky, and is likely to be a major source
    # of errors, however I have yet to find a better way to do this.
    regex = re.compile(
        r'@brief((?:(?!\*\/).)*)[^#]+#define\s(\S+)\s+\({2}Event\)\(?(\S+)\s*\|\s*((?:0x)?\d+)\)',
        flags=re.M|re.S
    )

    # Create new table
    table = db.table('events')
    modules_table = db.table('modules')

    # For each event file
    for path in glob:
        # Read the file
        with open(path, 'r') as file:
            string = file.read()

        # Run the regex on the source
        for match in regex.finditer(string):

            # Get the module info from the modules table
            module = modules_table.get(where('symbol') == match.group(3))

            if module is None:
                raise RuntimeError(
                    f'Could not find module with symbol "{match.group(3)}"'
                )

            # Remove newlines and " * " from the description
            desc = match.group(1).strip().replace('\n', '').replace(' * ', ' ')

            # Insert a new row in the table
            table.insert({
                'symbol': match.group(2).strip(),
                'module': module['module_name'],
                'description': desc,
                'value': module['shifted_id'] | int(match.group(4), 0)
            })

    return db

def gen_datapool_table(root_dir, db):
    '''
    Generate the DataPool table of the database by parsing the
    DataPool_generated.json file.

    The format of the datapool table is:
        - symbol
        - module
        - description
        - value
    '''

    # Path to the module_ids file
    file_path = root_dir.joinpath('src', 'system', 'data_pool', 'DataPool_generated.json')

    # Load the generated dp file
    with open(file_path) as dp_file:
        dp_data = json.load(dp_file)

    # Create new table
    table = db.table('datapool')
    modules_table = db.table('modules')

    # For each parameter in the dp insert a new entry in the table
    for (symbol, param) in dp_data.items():
        table.insert({
            'symbol': symbol,
            'module': modules_table.get(where('id') == param['block_id'])['module_name'],
            'value': param['dp_id'],
            'description': param['brief']
        })

    return db

if __name__ == '__main__':
    print(gen_const_db())