//////////////////////////////////////////////////////////////////////
/// @file chessboard.cpp
/// @author David Crow, CS5400 D
/// @brief This file contains the class implementation for the chessboard
//////////////////////////////////////////////////////////////////////

#include "chessboard.h"
using namespace std;

int getRank(const int i)
{
  return i / HEIGHT_WIDTH + 1;
}

string getFile(const int i)
{
  string file;
  int temp_file = i % HEIGHT_WIDTH + 1;

  switch(temp_file)
  {
    case 1:
      file = "a";
      break;
    case 2:
      file = "b";
      break;
    case 3:
      file = "c";
      break;
    case 4:
      file = "d";
      break;
    case 5:
      file = "e";
      break;
    case 6:
      file = "f";
      break;
    case 7:
      file = "g";
      break;
    default:
      file = "h";
  }

  return file;
}

int getIndex(const int rank, const string file)
{
  int index = (rank - 1) * HEIGHT_WIDTH;
  switch(file[0])
  {
    case 'a':
      index += 0;
      break;
    case 'b':
      index += 1;
      break;
    case 'c':
      index += 2;
      break;
    case 'd':
      index += 3;
      break;
    case 'e':
      index += 4;
      break;
    case 'f':
      index += 5;
      break;
    case 'g':
      index += 6;
      break;
    case 'h':
      index += 7;
      break;
  }

  return index;
}

// this initializes an empty board (i.e. no pieces)
Chessboard::Chessboard()
{
  for (int i = 0; i < NUM_TYPES; i++)
  {
    black[i] = 0;
    white[i] = 0;
  }

  b_pieces = 0;
  w_pieces = 0;
  all_pieces = 0;
}

// copy constructor
Chessboard::Chessboard(const Chessboard& b)
{
  b_pieces = b.b_pieces;
  w_pieces = b.w_pieces;
  all_pieces = b.all_pieces;

  for (int i = 0; i < NUM_TYPES; i++)
  {
    black[i] = b.black[i];
    white[i] = b.white[i];
  }
}

// places black and white pieces according to standard chess rules
void Chessboard::readBoard(vector<BasicPiece> blk_pieces, vector<BasicPiece> wht_pieces)
{
  for (int i = 0; i < blk_pieces.size(); i++)
  {
    if (blk_pieces[i].type == "King")
      black[KING][blk_pieces[i].index] = 1;
    else if (blk_pieces[i].type == "Queen")
      black[QUEEN][blk_pieces[i].index] = 1;
    else if (blk_pieces[i].type == "Rook")
      black[ROOKS][blk_pieces[i].index] = 1;
    else if (blk_pieces[i].type == "Bishop")
      black[BISHOPS][blk_pieces[i].index] = 1;
    else if (blk_pieces[i].type == "Knight")
      black[KNIGHTS][blk_pieces[i].index] = 1;
    else if (blk_pieces[i].type == "Pawn")
      black[PAWNS][blk_pieces[i].index] = 1;
  }

  for (int i = 0; i < wht_pieces.size(); i++)
  {
    if (wht_pieces[i].type == "King")
      white[KING][wht_pieces[i].index] = 1;
    else if (wht_pieces[i].type == "Queen")
      white[QUEEN][wht_pieces[i].index] = 1;
    else if (wht_pieces[i].type == "Rook")
      white[ROOKS][wht_pieces[i].index] = 1;
    else if (wht_pieces[i].type == "Bishop")
      white[BISHOPS][wht_pieces[i].index] = 1;
    else if (wht_pieces[i].type == "Knight")
      white[KNIGHTS][wht_pieces[i].index] = 1;
    else if (wht_pieces[i].type == "Pawn")
      white[PAWNS][wht_pieces[i].index] = 1;
  }

  // int num_pieces = cpp_client::chess::Game->pieces.size();
  //
  // // kings
  // black[KING][B_KING] = 1;
  // white[KING][W_KING] = 1;
  //
  // // queens
  // black[QUEEN][B_QUEEN] = 1;
  // white[QUEEN][W_QUEEN] = 1;
  //
  // // rooks
  // black[ROOKS][B_ROOK_1] = 1;
  // black[ROOKS][B_ROOK_2] = 1;
  // white[ROOKS][W_ROOK_1] = 1;
  // white[ROOKS][W_ROOK_2] = 1;
  //
  // // bishops
  // black[BISHOPS][B_BISHOP_1] = 1;
  // black[BISHOPS][B_BISHOP_2] = 1;
  // white[BISHOPS][W_BISHOP_1] = 1;
  // white[BISHOPS][W_BISHOP_2] = 1;
  //
  // // knights
  // black[KNIGHTS][B_KNIGHT_1] = 1;
  // black[KNIGHTS][B_KNIGHT_2] = 1;
  // white[KNIGHTS][W_KNIGHT_1] = 1;
  // white[KNIGHTS][W_KNIGHT_2] = 1;
  //
  // // pawns
  // for (int i = B_PAWN_LEFT; i <= B_PAWN_RIGHT; i++)
  //   black[PAWNS][i] = 1;
  //
  // for (int i = W_PAWN_LEFT; i <= W_PAWN_RIGHT; i++)
  //   white[PAWNS][i] = 1;

  // all pieces
  b_pieces = black[KING] | black[QUEEN] | black[ROOKS]
    | black[BISHOPS] | black[KNIGHTS] | black[PAWNS];
  w_pieces = white[KING] | white[QUEEN] | white[ROOKS]
    | white[BISHOPS] | white[KNIGHTS] | white[PAWNS];
  all_pieces = b_pieces | w_pieces;

  return;
}

