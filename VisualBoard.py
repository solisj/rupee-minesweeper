import FancyText

import sys
import time
import random
import ctypes

#pygame
import pygame

width = 6
height = 5
max_num_bad = 8
num_trials = 1

MAXNUMADJACENTRUPEES = 16

#import c board library
libfile = "./board.so"
#libfile = glob.glob("build/*/board*.so")[0]
boardlib = ctypes.CDLL(libfile)
#boardlib.getbadnumbers.restype = ctypes.c_int #default restype is fine b/c void
boardlib.getbadnumbers.argtypes = [ctypes.c_size_t, #width
                                   ctypes.c_size_t, #height
                                   (ctypes.c_int * width) * height, #board array
                                   ctypes.c_int, #numbad
                                   ctypes.c_int, #maxnumbad
                                   ctypes.c_int, #minbadlocation
                                   (ctypes.c_long * width) * height, #output array
                                  ]

#setup getnumbadrupees function
boardlib.getboardnumbadrupees.restype = ctypes.c_int
boardlib.getboardnumbadrupees.argtypes = [ctypes.c_size_t, #width
                                          ctypes.c_size_t, #height
                                          (ctypes.c_int * width) * height, #board
                                         ]

#rupee "type" (corresponds to c rupee type)
#Good    = 0
Unknown = 1
Green   = 2 # worth 1; no adjacent rupees
Blue    = 3 # worth 5; 1 or 2 adjacent rupees
Red     = 4 # worth 20; 3 or 4 adjacent rupees
Silver  = 5 # worth 100; 5 or 6 adjacent rupees
Gold    = 6 # worth 300; 7 or 8 adjacent rupees
Bad     = 9 # either Bomb or Rupoor

#colors
BLACK = (0, 0, 0)
WHITE = (255, 255, 255)

GREEN = (0, 255, 0)
BLUE = (0, 0, 255)
RED   = (255, 0, 0)
SILVER = (192, 192, 192)
GOLD = (255, 215, 0)

CYAN = (0, 255, 255)

#screen size
SCREEN_HEIGHT = 1000
SCREEN_WIDTH = SCREEN_HEIGHT * 16 // 9

def type(text):
    typing_speed = 75 #wpm
    for char in text:
        sys.stdout.write(char)
        sys.stdout.flush()
        time.sleep(5/typing_speed)
    print('')

def get_rupee_text(rupee):
    """
    Given a rupee, return a string
    representation of that rupee.
    """
    #didnt use 'match' statement
    #because it does not exist in python 3.8
    #if rupee == Good:
    #    return "Good"
    if rupee == Unknown:
        return "Unknown"
    elif rupee == Green:
        return "Green"
    elif rupee == Blue:
        return "Blue"
    elif rupee == Red:
        return "Red"
    elif rupee == Silver:
        return "Silver"
    elif rupee == Gold:
        return "Gold"
    else:
        return "Bad"

def get_rupee_color(rupee):
    """
    Given a rupee, return an (R, G, B) tuple
    representing its color.
    """
    #didnt use 'match' statement
    #because it does not exist in python 3.8
    #if rupee == Good:
    #    return CYAN
    if rupee == Unknown:
        return WHITE
    elif rupee == Green:
        return GREEN
    elif rupee == Blue:
        return BLUE
    elif rupee == Red:
        return RED
    elif rupee == Silver:
        return SILVER
    elif rupee == Gold:
        return GOLD
    else:
        return BLACK

def increment_rupee_color(rupee):
    """
    Given a rupee, give a different rupee.
    Should loop through all rupees, and then back
    to the original if called successively enough times.
    """
    colors = [#Good,
              Unknown,
              Green,
              Blue,
              Red,
              Silver,
              Gold,
              #Bomb,
              #Rupoor,
              Bad]

    return colors[(colors.index(rupee) + 1) % len(colors)]

def get_bad_probabilities(bad_nums, max_num_bad):
    """
    Given a matrix representing the amount of times
    out of all possible board states that a given
    rupee is bad and the maximum possible number
    of bad rupees, return a matrix which contains
    the probabilities that a given rupee in the
    board is bad.

    Format of matrix:
    A list of lists of numbers such that one can
    index into a rupee (x,y) as matrix[y][x].
    """
    height = len(bad_nums)
    width = len(bad_nums[0])
    #get sum of all squares
    total = 0
    for j in range(height):
        for i in range(width):
            total += bad_nums[j][i]

    #construct empty matrix
    probabilities = [[0 for i in range(width)] for j in range(height)]

    if total != 0:
        for j in range(height):
            for i in range(width):
                probabilities[j][i] = bad_nums[j][i]*max_num_bad/total

    return probabilities

