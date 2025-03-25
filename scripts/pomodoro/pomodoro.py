#!/bin/python3

from playsound import playsound # pip install playsound
import random
import subprocess
import os
import sys
import time

# Speed up our_sleep to test program
DEMO = False

WORK_TIME_MINUTES = 25
WORK_FUZZ_MINUTES = 0
HEADS_UP_SECONDS = 15
PAUSE_MINUTES = [10, 10, 10, 10]

COLOR_FG_RED = '\33[31m'
COLOR_FG_GRE = '\33[32m'
COLOR_FG_YEL = '\33[93m'
COLOR_FG_BLU = '\33[34m'
COLOR_FG_MAG = '\33[35m'
COLOR_RESET  = '\33[0m'

SOUND_PLAYER = "mplayer"
SOUND_PATH = "chime.wav"
SOUND_ON = True

REMOVE_SUGGESTED_TASK = True
TODAYS_SUGGESTIONS = [
]

GENERAL_SUGGESTIONS = [
    "Water some plants",
    "Yoga or stretch on the floor",
    "Take a short walk",
    "Take care of some laundry",
    "Do a body scan",
    "Clean a bathroom",
    "Do a breathing exercise",
    "Pick up some dirty dishes",
    "Just rest"
]


def our_sleep(seconds):
    demo = False
    if DEMO:
        seconds /= 200.0
    time.sleep(seconds)


def run_command(command):
    proc = subprocess.Popen(command.split(),
                            stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE)
    out, err = proc.communicate()
    assert proc.returncode == 0


def chime(times=1):
    if not SOUND_ON:
        return
    for _ in range(0, times):
        time.sleep(0.2)
        try:
            playsound(SOUND_PATH)
        except Exception:
            pass


def color_print(color, string, **kwargs):
    print(f"{color}{string}{COLOR_RESET}", **kwargs)


def tombola(suggestions):
    max_num = len(suggestions) - 1;
    number = int(random.random() * max_num + 0.5)
    sleep_time = 0.1 / max_num
    padding = "                           "
    while sleep_time < 2:
        number = (number + 1) % max_num
        print(f"  - {suggestions[number][1]}{padding}\r", end="")
        sleep_time *= 1 + 0.5 * random.random()
        time.sleep(sleep_time)

    for _ in range(0,5):
        print(f"    {padding}{padding}\r", end="")
        time.sleep(.5)
        print(f"  - {suggestions[number][1]}{padding}\r", end="")
        time.sleep(.5)

    if REMOVE_SUGGESTED_TASK:
        suggestions.pop(number)

    print()


def print_suggestions(suggestions):
    print()
    print("For instance: ")
    for color, item in suggestions:
        color_print(color, f"  - {item}")
        time.sleep(0.2)
    print()
    our_sleep(1)
    print("Suggestion:")
    our_sleep(1)
    tombola(suggestions)
    print()


def heads_up_warning():
    heads_up = HEADS_UP_SECONDS

    print(f"Locking the screen in {heads_up} seconds")
    for i in range(heads_up, -1, -1):
        string = "|" * i + " " * (heads_up - i)
        print(f" {string}\r", end="")
        our_sleep(1)


