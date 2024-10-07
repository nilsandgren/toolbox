#!/bin/python3

from playsound import playsound # pip install playsound
import random
import subprocess
import os
import sys
import time

# Speed up ourSleep to test program
DEMO = False

WORK_TIME_MINUTES = 20
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


def ourSleep(seconds):
    demo = False
    if DEMO:
        seconds /= 200.0
    time.sleep(seconds)


def runCommand(command):
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


def colorPrint(color, string, **kwargs):
    print(f"{color}{string}{COLOR_RESET}", **kwargs)


def tombola(suggestions):
    maxNumber = len(suggestions) - 1;
    number = int(random.random() * maxNumber + 0.5)
    sleepTime = 0.1 / maxNumber
    padding = "                           "
    while sleepTime < 2:
        number = (number + 1) % maxNumber
        print(f"  - {suggestions[number][1]}{padding}\r", end="")
        sleepTime *= 1 + 0.5 * random.random()
        time.sleep(sleepTime)

    for _ in range(0,5):
        print(f"    {padding}{padding}\r", end="")
        time.sleep(.5)
        print(f"  - {suggestions[number][1]}{padding}\r", end="")
        time.sleep(.5)

    if REMOVE_SUGGESTED_TASK:
        suggestions.pop(number)

    print()


def printSuggestions(suggestions):
    print()
    print("For instance: ")
    for color, item in suggestions:
        colorPrint(color, f"  - {item}")
        time.sleep(0.2)
    print()
    ourSleep(1)
    print("Suggestion:")
    ourSleep(1)
    tombola(suggestions)
    print()


def headsUpWarning():
    headsUpSeconds = HEADS_UP_SECONDS

    print(f"Locking the screen in {headsUpSeconds} seconds")
    for i in range(headsUpSeconds, -1, -1):
        string = "|" * i + " " * (headsUpSeconds - i)
        print(f" {string}\r", end="")
        ourSleep(1)


