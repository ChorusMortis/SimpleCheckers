#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>  // for touppper()
 
// Constants
#define ROW           7
#define COL           5
#define ROW_INIT      (ROW + 1)
#define COL_INIT      (COL + 1)
#define MAX_POS_AMT   5
 
// Marks
#define ALPHA_MARK   'X'
#define BETA_MARK    'O'
#define EMPTY_MARK   ' '
 
// Game states
#define GAME_ONGOING   0
#define ALPHA_WIN      1
#define BETA_WIN       2
 
// Position in the game board
typedef struct pos {
    int row;
    int col;
} Pos;
 
// Variable that contains (almost) all the game data
typedef struct gameData {
    char gameBoard[ROW_INIT][COL_INIT]; // the actual game board
    Pos alphaStartPos[MAX_POS_AMT];     // Alpha's starting pieces
    Pos betaStartPos[MAX_POS_AMT];      // Beta's starting pieces
    Pos alphaPos[MAX_POS_AMT];          // Alpha's current pieces
    Pos betaPos[MAX_POS_AMT];           // Beta's current pieces
    int alphaPosAmt;                    // current amount of Alpha's pieces in play
    int betaPosAmt;                     // current amount of Beta's pieces in play
    int curPlayer;                      // the current player
} GameData;
 
/**
 * @brief
 *    Converts a column from its integer representation to its character
 *    counterpart.
 *
 * @param intCol   Integer column to be converted.
 *
 * @pre   intCol is a valid integer column in the game.
 *
 * @return The character equivalent of the given integer column.
 */
char intColToCharCol(int intCol) {
    char charCol = 0;
 
    switch (intCol) {
        case 1: charCol = 'A'; break;
        case 2: charCol = 'B'; break;
        case 3: charCol = 'C'; break;
        case 4: charCol = 'D'; break;
        case 5: charCol = 'E'; break;
    }
 
    return charCol;
}
 
/**
 * @brief
 *    Converts a column from its character representation to its integer
 *    counterpart.
 *
 * @param charCol   Character column to be converted.
 *
 * @pre   charCol is a valid character column in the game.
 *
 * @return The integer equivalent of the given character column.
 */
int charColToIntCol(char charCol) {
    int intCol = 0;
 
    switch (charCol) {
        case 'A': intCol = 1; break;
        case 'B': intCol = 2; break;
        case 'C': intCol = 3; break;
        case 'D': intCol = 4; break;
        case 'E': intCol = 5; break;
    }
 
    return intCol;
}
 
/**
 * @brief Displays the current positions of Alpha and Beta's pieces.
 *
 * @note Use only when debugging.
 *
 * @param pGame   Contains game data.
 */
void debug_showABPos(GameData *pGame) {
    char charCol = 0;
    int intCol = 0;
    int intRow = 0;
 
    printf("[DEBUG] Alpha's positions (%d):\n\n", pGame->alphaPosAmt);
    for (int i = 0; i < pGame->alphaPosAmt; i++) {
        charCol = intColToCharCol(pGame->alphaPos[i].col);
        intCol = pGame->alphaPos[i].col;
        intRow = pGame->alphaPos[i].row;
        printf("%d. %c%d (%d, %d)\n", i+1, charCol, intRow, intRow, intCol);
    }
   
    printf("\n");
 
    printf("[DEBUG] Beta's positions (%d):\n\n", pGame->betaPosAmt);
    for (int i = 0; i < pGame->betaPosAmt; i++) {
        charCol = intColToCharCol(pGame->betaPos[i].col);
        intCol = pGame->betaPos[i].col;
        intRow = pGame->betaPos[i].row;
        printf("%d. %c%d (%d, %d)\n", i+1, charCol, intRow, intRow, intCol);
    }
}
 
/**
 * @brief Searches an array of positions for the given position.
 *
 * @param positions   Array of positions to be searched.
 * @param posAmt      The amount of elements in positions.
 * @param givenPos    The position to be searched for.
 *
 * @pre   positions should not have duplicate positions.
 *
 * @return
 *    The index of givenPos   if givenPos was found.
 *    -1                      if givenPos was not found.
 */
int searchPos(Pos positions[], int posAmt, Pos givenPos) {
    for (int i = 0; i < posAmt; i++) {
        if (positions[i].row == givenPos.row &&
            positions[i].col == givenPos.col) {
                return i;
            }
    }
 
    return -1;
}
 
