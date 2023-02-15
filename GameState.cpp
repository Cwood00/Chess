//
// Created by Chris on 12/21/2022.
//
#include "GameState.h"

GameState::GameState()
{
    whiteKingSideCastlePrivilege = true;
    whiteQueenSideCastlePrivilege = true;
    blackKingSideCastlePrivilege = true;
    blackQueenSideCastlePrivilege = true;
    enPassantRow = -1;
    enPassantColumn = -1;
    whiteToMove = true;
    whiteHasWon = false;
    blackHasWon = false;
    gameIsDraw = false;
    movesSinceLastCaptureOrPawnMove = 0;
    legalMoves.reserve(60);
    setWhiteLegalMoves();

}
//The copy constructor does not copy the list of legal moves.
GameState::GameState(const GameState &oldGameState)
{
    for(int row = 0; row < 8; row++)
        for (int column = 0; column < 8; column++)
            this->board[row][column] = oldGameState.board[row][column];

    this->whiteKingSideCastlePrivilege = oldGameState.whiteKingSideCastlePrivilege;
    this->whiteQueenSideCastlePrivilege = oldGameState.whiteQueenSideCastlePrivilege;
    this->blackKingSideCastlePrivilege = oldGameState.blackKingSideCastlePrivilege;
    this->blackQueenSideCastlePrivilege = oldGameState.blackQueenSideCastlePrivilege;
    this->enPassantRow = oldGameState.enPassantRow;
    this->enPassantColumn = oldGameState.enPassantColumn;
    this->whiteToMove = oldGameState.whiteToMove;
    this->whiteHasWon = oldGameState.whiteHasWon;
    this->blackHasWon = oldGameState.blackHasWon;
    this->gameIsDraw = oldGameState.gameIsDraw;
    this->movesSinceLastCaptureOrPawnMove = oldGameState.movesSinceLastCaptureOrPawnMove;
    this->legalMoves.reserve(60);
}

void GameState::displayBoardWhitePOV() const
{
    cout << endl;
    for (int i = 7; i > -1; i--)
    {
        cout << i + 1 << ". ";
        for (int j = 0; j < 8; j++)
            cout << board[i][j] << "  ";
        cout << endl;
    }
    cout << "   a. b. c. d. e. f. g. h\n";
}

void GameState::displayBoardBlackPOV() const
{
    cout << endl << "   h. g. f. e. d. c. b. a\n";
    for (int i =0; i < 8; i++)
    {
        cout << i + 1 << ". ";
        for (int j = 7; j > - 1; j--)
            cout << board[i][j] << "  ";
        cout << endl;
    }
}

bool GameState::checkForInsufficientMaterial() const
{
    char piece;
    char minorPieces = 0;
    //Loop through every square on the board
    for (int row = 0; row < 8; row++)
        for(int column = 0; column < 8; column++)
        {
            piece = board[row][column];
            //If there are is a rook, a queen, or a pawn then there is sufficient material
            if (piece == 'R' || piece== 'r' || piece == 'Q' || piece == 'q' || piece == 'P' || piece == 'p')
                return false;
            //If there is a total of at lest two bishops and/or knights there is sufficient material
            if (piece == 'B' || piece == 'b' || piece == 'N' || piece == 'n')
            {
                minorPieces++;
                if (minorPieces == 2)
                    return  false;
            }
        }
    //If the conditions for sufficient material where not found, then there is insufficient material
    return true;
}
//Makes a move without validating it, or updating any other game data.
void GameState::movePiece(const moveStruct &move)
{
    char movedPiece = board[move.fromRow][move.fromColumn];
    if (!move.promotion)
    {
        board[move.toRow][move.toColumn] = board[move.fromRow][move.fromColumn];
        //If castling happens move the rook
        if (movedPiece == 'K' && move.fromColumn == 4)
        {
            if (move.toColumn == 2)
            {
                board[0][3] = 'R';
                board[0][0] = '_';
            }
            else if (move.toColumn == 6)
            {
                board[0][5] = 'R';
                board[0][7] = '_';
            }
        }
        else if ((movedPiece == 'k' && move.fromColumn == 4))
        {
            if (move.toColumn == 2)
            {
                board[7][3] = 'r';
                board[7][0] = '_';
            }
            else if (move.toColumn == 6)
            {
                board[7][5] = 'r';
                board[7][7] = '_';
            }
        }
        //If en passant happens removes the captured pawn
        else if (movedPiece == 'P' &&  move.toRow == enPassantRow && move.toColumn == enPassantColumn)
            board[4][move.toColumn] = '_';
        else if (movedPiece == 'p' &&  move.toRow == enPassantRow && move.toColumn == enPassantColumn)
            board[3][move.toColumn] = '_';
    }
    else
        board[move.toRow][move.toColumn] = move.promotion;
    //Empty the space the piece moved from
    board[move.fromRow][move.fromColumn] = '_';
}

bool GameState::movesIntoCheck(const moveStruct &move, int kingRow, int kingColumn)
{
    bool inCheck;
    char movedOnto = board[move.toRow][move.toColumn];

    //Make the move
    movePiece(move);

    //Check to see if player is in check after making move
    if(whiteToMove)
        inCheck = getWhiteIsInCheck(kingRow, kingColumn);
    else
        inCheck = getBlackIsInCheck(kingRow, kingColumn);

    //Undo the move
    board[move.fromRow][move.fromColumn] = board[move.toRow][move.toColumn];
    board[move.toRow][move.toColumn] = movedOnto;
    //Undo en Passant
    if (board[move.fromRow][move.fromColumn] == 'P' &&  move.toRow == enPassantRow && move.toColumn == enPassantColumn)
        board[4][move.toColumn] = 'p';
    else if (board[move.fromRow][move.fromColumn] == 'p' &&  move.toRow == enPassantRow && move.toColumn == enPassantColumn)
        board[3][move.toColumn] = 'P';
    return inCheck;
}