def pause(minutes, suggestions):
    lockMinutes = minutes

    print()
    chime()
    colorPrint(COLOR_FG_MAG, f"Do something else for {lockMinutes} minutes")
    printSuggestions(suggestions)

    headsUpWarning()
    colorPrint(COLOR_FG_MAG, f"Locking for {lockMinutes} minutes now")
    time.sleep(1.5)
    # chime 1..x times to indicate length of pause
    chime(lockMinutes // 5)
    for i in range(0, lockMinutes):
        if i != 0 and i % 5 == 0:
            chime()
        # Lock every minute
        if not DEMO:
            runCommand("xdg-screensaver lock")
        ourSleep(60)
        print(f" Locked {i+1} minutes\r", end="")


def backFromPause():
    os.system("clear")
    pomodoro()
    chime(3)
    print()
    colorPrint(COLOR_FG_RED, "   ♥ ♥ ♥ ♥ ♥ ♥ ♥ ♥ ♥ ♥ ♥ ♥ ")
    colorPrint(COLOR_FG_GRE, "   Good that you took some ")
    colorPrint(COLOR_FG_GRE, "      time for yourself    ")
    colorPrint(COLOR_FG_RED, "   ♥ ♥ ♥ ♥ ♥ ♥ ♥ ♥ ♥ ♥ ♥ ♥ ")
    print()


def pomodoro():
    colorPrint(COLOR_FG_GRE, " ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀")
    colorPrint(COLOR_FG_GRE, "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀ ")
    colorPrint(COLOR_FG_GRE, "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠛⠻⣶⡆⠀⠿⠀⣶⠒⠊⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀ ")
    colorPrint(COLOR_FG_GRE, "⠀⠀⠀⠀⠀⠀⠀⠀⢀⣀⣴⠾⠛⢹⣶⡤⢶⣿⡟⠶⠦⠄⠀⠀⠀⠀⠀⠀⠀⠀ ")
    colorPrint(COLOR_FG_RED, "⠀⠀⠀⠀⠀⣠⣶⣤⣤⣤⣤⣴⠂⠸⠋⢀⣄⡉⠓⠀⠲⣶⣾⣿⣷⣄⠀⠀⠀⠀ ")
    colorPrint(COLOR_FG_RED, "⠀⠀⠀⢀⣾⡿⠋⠁⣠⣤⣿⡟⢀⣠⣾⣿⣿⣿⣷⣶⣤⣼⣿⣿⣿⣿⣆⠀⠀⠀ ")
    colorPrint(COLOR_FG_RED, "⠀⠀⠀⣾⡟⠀⣰⣿⣿⣿⣿⣷⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡄⠀⠀ ")
    colorPrint(COLOR_FG_RED, "⠀⠀⢸⡿⠀⣼⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⠀⠀ ")
    colorPrint(COLOR_FG_RED, "⠀⠀⢸⡇⢰⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⠀⠀ ")
    colorPrint(COLOR_FG_RED, "⠀⠀⢸⣿⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⠀⠀ ")
    colorPrint(COLOR_FG_RED, "⠀⠀⠸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠁⠀⠀ ")
    colorPrint(COLOR_FG_RED, "⠀⠀⠀⢻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠃⠀⠀⠀ ")
    colorPrint(COLOR_FG_RED, "⠀⠀⠀⠀⠙⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠟⠁⠀⠀⠀⠀ ")
    colorPrint(COLOR_FG_RED, "⠀⠀⠀⠀⠀⠀⠉⠛⠿⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠟⠋⠀⠀⠀⠀⠀⠀⠀ ")
    colorPrint(COLOR_FG_RED, "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⠉⠉⠉⠉⠉⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀ ")


def printPercentage(percentage):
    percentage = min(percentage, 100)
    step = 100/70.
    count = 0
    print(f" [", end="")
    while percentage >= step:
        percentage -= step
        colorPrint(COLOR_FG_GRE, "█", end="")
        count += 1

    fraction = percentage / step

    if fraction < 1/16:
        colorPrint(COLOR_FG_GRE, " ",  end="")
    elif fraction < 1/8:
        colorPrint(COLOR_FG_GRE, "▏",  end="")
    elif fraction < 1/4:
        colorPrint(COLOR_FG_GRE, "▎",  end="")
    elif fraction < 3/8:
        colorPrint(COLOR_FG_GRE, "▍",  end="")
    elif fraction < 1/2:
        colorPrint(COLOR_FG_GRE, "▌",  end="")
    elif fraction < 5/8:
        colorPrint(COLOR_FG_GRE, "▋", end="")
    elif fraction < 3/4:
        colorPrint(COLOR_FG_GRE, "▊", end="")
    elif fraction < 7/8:
        colorPrint(COLOR_FG_GRE, "▊", end="")
    else:
        colorPrint(COLOR_FG_GRE, "▉", end="")

    count += 1

    while count < int(100 / step + 0.5):
        colorPrint(COLOR_FG_GRE, " ", end="")
        count += 1

    print("]\r", end="")


def main():

    pomodoro()

    pause_index = random.choice(range(0, len(PAUSE_MINUTES)))

    def getPauseMinutes():
        nonlocal pause_index
        result = PAUSE_MINUTES[pause_index % len(PAUSE_MINUTES)]
        pause_index += 1
        return result

    workSeconds = None
    if len(sys.argv) > 1:
        workSeconds = int(sys.argv[1]) * 60

    suggestions = [(COLOR_FG_GRE, s) for s in GENERAL_SUGGESTIONS]
    suggestions += [(COLOR_FG_YEL, s) for s in TODAYS_SUGGESTIONS]
    if workSeconds == 0:
        pause(getPauseMinutes(), suggestions)
        backFromPause()

    while True:
        print()
        if workSeconds is None or workSeconds <= 0:
            workSeconds = int(WORK_TIME_MINUTES + random.random() * WORK_FUZZ_MINUTES) * 60

        while workSeconds > 0:
            percentage = 100 * workSeconds / (WORK_TIME_MINUTES * 60)
            printPercentage(percentage)
            ourSleep(5)
            workSeconds -= 5
        printPercentage(0)

        pause(getPauseMinutes(), suggestions)
        backFromPause()


if __name__ == '__main__':
    main()