/**
 * @brief Updates the game board visually, but does not modify anything else.
 *
 * @param pGame        Contains game data.
 * @param givenPos     The position to be marked.
 * @param playerMark   The mark to be visually placed in givenPos.
 *
 * @pre   givenPos is a valid position in the game board.
 */
void markBoard(GameData *pGame, Pos givenPos, char playerMark) {
    pGame->gameBoard[givenPos.row][givenPos.col] = playerMark;
}
 
/**
 * @brief Adds the given position to the given position array.
 * @details
 *    The board is first updated visually by marking the board at the given
 *    position. The position is then added at the end of the given positions
 *    array. The amount of positions is then incremented.
 *
 * @param pGame        Contains game data.
 * @param positions    Array of positions where the given position is to be
 *                     added to.
 * @param posAmt       The amount of positions in the positions array.
 * @param givenPos     The given position to be added and visually marked.
 * @param playerMark   The mark to be visually placed in the given position.
 *
 * @pre   positions has < MAX_POS_AMT elements.
 * @pre   posAmt is < MAX_POS_AMT.
 * @pre   givenPos is a valid position in the game board.
 */
void addPos(GameData *pGame, Pos positions[], int *posAmt, Pos givenPos, char playerMark) {
    markBoard(pGame, givenPos, playerMark);
    positions[*posAmt] = givenPos;
    *posAmt += 1;
}
 
/**
 * @brief
 *    Deletes the given position in the given position array, then shifts
 *    the remaining elements forward.
 * @details
 *    The board at the given position is first visually marked empty.
 *    The given position is then deleted in the given positions array.
 *    Succeeding elements are shifted forwards by one index, then the last
 *    element is set to 0. The amount of positions is then decremented.
 *
 * @param pGame       Contains game data.
 * @param positions   Array of positions where the given position is to be
 *                    deleted from.
 * @param posAmt      The amount of positions in the positions array.
 * @param givenPos    The position to be deleted.
 *
 * @pre   positions should have n > 0 elements.
 * @pre   givenPos is a valid position in the game board.
 */
void delPos(GameData *pGame, Pos positions[], int *posAmt, Pos givenPos) {
    markBoard(pGame, givenPos, EMPTY_MARK);
 
    // find the given position in the array, then start shifting elements
    int index = searchPos(positions, *posAmt, givenPos);
    for (int i = index; i <= *posAmt - 2; i++) {
        positions[i] = positions[i+1];
    }
 
    // set the last value to 0 and reduce the amount of positions in the array
    *posAmt -= 1;
    positions[*posAmt].row = 0;
    positions[*posAmt].col = 0;
}
 
/**
 * @brief Prints the character column "names" atop the game board.
 *
 * @note
 *    When modifying the game board (i.e. in appearance, in size, etc.),
 *    this function will most likely need to be tweaked.
 */
void printColHeader(void) {
    printf("  ");
    char c = 'A';
    for (int i = 0; i < COL; i++) {
        printf("     %c", c);
        c++;
    }
    printf("\n\n");
}
 
/**
 * @brief Prints the first two lines of each row of the game board.
 * @details
 *    For the first line, empty spaces divided by vertical bars are printed.
 *    For the second line, the game board's elements divided by vertical bars
 *    are printed.
 *
 * @note
 *    When modifying the game board (i.e. in appearance, in size, etc.),
 *    this function will most likely need to be tweaked.
 *
 * @param rowElems   The array of elements in a row.
 * @param cols       The amount of columns there are for each row (how many
 *                   elements per row).
 * @param rowNum     What row number it is. (used for row labels before the
 *                   board)
 */
void printRow(char rowElems[], int cols, int rowNum) {
    for (int i = 0; i < 2; i++) {
 
        // prints the row numbers just beside the board
        if (i == 1) {
            printf(" %d   ", rowNum);
        } else {
            printf("     ");
        }
 
        // prints the actual elements of each row along with a column divider
        for (int j = 0; j < cols; j++) {
            printf("  %c  ", i == 1 ? rowElems[j] : EMPTY_MARK);
            if (j <= cols - 2) {
                printf("|");
            }
        }
        printf("\n");
    }
}
 