bool GameState::getWhiteIsInCheck(int kingRow, int kingColumn) const
{
    int searchRow = kingRow + 1;
    int searchColumn = kingColumn + 1;
    //Used for bishop, rook and queen moves to determine if it is needed to keep searching in that direction
    bool keepSearching = true;

    //Top right diagonal
    if (searchRow < 8 && searchColumn < 8)
    {
        //Check for adjacent kings or pawns
        if (board[searchRow][searchColumn] == 'p' || board[searchRow][searchColumn] == 'k')
            return true;
        while (keepSearching)
        {
            //If there is a bishop or queen along the fist non-empty spot in diagonal then the king is in check
            if (board[searchRow][searchColumn] == 'b' || board[searchRow][searchColumn] == 'q')
                return true;
            //We stop searching the diagonal if the spot is not empty
            else if (board[searchRow][searchColumn] != '_')
                keepSearching = false;
            //adjust the search square and make sure it is still on the board
            else
            {
                searchRow++;
                searchColumn++;
                if (!(searchRow < 8 && searchColumn < 8))
                    keepSearching = false;
            }
        }
    }
    //Top left diagonal
    searchRow = kingRow + 1;
    searchColumn = kingColumn - 1;
    keepSearching = true;
    if (searchRow < 8 && searchColumn > -1)
    {
        //Check for adjacent kings or pawns
        if (board[searchRow][searchColumn] == 'p' || board[searchRow][searchColumn] == 'k')
            return true;
        while (keepSearching)
        {
            //If there is a bishop or queen along the fist non-empty spot in diagonal then the king is in check
            if (board[searchRow][searchColumn] == 'b' || board[searchRow][searchColumn] == 'q')
                return true;
            //We stop searching the diagonal if the spot is not empty
            else if (board[searchRow][searchColumn] != '_')
                keepSearching = false;
            //adjust the search square and make sure it is still on the board
            else
            {
                searchRow++;
                searchColumn--;
                if (!(searchRow < 8 && searchColumn > -1))
                    keepSearching = false;
            }
        }
    }
    //Bottom  left diagonal
    searchRow = kingRow - 1;
    searchColumn = kingColumn - 1;
    keepSearching = true;
    if (searchRow > -1 && searchColumn > -1)
    {
        //Check for adjacent kings
        if (board[searchRow][searchColumn] == 'k')
            return true;
        while (keepSearching)
        {
            //If there is a bishop or queen along the fist non-empty spot in diagonal then the king is in check
            if (board[searchRow][searchColumn] == 'b' || board[searchRow][searchColumn] == 'q')
                return true;
            //We stop searching the diagonal if the spot is not empty
            else if (board[searchRow][searchColumn] != '_')
                keepSearching = false;
            //adjust the search square and make sure it is still on the board
            else
            {
                searchRow--;
                searchColumn--;
                if (!(searchRow > -1 && searchColumn > -1))
                    keepSearching = false;
            }
        }
    }
    //Bottom right diagonal
    searchRow = kingRow - 1;
    searchColumn = kingColumn + 1;
    keepSearching = true;
    if (searchRow > -1 && searchColumn < 8)
    {
        //Check for adjacent kings
        if (board[searchRow][searchColumn] == 'k')
            return true;
        while (keepSearching)
        {
            //If there is a bishop or queen along the fist non-empty spot in diagonal then the king is in check
            if (board[searchRow][searchColumn] == 'b' || board[searchRow][searchColumn] == 'q')
                return true;
            //We stop searching the diagonal if the spot is not empty
            else if (board[searchRow][searchColumn] != '_')
                keepSearching = false;
            //adjust the search square and make sure it is still on the board
            else
            {
                searchRow--;
                searchColumn++;
                if (!(searchRow > -1 && searchColumn < 8))
                    keepSearching = false;
            }
        }
    }
    //Up
    searchRow = kingRow + 1;
    searchColumn = kingColumn;
    keepSearching = true;
    if (searchRow < 8)
    {
        //Check for adjacent kings
        if (board[searchRow][searchColumn] == 'k')
            return true;
        while (keepSearching)
        {
            //If there is a rook or queen along the fist non-empty spot in the column then the king is in check
            if (board[searchRow][searchColumn] == 'r' || board[searchRow][searchColumn] == 'q')
                return true;
            //We stop searching the column if the spot is not empty
            else if (board[searchRow][searchColumn] != '_')
                keepSearching = false;
            //adjust the search square and make sure it is still on the board
            else
            {
                searchRow++;
                if (searchRow > 7)
                    keepSearching = false;
            }
        }
    }
    //Down
    searchRow = kingRow - 1;
    searchColumn = kingColumn;
    keepSearching = true;
    if (searchRow > -1)
    {
        //Check for adjacent kings
        if (board[searchRow][searchColumn] == 'k')
            return true;
        while (keepSearching)
        {
            //If there is a rook or queen along the fist non-empty spot in the column then the king is in check
            if (board[searchRow][searchColumn] == 'r' || board[searchRow][searchColumn] == 'q')
                return true;
            //We stop searching the column if the spot is not empty
            else if (board[searchRow][searchColumn] != '_')
                keepSearching = false;
            //adjust the search square and make sure it is still on the board
            else
            {
                searchRow--;
                if (searchRow < 0)
                    keepSearching = false;
            }
        }
    }
    //Right
    searchRow = kingRow;
    searchColumn = kingColumn + 1;
    keepSearching = true;
    if (searchColumn < 8)
    {
        //Check for adjacent kings
        if (board[searchRow][searchColumn] == 'k')
            return true;
        while (keepSearching)
        {
            //If there is a rook or queen along the fist non-empty spot in the row then the king is in check
            if (board[searchRow][searchColumn] == 'r' || board[searchRow][searchColumn] == 'q')
                return true;
            //We stop searching the row if the spot is not empty
            else if (board[searchRow][searchColumn] != '_')
                keepSearching = false;
                //adjust the search square and make sure it is still on the board
            else
            {
                searchColumn++;
                if (searchColumn > 7)
                    keepSearching = false;
            }
        }
    }
    //left
    searchRow = kingRow;
    searchColumn = kingColumn - 1;
    keepSearching = true;
    if (searchColumn > -1)
    {
        //Check for adjacent kings
        if (board[searchRow][searchColumn] == 'k')
            return true;
        while (keepSearching)
        {
            //If there is a rook or queen along the fist non-empty spot in the row then the king is in check
            if (board[searchRow][searchColumn] == 'r' || board[searchRow][searchColumn] == 'q')
                return true;
            //We stop searching the row if the spot is not empty
            else if (board[searchRow][searchColumn] != '_')
                keepSearching = false;
            //adjust the search square and make sure it is still on the board
            else
            {
                searchColumn--;
                if (searchColumn < 0)
                    keepSearching = false;
            }
        }
    }
    //Check for night checks
    searchRow = kingRow + 2;
    if (searchRow < 8)
    {
        searchColumn = kingColumn + 1;
        if(searchColumn < 8 && board[searchRow][searchColumn] == 'n')
            return true;
        searchColumn = kingColumn - 1;
        if(searchColumn > -1 && board[searchRow][searchColumn] == 'n')
            return true;
    }
    searchColumn = kingColumn + 2;
    if (searchColumn < 8)
    {
        searchRow = kingRow + 1;
        if(searchRow < 8 && board[searchRow][searchColumn] == 'n')
            return true;
        searchRow = kingRow - 1;
        if(searchColumn > -1 && board[searchRow][searchColumn] == 'n')
            return true;
    }
    searchRow = kingRow - 2;
    if (searchRow > -1)
    {
        searchColumn = kingColumn + 1;
        if(searchColumn < 8 && board[searchRow][searchColumn] == 'n')
            return true;
        searchColumn = kingColumn - 1;
        if(searchColumn > -1 && board[searchRow][searchColumn] == 'n')
            return true;
    }
    searchColumn = kingColumn - 2;
    if (searchColumn > -1)
    {
        searchRow = kingRow + 1;
        if(searchRow < 8 && board[searchRow][searchColumn] == 'n')
            return true;
        searchRow = kingRow - 1;
        if(searchColumn > -1 && board[searchRow][searchColumn] == 'n')
            return true;
    }
    //Default return if the king is not in check
    return false;
}

