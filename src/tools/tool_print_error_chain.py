'''
Uses tool_const_lookup to decode an error chain into human readable format.
'''

import argparse
from rich.console import Console
from rich.padding import Padding
from tool_const_lookup import const_lookup

def print_error_chain(error_chain_str):
    '''
    Prints a human-readable version of the given error chain string.
    '''
    console = Console(highlight=False)
    console.print(
        f'[bold]Error Chain {error_chain_str}[/bold]:'
    )

    # Remove any brackets and split the chain on spaces
    error_chain_str = error_chain_str.replace('[', '')
    error_chain_str = error_chain_str.replace(']', '')
    error_codes = error_chain_str.split(' ')

    # Print out the meanings of each error
    for error_code in error_codes:
        consts = const_lookup(f'0x{error_code}', const_type='errors')

        if len(consts) == 0:
            console.print(f'    0x{error_code}: [red][bold]Unknown[/bold][/red]')
            continue
        elif len(consts) > 1:
            console.print(f'    [red][bold]Multiple Matches[/bold][/red]')
        
        for const in consts:
            console.print(f'    0x{const["value"]:04X}: [bold][cyan]{const["symbol"]}[/cyan][/bold]')
            
def _parse_args():
    parser = argparse.ArgumentParser(
        description='Convert error chains into human readable format'
    )
    parser.add_argument(
        'error_chain',
        help='The error chain to process, for example "[D407 8805 6005]"',
        type=str
    )

    return parser.parse_args()

if __name__ == '__main__':
    args = _parse_args()

    print_error_chain(args.error_chain)
