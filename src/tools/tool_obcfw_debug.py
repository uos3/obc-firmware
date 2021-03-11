'''
A tool to aid debugging of the obc-firmware
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
import json
import dataclasses
from dataclasses import dataclass, field
import npyscreen

# Whether or not we're running on a POSIX system (linux, mac etc.)
ON_POSIX = 'posix' in sys.builtin_module_names

# Log levels, in ascending order
LOG_LEVELS = ['all', 'trace', 'debug', 'info', 'warning', 'error']

# map between obc-log codes and levels
LOG_LEVEL_MAP = {
    'TRC': 'trace',
    'DBG': 'debug',
    '---': 'info',
    'WRN': 'warning',
    'ERR': 'error'
}

# Regex pattern that splits of ANSI escape sequences
ANSI_ESCAPE = re.compile(r'(\x9B|\x1B\[)[0-?]*[ -\/]*[@-~]')

# ----------------------------------------------------------------------------
# OBC-FW ABSTRACTION
# ----------------------------------------------------------------------------

class ObcFwInstance:
    @dataclass
    class State:
        filter: dict = field(default_factory=lambda: {
            '*': 'all'
        })

    def __init__(self):
        self.log_lines = []

    def init(self, root_path, exec_path = None):
        # If given an exec path need to open the process and connect a new
        # thread which will pull log messages out of stdout.
        if exec_path is not None:
            self.process = subprocess.Popen(
                exec_path, 
                stdout=subprocess.PIPE,
                close_fds=ON_POSIX
            )
            self.queue = Queue()
            self.thread = Thread(
                target=ObcFwInstance.enqueue_output, 
                args=(self.process.stdout, self.queue)
            )
            self.thread.daemon = True
            self.thread.start()

            self.exec_path = exec_path
        # Otherwise setup serial connection to the OBC fw instance
        else:
            raise RuntimeError('Serial connection not yet supported')

        self.root_path = root_path

        # Load the state file, which contains persistant info. If it doesn't
        # exist create a new empty state. File will be written on quit
        self.state_path = root_path.joinpath(
            'builds/tool_obcfw_debug_state.json'
        )
        if self.state_path.is_file():
            self.state = self.load_state()
        else:
            self.state = ObcFwInstance.State()

        
    def update(self):
        try:
            line = self.queue.get_nowait()
        except Empty:
            pass
        else:
            # Decode the line from raw bytes to ascii, also strip any
            # newline/whitspace off the line
            line_ascii = line.decode('ascii').strip()

            # Add the line to the log
            self.log_lines.append(LogLine.parse(line_ascii))

    @staticmethod
    def enqueue_output(out, queue):
        for line in iter(out.readline, b''):
            queue.put(line)
        out.close()
    
    def set_filter(self, filter_cmd):
        # The command is split on spaces, if there's two the first should be 
        # the module, second the level. If there's one its the global level,
        # otherwise an error
        if len(filter_cmd) == 1 and filter_cmd[0] in LOG_LEVELS:
            self.state.filter['*'] = filter_cmd[0]
        elif len(filter_cmd) == 2 and filter_cmd[1] in LOG_LEVELS:
            self.state.filter[filter_cmd[0].lower()] = filter_cmd[1]
        else:
            raise RuntimeError('Invalid filter string')

    def get_filtered(self):
        return list(filter(self.log_line_filter, self.log_lines))
    
    def log_line_filter(self, log_line):
        filter_level = None
        # First get the level to filter for, if the log line's module is in the
        # filter dict use that, otherwise use the global level
        if log_line.module.lower() in self.state.filter.keys():
            filter_level = LOG_LEVELS.index(
                self.state.filter[log_line.module.lower()]
            )
        else:
            filter_level = LOG_LEVELS.index(self.state.filter['*'])

        # Then filter, if the level is less than the filter then it's not being
        # displayed.
        if LOG_LEVELS.index(log_line.level) < filter_level:
            return None
        else:
            return log_line
        
    def write_state(self):
        with open(self.state_path, 'w') as f:
            json.dump(dataclasses.asdict(self.state), f)

    def load_state(self):
        def is_dataclass_dict(obj):
            return isinstance(obj, TypeDict)

        def fromdict(obj):
            if not is_dataclass_dict(obj):
                raise TypeError("fromdict() should be called on TypeDict instances")
            return _fromdict_inner(obj)

        with open(self.state_path, 'r') as f:
            state_dict = json.load(f)

        # reconstruct the dataclass using the type tag
        if is_dataclass_dict(state_dict):
            result = {}
            for name, data in obj.items():
                result[name] = _fromdict_inner(data)
            return state_dict.type(**result)

        # exactly the same as before (without the tuple clause)
        elif isinstance(state_dict, (list, tuple)):
            return type(state_dict)(_fromdict_inner(v) for v in state_dict)
        elif isinstance(state_dict, dict):
            return type(state_dict)((_fromdict_inner(k), _fromdict_inner(v))
                            for k, v in state_dict.items())
        else:
            return copy.deepcopy(state_dict)

class LogLine:
    raw_ascii = None
    full = None
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
        log_line.full = ANSI_ESCAPE.sub('', line)

        # Split the line on spaces
        parts = log_line.full.split()

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

# ----------------------------------------------------------------------------
# INTERFACE
# ----------------------------------------------------------------------------

class DebugApp(npyscreen.NPSApp):
    def __init__(self, root_path, exec_path=None):
        self.root_path = root_path
        self.exec_path = exec_path

    def main(self):
        df = DebugForm()

        if self.exec_path is not None:
            df.wStatus1.value = f' OBC-FIRMWARE DEBUG: {self.exec_path} '
        else:
            df.wStatus1.value = ' OBC-FIRMWARE DEBUG: serial '

        df.wStatus2.value = ''
        df.value.init(self.root_path, exec_path=self.exec_path)
        df.keypress_timeout = 1

        df.edit()

class DebugActionController(npyscreen.ActionControllerSimple):
    def create(self):
        self.add_action(r'^:q(uit)?', self.quit, False)
        self.add_action(r'^:f(ilter)?.*', self.set_filter, False)
        self.add_action(r'^:r(un)?', self.run, False)

    def quit(self, command, widget_proxy, live):
        self.parent.value.write_state()
        exit(0)

    def set_filter(self, command, widget_proxy, live):
        print(command.split()[1:])
        try:
            self.parent.value.set_filter(command.split()[1:])
        except RuntimeError as e:
            self.parent.wStatus2.value = f'Error: {e} '
        
        self.parent.display()

    def run(self, command, widget_proxy, live):
        pass

class DebugForm(npyscreen.FormMuttActiveTraditional):
    ACTION_CONTROLLER = DebugActionController
    DATA_CONTROLER = ObcFwInstance

    def while_editing(self, widget_proxy):
        self.value.update()
        self.wMain.values = [l.full for l in self.value.get_filtered()]

    def while_waiting(self):
        self.value.update()
        self.wMain.values = [l.full for l in self.value.get_filtered()]

# ----------------------------------------------------------------------------
# MAIN
# ----------------------------------------------------------------------------

if __name__ == '__main__':

    # Get the root path, i.e. the one containing src and builds
    root_path = Path(__file__).parent.parent.parent.absolute()

    # Parse arguments
    parser = argparse.ArgumentParser(
        description='Debuging interface for the OBC firmware.'
    )
    parser.add_argument(
        'executable', metavar='E', type=str, nargs='?',
        help='executable to run, or none to connect to serial'
    )
    args = parser.parse_args()

    da = DebugApp(
        root_path,
        exec_path=args.executable
    )
    da.run()