/**
 * @brief
 *    Prints the third line of each row (which consists of row and column
 *    dividers).
 * @details
 *    Prints the row dividers first (underscores usually work best), then
 *    prints the column dividers (vertical bars usually work best).
 *
 * @note
 *    When modifying the game board (i.e. in appearance, in size, etc.),
 *    this function will most likely need to be tweaked.
 *
 * @param rowDivider   The string that serves as the row divider.
 * @param colDivider   The string that serves as the column divider.
 * @param cols         The amount of columns there are for each row (how many
 *                     elements per row).
 */
void printDivider(char rowDivider[], char colDivider[], int cols) {
    for (int i = 0; i < cols; i++) {
        printf("%s", rowDivider);
        if (i <= cols - 2) {
            printf("%s", colDivider);
        }
    }
    printf("\n");
}
 
/**
 * @brief Prints the game board.
 * @details
 *    Prints the column names first atop the game board. When printing each
 *    row, the first two lines are printed, which consist of the game board's
 *    elements along with column dividers. The third line consists of both row
 *    and column dividers. In the very last row, however, there is no row
 *    divider as there are no more rows to print.
 *
 * @note
 *    When modifying the game board (i.e. in appearance, in size, etc.),
 *    this function will most likely need to be tweaked.
 *
 * @param board   A 2D character array that represents the game board.
 */
void displayBoard(char board[][COL_INIT]) {
 
    printColHeader();
 
    // prints the actual board itself along with row numbers
    for (int i = 1; i <= ROW; i++) {
        // start at board[1][1] since there is an extra column and row for programming convenience
        printRow(board[i] + 1, COL, i); // prints the first two lines of each row
 
        // prints the third line of each row (row and column dividers)
        printf("     ");
        if (i < ROW) {
            printDivider("_____", "|", COL);
        } else {
            printDivider("     ", "|", COL);
        }
    }
}
 
/**
 * @brief Checks if the given position is an "S square".
 * @details
 *    S squares are positions in the game board such that the row and column
 *    value divided by 2 yield the same remainder. These positions are where
 *    the row and column values are either both even or both odd. These
 *    S squares are the only positions in the game board where captures can
 *    occur.
 *
 * @note Change the rules of what an S square is in this function.
 *
 * @param givenPos   The position to be checked if it is an "S square".
 *
 * @pre   givenPos is a valid position in the board.
 *
 * @return
 *    1   if the given position is an S square.
 *    0   if the given position is not an S square.
 */
int isSquareS(Pos givenPos) {
    if (givenPos.row % 2 == givenPos.col % 2) {
        return 1;
    } else {
        return 0;
    }
}
 
/**
 * @brief Checks if the given position is one of Alpha's starting positions.
 * @details
 *    Alpha's starting positions are S squares which row value is >= 6. These
 *    positions are located at the bottom side of the board.
 *
 * @note Change rules for Alpha's starting positions here.
 *
 * @param givenPos   The position to be checked if it is one of Alpha's
 *                   starting positions.
 *
 * @pre   givenPos is a valid position in the board.
 *
 * @return
 *    1   if the given position is one of Alpha's starting positions.
 *    0   if the given position is not one of Alpha's starting positions.
 */
int isPosAlphaStart(Pos givenPos) {
    int posIsAlphaStart = 0;
 
    if (isSquareS(givenPos) == 1 && givenPos.row >= 6) {
        posIsAlphaStart = 1;
    }
 
    return posIsAlphaStart;
}
 
/**
 * @brief Checks if the given position is one of Beta's starting positions.
 * @details
 *    Beta's starting positions are S squares which row value is <= 2. These
 *    positions are located at the top side of the board.
 *
 * @note Change rules for Beta's starting positions here.
 *
 * @param givenPos   The position to be checked if it is one of Beta's
 *                   starting positions.
 *
 * @pre   givenPos is a valid position in the board.
 *
 * @return
 *    1   if the given position is one of Beta's starting positions.
 *    0   if the given position is not one of Beta's starting positions.
 */
int isPosBetaStart(Pos givenPos) {
    int posIsBetaStart = 0;
 
    if (isSquareS(givenPos) == 1 && givenPos.row <= 2) {
        posIsBetaStart = 1;
    }
 
    return posIsBetaStart;
}
 