// returns all valid king moves given the current location of all pieces
bitset<BOARD_SIZE> Chessboard::getKingMoves(const string c, const int i, const AttackPiece& a)
{
  bitset<BOARD_SIZE> my_side;

  if (c == BLACK) // black
    my_side = b_pieces;
  else // white
    my_side = w_pieces;

  return a.attacking_king[i] & ~my_side;
}

bitset<BOARD_SIZE> Chessboard::getQueenMoves(const string c, const int i, const AttackPiece& a)
{
  bitset<BOARD_SIZE> my_side;
  bitset<BOARD_SIZE> valid_moves;
  int slider;

  if (c == BLACK) // black
    my_side = b_pieces;
  else // white
    my_side = w_pieces;

  valid_moves = a.attacking_queen[i] & ~my_side;

  // all moves N
  slider = i + HEIGHT_WIDTH;
  while (slider < BOARD_SIZE)
  {
    // if it finds an invalid move or a piece occupies a square on the path
    if (valid_moves[slider] == 0 || all_pieces[i] == 1)
    {
      // clear all "valid" moves that exist after the invalid/blocked square
      while (slider < BOARD_SIZE)
      {
        valid_moves[slider] = 0;
        slider += HEIGHT_WIDTH;
      }

      break;
    }

    slider += HEIGHT_WIDTH;
  }

  // all moves S
  slider = i - HEIGHT_WIDTH;
  while (slider >= 0)
  {
    // if it finds an invalid move or a piece occupies a square on the path
    if (valid_moves[slider] == 0 || all_pieces[i] == 1)
    {
      // clear all "valid" moves that exist after the invalid/blocked square
      while (slider >= 0)
      {
        valid_moves[slider] = 0;
        slider -= HEIGHT_WIDTH;
      }

      break;
    }

    slider -= HEIGHT_WIDTH;
  }

  // all moves E
  slider = i + LEFT_RIGHT;
  while (slider % HEIGHT_WIDTH != RIGHT_EDGE)
  {
    // if it finds an invalid move or a piece occupies a square on the path
    if (valid_moves[slider] == 0 || all_pieces[i] == 1)
    {
      // clear all "valid" moves that exist after the invalid/blocked square
      while (slider % HEIGHT_WIDTH != 0)
      {
        valid_moves[slider] = 0;
        slider += LEFT_RIGHT;
      }

      break;
    }

    slider += LEFT_RIGHT;
  }

  // all moves W
  slider = i - LEFT_RIGHT;
  while (slider % HEIGHT_WIDTH != 0)
  {
    // if it finds an invalid move or a piece occupies a square on the path
    if (valid_moves[slider] == 0 || all_pieces[i] == 1)
    {
      // clear all "valid" moves that exist after the invalid/blocked square
      while (slider % HEIGHT_WIDTH != RIGHT_EDGE)
      {
        valid_moves[slider] = 0;
        slider -= LEFT_RIGHT;
      }

      break;
    }

    slider -= LEFT_RIGHT;
  }

  // all moves NE
  slider = i + NE_SW_DIAGONAL;
  while (slider < BOARD_SIZE && slider % HEIGHT_WIDTH != RIGHT_EDGE)
  {
    // if it finds an invalid move or a piece occupies a square on the path
    if (valid_moves[slider] == 0 || all_pieces[i] == 1)
    {
      // clear all "valid" moves that exist after the invalid/blocked square
      while (slider < BOARD_SIZE && slider % HEIGHT_WIDTH != 0)
      {
        valid_moves[slider] = 0;
        slider += NE_SW_DIAGONAL;
      }

      break;
    }

    slider += NE_SW_DIAGONAL;
  }

  // all moves SE
  slider = i - NW_SE_DIAGONAL;
  while (slider >= 0 && slider % HEIGHT_WIDTH != RIGHT_EDGE)
  {
    // if it finds an invalid move or a piece occupies a square on the path
    if (valid_moves[slider] == 0 || all_pieces[i] == 1)
    {
      // clear all "valid" moves that exist after the invalid/blocked square
      while (slider >= 0 && slider % HEIGHT_WIDTH != 0)
      {
        valid_moves[slider] = 0;
        slider -= NW_SE_DIAGONAL;
      }

      break;
    }

    slider -= NW_SE_DIAGONAL;
  }

  // all moves NW
  slider = i + NW_SE_DIAGONAL;
  while (slider < BOARD_SIZE && slider % HEIGHT_WIDTH != RIGHT_EDGE)
  {
    // if it finds an invalid move or a piece occupies a square on the path
    if (valid_moves[slider] == 0 || all_pieces[i] == 1)
    {
      // clear all "valid" moves that exist after the invalid/blocked square
      while (slider < BOARD_SIZE && slider % HEIGHT_WIDTH != RIGHT_EDGE)
      {
        valid_moves[slider] = 0;
        slider += NW_SE_DIAGONAL;
      }

      break;
    }

    slider += NW_SE_DIAGONAL;
  }

  // all moves SW
  slider = i - NE_SW_DIAGONAL;
  while (slider >= 0 && slider % HEIGHT_WIDTH != 0)
  {
    // if it finds an invalid move or a piece occupies a square on the path
    if (valid_moves[slider] == 0 || all_pieces[i] == 1)
    {
      // clear all "valid" moves that exist after the invalid/blocked square
      while (slider >= 0 && slider % HEIGHT_WIDTH != RIGHT_EDGE)
      {
        valid_moves[slider] = 0;
        slider -= NE_SW_DIAGONAL;
      }

      break;
    }

    slider -= NE_SW_DIAGONAL;
  }

  return valid_moves;
}

