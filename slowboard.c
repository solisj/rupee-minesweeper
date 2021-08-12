#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#include "board.h"
#define WIDTH 5
#define HEIGHT 4 //TODO use or dont use?

long factorial(long n) {
    if (n == 0) return 1;
    return n * factorial(n-1);
}

// https://dev.to/rrampage/algorithms-generating-combinations-100daysofcode-4o0a
long combinations(long n, long k) {
    // calculate n choose k

    long product = 1;
    if (k > n - k) k = n - k;
    for (long i = 0; i < k; i++) {
        product *= (n - i);
        product /= (i + 1);
    }
    return product;
}

void getknownbox(size_t width, size_t height, rupee board[height][width], struct coordinate *topleft, struct coordinate *bottomright) {
    /* Given a game board, get the smallest
     * box that contains only known rupees.
     * Additionally, all rupees in this box
     * must either only be adjacent to other
     * rupees in the box, or be unknown rupees.
     *
     * Assumes board has at least one known rupee.
     */
    size_t i; size_t j;

    bool emptyrows[height];
    for (j = 0; j < height; j++)
        emptyrows[j] = true;
    bool emptycolumns[width];
    for (i = 0; i < width; i++)
        emptycolumns[i] = true;

    for (size_t j = 0; j < height; j++) {
        for (size_t i = 0; i < width; i++) {
            if (board[j][i] != Unknown) {
                emptyrows[j] = false;
                emptycolumns[i] = false;
            }
        }
    }

    //get top
    int top = -1;
    for (j = 0; j < height; j++) {
        if (emptyrows[j]) {
            top++;
        } else {
            break;
        }
    }
    top = (top < 0) ? 0 : top;

    //get bottom
    int bottom = height;
    for (j = height - 1; j >= 0; j--) {
        if (emptyrows[j]) {
            bottom--;
        } else {
            break;
        }
    }
    bottom = (bottom >= height) ? height - 1 : bottom;

    //get left
    int left = -1;
    for (i = 0; i < width; i++) {
        if (emptycolumns[i]) {
            left++;
        } else {
            break;
        }
    }
    left = (left < 0) ? 0 : left;

    //get right
    int right = width;
    for (i = width - 1; i >= 0; i--) {
        if (emptycolumns[i]) {
            right--;
        } else {
            break;
        }
    }
    right = (right >= width) ? width - 1 : right;

    //inject values into coordinate structs
    topleft->x = left;
    topleft->y = top;

    bottomright->x = right;
    bottomright->y = bottom;
}

void getknownarea(size_t width, size_t height, rupee board[height][width], bool knownarea[height][width]) {
    /* Given a board, mark off areas as "known"
     * which have at least one known neighbor (or
     * are themselves known).
     *
     * knownarea matrix will be marked as true
     * in these such areas and false elsewhere
     */
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            bool known = false;
            for (int y = j - 1; y <= j + 1; y++) {
                for (int x = i - 1; x <= i + 1; x++) {
                    if (x >= 0 && x < width &&
                        y >= 0 && y < height) {
                        if (board[y][x] != Unknown) known = true;
                    }
                }
            }
            knownarea[j][i] = known;
            printf(known?"X":"O");
        }
        printf("\n");
    }
    printf("\n");
}

bool validateboard(size_t width, size_t height, rupee board[height][width]) {
    /* Checks if the current state board is valid in a real game.
     * Returns true if valid, false otherwise.
     */
    bool valid = true;

    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            valid = validaterupee(width, height, board, i, j);
            if (!valid) break;
        }
        if (!valid) break;
    }

    return valid;
}

int getboardnumbadrupees(size_t width, size_t height, rupee board[height][width]) {
    /* Returns the number of bad rupees
     * currently on the board.
     */
    int numbadrupees = 0;
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            rupee currentrupee = board[j][i];
            if (//currentrupee == Bomb ||
                //currentrupee == Rupoor ||
                currentrupee == Bad) {
                numbadrupees++;
            }
        }
    }
    return numbadrupees;
}

int getboardnumunknownrupees(size_t width, size_t height, rupee board[height][width]) {
    /* Returns the number of unknown rupees
     * currently on the board.
     */
    int numunknownrupees = 0;
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            rupee currentrupee = board[j][i];
            if (currentrupee == Unknown) {
                numunknownrupees++;
            }
        }
    }
    return numunknownrupees;
}

