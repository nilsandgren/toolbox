#!/bin/python3

from playsound import playsound # pip install playsound
import random
import subprocess
import sys
import time

# Speed up ourSleep to test program
DEMO = False

WORK_TIME_MINUTES = 20
WORK_FUZZ_MINUTES = 10
HEADS_UP_SECONDS = 10

COLOR_FG_RED = '\33[31m'
COLOR_FG_GRE = '\33[32m'
COLOR_FG_YEL = '\33[93m'
COLOR_FG_BLU = '\33[34m'
COLOR_FG_MAG = '\33[35m'
COLOR_RESET  = '\33[0m'

SOUND_PLAYER = "mplayer"
SOUND_PATH = "chime.wav"

TODAYS_SUGGESTIONS = [
    "Go to the grocery store"
]

GENERAL_SUGGESTIONS = [
    "Water some plants",
    "Yoga or stretch on the floor",
    "Yoga or stretch on a chair",
    "Take a short walk",
    "Take care of some laundry",
    "Eat some fruit",
    "Do a body scan",
    "Clean a bathroom",
    "Do a breathing exercise",
    "Pick up some dirty dishes",
    "Prepare some fire wood",
    "Make a sandwich",
    "Lift some weights",
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
    for _ in range(0, times):
        time.sleep(0.2)
        try:
            playsound(SOUND_PATH)
        except Exception:
            pass


def colorPrint(color, string):
    print(f"{color}{string}{COLOR_RESET}")


def tombola(suggestions):
    maxNumber = len(suggestions) - 1;
    number = int(random.random() * maxNumber + 0.5)
    sleepTime = 0.1 / maxNumber
    padding = "                           "
    while sleepTime < 2:
        number = (number + 1) % maxNumber
        print(f"  - {suggestions[number]}{padding}\r", end="")
        sleepTime *= 1 + 0.5 * random.random()
        time.sleep(sleepTime)

    for _ in range(0,5):
        print(f"    {padding}{padding}\r", end="")
        time.sleep(.5)
        print(f"  - {suggestions[number]}{padding}\r", end="")
        time.sleep(.5)

    print()


def printSuggestions():
    suggestions = [(COLOR_FG_GRE, s) for s in GENERAL_SUGGESTIONS]
    suggestions += [(COLOR_FG_YEL, s) for s in TODAYS_SUGGESTIONS]
    print()
    print("For instance: ")
    for color, item in suggestions:
        colorPrint(color, f"  - {item}")
        time.sleep(0.2)
    print()
    time.sleep(1)
    print("Suggestion:")
    time.sleep(1)
    tombola(GENERAL_SUGGESTIONS + TODAYS_SUGGESTIONS)
    print()


def headsUpWarning():
    headsUpSeconds = HEADS_UP_SECONDS

    print(f"Locking the screen in {headsUpSeconds} seconds")
    for i in range(headsUpSeconds, -1, -1):
        string = "|" * i + " " * (headsUpSeconds - i)
        print(f" {string}\r", end="")
        ourSleep(1)


def pause(minutes):
    lockMinutes = minutes

    print()
    chime()
    colorPrint(COLOR_FG_MAG, f"Do something else for {lockMinutes} minutes")
    printSuggestions()

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
    pomodoro()
    chime(3)
    print()
    colorPrint(COLOR_FG_GRE, "   Good that you took some time for yourself!")
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


def main():

    pomodoro()

    pause_minutes = [5, 10, 10, 15]
    pause_index = random.choice(range(0, len(pause_minutes)))

    def getPauseMinutes():
        nonlocal pause_index
        result = pause_minutes[pause_index % len(pause_minutes)]
        pause_index += 1
        return result

    workMinutes = None
    if len(sys.argv) > 1:
        workMinutes = int(sys.argv[1])

    if workMinutes == 0:
        pause(getPauseMinutes())
        backFromPause()

    while True:
        print()
        print("You can work for a while now")
        if workMinutes is None or workMinutes <= 0:
            workMinutes = int(WORK_TIME_MINUTES + random.random() * WORK_FUZZ_MINUTES)

        while workMinutes > 0:
            string = "|" * workMinutes + " " * workMinutes
            print(f" {string}\r", end="")
            ourSleep(60)
            workMinutes -= 1

        pause(getPauseMinutes())
        backFromPause()


if __name__ == '__main__':
    main()