bitset<BOARD_SIZE> Chessboard::getRookMoves(const string c, const int i, const AttackPiece& a)
{
  bitset<BOARD_SIZE> my_side;
  bitset<BOARD_SIZE> valid_moves;
  int slider;

  if (c == BLACK) // black
    my_side = b_pieces;
  else // white
    my_side = w_pieces;

  valid_moves = a.attacking_rook[i] & ~my_side;

  // all moves N
  slider = i + HEIGHT_WIDTH;
  while (slider < BOARD_SIZE)
  {
    // if it finds an invalid move or a piece occupies a square on the path
    if (valid_moves[slider] == 0 || all_pieces[i] == 1)
    {
      // clear all "valid" moves that exist after the invalid/blocked square
      while (slider < BOARD_SIZE)
      {
        valid_moves[slider] = 0;
        slider += HEIGHT_WIDTH;
      }

      break;
    }

    slider += HEIGHT_WIDTH;
  }

  // all moves S
  slider = i - HEIGHT_WIDTH;
  while (slider >= 0)
  {
    // if it finds an invalid move or a piece occupies a square on the path
    if (valid_moves[slider] == 0 || all_pieces[i] == 1)
    {
      // clear all "valid" moves that exist after the invalid/blocked square
      while (slider >= 0)
      {
        valid_moves[slider] = 0;
        slider -= HEIGHT_WIDTH;
      }

      break;
    }

    slider -= HEIGHT_WIDTH;
  }

  // all moves E
  slider = i + LEFT_RIGHT;
  while (slider % HEIGHT_WIDTH != RIGHT_EDGE)
  {
    // if it finds an invalid move or a piece occupies a square on the path
    if (valid_moves[slider] == 0 || all_pieces[i] == 1)
    {
      // clear all "valid" moves that exist after the invalid/blocked square
      while (slider % HEIGHT_WIDTH != 0)
      {
        valid_moves[slider] = 0;
        slider += LEFT_RIGHT;
      }

      break;
    }

    slider += LEFT_RIGHT;
  }

  // all moves W
  slider = i - LEFT_RIGHT;
  while (slider % HEIGHT_WIDTH != 0)
  {
    // if it finds an invalid move or a piece occupies a square on the path
    if (valid_moves[slider] == 0 || all_pieces[i] == 1)
    {
      // clear all "valid" moves that exist after the invalid/blocked square
      while (slider % HEIGHT_WIDTH != RIGHT_EDGE)
      {
        valid_moves[slider] = 0;
        slider -= LEFT_RIGHT;
      }

      break;
    }

    slider -= LEFT_RIGHT;
  }

  return valid_moves;
}

