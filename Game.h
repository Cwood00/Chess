//
// Created by Chris on 1/10/2023.
//

#ifndef CHESS_GAME_H
#define CHESS_GAME_H

#include <iostream>
#include <string>
#include <list>
#include "GameState.h"

class Game
{
private:
    list<GameState> previousGameStates;
    GameState *currentGameState;

    bool checkForDrawByRepetition() const;
public:

    Game();
    void displayBoardWhitePOV() const;
    void displayBoardBlackPOV() const;
    bool makeMove(const moveStruct &move, bool validMove = false);
    bool getWhiteToMove() const;
    bool getWhiteHasWon() const;
    bool getBlackHasWon() const;
    bool getGameIsDraw () const;
    bool getWhiteKingSideCastlePrivilege() const;
    bool getWhiteQueenSideCastlePrivilege() const;
    bool getBlackKingSideCastlePrivilege() const;
    bool getBlackQueenSideCastlePrivilege() const;
    void resign();
    void declareDraw();
    bool hasGameEnded() const;
    void undoMove();
    int getGameLength();

    friend int recursiveScoreMove(const moveStruct &move, Game currentGame, int currentDepth);
    friend void makeComputerMove(Game &currentGame);
    friend bool computerAcceptDraw(Game &currentGame);
};


#endif //CHESS_GAME_H
