#include <iostream>
#include <string>
#include "GameState.h"
using namespace std;

#define maxComputerDepth 6

//Constants used by the computer to determine the value of having a piece on a specific square
const float pawnPoints[8][8] = {
        {0.0 ,0.0 ,0.0 ,0.0 ,0.0 ,0.0 ,0.0 ,0.0 },
        {1.1 ,1.1 ,1.1 ,1.0 ,1.0 ,1.1 ,1.1 ,1.1 },
        {1.05,1.02 ,1.0 ,1.05,1.05,1.0 ,1.02 ,1.05},
        {0.9 ,0.9 ,0.9 ,1.15,1.15,0.9 ,0.9 ,0.9 },
        {1.0 ,1.0 ,1.0 ,1.15,1.15,1.0 ,1.0 ,1.0 },
        {1.1 ,1.1 ,1.1 ,1.15,1.15,1.1 ,1.1 ,1.1 },
        {1.2 ,1.2 ,1.2 ,1.2 ,1.2 ,1.2 ,1.2 ,1.2 },
        {0.0 ,0.0 ,0.0 ,0.0 ,0.0 ,0.0 ,0.0 ,0.0}
};
const float kingPoints[8][8] = {
        {0.1 ,0.2 ,0.05,0.0 ,0.0 ,0.05,0.23,0.13},
        {0.5 ,0.1 ,0.0,-0.05,-0.05,0.0,0.13,0.08},
        {0.0 ,0.0 ,0.0 ,0.0 ,0.0 ,0.0 ,0.0 ,0.0 },
        {0.0 ,0.0 ,0.0 ,0.0 ,0.0 ,0.0 ,0.0 ,0.0 },
        {0.0 ,0.0 ,0.0 ,0.0 ,0.0 ,0.0 ,0.0 ,0.0 },
        {0.0 ,0.0 ,0.0 ,0.0 ,0.0 ,0.0 ,0.0 ,0.0 },
        {0.0 ,0.0 ,0.0 ,0.0 ,0.0 ,0.0 ,0.0 ,0.0 },
        {0.0 ,0.0 ,0.0 ,0.0 ,0.0 ,0.0 ,0.0 ,0.0 },
};
const float queenPoints[8][8] = {
        {8.85,8.85,8.85,8.8 ,8.85,8.85,8.85,8.85},
        {9.0 ,9.0 ,9.0 ,9.0 ,9.0 ,9.0 ,9.0 ,9.0 },
        {9.0 ,9.0 ,9.05,9.05,9.05,9.05,9.0 ,9.0 },
        {9.0 ,9.05,9.05,9.05,9.05,9.05,9.05,9.0 },
        {9.0 ,9.05,9.05,9.05,9.05,9.05,9.05,9.0 },
        {9.0 ,9.0 ,9.05,9.05,9.05,9.05,9.0 ,9.0 },
        {9.0 ,9.0 ,9.0 ,9.0 ,9.0 ,9.0 ,9.0 ,9.0 },
        {8.9 ,9.0 ,9.0 ,9.0 ,9.0 ,9.0 ,9.0 ,8.9 },
};
const float knightPoints[8][8] = {
        {2.8 ,2.8 ,2.85,2.85,2.85,2.85,2.8 ,2.8 },
        {2.85,2.95,3.0 ,3.05,3.05,3.0 ,2.95,2.85},
        {2.85,3.0 ,3.05,3.05,3.05,3.05,3.0 ,2.85},
        {2.85,3.02,3.07,3.08,3.08,3.07,3.02,2.85},
        {2.95,3.08,3.1 ,3.11,3.11,3.1 ,3.08,2.95},
        {2.95,3.1 ,3.12,3.13,3.13,3.12,3.1 ,2.95},
        {2.9 ,3.0 ,3.05,3.05,3.05,3.05,3.0 ,2.9},
        {2.5 ,2.95,2.97,3.0 ,3.0 ,2.97,2.95,2.5}
};
const float rookPoints[8][8] = {
        {5.0 ,5.05,5.1 ,5.07,5.07,5.1 ,5.05,5.0 },
        {4.95,4.95,5.0 ,5.0 ,5.0 ,5.0 ,4.95,4.95},
        {4.95,4.95,4.95,4.95,4.95,4.95,4.95,4.95},
        {4.95,4.95,4.95,4.95,4.95,4.95,4.95,4.95},
        {4.95,4.95,4.95,4.95,4.95,4.95,4.95,4.95},
        {4.95,4.95,4.95,4.95,4.95,4.95,4.95,4.95},
        {5.03,5.04,5.05,5.05,5.05,5.05,5.04,5.03},
        {5.0 ,5.0 ,5.0 ,5.0 ,5.0 ,5.0 ,5.0 ,5.0 }
};
const float bishopPoints[8][8] = {
        {3.0 ,2.9 ,2.85,2.85,2.85,2.85,2.9 ,3.0 },
        {3.0 ,3.1 ,3.0 ,3.0 ,3.0 ,3.0 ,3.1 ,3.0 },
        {2.95,3.0 ,3.03,3.0 ,3.0 ,3.03,3.0, 2.95},
        {2.95,3.0 ,3.05,3.03,3.03,3.05,3.0 ,2.95},
        {2.95,3.08,3.0 ,3.02,3.02,3.0 ,3.08,2.95},
        {2.95,3.0 ,3.0 ,3.02,3.02,3.0 ,3.0 ,2.95},
        {3.0 ,2.98,2.98,2.98,2.98,2.98,2.98,3.0 },
        {2.9 ,2.9 ,2.9 ,2.9 ,2.9 ,2.9 ,2.9 ,2.9 }
};