/**
 * @brief Displays all S squares in the game board.
 * @details
 *    S squares are positions in the game board such that the row and column
 *    value divided by 2 yield the same remainder. These positions are where
 *    the row and column values are either both even or both odd. These
 *    S squares are the only positions in the game board where captures can
 *    occur.
 *
 *    A temporary game board is first created. Its positions is then marked
 *    with 'S' if the position is an S square, and it is marked blank if it
 *    is not. The temporary game board is then displayed using displayBoard().
 */
void displaySSquares(void) {
    char tempBoard[ROW_INIT][COL_INIT] = {0};
    for (int i = 1; i <= ROW; i++) {
        for (int j = 1; j <= COL; j++) {
            Pos square = {i, j};
            if (isSquareS(square) == 1) {
                tempBoard[i][j] = 'S';
            } else {
                tempBoard[i][j] = EMPTY_MARK;
            }
        }
    }
 
    displayBoard(tempBoard);
}
 
/**
 * @brief Displays the gameplay instructions.
 */
void displayInstructions(void) {
    char instructions[] =
        "Instructions:\n\n"
       
        "- There are two players: Alpha and Beta.\n"
        "- Alpha starts at the bottom of the board and moves first.\n"
        "- Beta starts at the top of the board and moves second.\n"
        "- Each player is given 5 pieces which can be moved a square forwards, either directly forward\n"
        "  or diagonally.\n"
        "- If a square is occupied by an opponent's piece, a player may capture it only if it is\n"
        "  in an \"S square\", where the row and column are both even or both odd (shown below).\n"
        "- If a piece is captured, it is removed from play and the capturing piece moves to that square.\n"
        "- The game ends if a player's pieces are all captured or if all the player's remaining pieces manage\n"
        "  to get to the starting positions of the opposing player.\n";
 
    printf("%s\n", instructions);
    displaySSquares();
    printf("\n\n");
}
 
/**
 * @brief Initializes the game board by setting all positions to blank.
 *
 * @param pGame   Contains game data.
 */
void initBoard(GameData *pGame) {
    for (int i = 1; i <= ROW; i++) {
        for (int j = 1; j <= COL; j++) {
            pGame->gameBoard[i][j] = EMPTY_MARK;
        }
    }
}
 
/**
 * @brief Sets Alpha's and Beta's starting positions in the game board.
 * @details
 *    The function iterates through every single possible position in the game
 *    board, and if the position is either Alpha or Beta's starting position,
 *    it is stored in Alpha or Beta's starting and current positions array.
 *    The board is also visually updated accordingly.
 *
 * @param pGame   Contains game data.
 */
void initPos(GameData *pGame) {
    pGame->alphaPosAmt = 0;
    pGame->betaPosAmt = 0;
 
    for (int i = 1; i <= ROW; i++) {
        for (int j = 1; j <= COL; j++) {
            Pos square = {i, j};
            if (isPosBetaStart(square) == 1) {
                if (pGame->betaPosAmt < MAX_POS_AMT) {
                    pGame->betaStartPos[pGame->betaPosAmt] = square;
                    addPos(pGame, pGame->betaPos, &pGame->betaPosAmt, square, BETA_MARK);
                }
            } else if (isPosAlphaStart(square) == 1) {
                if (pGame->alphaPosAmt < MAX_POS_AMT) {
                    pGame->alphaStartPos[pGame->alphaPosAmt] = square;
                    addPos(pGame, pGame->alphaPos, &pGame->alphaPosAmt, square, ALPHA_MARK);
                }
            }
        }
    }
}
 
/**
 * @brief Initializes game data.
 * @details
 *    Sets the current player to 1 (since Alpha is player 1, who goes first).
 *    The game board is then initialized to having empty squares, then Alpha
 *    and Beta's starting positions are initialized and copied.
 *
 * @param pGame   Contains game data.
 */
void initGameData(GameData *pGame) {
    pGame->curPlayer = 1;
    initBoard(pGame);
    initPos(pGame);
}
 
/**
 * @brief
 *    Returns the integer representation of which player goes next in a
 *    turn.
 *
 * @param curPlayer   The integer representation of the current player.
 *
 * @pre   curPlayer is a valid integer representation of the current player.
 *
 * @return
 *    1   if curPlayer is 2.
 *    2   if curPlayer is 1.
 */
