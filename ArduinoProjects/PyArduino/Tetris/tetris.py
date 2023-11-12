import pygame
from pygame.locals import *
import sys
import random
import pickle
import copy
import serial

#################################################### GLOBAL VARIABLES ######################################################

PORT = '/dev/ttyACM0'
arduinoControl = serial.Serial(PORT, 9600)
arduinoControl.timeout = 0.5

SCREEN_W = 1000
SCREEN_H = 900
GAME_W = 400
GAME_H = 800

ROWS = 20
COLS = 10

BLOCK_SIZE = GAME_H // ROWS     # =40

TOP_LEFTx = (SCREEN_W - GAME_W) // 2
TOP_LEFTy = SCREEN_H - GAME_H


# Colors
BORDER = (70, 85, 70)
GRID_LINE = (100, 75, 75)


LOCKED_POS = {}


# Score
SCORE = 0
try:
    with open('high_score.dat', 'rb') as file:
        HIGH_SCORE = pickle.load(file)
except:
    HIGH_SCORE = 0


# SHAPE FORMATS:
S = [['.....',
      '.....',
      '..00.',
      '.00..',
      '.....'],
     ['.....',
      '..0..',
      '..00.',
      '...0.',
      '.....']]

Z = [['.....',
      '.....',
      '.00..',
      '..00.',
      '.....'],
     ['.....',
      '...0.',
      '..00.',
      '..0..',
      '.....']]

I = [['.....',
      '.....',
      '..0..',
      '..0..',
      '..0..',
      '..0..'],
     ['.....',
      '.....',
      '0000.',
      '.....',
      '.....']]

O = [['.....',
      '.....',
      '.00..',
      '.00..',
      '.....']]

J = [['.....',
      '.....',
      '.0...',
      '.000.',
      '.....'],
     ['.....',
      '..00.',
      '..0..',
      '..0..',
      '.....'],
     ['.....',
      '.....',
      '.000.',
      '...0.',
      '.....'],
     ['.....',
      '..0..',
      '..0..',
      '.00..',
      '.....']]

L = [['.....',
      '.....',
      '...0.',
      '.000.',
      '.....'],
     ['.....',
      '..0..',
      '..0..',
      '..00.',
      '.....'],
     ['.....',
      '.....',
      '.000.',
      '.0...',
      '.....'],
     ['.....',
      '.00..',
      '..0..',
      '..0..',
      '.....']]

T = [['.....',
      '.....',
      '..0..',
      '.000.',
      '.....'],
     ['.....',
      '..0..',
      '..00.',
      '..0..',
      '.....'],
     ['.....',
      '.....',
      '.000.',
      '..0..',
      '.....'],
     ['.....',
      '..0..',
      '.00..',
      '..0..',
      '.....']]

shapes = [S, Z, I, O, J, L, T]
shape_colours = [(0, 255, 0), (255, 0, 0), (0, 0, 255), (255, 255, 0), (0, 255, 255), (255, 0, 255), (150, 150, 150)]

######################################################################################################################################################

##################################################################### CLASS ##########################################################################

class Piece:

    locked_pos = {}


    def random_init_pos(self):
        x = random.randrange(0, COLS-4)
        y = -2
        while (x, y+3) in LOCKED_POS:
            x = random.randrange(0, COLS-4)
            y = -2
        return (x, y)


    def __init__(self, shape):
        self.shape = shape
        self.color = shape_colours[shapes.index(shape)]
        self.pos = self.random_init_pos()
        self.orientation = 0
        self.dirny = 1
        self.dirnx = 0


    def change_orientation(self):
        self.orientation += 1
        self.orientation = self.orientation % len(self.shape)


    def has_stopped(self):
        return self.dirnx == 0 and self.dirny == 0


    def set_locked_pos(self):
        if self.has_stopped():
            return

        self.locked_pos = {}
        for i in range(len(self.shape[self.orientation])):
            temp = self.shape[self.orientation][i]
            if '0' in temp:
                for j, char in enumerate(temp):
                    if char == '0':
                        self.locked_pos[(self.pos[0] + j, self.pos[1] + i)] = self.color


    # dirn belongs to {'l', 'r', 'd', 's', 'u'}
    def move(self, dirn='d'):
        if dirn == 'l':
            self.dirnx = -1
            self.dirny = 1
        elif dirn == 'r':
            self.dirnx = 1
            self.dirny = 1
        elif dirn == 's':
            self.dirnx = 0
            self.dirny = 0
        elif dirn == 'u':
            self.dirnx = 0
            self.dirny = -1
        else:
            self.dirnx = 0
            self.dirny = 1
        self.pos = (self.pos[0] + self.dirnx, self.pos[1] + self.dirny)