void getboardunknownrupees(size_t width, size_t height, rupee board[height][width], bool unknownrupees[height][width]) {
    /* Get the locations of the unknown rupees
     * on the board; marking true on unknownrupees
     * where there is an unknown rupee, and
     * false otherwise.
     */
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            unknownrupees[j][i] = (board[j][i] == Unknown);
        }
    }
}

bool validaterupee(size_t width, size_t height, rupee board[height][width], int x, int y) {
    /* Given a board and a coordinate,
     * determine if the amount of bad/good rupees
     * surrounding the coord is consistent with its
     * color. e.g. determines if a green rupee is not
     * surrounded by any bad rupees.
     *
     * Returns true if possible, false otherwise.
     */

    int numbad = 0;
    int numunknown = 0;
    int numadjacent = 0;

    rupee currentrupee;
    for (int j = y - 1; j <= y + 1; j++) {
        for (int i = x - 1; i <= x + 1; i++) {
            if ((x == i && y == j) ||
                (i < 0 || i >= width) ||
                (j < 0 || j >= height)) {
            } else {
                //make sure to not include
                //center or points outside board
                currentrupee = board[j][i];
                if (//currentrupee == Bomb ||
                    //currentrupee == Rupoor ||
                    currentrupee == Bad) {
                    numbad++;
                } else if (currentrupee == Unknown) {
                    numunknown++;
                }
                numadjacent++;
            }
        }
    }
    rupee center = board[y][x];

    //int numbad = getnumbadrupees(adjacent, numadjacent);
    //int numunknown = getnumunknownrupees(adjacent, numadjacent);
    //int numgood = numadjacent - numbad - numunknown;

    //default values for "bad" or unknown rupees
    int minbad = 0;
    int maxbad = 8;
    switch(center) {
        case Green:
            minbad = 0;
            maxbad = 0;
            break;
        case Blue:
            minbad = 1;
            maxbad = 2;
            break;
        case Red:
            minbad = 3;
            maxbad = 4;
            break;
        case Silver:
            minbad = 5;
            maxbad = 6;
            break;
        case Gold:
            minbad = 7;
            maxbad = 8;
            break;
        default:
            break;
    }

    bool minvalid = minbad <= numbad + numunknown;
    bool maxvalid = maxbad >= numbad;
    return minvalid && maxvalid;
}

bool validateneighbors(size_t width, size_t height, rupee board[height][width], int x, int y) {
    bool valid = true;

    for (int j = y - 1; j <= y + 1; j++) {
        for (int i = x - 1; i <= x + 1; i++) {
            if ((x == i && y == j) ||
                (i < 0 || i >= width) ||
                (j < 0 || j >= height)) {
            } else {
                //make sure to not include
                //center or points outside board
                valid = validaterupee(width, height, board, i, j);
            }
            if (!valid) {
                break;
            }
        }
        if (!valid) {
            break;
        }
    }

    return valid;
}

int getnumbadrupees(rupee adjacent[MAXNUMADJACENTRUPEES], int numadjacent) {
    /* Given an array of rupees, get the total number of
     * bombs, rupoors, or bad rupees.
     */
    int numbad = 0;
    for (int i = 0; i < numadjacent; i++) {
        rupee currentrupee = adjacent[i];
        if (//currentrupee == Bomb ||
            //currentrupee == Rupoor ||
            currentrupee == Bad) {
            numbad++;
        }
    }
    return numbad;
}

int getnumunknownrupees(rupee adjacent[MAXNUMADJACENTRUPEES], int numadjacent) {
    /* Given an array of rupees, get the total number of
     * unknown rupees.
     */
    int numunknown = 0;
    for (int i = 0; i < numadjacent; i++) {
        if (adjacent[i] == Unknown) {
            numunknown++;
        }
    }
    return numunknown;
}

