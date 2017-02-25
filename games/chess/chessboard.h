//////////////////////////////////////////////////////////////////////
/// @file chessboard.h
/// @author David Crow, CS5400 D
/// @brief This file contains the class declaration for the chessboard
//////////////////////////////////////////////////////////////////////

#pragma once

#include "constants.h"
#include "attack_piece.h"
#include <iostream>
#include <bitset>
#include <string>
using namespace std;

// this class holds the locations of all pieces
class Chessboard // need public and private members (getters/setters)
{
public:
  bitset<BOARD_SIZE> black[NUM_TYPES];
  bitset<BOARD_SIZE> white[NUM_TYPES];

  bitset<BOARD_SIZE> b_pieces;
  bitset<BOARD_SIZE> w_pieces;
  bitset<BOARD_SIZE> all_pieces;

	Chessboard();
	Chessboard(const Chessboard& b);

	void startGame();
  bitset<BOARD_SIZE> getKingMoves(const string c, const int i, const AttackPiece& a);
  bitset<BOARD_SIZE> getQueenMoves(const string c, const int i, const AttackPiece& a);
  bitset<BOARD_SIZE> getRookMoves(const string c, const int i, const AttackPiece& a);
  bitset<BOARD_SIZE> getBishopMoves(const string c, const int i, const AttackPiece& a);
  bitset<BOARD_SIZE> getKnightMoves(const string c, const int i, const AttackPiece& a);
  bitset<BOARD_SIZE> getPawnMoves(const string c, const int i, const AttackPiece& a);
  bool gameOver();
};
