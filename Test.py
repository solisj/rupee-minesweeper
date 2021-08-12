import unittest
import ctypes
import random
import time

import VisualBoard

# Test that new (faster) version of getbadnumbers works
# by comparing output to older (slower) version

#import slow c board library
slowlibfile = "./slowboard.so"
slowboardlib = ctypes.CDLL(slowlibfile)
#boardlib.getbadnumbers.restype = ctypes.c_int #TODO is default restype void?
slowboardlib.getbadnumbers.argtypes = [ctypes.c_size_t, #width
                                       ctypes.c_size_t, #height
                                       (ctypes.c_int * VisualBoard.width) * VisualBoard.height, #board array TODO how to deal with enum?
                                       ctypes.c_int, #numbad
                                       ctypes.c_int, #maxnumbad
                                       ctypes.c_int, #minbadlocation
                                       (ctypes.c_long * VisualBoard.width) * VisualBoard.height, #output array
                                      ]

#setup slow getnumbadrupees function
slowboardlib.getboardnumbadrupees.restype = ctypes.c_int
slowboardlib.getboardnumbadrupees.argtypes = [ctypes.c_size_t, #width
                                              ctypes.c_size_t, #height
                                              (ctypes.c_int * VisualBoard.width) * VisualBoard.height, #board
                                             ]

def random_valid_board(width, height, num_unknown):
    #initialize board
    row = (ctypes.c_int * VisualBoard.width)(*[VisualBoard.Unknown for i in range(VisualBoard.width)])
    board = ((ctypes.c_int * VisualBoard.width) * VisualBoard.height)(*[row for j in range(VisualBoard.height)])

    bad_locations = get_random_rupees(board, VisualBoard.max_num_bad)
    for bad_location in bad_locations:
        x = bad_location[0]
        y = bad_location[1]
        board[y][x] = VisualBoard.Bad

    #populate entire board
    for i in range(VisualBoard.width):
        for j in range(VisualBoard.height):
            if board[j][i] != VisualBoard.Bad:
                num_bad_neighbors = get_num_bad_neighbors(board, i, j)
                if num_bad_neighbors == 0:
                    rupee = VisualBoard.Green
                elif num_bad_neighbors == 1 or num_bad_neighbors == 2:
                    rupee = VisualBoard.Blue
                elif num_bad_neighbors == 3 or num_bad_neighbors == 4:
                    rupee = VisualBoard.Red
                elif num_bad_neighbors == 5 or num_bad_neighbors == 6:
                    rupee = VisualBoard.Silver
                else: #num_bad_neighbors is 7 or 8
                    rupee = VisualBoard.Gold
                board[j][i] = rupee

    #mark num_unknown tiles as Unknown
    unknown_locations = get_random_rupees(board, num_unknown)
    for unknown_location in unknown_locations:
        x = unknown_location[0]
        y = unknown_location[1]
        board[y][x] = VisualBoard.Unknown

    return board

def get_num_bad_neighbors(board, x, y):
    """
    Determine the amount of adjacent bad
    squares around (x, y).
    """
    num_bad_neighbors = 0
    for i in range(x - 1, x + 1 + 1):
        for j in range(y - 1, y + 1 + 1):
            if (i == x and j == y):
                pass
            elif (i >= 0 and i < len(board[0]) and
                j >= 0 and j < len(board)):
                if board[j][i] == VisualBoard.Bad:
                    num_bad_neighbors += 1
    return num_bad_neighbors

def get_random_rupees(board, k):
    """
    Return a list containing
    k pairs (tuples of length 2)
    of distinct, random coordinates
    inside of board.
    """
    width = len(board[0])
    height = len(board)
    random_rupees = []
    locations = random.sample(range(len(board[0]) * len(board)), k)
    for location in locations:
        x = location % width
        y = (location - (location % width)) // height
        random_rupees.append((x, y))
    return random_rupees

def compare_board_results(board):
    """
    Run getbadnumbers on the old
    and new implementations, and
    verify that the results are
    the same.

    Return true if the resulting
    matrices have equal contents."""
    row = (ctypes.c_long * VisualBoard.width)(*[0 for i in range(VisualBoard.width)])
    slowoutput = ((ctypes.c_long * VisualBoard.width) * VisualBoard.height)(*[row for j in range(VisualBoard.height)])

    row = (ctypes.c_long * VisualBoard.width)(*[0 for i in range(VisualBoard.width)])
    output = ((ctypes.c_long * VisualBoard.width) * VisualBoard.height)(*[row for j in range(VisualBoard.height)])

    num_bad = VisualBoard.boardlib.getboardnumbadrupees(VisualBoard.width,
                                                        VisualBoard.height,
                                                        board)
    slow_num_bad = slowboardlib.getboardnumbadrupees(VisualBoard.width,
                                                     VisualBoard.height,
                                                     board)

    min_bad_location = 0

    #time fast version
    start = time.time()
    VisualBoard.boardlib.getbadnumbers(VisualBoard.width,
                                       VisualBoard.height,
                                       board,
                                       num_bad,
                                       VisualBoard.max_num_bad,
                                       min_bad_location,
                                       output)
    duration = time.time() - start
    #print("Fast Version took " + str(duration) + " seconds.")

    start = time.time()
    slowboardlib.getbadnumbers(VisualBoard.width,
                               VisualBoard.height,
                               board,
                               slow_num_bad,
                               VisualBoard.max_num_bad,
                               min_bad_location,
                               slowoutput)
    duration = time.time() - start
    #print("Slow Version took " + str(duration) + " seconds.")

    #verify that slowoutput and output are the same
    outputs_equal = True
    for i in range(VisualBoard.width):
        for j in range(VisualBoard.height):
            outputs_equal = output[j][i] == slowoutput[j][i]
    return outputs_equal

class TestBoard(unittest.TestCase):
    def test_empty(self):
        #initialize board
        row = (ctypes.c_int * VisualBoard.width)(*[VisualBoard.Unknown for i in range(VisualBoard.width)])
        board = ((ctypes.c_int * VisualBoard.width) * VisualBoard.height)(*[row for j in range(VisualBoard.height)])

        self.assertTrue(compare_board_results(board))
    def test_not_empty(self):
        #num_unknown = random.randrange(VisualBoard.width * VisualBoard.height)
        num_trials = 100
        for i in range(num_trials):
            print("trial " + str(i) + "!!!")
            for num_unknown in range(VisualBoard.width * VisualBoard.height):
                #print("(num_unknown: " + str(num_unknown) + ")")
                board = random_valid_board(VisualBoard.width,
                                           VisualBoard.height,
                                           num_unknown)
                self.assertTrue(compare_board_results(board))

if __name__ == "__main__":
    unittest.main()