void getbadnumbers2(size_t width, size_t height, rupee board[height][width], int numbad, int maxnumbad, int minbadlocation, long outputnumbers[height][width]) {
    /* Given a board and the current number of bad
     * rupees as well as the maximum possible number
     * of bad rupees,
     * add to outputnumbers a matrix which contains
     * the total number of times that a given rupee
     * in the board is bad (out of all possible
     * solutions to the puzzle).
     *
     * Requires an argument that gives the
     * minimum place that the first bad rupee can be
     * located; placements sorted first by smallest
     * height (i.e. highest location physically) and
     * then by smallest width (i.e. leftmost
     * location physically).
     *
     * Assumes numbad <= maxnumbad.
     * Assumes board is in a valid state.
     *
     * Guaranteed to exit with board in same condition
     * as it was when passed to this function.
     */

    rupee boardcopy[height][width];
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            boardcopy[j][i] = board[j][i];
        }
    }

    if (maxnumbad == numbad) {
        // we need simply add a matrix with 1
        // where there are currently bad rupees
        // and 0 elsewhere onto outputnumbers
        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {
                rupee currentrupee = board[j][i];
                if (currentrupee == Bad) {
                    outputnumbers[j][i] += 1;
                }
            }
        }
    } else if (minbadlocation == width * height) {
    } else {
        if (minbadlocation == 0) {
            minbadlocation = width * height;

            /* mark certain areas as "known",
             * and similarly compute values
             * separately for squares inside (recursively)
             * and outside (with equation), and combine
             */
            bool knownarea[height][width];
            getknownarea(width, height, board, knownarea);
            int numunknown = 0;
            for (int j = 0; j < height; j++) {
                for (int i = 0; i < width; i++) {
                    if (!knownarea[j][i]) numunknown++;
                }
            }
            if (getboardnumunknownrupees(width, height, board) == width * height) {
                //compute using equation
                long badnumber = combinations(width * height, maxnumbad) * maxnumbad / (width * height);
                for (int j = 0; j < height; j++) {
                    for (int i = 0; i < width; i++) {
                        outputnumbers[j][i] += badnumber;
                    }
                }
            } else if (numunknown != 0) { //make sure the area is not the entire board
                //get numbers for all ways bombs can be split between
                // unknown rupees and the area
                int maxnumbadinsidearea;
                long unknowncombinations;
                int minbadlocationinsidearea = 0;
                long areacombinations;
                long unknownbadnumber;
                bool unknownrupees[height][width];
                // update board for "known area"
                //  (by making the unknown areas "known",
                //   since this does not change the values
                //   obtained when recursing)
                long areaoutput[height][width];
                for (int j = 0; j < height; j++) {
                    for (int i = 0; i < width; i++) {
                        if (!knownarea[j][i]) board[j][i] = Good;
                        printf("%d", board[j][i]);
                    }
                    printf("\n");
                }
                printf("\n");

                for (int maxnumbadoutsidearea = 0;
                     maxnumbadoutsidearea <= maxnumbad - numbad;
                     maxnumbadoutsidearea++) {
                    //for unknowns around area, compute using equation
                    unknowncombinations = (maxnumbadoutsidearea <= numunknown) ?
                                          (combinations(numunknown, maxnumbadoutsidearea)):
                                          (0);
                    unknownbadnumber = unknowncombinations * maxnumbadoutsidearea / numunknown;
                    //for area, compute recursively
                    zeromatrix(width, height, areaoutput);
                    maxnumbadinsidearea = maxnumbad - maxnumbadoutsidearea;
                    getbadnumbers(width,
                                  height,
                                  board,
                                  numbad,
                                  maxnumbadinsidearea,
                                  minbadlocationinsidearea,
                                  areaoutput);
                    areacombinations = summatrixtiles(width, height, areaoutput);
                    if (maxnumbadinsidearea == 0) {
                        //must check if not adding any bad rupees
                        // to the box is valid
                        getboardunknownrupees(width, height, board, unknownrupees);
                        for (int i = 0; i < width; i++) {
                            for (int j = 0; j < height; j++) {
                                if (unknownrupees[j][i])
                                    board[j][i] = Good;
                            }
                        }
                        areacombinations = validateboard(width, height, board) ?
                                          1:
                                          0;
                        for (int i = 0; i < width; i++) {
                            for (int j = 0; j < height; j++) {
                                if (unknownrupees[j][i])
                                    board[j][i] = Unknown;
                            }
                        }
                    } else {
                        areacombinations /= maxnumbadinsidearea;
                    }
                    //combine by multiplication
                    // and inject back into original output matrix
                    for (int j = 0; j < height; j++) {
                        for (int i = 0; i < width; i++) {
                            bool isinsidearea = knownarea[j][i];
                            if (isinsidearea) {
                                outputnumbers[j][i] += (areaoutput[j][i] * unknowncombinations);
                            } else {
                                outputnumbers[j][i] += (unknownbadnumber * areacombinations);
                            }
                        }
                    }
                }
                for (int j = 0; j < height; j++) {
                    for (int i = 0; i < width; i++) {
                        if (!knownarea[j][i]) board[j][i] = Unknown;
                    }
                }
            } else {
                minbadlocation = 0;
            }
            //TODO possibly distribute work to other threads
        }
        // add or dont add bad rupee at first location after
        // min bad location that has an unknown rupee,
        // then increment min bad location and solve recursively
        //TODO clean up this while loop thing
        int minbadx; int minbady;
        //allunknowns[4][0] = Bad;
        //allunknowns[2][2] = Red;
        rupee rupeeatminbadlocation = Good; //something to make the for loop be false the first time

        while(rupeeatminbadlocation != Unknown &&
              minbadlocation < width * height) {
            minbadx = minbadlocation % width;
            minbady = (minbadlocation - (minbadlocation % width)) / width;
            rupeeatminbadlocation = board[minbady][minbadx];
            minbadlocation++;
        }
        minbadlocation--;

        if (rupeeatminbadlocation == Unknown){// && 
            //(width * height - minbadlocation >= maxnumbad - numbad)) { // there are enough spaces left for the amount of bad rupees left
            minbadlocation++;

            board[minbady][minbadx] = Good;
            if (validateboard(width, height, board)){//validateneighbors(width, height, board, minbadx, minbady)) {
                getbadnumbers(width, height, board, numbad, maxnumbad, minbadlocation, outputnumbers);
            } // TODO there might be a better way to ensure board is valid before recursing

            board[minbady][minbadx] = Bad;
            //validate before recursing
            if (validateboard(width, height, board)){//validateneighbors(width, height, board, minbadx, minbady)) {
                numbad++;
                bool valid = true;
                if (numbad == maxnumbad) {
                    int i; int j;
                    bool unknownrupees[height][width];
                    getboardunknownrupees(width, height, board, unknownrupees);
                    for (j = minbady; j < height; j++) {
                        for (i = 0; i < width; i++) {
                            if (unknownrupees[j][i]) {
                                //since all previous unknowns have been set,
                                //and all bad rupees have been allocated
                                board[j][i] = Good; 
                            }
                        }
                    }
                    valid = validateboard(width, height, board);
                    for (j = minbady; j < height; j++) {
                        for (i = 0; i < width; i++) {
                            if (unknownrupees[j][i]) {
                                //revert back to original state
                                board[j][i] = Unknown;
                            }
                        }
                    }
                }
                if (valid) {
                    getbadnumbers(width, height, board, numbad, maxnumbad, minbadlocation, outputnumbers);
                }
            }

            //change back so original state is preserved
            board[minbady][minbadx] = Unknown;
        }
    }
    bool boardsequal = true;
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            boardsequal = board[j][i] == boardcopy[j][i];
        }
    }
    if (boardsequal) {
        //printf("Board state preserved.\n");
    } else {
        printf("Board state not preserved.\n");
    }
}