int switchPlayer(int curPlayer) {
    int nextPlayer = 0;
 
    switch (curPlayer) {
        case 1: nextPlayer = 2; break;
        case 2: nextPlayer = 1; break;
    }
 
    return nextPlayer;
}
 
/**
 * @brief Gets the mark on the given position.
 *
 * @param pGame      Contains game data.
 * @param givenPos   The position which mark is to be obtained.
 *
 * @pre   givenPos is a valid position in the game board.
 *
 * @return The mark on the given position.
 */
char getSquareMark(GameData *pGame, Pos givenPos) {
    return pGame->gameBoard[givenPos.row][givenPos.col];
}
 
/**
 * @brief Gets the mark that is used by or assigned to a player.
 *
 * @param curPlayer   The integer representation of the current player.
 *
 * @pre   curPlayer is a valid integer representation of the current player.
 *
 * @return
 *    ALPHA_MARK   if curPlayer is 1.
 *    BETA_MARK    if curPlayer is 2.
 */
char getPlayerMark(int curPlayer) {
    char playerMark = 0;
 
    switch (curPlayer) {
        case 1: playerMark = ALPHA_MARK; break;
        case 2: playerMark = BETA_MARK; break;
    }
 
    return playerMark;
}
 
/**
 * @brief Gets and validates user input.
 * @details
 *    The player is prompted for their input in a specific format. Afterwards,
 *    lowercase characters are converted to uppercase characters so lowercase
 *    characters are handled and accepted as well. The standard input stream
 *    is then cleared using fflush() to get rid of excess characters.
 *    
 *    The input is then validated, and if it is not valid according to the
 *    given rules, then the player is asked for their input again. Once the
 *    player enters a valid input, it is stored using pointers to external
 *    variables.
 *
 * @note
 *    The function asks the player for their input in a specific format
 *    (character column + integer row). The character column is then converted
 *    to its integer representation since integers are used for columns in
 *    the game board 2D array.
 *
 * @param row   Pointer to a variable that stores the player's input for the
 *              row value of a position.
 * @param col   Pointer to a variable that stores the player's input for the
 *              column value of a position.
 */
void getUserInput(int *row, int *col) {
    char tempCol = 0;
    int tempRow = 0;
    int inputIsValid = 0;
 
    while (!inputIsValid) {
        printf("Enter your input in column/row format. (ex. A1, C5, E7)\n");
        printf("Input > ");
        scanf("%c%d", &tempCol, &tempRow);
        tempCol = toupper(tempCol);
        fflush(stdin); // clear input stream
 
        // change rules for valid input here
        if (tempCol >= 'A' && tempCol <= 'E' && tempRow >= 1 && tempRow <= ROW) {
            inputIsValid = 1;
        }
    }
 
    tempCol = charColToIntCol(tempCol);
    *row = tempRow;
    *col = tempCol;
}
 
/**
 * @brief Checks if the given position is available.
 *
 * @param pGame      Contains game data.
 * @param givenPos   The position to be checked if it is available.
 *
 * @pre   givenPos is a valid position in the game board.
 *
 * @return
 *    1   if the given position is unoccupied (an empty position).
 *    2   if the given position is occupied by an opponent's piece.
 *    0   if the given position is occupied by a current player's piece.
 */
int isPosAvailable(GameData *pGame, Pos givenPos) {
    char curPlayerMark = getPlayerMark(pGame->curPlayer);
 
    if (pGame->gameBoard[givenPos.row][givenPos.col] == EMPTY_MARK) {
        return 1; // empty space
    } else if (pGame->gameBoard[givenPos.row][givenPos.col] != curPlayerMark) {
        return 2; // space is occupied by an opponent's piece
    } else {
        return 0; // space is occupied by a current player's piece
    }
}
 
/**
 * @brief Checks if moving srcPos to destPos is legal.
 * @details
 *    Moving srcPos to destPos is legal if the movement is one
 *    position forward, either directly forward or diagonally.
 *    
 *    The checking is done by first checking if destPos.col is the same as
 *    srcPos.col or off to the right or left by 1 column. Next, it is checked
 *    if destPos.row is a position "forward" from srcPos.row, which depends on
 *    the curPlayer. If the curPlayer is Alpha, then "forward" is
 *    srcPos.row - 1, but if the curPlayer is Beta, then "forward" is
 *    srcPos.row + 1.
 *
 * @param curPlayer   The integer representation of which player's turn it is.
 * @param srcPos      The position to be moved.
 * @param destPos     The position where srcPos is to be moved to.
 *
 * @pre   curPlayer is a valid integer representation of the current player.
 * @pre   srcPos and destPos are valid positions in the game board.
 * @pre   destPos is either free or occupied by an opponent's piece.
 *
 * @return
 *    1   if moving srcPos to destPos is legal.
 *    0   if moving srcPos to destPos is not legal.
 */
