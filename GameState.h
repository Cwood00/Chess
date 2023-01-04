//
// Created by Chris on 12/21/2022.
//

#ifndef CHESS_GAMESTATE_H
#define CHESS_GAMESTATE_H

#include <iostream>
#include <string>
#include <list>
using namespace std;

struct moveStruct
{
    int fromRow;
    int fromColumn;
    int toRow;
    int toColumn;
    char promotion;

    moveStruct()
    {}

    moveStruct(int _fromRow, int _fromColumn, int _toRow, int _toColumn, char _promotion)
    {
        fromRow = _fromRow;
        fromColumn = _fromColumn;
        toRow = _toRow;
        toColumn = _toColumn;
        promotion = _promotion;
    }
};

class GameState{

private:
    //Sets up the board as an 8 by 8 2d array of chars
    char board[8][8] = {
            {'R','N','B','Q','K','B','N','R'},
            {'P','P','P','P','P','P','P','P'},
            {'_','_','_','_','_','_','_','_'},
            {'_','_','_','_','_','_','_','_'},
            {'_','_','_','_','_','_','_','_'},
            {'_','_','_','_','_','_','_','_'},
            {'p','p','p','p','p','p','p','p'},
            {'r','n','b','q','k','b','n','r'}
    };
    bool whiteKingSideCastlePrivilege;
    bool whiteQueenSideCastlePrivilege;
    bool blackKingSideCastlePrivilege;
    bool blackQueenSideCastlePrivilege;
    int enPassantRow;
    int enPassantColumn;
    int movesSinceLastCaptureOrPawnMove;
    list<moveStruct> legalMoves;
    list<GameState> previousGameStates;

    bool movesIntoCheck(const moveStruct &move, int kingRow, int kingColumn);
    void setWhiteLegalMoves();
    void setBlackLegalMoves();
    //Makes a move without verifying if the move is legal, or updating any other game parameters
    void movePiece(const moveStruct &move);
    bool getWhiteIsInCheck(int kingRow, int kingColumn) const;
    bool getBlackIsInCheck(int kingRow, int kingColumn) const;
    bool checkForInsufficientMaterial() const;
    bool checkForDrawByRepetition() const;

public:
    bool whiteToMove;
    bool whiteHasWon;
    bool blackHasWon;
    bool gameIsDraw;

    GameState();
    GameState(const GameState &oldGameState);
    void displayBoardWhitePOV() const;
    void displayBoardBlackPOV() const;
    bool makeMove(const moveStruct &move);
    void resign();
    bool hasGameEnded() const;
    bool getWhiteIsInCheck() const;
    bool getBlackIsInCheck() const;
    bool operator == (const GameState &rightHandSide) const;

};

#endif //CHESS_GAMESTATE_H