//Function prototypes
bool makePlayerMove(GameState &currentGameState, const string &playerMove);
void displayHelpMessage();
float finalScoreGameState(const GameState &gameState);
void stringToLower(string &inputString);

//Set to true to enable debugging output
const bool DEBUG = false;

int main()
{
    GameState currentGameState;
    bool moveHasBeenMade;
    bool hasResigned = false;
    bool drawnByAgreement = false;

    string playerMove;

    displayHelpMessage();
    //Main game loop
    while (!currentGameState.hasGameEnded())
    {
        //Display the board from the perspective of the player to move
        if (currentGameState.whiteToMove)
        {
            currentGameState.displayBoardWhitePOV();
            cout << "White to move\n";
        }
        else
        {
            currentGameState.displayBoardBlackPOV();
            cout << "Black to move\n";
        }
        //Process user input
        do {
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
                if (currentGameState.whiteToMove)
                    cout << "White has offered a draw, black do you accept (y)es or (n)o:";
                else
                    cout << "Black has offered a draw, white do you accept (y)es or (n)o:";
                cin >> playerMove;
                stringToLower(playerMove);
                if (playerMove == "y" || playerMove == "yes")
                {
                    currentGameState.gameIsDraw = true;
                    drawnByAgreement = true;
                }
                else
                    cout << "Draw declined\n";
            }
            else
                moveHasBeenMade = makePlayerMove(currentGameState, playerMove);

        }while(!moveHasBeenMade && !currentGameState.hasGameEnded());
    }
    if (currentGameState.whiteHasWon)
    {
        if (!hasResigned)
        {
            currentGameState.displayBoardBlackPOV();
            cout << "White has won by Checkmate";
        }
        else
            cout << "White has won by resignation";
    }
    else if (currentGameState.blackHasWon)
    {
        if (!hasResigned)
        {
            currentGameState.displayBoardWhitePOV();
            cout << "Black has won by Checkmate";
        }
        else
            cout << "Black has won by resignation";
    }
    else if (currentGameState.gameIsDraw)
    {
        if (!drawnByAgreement)
        {
            if (currentGameState.whiteToMove)
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
        //Converts the move into an array of ints, numerically representing the squares on the board
        move.fromColumn = playerMove[0] - 'a';
        move.fromRow = playerMove[1] - '1';
        move.toColumn = playerMove[2] - 'a';
        move.toRow = playerMove[3] - '1';
        //Stores the null terminator as the promotion if playerMove.length == 4 otherwise stores the promotion
        if(currentGameState.whiteToMove)
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

//Scores a games state, without calculating further positions
float finalScoreGameState(const GameState &gameState)
{
    float score = 0;
    //TODO implement scoring algorithm
    return score;
}

void stringToLower(string &inputString)
{
    for(char & letter : inputString)
        letter = tolower(letter);
}