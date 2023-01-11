#include <iostream>
#include <string>
#include "GameState.h"
using namespace std;

#define MAX_COMPUTER_DEPTH 4

//Constants used by the computer to determine the value of having a piece on a specific square
const int PAWN_POINTS[8][8] = {
        {0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  },
        {115,115,115,95 ,95 ,115,115,115},
        {107,105,100,100,100,100,105,107},
        {90 ,90 ,90 ,115,115,90 ,90 ,90 },
        {100,100,100,115,115,100,100,100},
        {110,110,110,115,115,110,110,110},
        {120,120,120,120,120,120,120,120},
        {0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  }
};
const int KING_POINTS[8][8] = {
        {10 ,20 ,5  ,0  ,0  ,5  ,23 ,13 },
        {10 ,5  ,0  ,-5 ,-5 ,0  ,13 ,8  },
        {0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  },
        {0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  },
        {0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  },
        {0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  },
        {0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  },
        {0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  }
};
const int QUEEN_POINTS[8][8] = {
        {885,885,885,885,885,885,885,885},
        {900,900,900,900,900,900,900,900},
        {900,900,905,905,905,905,900,900},
        {900,905,905,905,905,905,905,900},
        {900,905,905,905,905,905,905,900},
        {900,900,905,905,905,905,900,900},
        {900,900,900,900,900,900,900,900},
        {890,900,900,900,900,900,900,890}
};
const int KNIGHT_POINTS[8][8] = {
        {280,280,285,285,285,285,280,280},
        {285,295,300,305,305,300,295,285},
        {285,300,305,305,305,305,300,285},
        {285,302,307,308,308,307,302,285},
        {295,308,310,311,311,310,308,295},
        {295,310,312,313,313,312,310,295},
        {290,300,305,305,305,305,300,290},
        {250,295,297,300,300,297,295,250}
};
const int ROOK_POINTS[8][8] = {
        {500,505,510,507,507,510,505,500},
        {495,495,500,500,500,500,495,495},
        {495,495,495,495,495,495,495,495},
        {495,495,495,495,495,495,495,495},
        {495,495,495,495,495,495,495,495},
        {495,495,495,495,495,495,495,495},
        {503,504,505,505,505,505,504,503},
        {500,500,500,500,500,500,500,500}
};
const int BISHOP_POINTS[8][8] = {
        {300,290,285,285,285,285,290,300},
        {300,310,300,300,300,300,310,300},
        {295,300,303,300,300,303,300,295},
        {295,300,305,303,303,305,300,295},
        {295,308,300,302,302,300,308,295},
        {295,300,300,302,302,300,300,295},
        {300,298,298,298,298,298,298,300},
        {290,290,290,290,290,290,290,290}
};

//Function prototypes
bool makePlayerMove(GameState &currentGameState, const string &playerMove);
void displayHelpMessage();
void stringToLower(string &inputString);

//Set to true to enable debugging output
const bool DEBUG = true;

