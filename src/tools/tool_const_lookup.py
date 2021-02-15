'''
This tool provides a method to lookup constants to easily decode, for instance,
ErrorCodes.
'''

import argparse
from pathlib import Path
from tinydb import TinyDB, where
from rich.console import Console
from rich.padding import Padding

def const_lookup(value, const_type=None):
    '''
    Read the const_db.json TinyDB to look for the given value.

    Additionally if the type of the constant is known narrow down the search.
    '''
    
    # Get the root directory
    root_dir = Path(__file__).parent.parent.parent.absolute()

    # Open database
    db = open_database(root_dir)

    # Get the integer value to search for
    if value is not int:
        value = int(value, 0)

    # Matches dictionary
    matches = {table:[] for table in db.tables()}

    # Search
    if const_type is None:
        for table_name in db.tables():
            table_matches = db.table(table_name).search(where('value') == value)
            if len(table_matches) > 0:
                matches[table_name].extend(table_matches)

    return matches

def open_database(root_dir):
    '''
    Attempt to open the database by searching the root_dir for known locations
    '''
    # First try the build dir
    try:
        return TinyDB(
            root_dir.joinpath('builds/const_db.json'), 
            access_mode='r'
        )
    except FileNotFoundError:
        pass

    # Last resort is the tools dir
    return TinyDB(
        root_dir.joinpath('src/tools/const_db.json'),
        access_mode='r'
    )

def print_matches(matches):
    '''
    Format the matches in a nice way
    '''

    console = Console(highlight=False)

    # Print events
    console.print('[bold u]Events[/bold u]')
    if len(matches['events']) == 0:
        console.print('    [i]None[/i]')
    else:
        if len(matches['events']) > 1:
            print('')
            console.print('    [red][bold]WARNING[/bold] Multiple Events with the same value detected![/red]')
            console.print('            [red]This must be corrected in the header definition so there is no conflict![/red]')
        console.print('')
        for event in matches['events']:
            _print_const(console, event)
    console.print('')

    # Print errors
    console.print('[bold u]Errors[/bold u]')
    if len(matches['errors']) == 0:
        console.print('    [i]None[/i]')
    else:
        if len(matches['errors']) > 1:
            print('')
            console.print('    [red][bold]WARNING[/bold] Multiple ErrorCodes with the same value detected![/red]')
            console.print('            [red]This must be corrected in the header definition so there is no conflict![/red]')
        console.print('')
        for error in matches['errors']:
            _print_const(console, error)
    console.print('')
        
    # Print datapools
    console.print('[bold u]DataPool Parameters[/bold u]')
    if len(matches['datapool']) == 0:
        console.print('    [i]None[/i]')
    else:
        if len(matches['datapool']) > 1:
            print('')
            console.print('    [red][bold]WARNING[/bold] Multiple DataPool parameters with the same value detected![/red]')
            console.print('            [red]This must be corrected in the header definition so there is no conflict![/red]')
        console.print('')
        for dp in matches['datapool']:
            _print_const(console, dp)
    console.print('')

def _print_const(console, const):
    '''
    Print an individual constant to the given console.
    '''
    console.print(f'    [bold][cyan]{const["symbol"]}[/cyan]:[/bold] 0x{const["value"]:04X} ({const["value"]}, 0b{const["value"]:016b})')
    desc = Padding(const['description'], (0, 0, 0, 8))
    console.print(desc)


def _parse_args():
    '''
    Parse arguments for the command-line version of const_lookup
    '''

    parser = argparse.ArgumentParser(
        description='Lookup constant values in the software'
    )
    parser.add_argument(
        'const_type',
        help='The type of constant, either "error", "event", "datapool"',
        nargs='?'
    )
    parser.add_argument(
        'value',
        help='The value of the constant to lookup. Hex must be prefixed with 0x.',
        nargs=1
    )

    return parser.parse_args()

if __name__ == '__main__':
    args = _parse_args()

    matches = const_lookup(args.value[0], const_type=args.const_type)

    print_matches(matches)