###########################################################################################################################################################

#################################################################### ARDUINO PROCESSES ####################################################################

def main_menu_arduino_input():

    global arduinoControl

    data = arduinoControl.readline()
    data = str(data, 'utf-8').strip('\r\n')
 
    if data == 'Start':
        arduinoControl.write('Recieved\n'.encode())
        win.fill((0, 0, 0))
        game(win)
#__________________________________________________________________________________________________________________________________________________________

def game_arduino_input(block):

    global arduinoControl

    game_over = False
    data = arduinoControl.readline()
    data = str(data, 'utf-8').strip('\r\n')
 
    if data == 'Stop':
            game_over = True
            pygame.display.quit()
            pygame.quit()

    if data == 'Left':
        moveBlock(block, 'l')
        if not validSpace(block):
            moveBlock(block, 'r')
            if not validSpace(block):
                moveBlock(block, 'u')
                moveBlock(block, 'u')

    if data == 'Right':
        moveBlock(block, 'r')
        if not validSpace(block):
            moveBlock(block, 'l')
            if not validSpace(block):
                moveBlock(block, 'u')
                moveBlock(block, 'u')

    if data == 'Change':
        block.change_orientation()
        block.set_locked_pos()
        if not validSpace(block):
            for _ in range(len(block.shape) - 1):
                block.change_orientation()
                block.set_locked_pos()

    if data == 'Down':
        moveBlock(block, 'd')
        if not validSpace(block):
            moveBlock(block, 'u')

    return game_over

###########################################################################################################################################################

#################################################################### UTILITY FUNCTIONS ####################################################################

def makePiece():
    block = Piece(random.choice(shapes))
    block.set_locked_pos()
    return block
#__________________________________________________________________________________________________________________________________________________________

def makeGridColors(locked_pos=LOCKED_POS):
    grid_colors = [[(0, 0, 0) for _ in range(COLS)] for _ in range(ROWS)]

    for x in locked_pos:
        grid_colors[x[1]][x[0]] = locked_pos[x]

    return grid_colors
#__________________________________________________________________________________________________________________________________________________________

def validSpace(block):
    accepted_pos = [(j, i) for i in range(ROWS) for j in range(COLS)]

    for position in accepted_pos:
        if position in LOCKED_POS:
            accepted_pos = list(filter(lambda a: a!= position, accepted_pos))
    
    for pos in block.locked_pos:
        if not pos in accepted_pos:
            return False

    return True
#__________________________________________________________________________________________________________________________________________________________

def moveBlock(block, dirn='d'):
    block.move(dirn)
    block.set_locked_pos()

#__________________________________________________________________________________________________________________________________________________________

def drawGrid(surface, grid_colors):
    for i in range(ROWS):
        for j in range(COLS):
            x = TOP_LEFTx + j*BLOCK_SIZE
            y = TOP_LEFTy + i*BLOCK_SIZE
            pygame.draw.rect(surface, grid_colors[i][j], (x, y, BLOCK_SIZE, BLOCK_SIZE), 0)
            
    x = TOP_LEFTx
    y = TOP_LEFTy
    for i in range(ROWS):
        x += BLOCK_SIZE
        y += BLOCK_SIZE
        pygame.draw.line(surface, GRID_LINE, (TOP_LEFTx, y), (TOP_LEFTx + GAME_W, y))

    x = TOP_LEFTx
    y = TOP_LEFTy
    for j in range(COLS):
        x += BLOCK_SIZE
        y += BLOCK_SIZE
        pygame.draw.line(surface, GRID_LINE, (x, TOP_LEFTy), (x, TOP_LEFTy + GAME_H))

    pygame.draw.rect(surface, BORDER, (TOP_LEFTx - 3, TOP_LEFTy - 3, GAME_W + 3, GAME_H + 3), 3)

