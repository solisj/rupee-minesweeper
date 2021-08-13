# Rupee Minesweeper (Solver)

This application attempts to aid the user in winning a game of
"Rupee Minesweeper", as seen in The Legend of Zelda: Skyward Sword.

# Usage

Dependencies:

- gcc >= 9.3.0 (to compile C libraries)
- pygame >= 2.0.1 (for graphics)
- python >= 3.8.10

Download this repository, then run

`make all`

to compile the required C libraries. Then, run

`python3 VisualBoard.py`

to use.

# Rupee Minesweeper

The implementation of the game itself can be found in Eldin Volcano in
Skyward Sword.

The player controls Link, who is placed in a grid of holes. Link digs out the
contents of each hole one by one, until the game ends.

Each hole contains either a rupee, or a bomb.

Rupees hint at how many neighboring bombs/rupoors are adjacent:

- Green: None
- Blue: 1 or 2
- Red: 3 or 4
- Silver: 5 or 6
- Gold : 7 or 8

The game ends after either the time runs out, or Link digs up a bomb.

# Version History

This is version 0.0.3.

Removed unused functions/macros.

Version 0.0.2.

Removed unnecessary import and cleaned some comments.

Version 0.0.1.

First version with public source code. Allows user to edit a board, with the
intent of mimicking an actual board in the game. Then, the program will tell the
user the probability that each square is a bomb. The user also has the option to
set the board size (small, medium, or large). Small has a width of 5, a height
of 4, and 4 total bombs; medium has a width of 6, a height of 5, and 8 total
bombs; large has a width of 8, a height of 5, and 16 total bombs.

# Implementation

Version 0.0.x

I initially wrote a version of the program with only Python, using Pygame for
graphics, as I had previously used that library for graphics. However, as I
began the initial tests to see if the program worked on small boards, I soon
realized that Python may not be the right language for the task.

I then rewrote the back-end in C, since C has less overhead, is compiled instead
of interpreted, etc. This massively improved performance, but unfortunately
testing on larger boards (such as boards one might encounter in a real game)
showed that my implementation was still too slow.

Some performance gains were found by e.g. switching the order of loops
(i.e. iterating by row, then by column instead of vice-versa) to take advantage
of data caching. However, the largest board size in a real game was still too
slow, so I started taking a closer look at the algorithmic approach.

In the end, I expanded the amount of base cases significantly; this was enough
to get a maximum time of around 10 seconds (on my machine) for the largest board
size, and I felt that this was fast enough, since it would take just as long, if
not longer, to choose the next move by hand. The cases where my algorithm were
the slowest were those which had more "unknown" squares, so I had the case where
the board is unknown be computed using an equation, which was naturally faster
than checking all combinations by brute force. Then, I found a  way to "split"
the problem into parts that can be solved either by brute force or by equation,
and this ended up being fast enough.

The final program I ended up with might be too slow for arbitrarily large
boards, but the goal was to be able to use this program with the implementation
of the game in Skyward Sword. I learned how to interface Python with C, as well
as some performance engineering techniques.

# Further Work

A possible direction to take this project is to recommend squares that give
the highest chance of winning the game as opposed to the square with minimum
chance of being a bomb. One can also look into making an "efficient" version
of the program for arbitrarily large boards.
