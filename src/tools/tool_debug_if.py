'''
Debugging interface for the OBC firmware.
'''

import argparse
import subprocess
import sys
from threading import Thread
from queue import Queue, Empty
import signal
import re
import time
from pathlib import Path
from blessed import Terminal

# Whether or not we're running on a POSIX system (linux, mac etc.)
ON_POSIX = 'posix' in sys.builtin_module_names

# Regex pattern that splits of ANSI escape sequences
ANSI_ESCAPE = re.compile(r'(\x9B|\x1B\[)[0-?]*[ -\/]*[@-~]')

LOG_LEVEL_MAP = {
    'TRC': 'trace',
    'DBG': 'debug',
    '---': 'info',
    'WRN': 'warning',
    'ERR': 'error'
}

class GracefulKiller:
    kill_now = False
    def __init__(self):
        signal.signal(signal.SIGINT, self.exit_gracefully)
        signal.signal(signal.SIGTERM, self.exit_gracefully)

    def exit_gracefully(self,signum, frame):
        self.kill_now = True

class LogLine:
    raw_ascii = None
    time = None
    level = None
    file = None
    line = None
    module = None
    message = None
    event_code = None

    @staticmethod
    def parse(line):
        log_line = LogLine()

        log_line.raw_ascii = line.strip()

        # Remove ansi codes
        line = ANSI_ESCAPE.sub('', line)

        # Split the line on spaces
        parts = line.split()

        # Parse the pieces of the log line
        try:
            log_line.time = int(parts[1])
        except:
            pass
        log_line.level = LOG_LEVEL_MAP[parts[2][0:3]]
        log_line.file = parts[3].split(':')[0]
        log_line.line = parts[3].split(':')[1]
        log_line.message = ' '.join(parts[4:-1])

        # The module is either the part of the name before the _, or the whole
        # name. If the first letter is uppercase that's a module name,
        # otherwise it's the whole name.
        path = Path(log_line.file)
        if path.name[0].isupper():
            log_line.module = path.name.split('_')[0]
        else:
            log_line.module = path.stem

        # If the module is eventmanager and the message is 'EVENT: ', mark this
        # line as an event and add the code
        if log_line.message.startswith('EVENT: '):
            log_line.event = log_line.message.replace('EVENT: ', '')

        return log_line

    def print(self, term, link_num):
        # Get the root dir
        root_dir = Path(__file__).parent.parent.parent.absolute()

        # Replace the file position with a link which opens the file. TODO:
        # this doesn't seem to work just yet
        print(
            self.raw_ascii.replace(
                f'{self.file}:{self.line}', 
                term.link(
                    f'file://{root_dir}/src/{self.file}', 
                    f'{self.file}:{self.line}',
                    link_num
                )
            )
        )

class Interface:
    run = True

    def __init__(self, exec_name):
        self.log_lines = []
        self.term = Terminal()
        self.term.enter_fullscreen()
        print(self.term.clear)
        self.term.csr(1, self.term.height - 2)
        self.log_pos = 0
        self.exec_name = exec_name
        self.opmode = 0
        self.time = 0
        self.command = ''
        self.num_links = 0

    def __del__(self):
        self.term.exit_fullscreen()

    def update(self):

        term = self.term
        filtered_lines = list(self.filtered_log_lines())

        # Handle key presses
        with self.term.cbreak():
            key = self.term.inkey(timeout=0)

            if not key:
                pass
            # elif key.name == 'KEY_UP' and self.log_pos > 0:
            #     self.log_pos -= 1
            #     self.update_log_lines = True
            # elif key.name == 'KEY_DOWN' and self.log_pos < len(filtered_lines):
            #     self.log_pos += 1
            #     self.update_log_lines = True
            # elif key == 'q':
            #     self.run = False
            else:
                self.command += key

        # Print exec name
        with term.hidden_cursor(), term.location(0, 0):
            print(self.exec_name)

            # Get string with time and opmode
            status_str = f'OPMODE: {self.opmode} TIME: {self.time}'
            print(
                term.move_xy(term.width - term.length(status_str), 0) 
                + 
                status_str
            )

        # Write command at the bottom
        print(term.move_xy(0, term.height - 1) + term.clear_eol + self.command)
        

    def add_log_line(self, log_line):
        self.log_lines.append(log_line)
        self.time = log_line.time
        if self.log_line_filter(log_line) is not None:
            log_line.print(self.term, self.num_links)
            self.num_links += 1
        self.update_log_lines = True

    def log_line_filter(self, log_line):
        if log_line.event_code:
            return log_line
        elif log_line.level == 'trace':
            return None
        else:
            return log_line

    def filtered_log_lines(self):
        return filter(self.log_line_filter, self.log_lines)


def main(executable = None):
    '''
    Run the main debugging environment.

    Parameters:
        executable - path to the executable to run, or None to connect to serial
    '''

    # Branch depending on whether connecting to serial or running exec
    if executable is None:
        # TODO: serial
        print('Serial not yet supported')
    else:
        run_exec(executable)

def enqueue_output(out, queue):
    for line in iter(out.readline, b''):
        queue.put(line)
    out.close()

def run_exec(executable):
    '''
    Run an executable firmware image using subprocess.
    '''

    process = subprocess.Popen(
        executable, 
        stdout=subprocess.PIPE,
        close_fds=ON_POSIX
    )
    queue = Queue()
    thread = Thread(target=enqueue_output, args=(process.stdout, queue))
    thread.daemon = True
    thread.start()
    killer = GracefulKiller()

    interface = Interface(executable)

    while interface.run:

        # Read a line from out in a non-blocking way
        try: 
            line = queue.get_nowait()
        except Empty:
            # No output
            pass
        else:
            # Decode the line from raw bytes to ascii, also strip any
            # newline/whitspace off the line
            line_ascii = line.decode('ascii').strip()

            # Parse the parts of the line and add it to the interface
            interface.add_log_line(LogLine.parse(line_ascii))

        # Update the interface
        interface.update()

    # Delete the interface, which exists full screen
    del interface

if __name__ == '__main__':
    # Parse arguments
    parser = argparse.ArgumentParser(
        description='Debuging interface for the OBC firmware.'
    )
    parser.add_argument(
        'executable', metavar='E', type=str, nargs='?',
        help='executable to run, or none to connect to serial'
    )

    args = parser.parse_args()

    main(args.executable)