def get_lowest_chances(bad_nums, board):
    """
    Given a matrix representing the amount of times
    out of all possible board states that a given
    rupee is bad and the maximum possible number
    of bad rupees, return a matrix which marks the
    unknown squares with the lowest chances of being
    a bad rupee. Such squares have a value of 1,
    others a value of 0.

    Format of matrix:
    A list of lists of numbers such that one can
    index into a rupee (x,y) as matrix[y][x].
    """
    width = len(bad_nums[0])
    height = len(bad_nums)

    lowest_chance = max([max(bad_nums[j]) for j in range(height)])
    for j in range(height):
        for i in range(width):
            if board[j][i] == Unknown:
                if bad_nums[j][i] < lowest_chance:
                    lowest_chance = bad_nums[j][i]

    lowest_chances = [[0 for i in range(width)] for j in range(height)]
    for j in range(height):
        for i in range(width):
            if bad_nums[j][i] == lowest_chance:
                if board[j][i] == Unknown:
                    lowest_chances[j][i] = 1

    return lowest_chances

if __name__ == "__main__":
    pygame.init()

    #open in new window
    size = (SCREEN_WIDTH, SCREEN_HEIGHT)
    screen = pygame.display.set_mode(size)
    pygame.display.set_caption("Rupee Minesweeper?")

    carry_on = True #continue game loop
    clock = pygame.time.Clock()

    myfont = pygame.font.SysFont('Comic Sans MS', 30)

    #initialize board
    row = (ctypes.c_int * width)(*[Unknown for i in range(width)])
    board = ((ctypes.c_int * width) * height)(*[row for j in range(height)])
    board[height-1][width-1] = Blue #set bottom right to Blue
    num_bad = 0
    #max_num_bad = 16
    min_bad_location = 0
    #initialize output matrix
    row = (ctypes.c_long * width)(*[0 for i in range(width)])
    output = ((ctypes.c_long * width) * height)(*[row for j in range(height)])

    #initialize "selector" square
    selector = Unknown

    board_left_edge = 0
    board_right_edge = SCREEN_WIDTH * 5/6
    board_top_edge = 0
    board_bottom_edge = SCREEN_HEIGHT

    square_pixel_width = (board_right_edge-board_left_edge)/width
    square_pixel_height = (board_bottom_edge-board_top_edge)/height

    line_thickness = 5

    selector_left_edge = SCREEN_WIDTH * 5/6
    selector_right_edge = SCREEN_WIDTH
    selector_top_edge = SCREEN_HEIGHT * 5/6
    selector_bottom_edge = SCREEN_HEIGHT

    selector_width = selector_right_edge - selector_left_edge
    selector_height = selector_bottom_edge - selector_top_edge

    updater_left_edge = SCREEN_WIDTH * 5/6
    updater_right_edge = SCREEN_WIDTH
    updater_top_edge = 0
    updater_bottom_edge = SCREEN_HEIGHT * 1/6

    updater_width = updater_right_edge - updater_left_edge
    updater_height = updater_bottom_edge - updater_top_edge

    new_small_board_left_edge = SCREEN_WIDTH * 5/6
    new_small_board_right_edge = SCREEN_WIDTH
    new_small_board_top_edge = SCREEN_HEIGHT * 1/6
    new_small_board_bottom_edge = SCREEN_HEIGHT * 2/6

    new_small_board_width = new_small_board_right_edge - new_small_board_left_edge
    new_small_board_height = new_small_board_bottom_edge - new_small_board_top_edge

    new_medium_board_left_edge = SCREEN_WIDTH * 5/6
    new_medium_board_right_edge = SCREEN_WIDTH
    new_medium_board_top_edge = SCREEN_HEIGHT * 2/6
    new_medium_board_bottom_edge = SCREEN_HEIGHT * 3/6

    new_medium_board_width = new_medium_board_right_edge - new_medium_board_left_edge
    new_medium_board_height = new_medium_board_bottom_edge - new_medium_board_top_edge

    new_large_board_left_edge = SCREEN_WIDTH * 5/6
    new_large_board_right_edge = SCREEN_WIDTH
    new_large_board_top_edge = SCREEN_HEIGHT * 3/6
    new_large_board_bottom_edge = SCREEN_HEIGHT * 4/6

    new_large_board_width = new_large_board_right_edge - new_large_board_left_edge
    new_large_board_height = new_large_board_bottom_edge - new_large_board_top_edge

    confirm_new_board_left_edge = SCREEN_WIDTH * 5/6
    confirm_new_board_right_edge = SCREEN_WIDTH
    confirm_new_board_top_edge = SCREEN_HEIGHT * 4/6
    confirm_new_board_bottom_edge = SCREEN_HEIGHT * 5/6

    confirm_new_board_width = confirm_new_board_right_edge - confirm_new_board_left_edge
    confirm_new_board_height = confirm_new_board_bottom_edge - confirm_new_board_top_edge

    #game loop

    rupee_blue = True

    durations = []

    #for i in range()
    for i in range(num_trials): 
        row = (ctypes.c_long * width)(*[0 for i in range(width)])
        output = ((ctypes.c_long * width) * height)(*[row for j in range(height)])
        start_time = time.time()
        boardlib.getbadnumbers(width,
                               height,
                               board,
                               num_bad,
                               max_num_bad,
                               min_bad_location,
                               output)
        duration = time.time() - start_time
        durations.append(duration)
        print("Trial " + str(i) + ": it took " + str(duration) + " seconds to calculate bad_numbers.")
    avg_duration = sum(durations)/num_trials
    print("Over " + str(num_trials) + " trials, the average time to calculate bad_numbers was " + str(avg_duration) + " seconds.")
    
    bad_probabilities = get_bad_probabilities(output, max_num_bad)
    lowest_chances = get_lowest_chances(output, board)

    new_small_board_selected = False
    new_medium_board_selected = False
    new_large_board_selected = False

    needs_update = False
    updating = False

    text = "null"
    #get user input
    while carry_on:
        for event in pygame.event.get():
            if event.type == pygame.MOUSEBUTTONUP:
                position = pygame.mouse.get_pos() #TODO do something with rupee at this spot?
                #update selector on click
                color = get_rupee_color(selector)
                selector_rect = pygame.Rect(selector_left_edge,
                                            selector_top_edge,
                                            selector_width,
                                            selector_height)
                if selector_rect.collidepoint(position):
                    selector = increment_rupee_color(selector)

                #make a new board of a certain size
                new_small_board_rect = pygame.Rect(new_small_board_left_edge,
                                                   new_small_board_top_edge,
                                                   new_small_board_width,
                                                   new_small_board_height)
                new_medium_board_rect = pygame.Rect(new_medium_board_left_edge,
                                                   new_medium_board_top_edge,
                                                   new_medium_board_width,
                                                   new_medium_board_height)
                new_large_board_rect = pygame.Rect(new_large_board_left_edge,
                                                   new_large_board_top_edge,
                                                   new_large_board_width,
                                                   new_large_board_height)
                if new_small_board_rect.collidepoint(position):
                    if new_small_board_selected == False:
                        new_small_board_selected = True
                        new_medium_board_selected = False
                        new_large_board_selected = False
                    else:
                        new_small_board_selected = False
                if new_medium_board_rect.collidepoint(position):
                    if new_medium_board_selected == False:
                        new_small_board_selected = False
                        new_medium_board_selected = True
                        new_large_board_selected = False
                    else:
                        new_medium_board_selected = False
                if new_large_board_rect.collidepoint(position):
                    if new_large_board_selected == False:
                        new_small_board_selected = False
                        new_medium_board_selected = False
                        new_large_board_selected = True
                    else:
                        new_large_board_selected = False

                confirm_new_board_rect = pygame.Rect(confirm_new_board_left_edge,
                                                   confirm_new_board_top_edge,
                                                   confirm_new_board_width,
                                                   confirm_new_board_height)
                if confirm_new_board_rect.collidepoint(position) and \
                   (new_small_board_selected or \
                    new_medium_board_selected or\
                    new_large_board_selected):
                    #change board size variables
                    if new_small_board_selected:
                        width = 5
                        height = 4
                        max_num_bad = 4
                        new_small_board_selected = False
                    elif new_medium_board_selected:
                        width = 6
                        height = 5
                        max_num_bad = 8
                        new_medium_board_selected = False
                    else: #large board selected
                        width = 8
                        height = 5
                        max_num_bad = 16
                        new_large_board_selected = False
                    #change c function arguments
                    boardlib.getbadnumbers.argtypes = [ctypes.c_size_t, #width
                                                       ctypes.c_size_t, #height
                                                       (ctypes.c_int * width) * height,
                                                       ctypes.c_int, #numbad
                                                       ctypes.c_int, #maxnumbad
                                                       ctypes.c_int, #minbadlocation
                                                       (ctypes.c_long * width) * height, #output array
                                                      ]
                    boardlib.getboardnumbadrupees.argtypes = [ctypes.c_size_t, #width
                                                              ctypes.c_size_t, #height
                                                              (ctypes.c_int * width) * height, #board
                                                             ]
                    #change size of squares
                    square_pixel_width = (board_right_edge-board_left_edge)/width
                    square_pixel_height = (board_bottom_edge-board_top_edge)/height
                    #initialize board
                    row = (ctypes.c_int * width)(*[Unknown for i in range(width)])
                    board = ((ctypes.c_int * width) * height)(*[row for j in range(height)])
                    updating = True

                #run getbadnumbers on click
                color = WHITE
                updater_rect = pygame.Rect(updater_left_edge,
                                           updater_top_edge,
                                           updater_width,
                                           updater_height)
                if updater_rect.collidepoint(position):
                    updating = True

                if updating:
                    #let user know update is in progress
                    color = WHITE
                    pygame.draw.rect(screen,
                                     color,
                                     [updater_left_edge,
                                      updater_top_edge,
                                      updater_width,
                                      updater_height],
                                      0)
                    border_color = BLACK
                    pygame.draw.rect(screen,
                                     border_color,
                                     [updater_left_edge,
                                      updater_top_edge,
                                      updater_width,
                                      updater_height],
                                      line_thickness)

                    left_edge = updater_left_edge
                    top_edge = updater_top_edge
                    text = "Updating..."
                    textsurface = FancyText.render(text, myfont, BLACK)
                    screen.blit(textsurface, (left_edge, top_edge))
                    pygame.display.flip() #TODO sketchy
                    #reset output matrix
                    row = (ctypes.c_long * width)(*[0 for i in range(width)])
                    output = ((ctypes.c_long * width) * height)(*[row for j in range(height)])
                    print("starting to calculate bad_numbers...")
                    start_time = time.time()
                    num_bad = boardlib.getboardnumbadrupees(width, height, board)
                    boardlib.getbadnumbers(width,
                                           height,
                                           board,
                                           num_bad,
                                           max_num_bad,
                                           min_bad_location,
                                           output) #TODO uh change this name idk
                    print("it took " + str(time.time() - start_time) + " seconds to calculate bad_numbers")
                    bad_probabilities = get_bad_probabilities(output, max_num_bad)
                    lowest_chances = get_lowest_chances(output, board)

                    needs_update = False
                    updating = False
                #update rupee on click
                for i in range(width):
                    for j in range(height):
                        left_edge = board_left_edge + \
                                    i/width*(board_right_edge-board_left_edge)
                        top_edge = board_top_edge + \
                                    j/height*(board_bottom_edge-board_top_edge)
                        rupee = board[j][i]
                        color = get_rupee_color(rupee)
                        rupee_rect = pygame.Rect(left_edge,
                                                 top_edge,
                                                 square_pixel_width,
                                                 square_pixel_height)
                        if rupee_rect.collidepoint(position):
                            board[j][i] = selector
                            needs_update = True
                            
            if event.type == pygame.QUIT: #user clicks 'quit'
                carry_on = False

        #game logic

        #TODO LOL

        pass

        #draw screen
        #clear screen
        screen.fill(WHITE)
        #draw stuff

        #textsurface = myfont.render("Player 1 HP: " + str(player_1.get_hp()),
        #                            False,
        #                            (0, 0, 0))
        #screen.blit(textsurface, (SCREEN_WIDTH*1/2, SCREEN_HEIGHT*3/4))
        #pygame.draw.rect(screen, GREEN, [SCREEN_WIDTH*1/2,
        #                                 SCREEN_HEIGHT*7/8 ,
        #                                 player_1.get_hp()/10*SCREEN_WIDTH*1/2,
        #                                 SCREEN_HEIGHT*1/16])

        #draw selector
        color = get_rupee_color(selector)
        pygame.draw.rect(screen,
                         color,
                         [selector_left_edge,
                          selector_top_edge,
                          selector_width,
                          selector_height],
                          0)
        border_color = BLACK
        pygame.draw.rect(screen,
                         border_color,
                         [selector_left_edge,
                          selector_top_edge,
                          selector_width,
                          selector_height],
                          line_thickness)

        left_edge = selector_left_edge
        top_edge = selector_top_edge
        text = "Current Rupee"
        textsurface = FancyText.render(text, myfont, BLACK)
        screen.blit(textsurface, (left_edge, top_edge))

        top_edge += myfont.get_height()
        text = "Selection:"
        textsurface = FancyText.render(text, myfont, BLACK)
        screen.blit(textsurface, (left_edge, top_edge))

        top_edge += myfont.get_height()
        text = get_rupee_text(selector)
        textsurface = FancyText.render(text, myfont, BLACK)
        screen.blit(textsurface, (left_edge, top_edge))

        #draw updater

        color = WHITE

        pygame.draw.rect(screen,
                         color,
                         [updater_left_edge,
                          updater_top_edge,
                          updater_width,
                          updater_height],
                          0)
        border_color = BLACK
        pygame.draw.rect(screen,
                         border_color,
                         [updater_left_edge,
                          updater_top_edge,
                          updater_width,
                          updater_height],
                          line_thickness)

        left_edge = updater_left_edge
        top_edge = updater_top_edge
        text = "Update" + ("*" if needs_update else "")
        textsurface = FancyText.render(text, myfont, BLACK)
        screen.blit(textsurface, (left_edge, top_edge))

        #draw new small board "button"

        color = WHITE

        pygame.draw.rect(screen,
                         color,
                         [new_small_board_left_edge,
                          new_small_board_top_edge,
                          new_small_board_width,
                          new_small_board_height],
                          0)
        border_color = BLACK
        pygame.draw.rect(screen,
                         border_color,
                         [new_small_board_left_edge,
                          new_small_board_top_edge,
                          new_small_board_width,
                          new_small_board_height],
                          line_thickness)

        left_edge = new_small_board_left_edge
        top_edge = new_small_board_top_edge
        text = "New Small"
        textsurface = FancyText.render(text, myfont, BLACK)
        screen.blit(textsurface, (left_edge, top_edge))

        top_edge += myfont.get_height()
        text = "Board"
        textsurface = FancyText.render(text, myfont, BLACK)
        screen.blit(textsurface, (left_edge, top_edge))

        if new_small_board_selected:
            top_edge += myfont.get_height()
            text = "(Selected)"
            textsurface = FancyText.render(text, myfont, BLACK)
            screen.blit(textsurface, (left_edge, top_edge))

        #draw new medium board "button"

        color = WHITE

        pygame.draw.rect(screen,
                         color,
                         [new_medium_board_left_edge,
                          new_medium_board_top_edge,
                          new_medium_board_width,
                          new_medium_board_height],
                          0)
        border_color = BLACK
        pygame.draw.rect(screen,
                         border_color,
                         [new_medium_board_left_edge,
                          new_medium_board_top_edge,
                          new_medium_board_width,
                          new_medium_board_height],
                          line_thickness)

        left_edge = new_medium_board_left_edge
        top_edge = new_medium_board_top_edge
        text = "New Medium"
        textsurface = FancyText.render(text, myfont, BLACK)
        screen.blit(textsurface, (left_edge, top_edge))

        top_edge += myfont.get_height()
        text = "Board"
        textsurface = FancyText.render(text, myfont, BLACK)
        screen.blit(textsurface, (left_edge, top_edge))

        if new_medium_board_selected:
            top_edge += myfont.get_height()
            text = "(Selected)"
            textsurface = FancyText.render(text, myfont, BLACK)
            screen.blit(textsurface, (left_edge, top_edge))

        #draw new large board "button"

        color = WHITE

        pygame.draw.rect(screen,
                         color,
                         [new_large_board_left_edge,
                          new_large_board_top_edge,
                          new_large_board_width,
                          new_large_board_height],
                          0)
        border_color = BLACK
        pygame.draw.rect(screen,
                         border_color,
                         [new_large_board_left_edge,
                          new_large_board_top_edge,
                          new_large_board_width,
                          new_large_board_height],
                          line_thickness)

        left_edge = new_large_board_left_edge
        top_edge = new_large_board_top_edge
        text = "New Large"
        textsurface = FancyText.render(text, myfont, BLACK)
        screen.blit(textsurface, (left_edge, top_edge))

        top_edge += myfont.get_height()
        text = "Board"
        textsurface = FancyText.render(text, myfont, BLACK)
        screen.blit(textsurface, (left_edge, top_edge))

        if new_large_board_selected:
            top_edge += myfont.get_height()
            text = "(Selected)"
            textsurface = FancyText.render(text, myfont, BLACK)
            screen.blit(textsurface, (left_edge, top_edge))

        #draw confirm new board "button"

        color = WHITE

        pygame.draw.rect(screen,
                         color,
                         [confirm_new_board_left_edge,
                          confirm_new_board_top_edge,
                          confirm_new_board_width,
                          confirm_new_board_height],
                          0)
        border_color = BLACK
        pygame.draw.rect(screen,
                         border_color,
                         [confirm_new_board_left_edge,
                          confirm_new_board_top_edge,
                          confirm_new_board_width,
                          confirm_new_board_height],
                          line_thickness)

        left_edge = confirm_new_board_left_edge
        top_edge = confirm_new_board_top_edge
        text = "CONFIRM NEW"
        textsurface = FancyText.render(text, myfont, BLACK)
        screen.blit(textsurface, (left_edge, top_edge))

        top_edge += myfont.get_height()
        text = "BOARD SELECTION"
        textsurface = FancyText.render(text, myfont, BLACK)
        screen.blit(textsurface, (left_edge, top_edge))

        #draw the squares***
        for i in range(width):
            for j in range(height):
                #TODO draw square (with possible meta-info?)
                left_edge = board_left_edge + \
                            i/width*(board_right_edge-board_left_edge)
                top_edge = board_top_edge + \
                            j/height*(board_bottom_edge-board_top_edge)
                rupee = board[j][i]
                color = get_rupee_color(rupee)
                pygame.draw.rect(screen,
                                 color,
                                 [left_edge,
                                  top_edge,
                                  square_pixel_width,
                                  square_pixel_height],
                                 0)

                #text = str(output[j][i])
                #textsurface = FancyText.render(text, myfont, BLACK)
                #screen.blit(textsurface, (left_edge, top_edge))

                #top_edge += myfont.get_height()
                text = get_rupee_text(rupee)
                textsurface = FancyText.render(text, myfont, BLACK)
                screen.blit(textsurface, (left_edge, top_edge))

                top_edge += myfont.get_height()
                text = "{0:.3f}%".format(bad_probabilities[j][i]*100)
                textsurface = FancyText.render(text, myfont, BLACK)
                screen.blit(textsurface, (left_edge, top_edge))

                if lowest_chances[j][i] == 1:
                    top_edge += myfont.get_height()
                    text = "lowest"
                    textsurface = FancyText.render(text, myfont, GOLD, BLACK)
                    screen.blit(textsurface, (left_edge, top_edge))

                    top_edge += myfont.get_height()
                    text = "chances"
                    textsurface = FancyText.render(text, myfont, GOLD, BLACK)
                    screen.blit(textsurface, (left_edge, top_edge))
        #draw lines (borders of squares)
        for i in range(width + 1):
            x = board_left_edge + \
                i/width*(board_right_edge-board_left_edge)
            top = (x, board_top_edge)
            bottom = (x, board_bottom_edge)
            color = BLACK
            pygame.draw.line(screen,
                             color,
                             top,
                             bottom,
                             line_thickness)
        for j in range(height + 1):
            y = board_top_edge + \
                j/height*(board_bottom_edge-board_top_edge)
            left = (board_left_edge, y)
            right = (board_right_edge, y)
            color = BLACK
            pygame.draw.line(screen,
                             color,
                             left,
                             right,
                             line_thickness)
        #pygame.draw.line(screen, GREEN, [0, 0], [100, 100], 5)
        #pygame.draw.ellipse(screen, BLACK, [20, 20, 250, 100], 2)

        #textsurface = myfont.render('Some Text', False, (0, 0, 0))
        #screen.blit(textsurface,(640/2, 480/2))

        #update screen
        pygame.display.flip()

        frame_limit = 60
        #limit fps
        clock.tick(frame_limit)