bool GameState::getBlackIsInCheck(int kingRow, int kingColumn) const
{
    int searchRow = kingRow + 1;
    int searchColumn = kingRow + 1;
    //Used for bishop, rook and queen moves to determine if it is needed to keep searching in that direction
    bool keepSearching = true;

    //Top right diagonal
    if (searchRow < 8 && searchColumn < 8)
    {
        //Check for adjacent king
        if (board[searchRow][searchColumn] == 'K')
            return true;
        while (keepSearching)
        {
            //If there is a bishop or queen along the fist non-empty spot in diagonal then the king is in check
            if (board[searchRow][searchColumn] == 'B' || board[searchRow][searchColumn] == 'Q')
                return true;
                //We stop searching the diagonal if the spot is not empty
            else if (board[searchRow][searchColumn] != '_')
                keepSearching = false;
                //adjust the search square and make sure it is still on the board
            else
            {
                searchRow++;
                searchColumn++;
                if (!(searchRow < 8 && searchColumn < 8))
                    keepSearching = false;
            }
        }
    }
    //Top left diagonal
    searchRow = kingRow + 1;
    searchColumn = kingColumn - 1;
    keepSearching = true;
    if (searchRow < 8 && searchColumn > -1)
    {
        //Check for adjacent king
        if (board[searchRow][searchColumn] == 'K')
            return true;
        while (keepSearching)
        {
            //If there is a bishop or queen along the fist non-empty spot in diagonal then the king is in check
            if (board[searchRow][searchColumn] == 'B' || board[searchRow][searchColumn] == 'Q')
                return true;
                //We stop searching the diagonal if the spot is not empty
            else if (board[searchRow][searchColumn] != '_')
                keepSearching = false;
                //adjust the search square and make sure it is still on the board
            else
            {
                searchRow++;
                searchColumn--;
                if (!(searchRow < 8 && searchColumn > -1))
                    keepSearching = false;
            }
        }
    }
    //Bottom  left diagonal
    searchRow = kingRow - 1;
    searchColumn = kingColumn - 1;
    keepSearching = true;
    if (searchRow > -1 && searchColumn > -1)
    {
        //Check for adjacent king or pawn
        if (board[searchRow][searchColumn] == 'P' || board[searchRow][searchColumn] == 'K')
            return true;
        while (keepSearching)
        {
            //If there is a bishop or queen along the fist non-empty spot in diagonal then the king is in check
            if (board[searchRow][searchColumn] == 'B' || board[searchRow][searchColumn] == 'Q')
                return true;
                //We stop searching the diagonal if the spot is not empty
            else if (board[searchRow][searchColumn] != '_')
                keepSearching = false;
                //adjust the search square and make sure it is still on the board
            else
            {
                searchRow--;
                searchColumn--;
                if (!(searchRow > -1 && searchColumn > -1))
                    keepSearching = false;
            }
        }
    }
    //Bottom right diagonal
    searchRow = kingRow - 1;
    searchColumn = kingColumn + 1;
    keepSearching = true;
    if (searchRow > -1 && searchColumn < 8)
    {
        //Check for adjacent king or pawn
        if (board[searchRow][searchColumn] == 'P' || board[searchRow][searchColumn] == 'K')
            return true;
        while (keepSearching)
        {
            //If there is a bishop or queen along the fist non-empty spot in diagonal then the king is in check
            if (board[searchRow][searchColumn] == 'B' || board[searchRow][searchColumn] == 'Q')
                return true;
                //We stop searching the diagonal if the spot is not empty
            else if (board[searchRow][searchColumn] != '_')
                keepSearching = false;
                //adjust the search square and make sure it is still on the board
            else
            {
                searchRow--;
                searchColumn++;
                if (!(searchRow > -1 && searchColumn < 8))
                    keepSearching = false;
            }
        }
    }
    //Up
    searchRow = kingRow + 1;
    searchColumn = kingColumn;
    keepSearching = true;
    if (searchRow < 8)
    {
        //Check for adjacent kings
        if (board[searchRow][searchColumn] == 'K')
            return true;
        while (keepSearching)
        {
            //If there is a rook or queen along the fist non-empty spot in the column then the king is in check
            if (board[searchRow][searchColumn] == 'R' || board[searchRow][searchColumn] == 'Q')
                return true;
                //We stop searching the column if the spot is not empty
            else if (board[searchRow][searchColumn] != '_')
                keepSearching = false;
                //adjust the search square and make sure it is still on the board
            else
            {
                searchRow++;
                if (searchRow > 7)
                    keepSearching = false;
            }
        }
    }
    //Down
    searchRow = kingRow - 1;
    searchColumn = kingColumn;
    keepSearching = true;
    if (searchRow > -1)
    {
        //Check for adjacent kings
        if (board[searchRow][searchColumn] == 'K')
            return true;
        while (keepSearching)
        {
            //If there is a rook or queen along the fist non-empty spot in the column then the king is in check
            if (board[searchRow][searchColumn] == 'R' || board[searchRow][searchColumn] == 'Q')
                return true;
                //We stop searching the column if the spot is not empty
            else if (board[searchRow][searchColumn] != '_')
                keepSearching = false;
                //adjust the search square and make sure it is still on the board
            else
            {
                searchRow--;
                if (searchRow < 0)
                    keepSearching = false;
            }
        }
    }
    //Right
    searchRow = kingRow;
    searchColumn = kingColumn + 1;
    keepSearching = true;
    if (searchColumn < 8)
    {
        //Check for adjacent kings
        if (board[searchRow][searchColumn] == 'K')
            return true;
        while (keepSearching)
        {
            //If there is a rook or queen along the fist non-empty spot in the row then the king is in check
            if (board[searchRow][searchColumn] == 'R' || board[searchRow][searchColumn] == 'Q')
                return true;
                //We stop searching the row if the spot is not empty
            else if (board[searchRow][searchColumn] != '_')
                keepSearching = false;
                //adjust the search square and make sure it is still on the board
            else
            {
                searchColumn++;
                if (searchColumn > 7)
                    keepSearching = false;
            }
        }
    }
    //left
    searchRow = kingRow;
    searchColumn = kingColumn - 1;
    keepSearching = true;
    if (searchColumn > -1)
    {
        //Check for adjacent kings
        if (board[searchRow][searchColumn] == 'K')
            return true;
        while (keepSearching)
        {
            //If there is a rook or queen along the fist non-empty spot in the row then the king is in check
            if (board[searchRow][searchColumn] == 'R' || board[searchRow][searchColumn] == 'Q')
                return true;
                //We stop searching the row if the spot is not empty
            else if (board[searchRow][searchColumn] != '_')
                keepSearching = false;
                //adjust the search square and make sure it is still on the board
            else
            {
                searchColumn--;
                if (searchColumn < 0)
                    keepSearching = false;
            }
        }
    }
    //Check for knight checks
    searchRow = kingRow + 2;
    if (searchRow < 8)
    {
        searchColumn = kingColumn + 1;
        if(searchColumn < 8 && board[searchRow][searchColumn] == 'N')
            return true;
        searchColumn = kingColumn - 1;
        if(searchColumn > -1 && board[searchRow][searchColumn] == 'N')
            return true;
    }
    searchColumn = kingColumn + 2;
    if (searchColumn < 8)
    {
        searchRow = kingRow + 1;
        if(searchRow < 8 && board[searchRow][searchColumn] == 'N')
            return true;
        searchRow = kingRow - 1;
        if(searchColumn > -1 && board[searchRow][searchColumn] == 'N')
            return true;
    }
    searchRow = kingRow - 2;
    if (searchRow > -1)
    {
        searchColumn = kingColumn + 1;
        if(searchColumn < 8 && board[searchRow][searchColumn] == 'N')
            return true;
        searchColumn = kingColumn - 1;
        if(searchColumn > -1 && board[searchRow][searchColumn] == 'N')
            return true;
    }
    searchColumn = kingColumn - 2;
    if (searchColumn > -1)
    {
        searchRow = kingRow + 1;
        if(searchRow < 8 && board[searchRow][searchColumn] == 'N')
            return true;
        searchRow = kingRow - 1;
        if(searchColumn > -1 && board[searchRow][searchColumn] == 'N')
            return true;
    }
    //Default return if the king is not in check
    return false;
}