int main()
{
    GameState currentGameState;
    bool moveHasBeenMade;
    bool hasResigned = false;
    bool drawnByAgreement = false;
    bool computerPlaysWhite = false;
    bool computerPlaysBlack = false;
    string playerMove;

    //Determine if the player wants to do PvP or PvE
    cout << "How many human players (1 or 2): ";
    do
    {
        cin >> playerMove;
    }while(playerMove != "1" && playerMove != "2");

    if(playerMove == "1")
    {
        cout << "Play as (W)hite or (B)lack: ";
        do
        {
            cin >> playerMove;
            stringToLower(playerMove);
            if (playerMove == "w" || playerMove == "white")
                computerPlaysBlack = true;

            else if (playerMove == "b" || playerMove == "black")
                computerPlaysWhite = true;

        }while(!computerPlaysWhite && !computerPlaysBlack);
    }

    displayHelpMessage();
    //Main game loop
    while (!currentGameState.hasGameEnded())
    {
        //Display the board from the perspective of the player to move
        if (currentGameState.getWhiteToMove())
        {
            currentGameState.displayBoardWhitePOV();
            cout << "White to move\n";
        }
        else
        {
            currentGameState.displayBoardBlackPOV();
            cout << "Black to move\n";
        }
        if (DEBUG)
            cout << finalScoreGameState(currentGameState) << endl;

        if (currentGameState.getWhiteToMove() && computerPlaysWhite || !currentGameState.getWhiteToMove() && computerPlaysBlack)
            makeComputerMove(currentGameState);
        else
        {
            //Process user input
            do
            {
                cout << "Enter move: ";
                cin >> playerMove;
                stringToLower(playerMove);

                if (playerMove == "h" || playerMove == "help")
                    displayHelpMessage();

                else if (playerMove == "r" || playerMove == "resign")
                {
                    currentGameState.resign();
                    hasResigned = true;
                }
                else if (playerMove == "d" || playerMove == "draw")
                {
                    if (currentGameState.getWhiteToMove())
                        cout << "White has offered a draw, black do you accept (y)es or (n)o:";
                    else
                        cout << "Black has offered a draw, white do you accept (y)es or (n)o:";
                    cin >> playerMove;
                    stringToLower(playerMove);
                    if (playerMove == "y" || playerMove == "yes")
                    {
                        currentGameState.declareDraw();
                        drawnByAgreement = true;
                    }
                    else
                        cout << "Draw declined\n";
                }
                else
                    moveHasBeenMade = makePlayerMove(currentGameState, playerMove);

            } while (!moveHasBeenMade && !currentGameState.hasGameEnded());
        }
    }//End of main game loop

    //Display final board position and end of game message
    if (currentGameState.getWhiteHasWon())
    {
        if (!hasResigned)
        {
            currentGameState.displayBoardBlackPOV();
            cout << "White has won by Checkmate";
        }
        else
            cout << "White has won by resignation";
    }
    else if (currentGameState.getBlackHasWon())
    {
        if (!hasResigned)
        {
            currentGameState.displayBoardWhitePOV();
            cout << "Black has won by Checkmate";
        }
        else
            cout << "Black has won by resignation";
    }
    else if (currentGameState.getGameIsDraw())
    {
        if (!drawnByAgreement)
        {
            if (currentGameState.getWhiteToMove())
                currentGameState.displayBoardWhitePOV();
            else
                currentGameState.displayBoardBlackPOV();
        }
        cout << "The game is a draw";
    }
    return 0;
}

bool makePlayerMove(GameState &currentGameState, const string &playerMove)
{
    moveStruct move;
    bool validMove = false;

    if (playerMove.length() >= 4
        && playerMove[0] >= 'a' && playerMove[0] <= 'h'
        && playerMove[1] >= '1' && playerMove[1] <= '8'
        && playerMove[2] >= 'a' && playerMove[2] <= 'h'
        && playerMove[3] >= '1' && playerMove[3] <= '8')
    {
        //Converts the move into a struct of ints, numerically representing the squares on the board
        move.fromColumn = playerMove[0] - 'a';
        move.fromRow = playerMove[1] - '1';
        move.toColumn = playerMove[2] - 'a';
        move.toRow = playerMove[3] - '1';
        //Stores the null terminator as the promotion if playerMove.length == 4 otherwise stores the promotion
        if(currentGameState.getWhiteToMove())
            move.promotion = toupper(playerMove[4]);
        else
            move.promotion = playerMove[4];

        validMove = currentGameState.makeMove(move);

        if(!validMove)
            cout << "Illegal move\n";
    }
    else
        cout << "Invalid move\n";
    return validMove;
}

void displayHelpMessage()
{
    cout << "Pieces are represented by the corresponding letters (p)awn, (r)ook, k(n)ight\n"
         << "(b)ishop, (k)ing, (q)ueen. Uppercase letters represent white pieces,\n"
         << "lowercase letters represent black pieces, and _ represents an empty square\n"
         << "Enter moves in the form of abcd, where ab represents the column and the row\n"
         << "of the piece being moved, and cd represents the column and the row of the\n"
         << "destination square, respectively. a and c should be letters a-h, well b and\n"
         << "d should be numbers between 1-8. e.g. e2e4. To castle move the king two\n"
         << "squares in the direction of the rook. To promote a pawn add the letter of\n"
         << "the piece being promoted to at the end of the move. e.g. a7a8Q\n"
         << "(r)esign to resign\n"
         << "(d)raw to offer a draw\n"
         << "(h)elp to display this message again" << endl;
}
void makeComputerMove(GameState &currentGameState)
{
    moveStruct bestMove;
    int bestScore;
    int newScore;
    bool addToListOfGameState = true;

    if (currentGameState.getWhiteToMove())
    {
        bestScore = -1000000;
        for (auto iter = currentGameState.legalMoves.begin(); iter != currentGameState.legalMoves.end(); iter++)
        {
            newScore = recursiveScoreMove(*iter, currentGameState, addToListOfGameState, 1);
            if (newScore > bestScore)
            {
                bestScore = newScore;
                bestMove = *iter;
            }
            addToListOfGameState = false;
        }
    }
    else
    {
        bestScore = 1000000;
        for (auto iter = currentGameState.legalMoves.begin(); iter != currentGameState.legalMoves.end(); iter++)
        {
            newScore = recursiveScoreMove(*iter, currentGameState, addToListOfGameState, 1);
            if (newScore < bestScore)
            {
                bestScore = newScore;
                bestMove = *iter;
            }
            addToListOfGameState = false;
        }
    }
    currentGameState.makeMove(bestMove, true, false);
}

