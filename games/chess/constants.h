//////////////////////////////////////////////////////////////////////
/// @file constants.h
/// @author David Crow, CS5400 D
/// @brief This file contains all necessary constants
//////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
using namespace std;

// general game-wide constants
const int CHECKMATE = 1000000;
const int MAX_DEPTH = 3;
const int MAX_POINTS = 1000;
const int DRAW_MOVES = 7; // 7 moves must occur before the next move can cause draw
const int DRAW_SHIFT = 4; // draw checks moves separated by 3 other moves for equivalence
const int NUM_TYPES = 6; // king, queen, rook, bishop, knight, pawn
const int BOARD_SIZE = 64; // 64 squares on a chessboard
const string BLACK = "Black";
const string WHITE = "White";

// minimax
const string MAX = "max";
const string MIN = "min";

// difference in  indices for different types of moves
const int HEIGHT_WIDTH = 8; // moving up/down; also marks left/right edges
const int RIGHT_EDGE = 7; // index % <this> indicates rightmost edge
const int LEFT_RIGHT = 1; // moving left/right
const int NW_SE_DIAGONAL = 7; // moving along \ line
const int NE_SW_DIAGONAL = 9; // moving along / line
const int PAWN_START = 2; // pawn can move twice at the start

// piece values
const int KING_VALUE = 20; // only used when seeing if a move checks the king
const int QUEEN_VALUE = 9;
const int ROOK_VALUE = 5;
const int BISHOP_VALUE = 3;
const int KNIGHT_VALUE = 3;
const int PAWN_VALUE = 1;

// special move conditions
const int PROMOTE_TYPES = 4; // pawn can promote to one of four types
const int PAWN_NOTATION = 2; // 2 characters in SAN for pawn movements
const int EN_PASSANT = 2; // can only EP if pawn moved 2 squares
const int EN_PASSANT_FEN = 5; // 5th character from end of FEN indicates en passant
const int CASTLE = 2; // distance king moves on a castle
const int CASTLE_WHITESPACE = 3; // castling capability is 3 whitespaces from end of FEN
const int CASTLE_FEN = 4; // 4 possible characters to signify castling in FEN
                          // even if castling is '-', other three characters in
                          // FEN are whitespace or color (i.e. won't match any of KQkq)

// determines knight movements based on index
// e.g. moving 2 up and 1 left is a difference of 17
const int KNIGHT_DIRECTIONS = 4;
const int KNIGHT_MOVES[KNIGHT_DIRECTIONS] = {17, 15, 10, 6};

// index of each type of piece in the arrays of bitboards
const int KING = 0;
const int QUEEN = 1;
const int ROOK = 2;
const int BISHOP = 3;
const int KNIGHT = 4;
const int PAWN = 5;

// starting locations of key black pieces (pawns have a range)
const int B_KING = 60;
const int B_ROOK_LEFT = 56;
const int B_ROOK_RIGHT = 63;
const int B_PAWN_LEFT = 48;
const int B_PAWN_RIGHT = 55;

// starting locations of key white pieces (pawns have a range)
const int W_KING = 4;
const int W_ROOK_LEFT = 0;
const int W_ROOK_RIGHT = 7;
const int W_PAWN_LEFT = 8;
const int W_PAWN_RIGHT = 15;