void getbadnumbers(size_t width,
                   size_t height,
                   rupee board[height][width],
                   int numbad,
                   int maxnumbad,
                   int minbadlocation,
                   long outputnumbers[height][width]) {
    /* Given a board and the current number of bad rupees, as well as the
     * maximum possible number of bad rupees, add to output numbers a matrix
     * which contains the number of times a given rupee can be bad (out of all
     * possible solutions to the puzzle). minbadlocation gives the minimum
     * place that any new bad rupee must be (0 is top left, increases by 1
     * going to the right, and by width when going down).
     *
     * Assumes numbad <= maxnumbad.
     * Assumes board is in a valid state.
     *
     * Guaranteed to exit with board in same condition as it was when passed
     * to this function.
     */
    if (maxnumbad == numbad) {
        /* In this case, there is only one possible solution, so add matrix
         * with 1 where there are bad rupees, and 0 elsewhere.
         */
        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {
                rupee currentrupee = board[j][i];
                if (currentrupee == Bad) outputnumbers[j][i]++;
            }
        }
    } else if (minbadlocation == width * height) { //TODO maybe get rid of this and require minbadlocation be on the board
        //do nothing
    } else {
        /* add or don't add bad rupee at minbadlocation (if possible),
         * then increment minbadlocation and solve recursively
         */
        int minbadx = minbadlocation % width;
        int minbady = (minbadlocation - (minbadlocation % width)) / width;
        rupee rupeeatminbadlocation = board[minbady][minbadx];

        if (rupeeatminbadlocation == Unknown) { //if unknown, replace with known value and recurse twice
            minbadlocation++;

            board[minbady][minbadx] = Good;
            if (validateboard(width, height, board)) {
                getbadnumbers(width, height, board, numbad, maxnumbad, minbadlocation, outputnumbers);
            }

            board[minbady][minbadx] = Bad;
            if (validateboard(width, height, board)) {
                numbad++;
                bool valid = true;
                if (numbad == maxnumbad) {
                    // set other unknown rupees to good since all bad
                    // have been placed
                    bool unknownrupees[height][width];
                    getboardunknownrupees(width, height, board, unknownrupees);
                    for (int j = minbady; j < height; j++) {
                        for (int i = 0; i < width; i++) {
                            if (unknownrupees[j][i])
                                board[j][i] = Good;
                        }
                    }
                    valid = validateboard(width, height, board);
                    for (int j = minbady; j < height; j++) {
                        for (int i = 0; i < width; i++) {
                            if (unknownrupees[j][i])
                                board[j][i] = Unknown;
                        }
                    }
                }
                if (valid) {
                    getbadnumbers(width, height, board, numbad, maxnumbad, minbadlocation, outputnumbers);
                }
            }
            board[minbady][minbadx] = Unknown;
        } else { //don't replace anything, increment minbadlocation and recurse
            minbadlocation++;
            getbadnumbers(width, height, board, numbad, maxnumbad, minbadlocation, outputnumbers);
        }
    }
}