int recursiveScoreMove(const moveStruct &move, GameState gameState, bool addPreviousToListOfGameState, int currentDepth)
{
    int newScore;
    int bestScore;
    bool addToListOfGameState = true;

    //Make the move
    gameState.makeMove(move, true, addPreviousToListOfGameState);

    if(currentDepth == MAX_COMPUTER_DEPTH)
        return finalScoreGameState(gameState);

    //Check to see if the game has ended
    if (gameState.getWhiteHasWon())
        return 1000000 - currentDepth;
    else if (gameState.getBlackHasWon())
        return -1000000 + currentDepth;
    else if (gameState.getGameIsDraw())
        return 0;

    if (gameState.getWhiteToMove())
    {
        bestScore = -1000000;
        for (auto iter = gameState.legalMoves.begin(); iter != gameState.legalMoves.end(); iter++)
        {
            newScore = recursiveScoreMove(*iter, gameState, addToListOfGameState, currentDepth + 1);
            if (newScore > bestScore)
            {
                bestScore = newScore;
            }
            addToListOfGameState = false;
        }
    }
    else
    {
        bestScore = 1000000;
        for (auto iter = gameState.legalMoves.begin(); iter != gameState.legalMoves.end(); iter++)
        {
            newScore = recursiveScoreMove(*iter, gameState, addToListOfGameState, currentDepth + 1);
            if (newScore < bestScore)
            {
                bestScore = newScore;
            }
            addToListOfGameState = false;
        }
    }
    gameState.previousGameStates->pop_back();
    return bestScore;
}

//Scores a games state, without calculating further positions
int finalScoreGameState(const GameState &gameState)
{
    int score = 0;

    if (gameState.getWhiteHasWon())
        return 1000000 - MAX_COMPUTER_DEPTH;
    if (gameState.getBlackHasWon())
        return -1000000 + MAX_COMPUTER_DEPTH;
    if (gameState.getGameIsDraw())
        return 0;
    //Adjust score based on castling privileges
    if (gameState.getWhiteKingSideCastlePrivilege())
        score += 5;
    if (gameState.getWhiteQueenSideCastlePrivilege())
        score += 5;
    if (gameState.getBlackKingSideCastlePrivilege())
        score -= 5;
    if (gameState.getBlackQueenSideCastlePrivilege())
        score -= 5;
    //Adjust score based on piece position
    for(int row = 0; row < 8; row++)
        for(int column = 0; column < 8; column++)
        {
            switch (gameState.board[row][column])
            {
                case 'P':
                    score += PAWN_POINTS[row][column];
                    break;
                case 'p':
                    score -= PAWN_POINTS[7 - row][column];
                    break;
                case 'K':
                    score += KING_POINTS[row][column];
                    break;
                case 'k':
                    score -= KING_POINTS[7 - row][column];
                    break;
                case 'Q':
                    score += QUEEN_POINTS[row][column];
                    break;
                case 'q':
                    score -= QUEEN_POINTS[7 - row][column];
                    break;
                case 'N':
                    score += KNIGHT_POINTS[row][column];
                    break;
                case 'n':
                    score -= KNIGHT_POINTS[7 - row][column];
                    break;
                case 'R':
                    score += ROOK_POINTS[row][column];
                    break;
                case 'r':
                    score -= ROOK_POINTS[7 - row][column];
                    break;
                case 'B':
                    score += BISHOP_POINTS[row][column];
                    break;
                case 'b':
                    score -= BISHOP_POINTS[7 - row][column];
                    break;

                    default:
                        break;
            }
        }

    return score;
}

void stringToLower(string &inputString)
{
    for(char & letter : inputString)
        letter = tolower(letter);
}