int isMoveLegal(int curPlayer, Pos srcPos, Pos destPos) {
    int moveIsLegal = 0;
 
    if ((destPos.col == srcPos.col ||
         destPos.col == srcPos.col + 1 ||
         destPos.col == srcPos.col - 1) &&
        ((curPlayer == 1 && destPos.row == srcPos.row - 1) ||
         (curPlayer == 2 && destPos.row == srcPos.row + 1))) {
             moveIsLegal = 1;
         }
   
    return moveIsLegal;
}
 
/**
 * @brief Moves a piece from srcPos to destPos.
 *
 * @note This only moves a piece. This does not validate anything.
 *
 * @param pGame     Contains game data.
 * @param srcPos    The position to be moved.
 * @param destPos   The position where srcPos is to be moved to.
 *
 * @pre   srcPos and destPos are valid positions.
 * @pre   srcPos and destPos are not empty.
 * @pre   Alpha or Beta, whoever's piece is going to be moved,
 *        has > 0 and <= MAX_POS_AMT positions.
 */
void movePiece(GameData *pGame, Pos srcPos, Pos destPos) {
    char mark = getSquareMark(pGame, srcPos);
 
    if (mark == ALPHA_MARK) {
        delPos(pGame, pGame->alphaPos, &pGame->alphaPosAmt, srcPos);
        addPos(pGame, pGame->alphaPos, &pGame->alphaPosAmt, destPos, mark);
    } else {
        delPos(pGame, pGame->betaPos, &pGame->betaPosAmt, srcPos);
        addPos(pGame, pGame->betaPos, &pGame->betaPosAmt, destPos, mark);
    }
}
 
/**
 * @brief destPos is captured by srcPos.
 *
 * @note This only does the capturing. This does not validate anything.
 *
 * @param pGame     Contains game data.
 * @param srcPos    The capturing position.
 * @param destPos   The captured position.
 *
 * @pre   srcPos and destPos are valid positions.
 * @pre   srcPos and destPos are not empty.
 * @pre   Alpha or Beta, whoever's piece is the captured, has > 0 positions.
 */
void capturePiece(GameData *pGame, Pos srcPos, Pos destPos) {
    // delete the captured piece first
    char mark = getSquareMark(pGame, srcPos);
    if (mark == ALPHA_MARK) {
        delPos(pGame, pGame->betaPos, &pGame->betaPosAmt, destPos);
    } else {
        delPos(pGame, pGame->alphaPos, &pGame->alphaPosAmt, destPos);
    }
 
    // after deleting the captured piece, move capturing piece to the captured
    // piece's position
    movePiece(pGame, srcPos, destPos);
}
 
/**
 * @brief Checks if Alpha has won or lost yet, or neither.
 * @details
 *    Checks first if Alpha has any remaining pieces left. If there are none,
 *    then they have lost.
 *    
 *    If Alpha still has remaining pieces left, assume they have won. Then,
 *    check if all their remaining pieces are on Beta's starting positions.
 *    If that is the case, then Alpha has won. Otherwise, they have neither
 *    won nor lost.
 *
 * @param pGame   Contains game data.
 *
 * @return
 *    -1   if Alpha has lost.
 *    1    if Alpha has won.
 *    0    if Alpha has neither won nor lost.
 */
int hasAlphaWon(GameData *pGame) {
    // if Alpha has no more pieces, then Alpha has lost
    if (pGame->alphaPosAmt == 0) {
        return -1;
    }
 
    // assume Alpha has won, and then check if Alpha really has won
    int alphaHasWon = 1;
    for (int i = 0; i < pGame->alphaPosAmt && alphaHasWon == 1; i++) {
        int inStartPos = 0;
        int row1 = pGame->alphaPos[i].row;
        int col1 = pGame->alphaPos[i].col;
 
        // check if all of Alpha's pieces are in Beta's starting positions
        for (int j = 0; j < MAX_POS_AMT && inStartPos == 0; j++) {
            int row2 = pGame->betaStartPos[j].row;
            int col2 = pGame->betaStartPos[j].col;
 
            if (row1 == row2 && col1 == col2) {
                inStartPos = 1;
            }
        }
 
        alphaHasWon = inStartPos;
    }
 
    return alphaHasWon;
}
 