def pause(minutes, suggestions):
    lock_minutes = minutes

    print()
    chime()
    color_print(COLOR_FG_MAG, f"Do something else for {lock_minutes} minutes")
    print_suggestions(suggestions)

    heads_up_warning()
    color_print(COLOR_FG_MAG, f"Locking for {lock_minutes} minutes now")
    time.sleep(1.5)
    # chime 1..x times to indicate length of pause
    chime(lock_minutes // 5)
    for i in range(0, lock_minutes):
        if i != 0 and i % 5 == 0:
            chime()
        # Lock every minute
        if not DEMO:
            run_command("xdg-screensaver lock")
        our_sleep(60)
        print(f" Locked {i+1} minutes\r", end="")


def back_from_pause():
    os.system("clear")
    pomodoro()
    chime(3)
    print()
    color_print(COLOR_FG_RED, "   ♥ ♥ ♥ ♥ ♥ ♥ ♥ ♥ ♥ ♥ ♥ ♥ ")
    color_print(COLOR_FG_GRE, "   Good that you took some ")
    color_print(COLOR_FG_GRE, "      time for yourself    ")
    color_print(COLOR_FG_RED, "   ♥ ♥ ♥ ♥ ♥ ♥ ♥ ♥ ♥ ♥ ♥ ♥ ")
    print()


def pomodoro():
    color_print(COLOR_FG_GRE, " ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀")
    color_print(COLOR_FG_GRE, "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀ ")
    color_print(COLOR_FG_GRE, "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠛⠻⣶⡆⠀⠿⠀⣶⠒⠊⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀ ")
    color_print(COLOR_FG_GRE, "⠀⠀⠀⠀⠀⠀⠀⠀⢀⣀⣴⠾⠛⢹⣶⡤⢶⣿⡟⠶⠦⠄⠀⠀⠀⠀⠀⠀⠀⠀ ")
    color_print(COLOR_FG_RED, "⠀⠀⠀⠀⠀⣠⣶⣤⣤⣤⣤⣴⠂⠸⠋⢀⣄⡉⠓⠀⠲⣶⣾⣿⣷⣄⠀⠀⠀⠀ ")
    color_print(COLOR_FG_RED, "⠀⠀⠀⢀⣾⡿⠋⠁⣠⣤⣿⡟⢀⣠⣾⣿⣿⣿⣷⣶⣤⣼⣿⣿⣿⣿⣆⠀⠀⠀ ")
    color_print(COLOR_FG_RED, "⠀⠀⠀⣾⡟⠀⣰⣿⣿⣿⣿⣷⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡄⠀⠀ ")
    color_print(COLOR_FG_RED, "⠀⠀⢸⡿⠀⣼⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⠀⠀ ")
    color_print(COLOR_FG_RED, "⠀⠀⢸⡇⢰⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⠀⠀ ")
    color_print(COLOR_FG_RED, "⠀⠀⢸⣿⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⠀⠀ ")
    color_print(COLOR_FG_RED, "⠀⠀⠸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠁⠀⠀ ")
    color_print(COLOR_FG_RED, "⠀⠀⠀⢻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠃⠀⠀⠀ ")
    color_print(COLOR_FG_RED, "⠀⠀⠀⠀⠙⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠟⠁⠀⠀⠀⠀ ")
    color_print(COLOR_FG_RED, "⠀⠀⠀⠀⠀⠀⠉⠛⠿⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠟⠋⠀⠀⠀⠀⠀⠀⠀ ")
    color_print(COLOR_FG_RED, "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⠉⠉⠉⠉⠉⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀ ")


def print_percentage(percentage):
    percentage = min(percentage, 100)
    step = 100/70.
    count = 0
    print(f" [", end="")
    while percentage >= step:
        percentage -= step
        color_print(COLOR_FG_GRE, "█", end="")
        count += 1

    fraction = percentage / step

    if fraction < 1/16:
        color_print(COLOR_FG_GRE, " ",  end="")
    elif fraction < 1/8:
        color_print(COLOR_FG_GRE, "▏",  end="")
    elif fraction < 1/4:
        color_print(COLOR_FG_GRE, "▎",  end="")
    elif fraction < 3/8:
        color_print(COLOR_FG_GRE, "▍",  end="")
    elif fraction < 1/2:
        color_print(COLOR_FG_GRE, "▌",  end="")
    elif fraction < 5/8:
        color_print(COLOR_FG_GRE, "▋", end="")
    elif fraction < 3/4:
        color_print(COLOR_FG_GRE, "▊", end="")
    elif fraction < 7/8:
        color_print(COLOR_FG_GRE, "▊", end="")
    else:
        color_print(COLOR_FG_GRE, "▉", end="")

    count += 1

    while count < int(100 / step + 0.5):
        color_print(COLOR_FG_GRE, " ", end="")
        count += 1

    print("]\r", end="")


def main():

    pomodoro()

    pause_index = random.choice(range(0, len(PAUSE_MINUTES)))

    def get_pause_minutes():
        nonlocal pause_index
        result = PAUSE_MINUTES[pause_index % len(PAUSE_MINUTES)]
        pause_index += 1
        return result

    work_seconds = None
    if len(sys.argv) > 1:
        work_seconds = int(sys.argv[1]) * 60

    suggestions = [(COLOR_FG_GRE, s) for s in GENERAL_SUGGESTIONS]
    suggestions += [(COLOR_FG_YEL, s) for s in TODAYS_SUGGESTIONS]
    if work_seconds == 0:
        pause(get_pause_minutes(), suggestions)
        back_from_pause()

    while True:
        print()
        if work_seconds is None or work_seconds <= 0:
            work_seconds = int(WORK_TIME_MINUTES + random.random() * WORK_FUZZ_MINUTES) * 60

        while work_seconds > 0:
            percentage = 100 * work_seconds / (WORK_TIME_MINUTES * 60)
            print_percentage(percentage)
            our_sleep(5)
            work_seconds -= 5
        print_percentage(0)

        pause(get_pause_minutes(), suggestions)
        back_from_pause()


if __name__ == '__main__':
    main()