bitset<BOARD_SIZE> Chessboard::getBishopMoves(const string c, const int i, const AttackPiece& a)
{
  bitset<BOARD_SIZE> pieces = all_pieces;
  bitset<BOARD_SIZE> my_side;
  bitset<BOARD_SIZE> valid_moves;
  int slider;

  if (c == BLACK) // black
    my_side = b_pieces;
  else // white
    my_side = w_pieces;

  valid_moves = a.attacking_bishop[i] & ~my_side;

  // all moves NE
  slider = i + NE_SW_DIAGONAL;
  while (slider < BOARD_SIZE && slider % HEIGHT_WIDTH != RIGHT_EDGE)
  {
    // if it finds an invalid move or a piece occupies a square on the path
    if (valid_moves[slider] == 0 || all_pieces[i] == 1)
    {
      // clear all "valid" moves that exist after the invalid/blocked square
      while (slider < BOARD_SIZE && slider % HEIGHT_WIDTH != 0)
      {
        valid_moves[slider] = 0;
        slider += NE_SW_DIAGONAL;
      }

      break;
    }

    slider += NE_SW_DIAGONAL;
  }

  // all moves SE
  slider = i - NW_SE_DIAGONAL;
  while (slider >= 0 && slider % HEIGHT_WIDTH != RIGHT_EDGE)
  {
    // if it finds an invalid move or a piece occupies a square on the path
    if (valid_moves[slider] == 0 || all_pieces[i] == 1)
    {
      // clear all "valid" moves that exist after the invalid/blocked square
      while (slider >= 0 && slider % HEIGHT_WIDTH != 0)
      {
        valid_moves[slider] = 0;
        slider -= NW_SE_DIAGONAL;
      }

      break;
    }

    slider -= NW_SE_DIAGONAL;
  }

  // all moves NW
  slider = i + NW_SE_DIAGONAL;
  while (slider < BOARD_SIZE && slider % HEIGHT_WIDTH != RIGHT_EDGE)
  {
    // if it finds an invalid move or a piece occupies a square on the path
    if (valid_moves[slider] == 0 || all_pieces[i] == 1)
    {
      // clear all "valid" moves that exist after the invalid/blocked square
      while (slider < BOARD_SIZE && slider % HEIGHT_WIDTH != RIGHT_EDGE)
      {
        valid_moves[slider] = 0;
        slider += NW_SE_DIAGONAL;
      }

      break;
    }

    slider += NW_SE_DIAGONAL;
  }

  // all moves SW
  slider = i - NE_SW_DIAGONAL;
  while (slider >= 0 && slider % HEIGHT_WIDTH != 0)
  {
    // if it finds an invalid move or a piece occupies a square on the path
    if (valid_moves[slider] == 0 || all_pieces[i] == 1)
    {
      // clear all "valid" moves that exist after the invalid/blocked square
      while (slider >= 0 && slider % HEIGHT_WIDTH != RIGHT_EDGE)
      {
        valid_moves[slider] = 0;
        slider -= NE_SW_DIAGONAL;
      }

      break;
    }

    slider -= NE_SW_DIAGONAL;
  }

  return valid_moves;
}

// returns all valid knight moves given the current location of all pieces
bitset<BOARD_SIZE> Chessboard::getKnightMoves(const string c, const int i, const AttackPiece& a)
{
  bitset<BOARD_SIZE> my_side;

  if (c == BLACK) // black
    my_side = b_pieces;
  else // white
    my_side = w_pieces;

  return a.attacking_knight[i] & ~my_side;
}

// returns all valid pawn moves given the current location of all pieces
bitset<BOARD_SIZE> Chessboard::getPawnMoves(const string c, const int i, const AttackPiece& a)
{
  bitset<BOARD_SIZE> valid_moves;
  int slider = i;

  if (c == BLACK) // black
  {
    valid_moves =  (a.attacking_b_pawn[i] & w_pieces) | (a.moving_b_pawn[i] & ~all_pieces);

    // if square directly in front of pawn is blocked, so is the square after
    if (valid_moves[i - HEIGHT_WIDTH] == 0)
      valid_moves[i - HEIGHT_WIDTH - HEIGHT_WIDTH] = 0;
  }

  else // white
  {
    valid_moves = (a.attacking_w_pawn[i] & b_pieces) | (a.moving_w_pawn[i] & ~all_pieces);

    // if square directly in front of pawn is blocked, so is the square after
    if (valid_moves[i + HEIGHT_WIDTH] == 0)
      valid_moves[i + HEIGHT_WIDTH + HEIGHT_WIDTH] = 0;
  }

  return valid_moves;
}

// returns true if king has been captured; false otherwise
bool Chessboard::gameOver()
{
  if (black[KING] == 0 || white[KING] == 0)
    return true;

  return false;
}
