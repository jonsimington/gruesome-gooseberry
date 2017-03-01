//////////////////////////////////////////////////////////////////////
/// @file constants.h
/// @author David Crow, CS5400 D
/// @brief This file contains all necessary constants
//////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
using namespace std;

// general game-wide constants
const int NUM_TYPES = 6; // king, queen, rook, bishop, knight, pawn
const int BOARD_SIZE = 64; // 64 squares on a chessboard
const string BLACK = "Black";
const string WHITE = "White";

// difference in  indices for different types of moves
const int HEIGHT_WIDTH = 8; // moving up/down; also marks left/right edges
const int RIGHT_EDGE = 7; // index % <this> indicates rightmost edge
const int LEFT_RIGHT = 1; // moving left/right
const int NW_SE_DIAGONAL = 7; // moving along \ line
const int NE_SW_DIAGONAL = 9; // moving along / line
const int PAWN_START = 2; // pawn can move twice at the start
const int CASTLE = 2; // distance king moves on a castle

// determines knight movements based on index
// e.g. moving 2 up and 1 left is a difference of 17
const int KNIGHT_DIRECTIONS = 4;
const int KNIGHT_MOVES[KNIGHT_DIRECTIONS] = {17, 15, 10, 6};

// index of each type of piece in the arrays of bitboards
const int KING = 0;
const int QUEEN = 1;
const int ROOKS = 2;
const int BISHOPS = 3;
const int KNIGHTS = 4;
const int PAWNS = 5;

// starting indices of each black piece (pawns have a range)
const int B_KING = 60;
const int B_ROOK_1 = 56;
const int B_ROOK_2 = 63;
const int B_PAWN_LEFT = 48;
const int B_PAWN_RIGHT = 55;

// starting indices of each white piece (pawns have a range)
const int W_KING = 4;
const int W_ROOK_1 = 0;
const int W_ROOK_2 = 7;
const int W_PAWN_LEFT = 8;
const int W_PAWN_RIGHT = 15;

const int PAWN_NOTATION = 2; // 2 characters in SAN for pawn movements
const int EN_PASSANT = 2; // can only EP if pawn moved 2 squares
