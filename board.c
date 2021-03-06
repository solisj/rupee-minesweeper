#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#include "board.h"

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
        }
    }
}

bool validateboard(size_t width, size_t height, rupee board[height][width]) {
    /* Checks if board is valid in a real game.
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

void getboardunknownrupees(size_t width,
                           size_t height,
                           rupee board[height][width],
                           bool unknownrupees[height][width]) {
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

bool validaterupee(size_t width,
                   size_t height,
                   rupee board[height][width],
                   int x,
                   int y) {
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
                if (currentrupee == Bad) {
                    numbad++;
                } else if (currentrupee == Unknown) {
                    numunknown++;
                }
                numadjacent++;
            }
        }
    }
    rupee center = board[y][x];

    int minbad;
    int maxbad;
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
            //for "bad" or unknown rupees
            minbad = 0;
            maxbad = 8;
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
    } else if (minbadlocation == width * height) {
    } else {
        if (minbadlocation == 0) {
            minbadlocation = width * height;

            /* mark certain areas as "known", and compute values separately
             * for squares inside (recursively) and outside (with equation)
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
            } else if (numunknown != 0) { //make sure area is not the entire board
                //get numbers for all ways bad rupees can be split between
                // inside and outside
                int maxnumbadinsidearea;
                long unknowncombinations;   
                int minbadlocationinsidearea = 0;
                long areacombinations;
                long unknownbadnumber;
                bool unknownrupees[height][width];
                /* update board for "unknown area" (by making the unknown areas
                 * "known", since this does not change the bad numbers)
                 */
                long areaoutput[height][width];
                for (int j = 0; j < height; j++) {
                    for (int i = 0; i < width; i++) {
                        if (!knownarea[j][i]) board[j][i] = Good;
                    }
                }

                for (int maxnumbadoutsidearea = 0;
                     maxnumbadoutsidearea <= maxnumbad - numbad;
                     maxnumbadoutsidearea++) {
                    //for unknowns outside area, compute using equation
                    unknowncombinations = (maxnumbadoutsidearea <= numunknown) ?
                                          combinations(numunknown, maxnumbadoutsidearea) :
                                          0;
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
                    if (maxnumbadinsidearea == numbad) {
                        //must check if not adding any bad rupees is valid
                        getboardunknownrupees(width, height, board, unknownrupees);
                        for (int i = 0; i < width; i++) {
                            for (int j = 0; j < height; j++) {
                                if (unknownrupees[j][i]) board[j][i] = Good;
                            }
                        }
                        if (validateboard(width, height, board)) {
                            areacombinations = 1;
                        } else {
                            areacombinations = 0;
                            unknowncombinations = 0; //TODO making areaoutput all zeroes is more "accurate", but this should give same result?
                        }
                        for (int i = 0; i < width; i++) {
                            for (int j = 0; j < height; j++) {
                                if (unknownrupees[j][i]) board[j][i] = Unknown;
                            }
                        }
                    } else {
                        areacombinations /= maxnumbadinsidearea;
                    }
                    //combine by multiplication and inject back into original
                    // output matrix
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
        }
        /* add or don't add bad rupee at minbadlocation (if possible),
         * then increment minbadlocation and solve recursively
         */
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

        if (rupeeatminbadlocation == Unknown) { //if unknown, replace with known value and recurse twice
            minbadlocation++;

            board[minbady][minbadx] = Good;
            if (validateneighbors(width, height, board, minbadx, minbady)) {
                getbadnumbers(width, height, board, numbad, maxnumbad, minbadlocation, outputnumbers);
            }

            board[minbady][minbadx] = Bad;
            if (validateneighbors(width, height, board, minbadx, minbady)) {
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