#__________________________________________________________________________________________________________________________________________________________

def showNextPiece(surface, next_piece):
    global S, Z, I, O, J, L, T
    x = TOP_LEFTx + GAME_W + BLOCK_SIZE
    y = SCREEN_H // 2
    
    font = pygame.font.SysFont("monospace", 21)
    label = font.render("Next Piece:", 1, (255, 255, 255))
    surface.blit(label, (x + BLOCK_SIZE - 8, y))
    
    a = 6 if next_piece.shape == I else 5
    k = 1
    for i in range(a):
        temp = next_piece.shape[next_piece.orientation][i]
        y = SCREEN_H // 2
        if '0' in temp:
            y = y + k*BLOCK_SIZE
            k += 1
            x = TOP_LEFTx + GAME_W
            for j, char in enumerate(temp):
                x = x + BLOCK_SIZE
                if char == '0':
                    pygame.draw.rect(surface, next_piece.color, (x, y, BLOCK_SIZE, BLOCK_SIZE))
                else:
                    pygame.draw.rect(surface, (0, 0, 0), (x, y, BLOCK_SIZE, BLOCK_SIZE))

#__________________________________________________________________________________________________________________________________________________________

def clearPrevPiece(surface, piece):
    global S, Z, I, O, J, L, T
    x = TOP_LEFTx + GAME_W + BLOCK_SIZE
    y = SCREEN_H // 2
    
    font = pygame.font.SysFont("monospace", 21)
    label = font.render("Next Piece:", 1, (255, 255, 255))
    surface.blit(label, (x + BLOCK_SIZE - 8, y))
    
    a = 6 if piece.shape == I else 5
    k = 1
    for i in range(a):
        temp = piece.shape[piece.orientation][i]
        y = SCREEN_H // 2
        if '0' in temp:
            y = y + k*BLOCK_SIZE
            k += 1
            x = TOP_LEFTx + GAME_W
            for j, char in enumerate(temp):
                x = x + BLOCK_SIZE
                pygame.draw.rect(surface, (0, 0, 0), (x, y, BLOCK_SIZE, BLOCK_SIZE))

#__________________________________________________________________________________________________________________________________________________________

