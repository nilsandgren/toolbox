#!/usr/bin/python3

"""
Tool for creating tmux sessions with window and pane layout defined in JSON.

NOTE: According to tmux a vertical split is a split that adds a new pane
      vertically relative the the current pane, in other words above or below.
      This is NOT how the term is used in this script. Instead a split's type
      refers to the direction of the split / cut itself. E.g. in the example
      below the window is first split vertically and then the right pane is
      split horizontally:

        .----------------------------------------.
        |                    |                   |
        |                    |                   |
        |                    |                   |
        |                    |                   |
        |                    |                   |
        |                    |-------------------|
        |                    |                   |
        |                    |                   |
        |                    |                   |
        |                    |                   |
        |                    |                   |
        '----------------------------------------'
"""

import json
import subprocess
import sys

VERBOSE=False

DEFAULT_LAYOUT = {
    "session": "default",
    "windows": [
        {
            "process": "vim",
            "name": "window1",
            "split": {
                "type": "vertical",
                "percentage": 50,
                "split": {
                    "type": "horizontal",
                    "percentage": 20
                }
            }
        },
        {
            "name": "window2",
            "split": {
                "type": "horizontal",
                "percentage": 30
            }
        }
    ]
}


def load_json(path):
    """Load JSON data from file path."""
    with open(path) as json_file:
        return json.load(json_file)




def main(argv):
    """Main entry point."""
    if len(argv) == 2:
        print(f"Loading layout configuration from: {argv[1]}")
        config = load_json(argv[1])
    else:
        print("Using default layout configuration")
        config = DEFAULT_LAYOUT

    print("Checking configuration")
    if not check_config(config):
        sys.exit(1)

    try:
        print("Loading layout")
        status = load_layout(config)
    except RuntimeError as re:
        status = False
        print("Error loading tmux layout", file=sys.stderr)
        print(re)

    rc = 0 if status == True else 1
    sys.exit(rc)


def check_config(config):
    """Sanity check of layout configuration."""
    session = config["session"]
    if " " in session:
        print("Session names cannot include spaces")
        return False
    if session_exists(session):
        print(f"There already exists a session named {session}")
        return False

    windows = config.get("windows")
    if windows is None:
        print('No "windows" list in layout configuration')
        return False

    for window in windows:
        if not check_window_config(window):
            return False

    return True


def session_exists(name):
    """Check if there is a tmux session with 'name'."""
    command = f'tmux has-session -t {name}'
    if run_command(command):
        return True
    return False


def check_window_config(window):
    """Sanity check of window configuration."""
    window_name = window.get("name")
    if window_name is None:
        print('Window has no "name"')
        return False

    if " " in window_name:
        print("Window names cannot include spaces")
        return False

    return check_split_config(window.get("split"))


def check_split_config(split):
    """Recursive sanity check of split configuration."""
    if split is None:
        # It is OK to not have any splits
        return True

    split_type = split.get("type")
    if split_type is None:
        print('No "type" field in split')
        return False

    if split_type not in ["horizontal", "vertical"]:
        print('The split type should be either "horizontal" or "vertical"')
        return False

    # Check next level
    return check_split_config(split.get("split"))


def load_layout(layout):
    """Create tmux session from layout."""
    session = layout["session"]
    window_counter = 1
    for window in layout["windows"]:
        if window_counter == 1:
            first_window_name = window["name"]
            add_session_and_window(session, window)
        else:
            add_window(session, window_counter, window)

        window_name = window["name"]
        split_window(window_name, window.get("split"))
        window_counter += 1

    # Select the first pane of the first window
    run_command(f"tmux select-window -t {first_window_name}")
    run_command(f"tmux select-pane -t 0")

    print()
    print(f'  Session named "{session}" created. ')
    print("  Enter the session:")
    print()
    print(f'      tmux attach -t {session}')
    print()
    return True


def add_session_and_window(session, window):
    """Create a session and the first window."""
    name = window["name"]
    process = window.get("process", "")
    command = f'tmux new -s {session} -n {name} -d {process}'
    if not run_command(command):
        raise RuntimeError("Failed to create session with window")


def add_window(session, number, window):
    """Add a window at session:number called 'name'."""
    name = window["name"]
    process = window.get("process", "")
    command = f'tmux new-window -t {session}:{number} -n {name} {process}'
    if not run_command(command):
        msg = f"Failed to add window {number} to session {session}"
        raise RuntimeError(msg)


def split_window(window_name, split, split_counter=1):
    """Recursively split the current window."""
    if split is None:
        return

    split_type = split.get("type")
    if split_type == "horizontal":
        split_command = f"tmux split-window -v -p 50 -d"
    else:
        split_command = f"tmux split-window -h -p 50 -d"

    if not run_command(split_command):
        print("Pane split command failed")
        return

    split_counter += 1
    select_command = f"tmux select-pane -t {split_counter}"
    if not run_command(select_command):
        print("Pane select command failed")
        return

    split_window(window_name, split.get("split"), split_counter=split_counter)


def run_command(command):
    """Run command as a subprocess. Return True if the exit code is 0."""
    global VERBOSE
    if VERBOSE:
        print(f"----------------------------")
        print(f"cmd   : {command}")

    try:
        out = ""
        err = ""
        proc = subprocess.Popen(command.split(), stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        out, err = proc.communicate()
    except OSError as e:
        print(f'Error: Failed to run command: {command}')
        print(f"stdout: {out}")
        print(f"stderr: {err}")
        return False

    if VERBOSE:
        print(f"rc    : {proc.returncode}")
        print(f"stdout: {out}")
        print(f"stderr: {err}")

    return proc.returncode == 0


if __name__ == "__main__":
    main(sys.argv)