/**
 * @brief Checks if Beta has won or lost yet, or neither.
 * @details
 *    Checks first if Beta has any remaining pieces left. If there are none,
 *    then they have lost.
 *    
 *    If Beta still has remaining pieces left, assume they have won. Then,
 *    check if all their remaining pieces are on Alpha's starting positions.
 *    If that is the case, then Beta has won. Otherwise, they have neither
 *    won nor lost.
 *
 * @param pGame   Contains game data.
 *
 * @return
 *    -1   if Beta has lost.
 *    1    if Beta has won.
 *    0    if Beta has neither won nor lost.
 */
int hasBetaWon(GameData *pGame) {
    // if Beta has no more pieces, then Beta has lost
    if (pGame->betaPosAmt == 0) {
        return -1;
    }
 
    // assume Beta has won, and then check if Beta really has won
    int betaHasWon = 1;
    for (int i = 0; i < pGame->betaPosAmt && betaHasWon == 1; i++) {
        int inStartPos = 0;
        int row1 = pGame->betaPos[i].row;
        int col1 = pGame->betaPos[i].col;
 
        // check if all of Beta's pieces are in Alpha's starting positions
        for (int j = 0; j < MAX_POS_AMT && inStartPos == 0; j++) {
            int row2 = pGame->alphaStartPos[j].row;
            int col2 = pGame->alphaStartPos[j].col;
 
            if (row1 == row2 && col1 == col2) {
                inStartPos = 1;
            }
        }
 
        betaHasWon = inStartPos;
    }
 
    return betaHasWon;
}
 
/**
 * @brief Checks if the game has ended or is ongoing.
 * @details
 *    There are two win conditions:
 *       (1) Either Alpha or Beta has no more pieces remaining (all of their
 *           pieces are captured).
 *       (2) Either Alpha or Beta has all their remaining pieces placed on
 *           their opponent's starting positions.
 *
 *    First, it checks if Alpha has won or lost. If so, it immediately returns
 *    a value. If not, it checks if Beta has won or lost. If so, it returns
 *    a value. Otherwise, it means that the game is ongoing and returns the
 *    appropriate value.
 *
 * @note This cannot detect if the game has drawed or softlocked.
 *
 * @param pGame   Contains game data.
 *
 * @return
 *    ALPHA_WIN      if Alpha has won.
 *    BETA_WIN       if Beta has won.
 *    GAME_ONGOING   if the game is ongoing (there is no winner yet).
 */
int checkGameStatus(GameData *pGame) {
    int gameStatus = hasAlphaWon(pGame);
    switch (gameStatus) {
        case  1: return ALPHA_WIN; break; // Alpha has won
        case -1: return BETA_WIN; break; // Alpha has lost (Beta has won)
    }
 
    gameStatus = hasBetaWon(pGame);
    switch (gameStatus) {
        case  1: return BETA_WIN; break; // Beta has won
        case -1: return ALPHA_WIN; break; // Beta has lost (Alpha has won)
    }
 
    // game is ongoing
    return GAME_ONGOING;
}
 
/**
 * @brief Prompts the player to select a piece that they will move.
 *
 * @param pGame   Contains game data.
 * @param row     Pointer to where the row value of the chosen position will
 *                be stored.
 * @param col     Pointer to where the column value of the chosen position
 *                will be stored.
 */
void playerSelectPiece(GameData *pGame, int *row, int *col) {
    int inputIsValid = 0;
    int posIsAvailable = 0;
 
    while (!inputIsValid) {
        printf("%s: Select a piece to move.\n",
                pGame->curPlayer == 1 ? "Alpha" : "Beta");
        Pos temp = {0};
        getUserInput(&temp.row, &temp.col);
        posIsAvailable = isPosAvailable(pGame, temp);
 
        // player must select a square that contains one of their pieces
        if (posIsAvailable == 0) {
            *row = temp.row;
            *col = temp.col;
            inputIsValid = 1;
        } else {
            printf("ERROR: You must select a piece of yours!\n");
        }
        printf("\n");
    }
}
 