def displayPoints(surface):
    font = pygame.font.SysFont("monospace", 21)
    label1 = font.render(f"Points: {SCORE}", 1, (255, 255, 255))
    label2 = font.render(f"High Score: {HIGH_SCORE}", 1, (255, 255, 255))
    pygame.draw.rect(surface, (0, 0, 0), (TOP_LEFTx - max(label1.get_width(), label2.get_width()) - BLOCK_SIZE, SCREEN_H // 2, max(label1.get_width(), label2.get_width()), label1.get_height() + label2.get_height() + 20))
    surface.blit(label1, (TOP_LEFTx - label1.get_width() - BLOCK_SIZE, SCREEN_H // 2))
    surface.blit(label2, (TOP_LEFTx - label2.get_width() - BLOCK_SIZE, SCREEN_H // 2 + label1.get_height() + 10))

#__________________________________________________________________________________________________________________________________________________________

def redraw(surface, block, next_block):

    global colors, HIGH_SCORE
    font = pygame.font.SysFont("monospace", (SCREEN_H - GAME_H) - 10)
    label = font.render("Tetris", 1, (255, 255, 255))
    surface.blit(label, (SCREEN_W//2 - label.get_width()//2, 5))
    
    colors = makeGridColors({**LOCKED_POS, **block.locked_pos})
    drawGrid(surface, colors)
    showNextPiece(surface, next_block)
    displayPoints(surface)
    
    if SCORE >= HIGH_SCORE:
        HIGH_SCORE = SCORE

    pygame.display.update()

#__________________________________________________________________________________________________________________________________________________________

def isGameOver():
    for j in range(COLS):
            if (j, 0) in LOCKED_POS:
                return True
    return False

#__________________________________________________________________________________________________________________________________________________________

def isRowFull():
    global SCORE
    for y in range(ROWS-1, -1, -1):
        bool_list = [False]*COLS
        for x in range(COLS):
            if (x, y) in LOCKED_POS:
                bool_list[x] = True

        check_list = list(filter(lambda a: a, bool_list))
        if len(check_list) == COLS:
            SCORE += COLS
            return y
    
    return None

#__________________________________________________________________________________________________________________________________________________________

def removeRow(row):
    for j in range(COLS):
        LOCKED_POS.pop((j, row))

    y = copy.deepcopy(row)
    for i in range(ROWS-1, 0, -1):
        for j in range(COLS):
            k = ROWS - 1
            while (j, y-1) in LOCKED_POS:
                if (j, k) not in LOCKED_POS:
                    LOCKED_POS[(j, k)] = LOCKED_POS[(j, y-1)]
                    LOCKED_POS.pop((j, y-1))
                    break
                k -= 1
        y -= 1
        if y == 0:
            break

#__________________________________________________________________________________________________________________________________________________________

def display_at_center(win, text):
    
    font = pygame.font.SysFont("monospace", 60)
    label = font.render(text, 1, (255, 255, 255))
    win.blit(label, (SCREEN_W//2 - label.get_width()//2, SCREEN_H//2 - label.get_height()//2))
    pygame.display.update()

#__________________________________________________________________________________________________________________________________________________________

def resetGlobals():
    global LOCKED_POS, SCORE
    LOCKED_POS = {}
    SCORE = 0

###########################################################################################################################################################

####################################################################### GAME FUNCTIONS ####################################################################

def game(win):
    
    # Create a block and the next block
    block = makePiece()
    next_block = makePiece()
    
    game_over = False
    
    global S, Z, T, L, J, O, I
    clk = pygame.time.Clock()
    fall_time = 0
    fall_time_limit = 1       # in seconds
    fall_speed_time = 0
    
    # Game Loop
    while not game_over:
       
        fall_time += clk.get_rawtime()          # in ms
        fall_speed_time += clk.get_rawtime()    # in ms
        clk.tick()

        # Move the block down every fall_time_limit seconds
        if fall_time/1000 > fall_time_limit:
            fall_time = 0
            moveBlock(block)
            if not validSpace(block):
                moveBlock(block, 'u')
                LOCKED_POS.update(block.locked_pos)
                block = next_block
                clearPrevPiece(win, block)
                next_block = makePiece()
        
        # Increase speed of falling block every 5 seconds upto terminal velocity
        if fall_speed_time/1000 > 5:
            fall_speed_time = 0
            if fall_time_limit > 0.12:          # Terminal velociy
                fall_time_limit -= 0.01
        

        # Check and deal with filled rows
        row_num = isRowFull()
        while row_num != None:
            removeRow(row_num)
            row_num = isRowFull()

        # Redraw Graphics
        redraw(win, block, next_block)

        # Handling User Input: Controller Inputs
        game_over = game_arduino_input(block)

        # Close window handling
        for event in pygame.event.get():
            if event.type == QUIT:
                game_over = True
                pygame.display.quit()
                pygame.quit()
                sys.exit()

        # Check for Game Over Condition
        if not game_over:
            game_over = isGameOver()

        if game_over:
            global arduinoControl
            arduinoControl.write('Exit Game Loop\n'.encode())

    try:
        win.fill((0, 0, 0))
        display_at_center(win, f"You Lost! SCORE: {SCORE}")
        resetGlobals()
        pygame.time.delay(2000)
    except Exception as e:
        print("game(win):", e)

#__________________________________________________________________________________________________________________________________________________________

def main_menu(win):

    global p1, p2
        
    run = True
    
    while run:
        
        try:
            win.fill((0, 0, 0))
            display_at_center(win, "Press Any Button To Play!")

            main_menu_arduino_input()
        except Exception as e:
            print("main_menu(win):", e)

###########################################################################################################################################################

if __name__ == '__main__':
    
    pygame.init()
    win = pygame.display.set_mode((SCREEN_W, SCREEN_H))
    pygame.display.set_caption("TETRIS")
    
    main_menu(win)

    with open('high_score.dat', 'wb') as file:
        pickle.dump(HIGH_SCORE, file)
    
    pygame.display.quit()
    pygame.quit()
    sys.exit()

###########################################################################################################################################################
