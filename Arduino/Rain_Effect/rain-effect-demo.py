## Library imports
import os
import random
import time

## Board parameters and declarations
ROWS = 8
COLUMNS = 16
DISPLAY_COLOR_BOARD = []

## Chosen "color numbers" for the drops
COLOR_SEQUENCE = [5, 4, 3, 2, 1, 0]
LEAD_MARKER = 5
DROPLESS_MARKER = 0
MAX_DROPS = 2
DROP_PERCENTAGE = 10 # 10 = 10%, 4 = 25%

def _initBoard():
    """
    Fills the board with zeros with correct column and row sizes.
    """
    y = 0
    while y < ROWS:
        DISPLAY_COLOR_BOARD.append([0] * COLUMNS)
        y+=1

def printBoard():
    os.system("clear")
    y = 0
    while y < ROWS:
        x = 0
        print(DISPLAY_COLOR_BOARD[y])
        y+=1

def checkColumnForRaindrop(column, dropless_marker=DROPLESS_MARKER):
    y = 0
    while y < ROWS:
        if DISPLAY_COLOR_BOARD[y][column] != dropless_marker:
            return True
        y+=1
    return False
    
def countRaindrops():
    raindrops = 0
    x = 0
    while x < COLUMNS:
        if checkColumnForRaindrop(x):
            raindrops += 1
        x+=1
    return raindrops

def _getNextColor(current_color):
    """
    Returns the next color in the sequence in COLOR_SEQUENCE but if
    the current_color cannot be found, it returns the current color
    just in case that's a static object of some sort.
    """
    try:
        next_color_index = COLOR_SEQUENCE.index(current_color) + 1
        if next_color_index >= len(COLOR_SEQUENCE):
            return DROPLESS_MARKER
        else:
            return COLOR_SEQUENCE[next_color_index]
    except:
        return current_color

def moveDrops(lead_marker=LEAD_MARKER):
    """
    Moves the drops, iterating from the bottom and moving up to prevent
    the overwriting of the lead of the drop.
    """
    y = ROWS - 1
    while y >= 0:
        x = COLUMNS - 1
        while x >= 0:
            DISPLAY_COLOR_BOARD[y][x] = _getNextColor(DISPLAY_COLOR_BOARD[y][x])
            try:
                if DISPLAY_COLOR_BOARD[y][x] == COLOR_SEQUENCE[1]:
                    DISPLAY_COLOR_BOARD[y+1][x] = COLOR_SEQUENCE[0]
            except:
                pass
            x-=1
        y-=1

def pickRandomColumn():
    return random.randint(0, COLUMNS - 1)

def pickRandomDropOrNot():
    probability = [True] + [False] * DROP_PERCENTAGE
    return random.choice(probability)

def iterateDrops():
    # move drops
    moveDrops()

    # Check number of active drops
    if countRaindrops() < MAX_DROPS:
        if pickRandomDropOrNot():
            DISPLAY_COLOR_BOARD[0][pickRandomColumn()] = LEAD_MARKER

    # Print the board
    printBoard()
    time.sleep(0.1)


## Do stuff
_initBoard()
while True:
    iterateDrops()