long summatrixtiles(size_t width, size_t height, long matrix[height][width]) {
    /* Return the sum of all the elements
     * in the provided matrix.
     */
    long sum = 0;
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            sum += matrix[j][i];
        }
    }
    return sum;
}

void addmatrices(size_t width, size_t height, long matrix1[height][width], long matrix2[height][width], long summatrix[height][width]) {
    /* Modify summatrix such that
     * it is the result of adding
     * the corresponding elements
     * of matrix1 and matrix2.
     *
     * Does not modify matrix1 or matrix2.
     */
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            summatrix[j][i] = matrix1[j][i] + matrix2[j][i];
        }
    }
}

void copymatrix(size_t width, size_t height, long matrix1[height][width], long matrix2[height][width]) {
    /* Copy the contents of matrix1
     * to matrix2.
     *
     * Does not modify matrix1.
     */
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            matrix2[j][i] = matrix1[j][i];
        }
    }
}

void zeromatrix(size_t width, size_t height, long matrix[height][width]) {
    //fills matrix with zeroes
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            matrix[j][i] = 0;
        }
    }
}

int add_ints(int int1, int int2) {
    return int1 + int2;
}

int main() {
    size_t width = 6;
    size_t height = 5;
    rupee allunknowns[height][width];
    for (size_t i = 0; i < width; i++) {
        for (size_t j = 0; j < height; j++) {
            allunknowns[j][i] = Unknown;
        }
    }
    allunknowns[4][0] = Bad;
    allunknowns[2][2] = Red;
    //allunknowns[2][2] = Blue;
    //allunknowns[5][5] = Red;
    struct coordinate topleft;
    struct coordinate bottomright;
    getknownbox(width, height, allunknowns, &topleft, &bottomright);
    printf("%d", topleft.x);
    printf("%d", topleft.y);
    printf("%d", bottomright.x);
    printf("%d\n", bottomright.y);
    /*for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            if (i <= bottomright.x && i >= topleft.x &&
                j <= bottomright.y && j >= topleft.y) {
                printf("X");
            } else {
                printf("O");
            }
        }
        printf("\n");
    }*/
    int numbad = 0;
    int maxnumbad = 8;
    int minbadlocation = 0;
    long badnumbers[height][width];

    int numtrials = 1;
    clock_t start, diff;
    int totalmsec;
    for (int trial = 0; trial < numtrials; trial++) {
        for (size_t j = 0; j < height; j++) {
            for (size_t i = 0; i < width; i++) {
                badnumbers[j][i] = 0;
            }
        }

        start = clock();
        getbadnumbers(width, height, allunknowns, numbad, maxnumbad, minbadlocation, badnumbers);
        diff = clock() - start;
        totalmsec += diff * 1000 / CLOCKS_PER_SEC;
    }
    totalmsec /= numtrials;
    printf("Average time taken over %d trials: %d seconds %d milliseconds\n", numtrials, totalmsec/1000, totalmsec%1000);

    for (size_t j = 0; j < height; j++) {
        for (size_t i = 0; i < width; i++) {
            printf("%ld ",badnumbers[j][i]);
        }
        printf("\n");
    }
    /*allunknowns[0][0] = Silver;
    if (validateboard(width, height, allunknowns)) {
        printf("all unknowns is valid\n");
    }*/
    return 0;
}