bool GameState::getWhiteIsInCheck() const
{
    int kingRow;
    int kingColumn;

    //Locates the white king
    bool foundKing = false;
    for (int row = 0; row < 8 && !foundKing; row++)
        for (int column = 7; column > -1 && !foundKing; column--)
            if (board[row][column] == 'K')
            {
                kingRow = row;
                kingColumn = column;
                foundKing = true;
            }

    return getWhiteIsInCheck(kingRow, kingColumn);
}

bool GameState::getBlackIsInCheck() const
{
    int kingRow;
    int kingColumn;

    //Locates the black king
    bool foundKing = false;
    for (int row = 7; row > -1 && !foundKing; row--)
        for (int column = 7; column > -1 && !foundKing; column--)
            if (board[row][column] == 'k')
            {
                kingRow = row;
                kingColumn = column;
                foundKing = true;
            }

    return getWhiteIsInCheck(kingRow, kingColumn);
}
//Populates a list of legal moves for white.
void GameState::setWhiteLegalMoves()
{
    //Used for checking if move puts king into check
    int kingRow;
    int kingColumn;
    moveStruct move;
    //Used for bishop, rook and queen moves to determine if the piece can keep moving in that directions
    bool canKeepMoving;
    //Used for performance improvements in determining if white can castle
    bool notInCheck = false;
    char possiblePromotions[] = {'Q','R','N','B'};

    //Locates the white king
    bool foundKing = false;
    for (int row = 0; row < 8 && !foundKing; row++)
        for (int column = 7; column > -1 && !foundKing; column--)
            if (board[row][column] == 'K')
            {
                kingRow = row;
                kingColumn = column;
                foundKing = true;
            }

    move.promotion = '\0';

    //Appends castling moves to the list of legal moves
    //King side
    if (whiteKingSideCastlePrivilege && board[0][5] == '_' && board[0][6] == '_' &&
        !getWhiteIsInCheck(kingRow, kingColumn))
    {
        notInCheck = true;
        if (!getWhiteIsInCheck(0, 5) && !getWhiteIsInCheck(0, 6))
        {
            legalMoves.emplace_back(0, 4, 0, 6, '\0');
        }
    }
    //Queen side
    if (whiteQueenSideCastlePrivilege && board[0][3] == '_' && board[0][2] == '_' && board[0][1] == '_'
    && (notInCheck || !getWhiteIsInCheck(kingRow, kingColumn))
    && !getWhiteIsInCheck(0, 3) && !getWhiteIsInCheck(0, 2))
    {
        legalMoves.emplace_back(0, 4, 0, 2, '\0');
    }
    //Loops through every square on the board, and if there is a piece there appends its legal moves to the end of
    //the list of legal moves
    for (int row = 0; row < 8; row++)
        for (int column = 0; column < 8; column++)
        {
            switch (board[row][column])
            {
                case 'P':
                    move.fromRow = row;
                    move.fromColumn = column;
                    //Checks for forward pawn moves
                    move.toRow = row + 1;
                    if(board[move.toRow][column] == '_')
                    {
                        move.toColumn = column;
                        if (!movesIntoCheck(move, kingRow, kingColumn))
                        {
                            if (row != 6)
                                legalMoves.push_back(move);
                            else
                            {
                                for (char promotion: possiblePromotions)
                                {
                                    move.promotion = promotion;
                                    legalMoves.push_back(move);
                                }
                                move.promotion = '\0';
                            }
                        }

                        //Checks for double steep move if pawn is on starting square
                        move.toRow = row + 2;
                        if (row == 1 && board[move.toRow][column] == '_' && !movesIntoCheck(move, kingRow, kingColumn))
                            legalMoves.push_back(move);

                    }
                    //Checks for possible captures
                    move.toRow = row + 1;
                    move.toColumn = column - 1;
                    if (move.toColumn > -1 && (islower(board[move.toRow][move.toColumn])
                        || (move.toRow == enPassantRow && move.toColumn == enPassantColumn))
                        && !movesIntoCheck(move, kingRow, kingColumn))
                    {
                        if (row != 6)
                            legalMoves.push_back(move);
                        else
                        {
                            for (char promotion: possiblePromotions)
                            {
                                move.promotion = promotion;
                                legalMoves.push_back(move);
                            }
                            move.promotion = '\0';
                        }
                    }

                    move.toColumn = column + 1;
                    if (move.toColumn < 8 && (islower(board[move.toRow][move.toColumn])
                    || (move.toRow == enPassantRow && move.toColumn == enPassantColumn))
                    && !movesIntoCheck(move, kingRow, kingColumn))
                    {
                        if (row != 6)
                            legalMoves.push_back(move);
                        else
                        {
                            for (char promotion: possiblePromotions)
                            {
                                move.promotion = promotion;
                                legalMoves.push_back(move);
                            }
                            move.promotion = '\0';
                        }
                    }

                    break;
                case 'N':
                    move.fromRow = row;
                    move.fromColumn = column;
                    //Checks knight moves in a clockwise directions
                    move.toRow = row + 2;
                    if (move.toRow < 8)
                    {
                        move.toColumn = column - 1;
                        if (move.toColumn > -1 && !isupper(board[move.toRow][move.toColumn]) && !movesIntoCheck(move, kingRow, kingColumn))
                            legalMoves.push_back(move);

                        move.toColumn = column + 1;
                        if (move.toColumn < 8 && !isupper(board[move.toRow][move.toColumn]) && !movesIntoCheck(move, kingRow, kingColumn))
                            legalMoves.push_back(move);
                    }

                    move.toColumn = column + 2;
                    if (move.toColumn < 8)
                    {
                        move.toRow = row + 1;
                        if (move.toRow < 8 && !isupper(board[move.toRow][move.toColumn]) && !movesIntoCheck(move, kingRow, kingColumn))
                            legalMoves.push_back(move);

                        move.toRow = row - 1;
                        if (move.toRow > -1 && !isupper(board[move.toRow][move.toColumn]) && !movesIntoCheck(move, kingRow, kingColumn))
                            legalMoves.push_back(move);
                    }

                    move.toRow = row - 2;
                    if (move.toRow > -1)
                    {
                        move.toColumn = column + 1;
                        if (move.toColumn < 8 && !isupper(board[move.toRow][move.toColumn]) && !movesIntoCheck(move, kingRow, kingColumn))
                            legalMoves.push_back(move);

                        move.toColumn = column - 1;
                        if (move.toColumn > -1 && !isupper(board[move.toRow][move.toColumn]) && !movesIntoCheck(move, kingRow, kingColumn))
                            legalMoves.push_back(move);
                    }

                    move.toColumn = column - 2;
                    if (move.toColumn > -1)
                    {
                        move.toRow = row - 1;
                        if (move.toRow > -1 && !isupper(board[move.toRow][move.toColumn]) && !movesIntoCheck(move, kingRow, kingColumn))
                            legalMoves.push_back(move);

                        move.toRow = row + 1;
                        if (move.toRow < 8 && !isupper(board[move.toRow][move.toColumn]) && !movesIntoCheck(move, kingRow, kingColumn))
                            legalMoves.push_back(move);
                    }
                    break;
                case 'K':
                    move.fromRow = row;
                    move.fromColumn = column;
                    //Checks king moves in a clockwise directions
                    move.toRow = row + 1;
                    move.toColumn = column + 1;
                    if (move.toRow < 8 && move.toColumn < 8 && !isupper(board[move.toRow][move.toColumn]) && !movesIntoCheck(move, move.toRow, move.toColumn))
                        legalMoves.push_back(move);

                    if (move.toColumn < 8 && !isupper(board[row][move.toColumn]))
                    {
                        move.toRow = row;
                        if(!movesIntoCheck(move, move.toRow, move.toColumn))
                            legalMoves.push_back(move);
                    }

                    move.toRow = row - 1;
                    if (move.toRow > -1 && move.toColumn < 8 && !isupper(board[move.toRow][move.toColumn]) && !movesIntoCheck(move, move.toRow, move.toColumn))
                        legalMoves.push_back(move);

                    if (move.toRow > -1 && !isupper(board[move.toRow][column]))
                    {
                        move.toColumn = column;
                        if (!movesIntoCheck(move, move.toRow, move.toColumn))
                            legalMoves.push_back(move);
                    }

                    move.toColumn = column - 1;
                    if (move.toRow > -1 && move.toColumn > -1 && !isupper(board[move.toRow][move.toColumn]) && !movesIntoCheck(move, move.toRow, move.toColumn))
                        legalMoves.push_back(move);

                    if (move.toColumn > -1 && !isupper(board[row][move.toColumn]))
                    {
                        move.toRow = row;
                        if(!movesIntoCheck(move, move.toRow, move.toColumn))
                            legalMoves.push_back(move);
                    }

                    move.toRow = row + 1;
                    if (move.toRow  < 8 && move.toColumn > -1 && !isupper(board[move.toRow ][move.toColumn]) && !movesIntoCheck(move, move.toRow, move.toColumn))
                        legalMoves.push_back(move);


                    if (move.toRow < 8 && !isupper(board[move.toRow][column]))
                    {
                        move.toColumn = column;
                        if (!movesIntoCheck(move, move.toRow, move.toColumn))
                            legalMoves.push_back(move);
                    }
                    break;
                    //Queen moves are either rook moves are bishop moves,
                    //so it can fall through the bishop and rook cases
                case 'Q':
                case 'R':
                    move.fromRow = row;
                    move.fromColumn = column;

                    canKeepMoving = true;
                    move.toColumn = column;
                    move.toRow = row;
                    while(canKeepMoving)
                    {
                        move.toRow++;
                        if(move.toRow < 8)
                        {
                            //If the square being moved to is empty we can move there and keep going
                            if (board[move.toRow][move.toColumn] == '_')
                            {
                                if (!movesIntoCheck(move, kingRow, kingColumn))
                                    legalMoves.push_back(move);
                                //If there is a black piece on the square we can move there, but then we must stop
                            }
                            else if (islower(board[move.toRow][move.toColumn]))
                            {
                                if (!movesIntoCheck(move, kingRow, kingColumn))
                                    legalMoves.push_back(move);
                                canKeepMoving = false;
                            }
                                //Otherwise there must be white piece on the square, so we can not move there and must stop
                            else
                                canKeepMoving = false;
                        }
                        else
                            canKeepMoving = false;
                    }

                    canKeepMoving = true;
                    move.toColumn = column;
                    move.toRow = row;
                    while(canKeepMoving)
                    {
                        move.toColumn++;
                        if(move.toColumn < 8)
                        {
                            //If the square being moved to is empty we can move there and keep going
                            if(board[move.toRow][move.toColumn] == '_')
                            {
                                if (!movesIntoCheck(move, kingRow, kingColumn))
                                    legalMoves.push_back(move);
                            }
                            //If there is a black piece on the square we can move there, but then we must stop
                            else if (islower(board[move.toRow][move.toColumn]))
                            {
                                if (!movesIntoCheck(move, kingRow, kingColumn))
                                    legalMoves.push_back(move);
                                canKeepMoving = false;
                            }
                            //Otherwise there must be white piece on the square, so we can not move there and must stop
                            else
                                canKeepMoving = false;
                        }
                        else canKeepMoving = false;
                    }

                    canKeepMoving = true;
                    move.toColumn = column;
                    move.toRow = row;
                    while(canKeepMoving)
                    {
                        move.toRow--;
                        if(move.toRow > -1) {
                            //If the square being moved to is empty we can move there and keep going
                            if (board[move.toRow][move.toColumn] == '_')
                            {
                                if (!movesIntoCheck(move, kingRow, kingColumn))
                                    legalMoves.push_back(move);
                            }
                            //If there is a black piece on the square we can move there, but then we must stop
                            else if (islower(board[move.toRow][move.toColumn]))
                            {
                                if (!movesIntoCheck(move, kingRow, kingColumn))
                                    legalMoves.push_back(move);
                                canKeepMoving = false;
                            }
                            //Otherwise there must be white piece on the square, so we can not move there and must stop
                            else
                                canKeepMoving = false;
                        }
                        else
                            canKeepMoving = false;
                    }

                    canKeepMoving = true;
                    move.toColumn = column;
                    move.toRow = row;
                    while(canKeepMoving)
                    {
                        move.toColumn--;
                        if(move.toColumn > -1)
                        {
                            //If the square being moved to is empty we can move there and keep going
                            if(board[move.toRow][move.toColumn] == '_')
                            {
                                if (!movesIntoCheck(move, kingRow, kingColumn))
                                    legalMoves.push_back(move);
                            }
                            //If there is a black piece on the square we can move there, but then we must stop
                            else if (islower(board[move.toRow][move.toColumn]))
                            {
                                if (!movesIntoCheck(move, kingRow, kingColumn))
                                    legalMoves.push_back(move);
                                canKeepMoving = false;
                            }
                            //Otherwise there must be white piece on the square, so we can not move there and must stop
                            else
                                canKeepMoving = false;
                        }
                        else canKeepMoving = false;
                    }
                    //We don't brake if the piece is a queen, so it can fall through to the bishop moves
                    if (board[row][column] == 'R')
                        break;

                case 'B':
                    //These values are already assigned for the queen, and don't need to be reassigned
                    if (board[row][column] != 'Q')
                    {
                        move.fromRow = row;
                        move.fromColumn = column;
                    }
                    canKeepMoving = true;
                    move.toColumn = column;
                    move.toRow = row;
                    while(canKeepMoving)
                    {
                        move.toRow++;
                        move.toColumn++;
                        if(move.toRow < 8 && move.toColumn < 8)
                        {
                            //If the square being moved to is empty we can move there and keep going
                            if(board[move.toRow][move.toColumn] == '_')
                            {
                                if (!movesIntoCheck(move, kingRow, kingColumn))
                                    legalMoves.push_back(move);
                            }
                            //If there is a black piece on the square we can move there, but then we must stop
                            else if (islower(board[move.toRow][move.toColumn]))
                            {
                                if (!movesIntoCheck(move, kingRow, kingColumn))
                                    legalMoves.push_back(move);
                                canKeepMoving = false;
                            }
                            //Otherwise there must be white piece on the square, so we can not move there and must stop
                            else
                                canKeepMoving = false;
                        }
                        else
                            canKeepMoving = false;
                    }

                    canKeepMoving = true;
                    move.toColumn = column;
                    move.toRow = row;
                    while(canKeepMoving)
                    {
                        move.toRow--;
                        move.toColumn++;
                        if(move.toRow > -1 && move.toColumn < 8)
                        {
                            //If the square being moved to is empty we can move there and keep going
                            if(board[move.toRow][move.toColumn] == '_')
                            {
                                if (!movesIntoCheck(move, kingRow, kingColumn))
                                    legalMoves.push_back(move);
                            }
                            //If there is a black piece on the square we can move there, but then we must stop
                            else if (islower(board[move.toRow][move.toColumn]))
                            {
                                if (!movesIntoCheck(move, kingRow, kingColumn))
                                    legalMoves.push_back(move);
                                canKeepMoving = false;
                            }
                            //Otherwise there must be white piece on the square, so we can not move there and must stop
                            else
                                canKeepMoving = false;
                        }
                        else
                            canKeepMoving = false;
                    }

                    canKeepMoving = true;
                    move.toColumn = column;
                    move.toRow = row;
                    while(canKeepMoving)
                    {
                        move.toRow--;
                        move.toColumn--;
                        if(move.toRow > -1 && move.toColumn > -1)
                        {
                            //If the square being moved to is empty we can move there and keep going
                            if(board[move.toRow][move.toColumn] == '_')
                            {
                                if (!movesIntoCheck(move, kingRow, kingColumn))
                                    legalMoves.push_back(move);
                            }
                            //If there is a black piece on the square we can move there, but then we must stop
                            else if (islower(board[move.toRow][move.toColumn]))
                            {
                                if (!movesIntoCheck(move, kingRow, kingColumn))
                                    legalMoves.push_back(move);
                                canKeepMoving = false;
                            }
                            //Otherwise there must be white piece on the square, so we can not move there and must stop
                            else
                                canKeepMoving = false;
                        }
                        else
                            canKeepMoving = false;
                    }

                    canKeepMoving = true;
                    move.toColumn = column;
                    move.toRow = row;
                    while(canKeepMoving)
                    {
                        move.toRow++;
                        move.toColumn--;
                        if(move.toRow < 8 && move.toColumn > -1)
                        {
                            //If the square being moved to is empty we can move there and keep going
                            if(board[move.toRow][move.toColumn] == '_')
                            {
                                if (!movesIntoCheck(move, kingRow, kingColumn))
                                    legalMoves.push_back(move);
                            }
                            //If there is a black piece on the square we can move there, but then we must stop
                            else if (islower(board[move.toRow][move.toColumn]))
                            {
                                if (!movesIntoCheck(move, kingRow, kingColumn))
                                    legalMoves.push_back(move);
                                canKeepMoving = false;
                            }
                            //Otherwise there must be white piece on the square, so we can not move there and must stop
                            else
                                canKeepMoving = false;
                        }
                        else
                            canKeepMoving = false;
                    }
                    break;
                default:
                    break;
            }
        }
}
//Populates a list of legal moves for black.
void GameState::setBlackLegalMoves()
{
    int kingRow;
    int kingColumn;
    moveStruct move;
    //Used for bishop, rook and queen moves to determine if the piece can keep moving in that directions
    bool canKeepMoving;
    bool notInCheck = false;
    char possiblePromotions[] = {'q', 'r', 'n', 'b'};

    //Locates the black king
    bool foundKing = false;
    for (int row = 7; row > -1 && !foundKing; row--)
        for (int column = 7; column > -1 && !foundKing; column--)
            if (board[row][column] == 'k')
            {
                kingRow = row;
                kingColumn = column;
                foundKing = true;
            }

    move.promotion = '\0';

    //Appends castling moves to the list of legal moves
    //King side
    if (blackKingSideCastlePrivilege && board[7][5] == '_' && board[7][6] == '_' && board[7][7] == 'r' &&
        !getBlackIsInCheck(kingRow, kingColumn))
    {
        notInCheck = true;
        if (!getBlackIsInCheck(7, 5) && !getBlackIsInCheck(7, 6))
        {
            legalMoves.emplace_back(7, 4, 7, 6, '\0');
        }
    }
    //Queen side
    if (blackQueenSideCastlePrivilege && board[7][3] == '_' && board[7][2] == '_' && board[7][1] == '_' &&
        board[7][0] == 'r'
        && (notInCheck || !getBlackIsInCheck(kingRow, kingColumn))
        && !getBlackIsInCheck(7, 3) && !getBlackIsInCheck(7, 2))
    {
        legalMoves.emplace_back(7, 4, 7, 2, '\0');
    }
    //Loops through every square on the board, and if there is a piece there appends its legal moves to the end of
    //the list of legal moves
    for (int row = 0; row < 8; row++)
        for (int column = 0; column < 8; column++)
        {
            switch (board[row][column])
            {
                case 'p':
                    move.fromRow = row;
                    move.fromColumn = column;
                    //Checks for forward pawn moves
                    move.toRow = row - 1;
                    if(board[move.toRow][column] == '_')
                    {
                        move.toColumn = column;
                        if (!movesIntoCheck(move, kingRow, kingColumn))
                        {
                            if (row != 1)
                                legalMoves.push_back(move);
                            else
                            {
                                for (char promotion: possiblePromotions)
                                {
                                    move.promotion = promotion;
                                    legalMoves.push_back(move);
                                }
                                move.promotion = '\0';
                            }
                        }
                        //Checks for double steep move if pawn is on starting square
                        move.toRow = row - 2;
                        if (row == 6 && board[move.toRow][column] == '_' && !movesIntoCheck(move, kingRow, kingColumn))
                            legalMoves.push_back(move);
                    }
                    //Checks for possible captures
                    move.toRow = row - 1;
                    move.toColumn = column - 1;
                    if (move.toColumn > -1 && (isupper(board[move.toRow][move.toColumn])
                       || (move.toRow == enPassantRow && move.toColumn == enPassantColumn))
                       && !movesIntoCheck(move, kingRow, kingColumn))
                    {
                        if (row != 1)
                            legalMoves.push_back(move);
                        else
                        {
                            for (char promotion: possiblePromotions)
                            {
                                move.promotion = promotion;
                                legalMoves.push_back(move);
                            }
                            move.promotion = '\0';
                        }
                    }

                    move.toColumn = column + 1;
                    if (move.toColumn < 8 && (isupper(board[move.toRow][move.toColumn])
                        || (move.toRow == enPassantRow && move.toColumn == enPassantColumn))
                        && !movesIntoCheck(move, kingRow, kingColumn))
                    {
                        if (row != 1)
                            legalMoves.push_back(move);
                        else
                        {
                            for (char promotion: possiblePromotions)
                            {
                                move.promotion = promotion;
                                legalMoves.push_back(move);
                            }
                            move.promotion = '\0';
                        }
                    }

                    break;
                case 'n':
                    move.fromRow = row;
                    move.fromColumn = column;
                    //Checks knight moves in a clockwise directions
                    move.toRow = row + 2;
                    if (move.toRow < 8)
                    {
                        move.toColumn = column - 1;
                        if (move.toColumn > -1 && !islower(board[move.toRow][move.toColumn]) && !movesIntoCheck(move, kingRow, kingColumn))
                            legalMoves.push_back(move);

                        move.toColumn = column + 1;
                        if (move.toColumn < 8 && !islower(board[move.toRow][move.toColumn]) && !movesIntoCheck(move, kingRow, kingColumn))
                            legalMoves.push_back(move);
                    }

                    move.toColumn = column + 2;
                    if (move.toColumn < 8)
                    {
                        move.toRow = row + 1;
                        if (move.toRow < 8 && !islower(board[move.toRow][move.toColumn]) && !movesIntoCheck(move, kingRow, kingColumn))
                            legalMoves.push_back(move);

                        move.toRow = row - 1;
                        if (move.toRow > -1 && !islower(board[move.toRow][move.toColumn]) && !movesIntoCheck(move, kingRow, kingColumn))
                            legalMoves.push_back(move);
                    }

                    move.toRow = row - 2;
                    if (move.toRow > -1)
                    {
                        move.toColumn = column + 1;
                        if (move.toColumn < 8 && !islower(board[move.toRow][move.toColumn]) && !movesIntoCheck(move, kingRow, kingColumn))
                            legalMoves.push_back(move);

                        move.toColumn = column - 1;
                        if (move.toColumn > -1 && !islower(board[move.toRow][move.toColumn]) && !movesIntoCheck(move, kingRow, kingColumn))
                            legalMoves.push_back(move);
                    }

                    move.toColumn = column - 2;
                    if (move.toColumn > -1)
                    {
                        move.toRow = row - 1;
                        if (move.toRow > -1 && !islower(board[move.toRow][move.toColumn]) && !movesIntoCheck(move, kingRow, kingColumn))
                            legalMoves.push_back(move);

                        move.toRow = row + 1;
                        if (move.toRow < 8 && !islower(board[move.toRow][move.toColumn]) && !movesIntoCheck(move, kingRow, kingColumn))
                            legalMoves.push_back(move);
                    }
                    break;
                case 'k':
                    move.fromRow = row;
                    move.fromColumn = column;
                    //Checks king moves in a clockwise directions
                    move.toRow = row + 1;
                    move.toColumn = column + 1;
					if (move.toRow < 8 && move.toColumn < 8 && !islower(board[move.toRow][move.toColumn]) && !movesIntoCheck(move, move.toRow, move.toColumn))
                        legalMoves.push_back(move);

                    if (move.toColumn < 8 && !islower(board[row][move.toColumn]))
                    {
                        move.toRow = row;
                        if (!movesIntoCheck(move, move.toRow, move.toColumn))
                            legalMoves.push_back(move);
                    }

                    move.toRow = row - 1;
                    if (move.toRow > -1 && move.toColumn < 8 && !islower(board[move.toRow][move.toColumn]) && !movesIntoCheck(move, move.toRow, move.toColumn))
                        legalMoves.push_back(move);

                    if (move.toRow > -1 && !islower(board[move.toRow][column]))
                    {
                        move.toColumn = column;
                        if (!movesIntoCheck(move, move.toRow, move.toColumn))
                            legalMoves.push_back(move);
                    }

                    move.toColumn = column - 1;
                    if (move.toRow > -1 && move.toColumn > -1 && !islower(board[move.toRow][move.toColumn]) && !movesIntoCheck(move, move.toRow, move.toColumn))
                        legalMoves.push_back(move);

                    if (move.toColumn > -1 && !islower(board[row][move.toColumn]))
                    {
                        move.toRow = row;
                        if (!movesIntoCheck(move, move.toRow, move.toColumn))
                            legalMoves.push_back(move);
                    }

                    move.toRow = row + 1;
                    if (move.toRow  < 8 && move.toColumn > -1 && !islower(board[move.toRow ][move.toColumn]) && !movesIntoCheck(move, move.toRow, move.toColumn))
                        legalMoves.push_back(move);

                    if (move.toRow < 8 && !islower(board[move.toRow][column]))
                    {
                        move.toColumn = column;
                        if (!movesIntoCheck(move, move.toRow, move.toColumn))
                            legalMoves.push_back(move);
                    }
                    break;
                    //Queen moves are either rook moves are bishop moves,
                    //so it can fall through the bishop and rook cases
                case 'q':
                case 'r':
                    move.fromRow = row;
                    move.fromColumn = column;

                    canKeepMoving = true;
                    move.toColumn = column;
                    move.toRow = row;
                    while(canKeepMoving)
                    {
                        move.toRow++;
                        if(move.toRow < 8)
                        {
                            //If the square being moved to is empty we can move there and keep going
                            if(board[move.toRow][move.toColumn] == '_')
                            {
                                if (!movesIntoCheck(move, kingRow, kingColumn))
                                    legalMoves.push_back(move);
                            }
                                //If there is a black piece on the square we can move there, but then we must stop
                            else if (isupper(board[move.toRow][move.toColumn]))
                            {
                                if(!movesIntoCheck(move, kingRow, kingColumn))
                                    legalMoves.push_back(move);
                                canKeepMoving = false;
                            }
                                //Otherwise there must be white piece on the square, so we can not move there and must stop
                            else
                                canKeepMoving = false;
                        }
                        else
                            canKeepMoving = false;
                    }

                    canKeepMoving = true;
                    move.toColumn = column;
                    move.toRow = row;
                    while(canKeepMoving)
                    {
                        move.toColumn++;
                        if(move.toColumn < 8)
                        {
                            //If the square being moved to is empty we can move there and keep going
                            if(board[move.toRow][move.toColumn] == '_')
                            {
                                if (!movesIntoCheck(move, kingRow, kingColumn))
                                    legalMoves.push_back(move);
                            }
                                //If there is a black piece on the square we can move there, but then we must stop
                            else if (isupper(board[move.toRow][move.toColumn]))
                            {
                                if(!movesIntoCheck(move, kingRow, kingColumn))
                                    legalMoves.push_back(move);
                                canKeepMoving = false;
                            }
                                //Otherwise there must be white piece on the square, so we can not move there and must stop
                            else
                                canKeepMoving = false;
                        }
                        else canKeepMoving = false;
                    }

                    canKeepMoving = true;
                    move.toColumn = column;
                    move.toRow = row;
                    while(canKeepMoving)
                    {
                        move.toRow--;
                        if(move.toRow > -1) {
                            //If the square being moved to is empty we can move there and keep going
                            if (board[move.toRow][move.toColumn] == '_')
                            {
                                if (!movesIntoCheck(move, kingRow, kingColumn))
                                    legalMoves.push_back(move);
                            }
                            //If there is a black piece on the square we can move there, but then we must stop
                            else if (isupper(board[move.toRow][move.toColumn]))
                            {
                                if(!movesIntoCheck(move, kingRow, kingColumn))
                                    legalMoves.push_back(move);
                                canKeepMoving = false;
                            }
                            //Otherwise there must be white piece on the square, so we can not move there and must stop
                            else
                                canKeepMoving = false;
                        }
                        else
                            canKeepMoving = false;
                    }

                    canKeepMoving = true;
                    move.toColumn = column;
                    move.toRow = row;
                    while(canKeepMoving)
                    {
                        move.toColumn--;
                        if(move.toColumn > -1)
                        {
                            //If the square being moved to is empty we can move there and keep going
                            if(board[move.toRow][move.toColumn] == '_')
                            {
                                if (!movesIntoCheck(move, kingRow, kingColumn))
                                    legalMoves.push_back(move);
                            }
                            //If there is a black piece on the square we can move there, but then we must stop
                            else if (isupper(board[move.toRow][move.toColumn]))
                            {
                                if(!movesIntoCheck(move, kingRow, kingColumn))
                                    legalMoves.push_back(move);
                                canKeepMoving = false;
                            }
                            //Otherwise there must be white piece on the square, so we can not move there and must stop
                            else
                                canKeepMoving = false;
                        }
                        else canKeepMoving = false;
                    }
                    //We don't brake if the piece is a queen, so it can fall through to the bishop moves
                    if (board[row][column] == 'r')
                        break;

                case 'b':
                    //These values are already assigned for the queen, and don't need to be reassigned
                    if (board[row][column] != 'q')
                    {
                        move.fromRow = row;
                        move.fromColumn = column;
                    }
                    canKeepMoving = true;
                    move.toColumn = column;
                    move.toRow = row;
                    while(canKeepMoving)
                    {
                        move.toRow++;
                        move.toColumn++;
                        if(move.toRow < 8 && move.toColumn < 8)
                        {
                            //If the square being moved to is empty we can move there and keep going
                            if(board[move.toRow][move.toColumn] == '_')
                            {
                                if (!movesIntoCheck(move, kingRow, kingColumn))
                                    legalMoves.push_back(move);
                            }
                            //If there is a black piece on the square we can move there, but then we must stop
                            else if (isupper(board[move.toRow][move.toColumn]))
                            {
                                if(!movesIntoCheck(move, kingRow, kingColumn))
                                    legalMoves.push_back(move);
                                canKeepMoving = false;
                            }
                            //Otherwise there must be white piece on the square, so we can not move there and must stop
                            else
                                canKeepMoving = false;
                        }
                        else
                            canKeepMoving = false;
                    }

                    canKeepMoving = true;
                    move.toColumn = column;
                    move.toRow = row;
                    while(canKeepMoving)
                    {
                        move.toRow--;
                        move.toColumn++;
                        if(move.toRow > -1 && move.toColumn < 8)
                        {
                            //If the square being moved to is empty we can move there and keep going
                            if(board[move.toRow][move.toColumn] == '_')
                            {
                                if (!movesIntoCheck(move, kingRow, kingColumn))
                                    legalMoves.push_back(move);
                            }
                            //If there is a black piece on the square we can move there, but then we must stop
                            else if (isupper(board[move.toRow][move.toColumn]))
                            {
                                if(!movesIntoCheck(move, kingRow, kingColumn))
                                    legalMoves.push_back(move);
                                canKeepMoving = false;
                            }
                            //Otherwise there must be white piece on the square, so we can not move there and must stop
                            else
                                canKeepMoving = false;
                        }
                        else
                            canKeepMoving = false;
                    }

                    canKeepMoving = true;
                    move.toColumn = column;
                    move.toRow = row;
                    while(canKeepMoving)
                    {
                        move.toRow--;
                        move.toColumn--;
                        if(move.toRow > -1 && move.toColumn > -1)
                        {
                            //If the square being moved to is empty we can move there and keep going
                            if(board[move.toRow][move.toColumn] == '_')
                            {
                                if (!movesIntoCheck(move, kingRow, kingColumn))
                                    legalMoves.push_back(move);
                            }
                            //If there is a black piece on the square we can move there, but then we must stop
                            else if (isupper(board[move.toRow][move.toColumn]))
                            {
                                if(!movesIntoCheck(move, kingRow, kingColumn))
                                    legalMoves.push_back(move);
                                canKeepMoving = false;
                            }
                            //Otherwise there must be white piece on the square, so we can not move there and must stop
                            else
                                canKeepMoving = false;
                        }
                        else
                            canKeepMoving = false;
                    }

                    canKeepMoving = true;
                    move.toColumn = column;
                    move.toRow = row;
                    while(canKeepMoving)
                    {
                        move.toRow++;
                        move.toColumn--;
                        if(move.toRow < 8 && move.toColumn > -1)
                        {
                            //If the square being moved to is empty we can move there and keep going
                            if(board[move.toRow][move.toColumn] == '_')
                            {
                                if (!movesIntoCheck(move, kingRow, kingColumn))
                                    legalMoves.push_back(move);
                            }
                            //If there is a black piece on the square we can move there, but then we must stop
                            else if (isupper(board[move.toRow][move.toColumn]))
                            {
                                if(!movesIntoCheck(move, kingRow, kingColumn))
                                    legalMoves.push_back(move);
                                canKeepMoving = false;
                            }
                            //Otherwise there must be white piece on the square, so we can not move there and must stop
                            else
                                canKeepMoving = false;
                        }
                        else
                            canKeepMoving = false;
                    }
                    break;
                default:
                    break;
            }
        }
}

void GameState::resign()
{
    if (whiteToMove)
        blackHasWon = true;
    else
        whiteHasWon = true;
}

void GameState::declareDraw()
{
    gameIsDraw = true;
}

bool GameState::operator==(const GameState &rightHandSide) const
{
    bool areEqual = this->whiteToMove == rightHandSide.whiteToMove &&
                    this->whiteKingSideCastlePrivilege == rightHandSide.whiteKingSideCastlePrivilege &&
                    this->whiteQueenSideCastlePrivilege == rightHandSide.whiteQueenSideCastlePrivilege &&
                    this->blackKingSideCastlePrivilege == rightHandSide.blackKingSideCastlePrivilege &&
                    this->blackQueenSideCastlePrivilege == rightHandSide.blackQueenSideCastlePrivilege &&
                    this->enPassantRow == rightHandSide.enPassantRow &&
                    this->enPassantColumn == rightHandSide.enPassantColumn;

    for (int row = 0; row < 8 && areEqual; row++)
        for (int column = 0; column < 8 && areEqual; column++)
            areEqual = this->board[row][column] == rightHandSide.board[row][column];

    return areEqual;
}