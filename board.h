#include <stdbool.h>

typedef enum {Good    = 0, // any of the various colorful rupee w/ worth > 0
              Unknown = 1,
              Green   = 2, // worth 1; no adjacent rupees
              Blue    = 3, // worth 5; 1 or 2 adjacent rupees
              Red     = 4, // worth 20; 3 or 4 adjacent rupees
              Silver  = 5, // worth 100; 5 or 6 adjacent rupees
              Gold    = 6, // worth 300; 7 or 8 adjacent rupees
              Bad     = 9  //either Bomb or Rupoor
             } rupee;

struct coordinate {
    int x;
    int y;
};

long combinations(long n, long k);

void getknownarea(size_t width, size_t height, rupee board[height][width], bool knownarea[height][width]);

bool validateboard(size_t width, size_t height, rupee board[height][width]);

int getboardnumbadrupees(size_t width, size_t height, rupee board[height][width]);

int getboardnumunknownrupees(size_t width, size_t height, rupee board[height][width]);

void getboardunknownrupees(size_t width, size_t height, rupee board[height][width], bool unknownrupees[height][width]);

bool validaterupee(size_t width, size_t height, rupee board[height][width], int x, int y);

bool validateneighbors(size_t width, size_t height, rupee board[height][width], int x, int y);

void getbadnumbers(size_t width, size_t height, rupee board[height][width], int numbad, int maxnumbad, int minbadlocation, long outputnumbers[height][width]);

long summatrixtiles(size_t width, size_t height, long matrix[height][width]);

void addmatrices(size_t width, size_t height, long board1[height][width], long board2[height][width], long summatrix[height][width]);

void copymatrix(size_t width, size_t height, long matrix1[height][width], long matrix2[height][width]);

void zeromatrix(size_t width, size_t height, long matrix[height][width]);
