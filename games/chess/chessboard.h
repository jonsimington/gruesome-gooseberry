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
#include <vector>
using namespace std;

// location and type of each piece; used for initializing board
struct BasicPiece
{
  int index;
  string type;
};

// whether king-side and queen-side castles are possible
struct CastleCheck
{
  bool king_moved;
  bool king_rook_moved;
  bool queen_rook_moved;
};

// this class holds the locations of all pieces
class Chessboard
{
public:
  bitset<BOARD_SIZE> black[NUM_TYPES];
  bitset<BOARD_SIZE> white[NUM_TYPES];

  bitset<BOARD_SIZE> b_pieces;
  bitset<BOARD_SIZE> w_pieces;
  bitset<BOARD_SIZE> all_pieces;

	Chessboard();
	Chessboard(const Chessboard& b);

  int getUtility(const string color, const AttackPiece& attack);
  void readBoard(vector<BasicPiece> blk_pieces, vector<BasicPiece> wht_pieces);
  bitset<BOARD_SIZE> getKingMoves(const string c, const int i, const AttackPiece& a);
  bitset<BOARD_SIZE> getQueenMoves(const string c, const int i, const AttackPiece& a);
  bitset<BOARD_SIZE> getRookMoves(const string c, const int i, const AttackPiece& a);
  bitset<BOARD_SIZE> getBishopMoves(const string c, const int i, const AttackPiece& a);
  bitset<BOARD_SIZE> getKnightMoves(const string c, const int i, const AttackPiece& a);
  bitset<BOARD_SIZE> getPawnMoves(const string c, const int i, const AttackPiece& a);
  bitset<BOARD_SIZE> getPawnAttacks(const string c, const int i, const AttackPiece& a);
};

// information about each movable piece, including all locations it can move to
struct PieceToMove
{
  bitset<BOARD_SIZE> piece_moves;
  int piece_rank;
  string piece_file;
  string piece_type;
};

// holds the board setup after a piece moves from current_index to new_index
struct State
{
  Chessboard board;
  bool checkmate;
  string type;
  int current_index;
  int new_index;
  int utility;
  int repetitions;
};

// additional, non-class-specific functions
bool operator>(const State& lhs, const State& rhs);
int getIndex(const int rank, const string file);
int getRank(const int i);
string getFile(const int i);
void printBoard(const bitset<BOARD_SIZE>& board);
bitset<BOARD_SIZE> getAttacked(const string their_color, Chessboard& board, const AttackPiece& attack);
bool isAttacked(const bitset<BOARD_SIZE>& attacked, const int location);
