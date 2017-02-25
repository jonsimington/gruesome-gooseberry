//////////////////////////////////////////////////////////////////////
/// @file attack_piece.h
/// @author David Crow, CS5400 D
/// @brief This file contains the class declaration for piece attacks
//////////////////////////////////////////////////////////////////////

#pragma once

#include "constants.h"
#include <iostream>
#include <bitset>
#include <string>
using namespace std;

// marks every square that a certain piece on a certain square can attack
class AttackPiece
{
// public because we only need one static AttackPiece
public:
  // 64 possible locations for each piece
  bitset<BOARD_SIZE> attacking_king[BOARD_SIZE];
  bitset<BOARD_SIZE> attacking_queen[BOARD_SIZE];
  bitset<BOARD_SIZE> attacking_rook[BOARD_SIZE];
  bitset<BOARD_SIZE> attacking_bishop[BOARD_SIZE];
  bitset<BOARD_SIZE> attacking_knight[BOARD_SIZE];
  bitset<BOARD_SIZE> attacking_b_pawn[BOARD_SIZE];
  bitset<BOARD_SIZE> attacking_w_pawn[BOARD_SIZE];
  bitset<BOARD_SIZE> moving_b_pawn[BOARD_SIZE];
  bitset<BOARD_SIZE> moving_w_pawn[BOARD_SIZE];

  // constructor
  AttackPiece();

  // attack-generating functions
  void generateAttacks();
  void kingAttacks();
  void queenAttacks();
  void rookAttacks();
  void bishopAttacks();
  void knightAttacks();
  void pawnAttacksMoves();
};