/**
 * @brief Asks the player where to move the piece they just selected.
 * @details
 *    Based on the piece the player just selected, they must now move it to a
 *    position that is both legal (directly or diagonally forward by one
 *    position) and available (unoccupied by one of their own pieces).
 *    The player is asked for their input until they enter a valid one.
 *    It is then stored using pointers to external variables.
 *    
 *    The function returns the "move type" that the player wishes to be
 *    executed, which is to just move a piece, or to capture an enemy piece.
 *
 * @param pGame     Contains game data.
 * @param srcPos    The position that is to be moved.
 * @param destPos   Pointer to where the position where srcPos will be moved
 *                  is stored.
 *
 * @return
 *    1   if the player's move is just to move a piece.
 *    2   if the player's move is to capture an opponent's piece.
 */
int playerMovePiece(GameData *pGame, Pos srcPos, Pos *destPos) {
    int inputIsValid = 0;
    int posIsAvailable = 0;
 
    while (!inputIsValid) {
        printf("%s: Select a square to move your selected piece to.\n",
                pGame->curPlayer == 1 ? "Alpha" : "Beta");
        getUserInput(&destPos->row, &destPos->col);
 
        // ask the player for input again if their move is not legal
        if (isMoveLegal(pGame->curPlayer, srcPos, *destPos) == 0) {
            printf("ERROR: You can only move one square forward, ");
            printf("either directly forward or diagonally!\n\n");
            continue;
        }
 
        // player must select a square that is either free or is occupied
        // by an opponent's piece
        posIsAvailable = isPosAvailable(pGame, *destPos);
        if (posIsAvailable != 0) {
            inputIsValid = 1;
 
            // if player selected a square occupied by an opponent's piece,
            // that square must be an S square
            if (posIsAvailable == 2) {
                if (isSquareS(*destPos) == 0) {
                    printf("ERROR: You can only capture pieces in S squares!\n");
                    inputIsValid = 0;
                }
            }
       
        // player cannot select a square occupied by one of their pieces
        } else {
            printf("ERROR: You can't capture your own pieces!\n");
            inputIsValid = 0;
        }
        printf("\n");
    }
 
    return posIsAvailable;
}
 
/**
 * @brief Executes the main game.
 * @details
 *    The gameplay goes like the following:
 *    1. Instructions and the game board are displayed.
 *    2. The player is asked which piece to move.
 *    3. The player's input is validated. If the input is invalid, the player
 *       is asked for input again.
 *    4. The player is asked which position to move the piece they selected
 *       before to.
 *    5. The player's input is validated. If the input is invalid, the player
 *       is asked for input again.
 *    6. The move is executed, and the turn changes so the other player can
 *       choose their move.
 *    7. The game goes on until either Alpha or Beta has no more pieces,
 *       or one has all their remaining pieces on the opponent's starting
 *       positions.
 */
void playGame(void) {
    GameData game;
    GameData *pGame = &game;
    initGameData(pGame);
 
    Pos srcPos = {0};
    Pos destPos = {0};
    int moveType = 0;
    int over = GAME_ONGOING;
 
    system("cls");
   
    while (over == GAME_ONGOING) {
        displayInstructions();
        displayBoard(pGame->gameBoard);
        // debug_showABPos(pGame); // uncomment when debugging
        printf("\n\n");
        playerSelectPiece(pGame, &srcPos.row, &srcPos.col);
        moveType = playerMovePiece(pGame, srcPos, &destPos);
 
        // change the board depending on whether the player decided to move
        // or capture a piece
        if (moveType == 1) {
            movePiece(pGame, srcPos, destPos);
        } else {
            capturePiece(pGame, srcPos, destPos);
        }
 
        over = checkGameStatus(pGame);
        pGame->curPlayer = switchPlayer(pGame->curPlayer);
 
        system("cls");
    }

    displayInstructions();
    displayBoard(pGame->gameBoard); // update board to reflect latest move
    printf("%s has won!\n", over == ALPHA_WIN ? "Alpha" : "Beta");
}
 
int main(void) {
    playGame();
 
    return 0;
}