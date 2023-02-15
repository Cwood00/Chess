//
// Created by Chris on 1/10/2023.
//

#include "Game.h"

Game::Game()
{
    previousGameStates.emplace_front();
    currentGameState = &previousGameStates.back();
}

void Game::displayBoardWhitePOV() const
{
    currentGameState->displayBoardWhitePOV();
}

void Game::displayBoardBlackPOV() const
{
    currentGameState->displayBoardBlackPOV();
}

//Validates a move, and attempts to make it. Returns true if the move is valid and false otherwise.
//Also updates other relevant game data.
bool Game::makeMove(const moveStruct &move, bool validMove)
{
    char movedPiece = currentGameState->board[move.fromRow][move.fromColumn];
    char movedOnto = currentGameState->board[move.toRow][move.toColumn];

    //Checks to see if the move is in the list of legal moves
    for (auto iter = currentGameState->legalMoves.begin(); iter != currentGameState->legalMoves.end() && !validMove; iter++)
        validMove = move.toColumn == iter->toColumn &&
                    move.toRow == iter->toRow &&
                    move.fromColumn == iter->fromColumn &&
                    move.fromRow == iter->fromRow &&
                    move.promotion == iter->promotion;

    if (validMove)
    {
        //Add a copy of the game state to the list of previous game states
        previousGameStates.emplace_back(*currentGameState);
        currentGameState = &previousGameStates.back();
        //Make the move
        currentGameState->movePiece(move);
        //Update castling privileges
        if (movedPiece == 'K')
        {
            currentGameState->whiteKingSideCastlePrivilege = false;
            currentGameState->whiteQueenSideCastlePrivilege = false;
        }
        else if (movedPiece == 'k')
        {
            currentGameState->blackKingSideCastlePrivilege = false;
            currentGameState->blackQueenSideCastlePrivilege = false;
        }
        if (currentGameState->board[0][0] != 'R')
            currentGameState->whiteQueenSideCastlePrivilege = false;
        if (currentGameState->board[0][7] != 'R')
            currentGameState->whiteKingSideCastlePrivilege = false;
        if (currentGameState->board[7][0] != 'r')
            currentGameState->blackQueenSideCastlePrivilege = false;
        if (currentGameState->board[7][7] != 'r')
            currentGameState->blackKingSideCastlePrivilege = false;

        //Update en Passant Square
        if (movedPiece == 'P' && move.toRow == 3 && move.fromRow == 1)
        {
            currentGameState->enPassantRow = 2;
            currentGameState->enPassantColumn = move.toColumn;
        }
        else if (movedPiece == 'p' && move.toRow == 4 && move.fromRow == 6)
        {
            currentGameState->enPassantRow = 5;
            currentGameState->enPassantColumn = move.toColumn;
        }
        else
        {
            currentGameState->enPassantRow = -1;
            currentGameState->enPassantColumn = -1;
        }

        //Change whose move it is
        currentGameState->whiteToMove = !currentGameState->whiteToMove;
        //Updates the list of legal moves
        if (currentGameState->whiteToMove)
            currentGameState->setWhiteLegalMoves();
        else
            currentGameState->setBlackLegalMoves();

        //Check for checkmate or stalemate
        if (currentGameState->legalMoves.empty())
        {
            if (currentGameState->whiteToMove && currentGameState->getWhiteIsInCheck())
                currentGameState->blackHasWon = true;
            else if (!currentGameState->whiteToMove && currentGameState->getBlackIsInCheck())
                currentGameState->whiteHasWon = true;
            else
                currentGameState->gameIsDraw = true;
        }
            //Update fifty move rule and check for insufficient material if needed
        else
        {
            if (movedOnto != '_')
            {
                currentGameState->movesSinceLastCaptureOrPawnMove = 0;
                currentGameState->gameIsDraw = currentGameState->checkForInsufficientMaterial();
            }
            else if (movedPiece == 'p' || movedPiece == 'P')
            {
                currentGameState->movesSinceLastCaptureOrPawnMove = 0;
                if (move.promotion == 'N' || move.promotion == 'n' ||
                    move.promotion == 'B' || move.promotion == 'b')
                    currentGameState->gameIsDraw = currentGameState->checkForInsufficientMaterial();
            }
            else
            {
                currentGameState->movesSinceLastCaptureOrPawnMove++;
                //100 is used instead of 50, as the 50 moved rule counts both players each making one moves as a single move
                if (currentGameState->movesSinceLastCaptureOrPawnMove == 100)
                    currentGameState->gameIsDraw = true;
            }
            //Check for draw by repetition
            if (!currentGameState->gameIsDraw && currentGameState->movesSinceLastCaptureOrPawnMove > 7)
                currentGameState->gameIsDraw = checkForDrawByRepetition();
        }
    }
    return validMove;
}

bool Game::getWhiteToMove() const
{
    return currentGameState->whiteToMove;
}

bool Game::getWhiteHasWon() const
{
    return currentGameState->whiteHasWon;
}

bool Game::getBlackHasWon() const
{
    return currentGameState->blackHasWon;
}

bool Game::getGameIsDraw() const
{
    return currentGameState->gameIsDraw;
}

bool Game::getWhiteKingSideCastlePrivilege() const
{
    return currentGameState->whiteKingSideCastlePrivilege;
}

bool Game::getWhiteQueenSideCastlePrivilege() const
{
    return currentGameState->whiteQueenSideCastlePrivilege;
}

bool Game::getBlackKingSideCastlePrivilege() const
{
    return currentGameState->blackKingSideCastlePrivilege;
}

bool Game::getBlackQueenSideCastlePrivilege() const
{
    return currentGameState->blackQueenSideCastlePrivilege;
}

void Game::resign()
{
    currentGameState->resign();
}

void Game::declareDraw()
{
    currentGameState->declareDraw();
}

bool Game::hasGameEnded() const
{
    return currentGameState->whiteHasWon || currentGameState->blackHasWon || currentGameState->gameIsDraw;
}

bool Game::checkForDrawByRepetition() const
{
    int repetitions = 1;
    int movesToCheck = currentGameState->movesSinceLastCaptureOrPawnMove - 1;

    for (auto iter = previousGameStates.rbegin(); movesToCheck > 0; movesToCheck -=2)
    {
        //Ever other move can be skipped, as a different player will have the turn
        iter++;
        iter++;
        if (*currentGameState == *iter)
        {
            repetitions++;
            if (repetitions == 3)
                return true;
        }
    }
    return false;
}

void Game::undoMove()
{
    previousGameStates.pop_back();
    currentGameState = &previousGameStates.back();
}

int Game::getGameLength()
{
    //The move count is based on the number of moves white has mode, so the total number
    //of previous game states is divided by 2
    return (previousGameStates.size() - 1) / 2 + 1;
}