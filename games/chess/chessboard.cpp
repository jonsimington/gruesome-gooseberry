//////////////////////////////////////////////////////////////////////
/// @file chessboard.cpp
/// @author David Crow, CS5400 D
/// @brief This file contains the class implementation for the chessboard
//////////////////////////////////////////////////////////////////////

#include "chessboard.h"
using namespace std;

// returns the rank of a specificolor index
int getRank(const int i)
{
  return i / HEIGHT_WIDTH + 1;
}

// returns the file of a specificolor index
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

// returns the index of a specificolor rank and file
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

// identifies all squares attacked by enemy player in current board setup
bitset<BOARD_SIZE> getAttacked(const string their_color, Chessboard& board, const AttackPiece& attack)
{
  bitset<BOARD_SIZE> attacked = 0;

  // for every square on the board
  for (int i = 0; i < BOARD_SIZE; i++)
  {
    if (their_color == BLACK)
    {
      // generate squares attacked by the piece on square i
      if (board.black[KING][i] == 1)
        attacked |= board.getKingMoves(their_color, i, attack);
      else if (board.black[QUEEN][i] == 1)
        attacked |= board.getQueenMoves(their_color, i, attack);
      else if (board.black[ROOK][i] == 1)
        attacked |= board.getRookMoves(their_color, i, attack);
      else if (board.black[BISHOP][i] == 1)
        attacked |= board.getBishopMoves(their_color, i, attack);
      else if (board.black[KNIGHT][i] == 1)
        attacked |= board.getKnightMoves(their_color, i, attack);
      else if (board.black[PAWN][i] == 1)
        attacked |= board.getPawnAttacks(their_color, i, attack);
    }

    else // they are white
    {
      // generate squares attacked by the piece on square i
      if (board.white[KING][i] == 1)
        attacked |= board.getKingMoves(their_color, i, attack);
      else if (board.white[QUEEN][i] == 1)
        attacked |= board.getQueenMoves(their_color, i, attack);
      else if (board.white[ROOK][i] == 1)
        attacked |= board.getRookMoves(their_color, i, attack);
      else if (board.white[BISHOP][i] == 1)
        attacked |= board.getBishopMoves(their_color, i, attack);
      else if (board.white[KNIGHT][i] == 1)
        attacked |= board.getKnightMoves(their_color, i, attack);
      else if (board.white[PAWN][i] == 1)
        attacked |= board.getPawnAttacks(their_color, i, attack);
    }
  }

  return attacked;
}

// prints any bitboard in a nice format
void printBoard(const bitset<BOARD_SIZE>& board)
{
  // top two corners of the board
  int min = B_ROOK_LEFT;
  int max = B_ROOK_RIGHT;

  // prints row by row from top to bottom
  while (min >= 0)
  {
    // rank
    cout << max / HEIGHT_WIDTH << " |";

    // bit at square i
    for (int i = min; i < max; i++)
    {
      cout << board[i] << " ";
    }

    cout << endl;

    // go down a row
    min -= HEIGHT_WIDTH;
    max -= HEIGHT_WIDTH;
  }

  // output file labels
  cout << "   ---------------" << endl;
  cout << "   a b color d e f g h" << endl;

  cout << endl;
  return;
}

// checks whether a given square is attacked by the enemy
bool isAttacked(const bitset<BOARD_SIZE>& attacked, const int location)
{
  if (attacked[location] == 1)
    return true;

  return false;
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

// calculates the utility of a specificolor board setup
int Chessboard::getUtility(const string color, const AttackPiece& attack)
{
  int utility = 0;
  bitset<BOARD_SIZE> my_side[NUM_TYPES];
  bitset<BOARD_SIZE> their_side[NUM_TYPES];

  bitset<BOARD_SIZE> all_my_pieces;
  bitset<BOARD_SIZE> all_their_pieces;

  bitset<BOARD_SIZE> my_attacks = getAttacked(color, *this, attack);
  bitset<BOARD_SIZE> their_attacks = getAttacked(
    (color == BLACK ? WHITE : BLACK), *this, attack);

  if (color == BLACK)
  {
    for (int i = 0; i < NUM_TYPES; i++)
    {
      my_side[i] = black[i];
      their_side[i] = white[i];
    }
  }

  else // color is white
  {
    for (int i = 0; i < NUM_TYPES; i++)
    {
      my_side[i] = white[i];
      their_side[i] = black[i];
    }
  }

  // bonus for what I attack
  utility += (my_attacks & their_side[KING]).count() * KING_VALUE / 2;
  utility += (my_attacks & their_side[QUEEN]).count() * QUEEN_VALUE / 2;
  utility += (my_attacks & their_side[ROOK]).count() * ROOK_VALUE / 2;
  utility += (my_attacks & their_side[BISHOP]).count() * BISHOP_VALUE / 2;
  utility += (my_attacks & their_side[KNIGHT]).count() * KNIGHT_VALUE / 2;
  utility += (my_attacks & their_side[PAWN]).count() * PAWN_VALUE / 2;

  // penalty for what they attack
  utility -= (their_attacks & my_side[KING]).count() * KING_VALUE * 3;
  utility -= (their_attacks & my_side[QUEEN]).count() * QUEEN_VALUE;
  utility -= (their_attacks & my_side[ROOK]).count() * ROOK_VALUE;
  utility -= (their_attacks & my_side[BISHOP]).count() * BISHOP_VALUE;
  utility -= (their_attacks & my_side[KNIGHT]).count() * KNIGHT_VALUE;
  utility -= (their_attacks & my_side[PAWN]).count() * PAWN_VALUE;

  // all of my pieces
  utility += my_side[QUEEN].count() * QUEEN_VALUE;
  utility += my_side[ROOK].count() * ROOK_VALUE;
  utility += my_side[BISHOP].count() * BISHOP_VALUE;
  utility += my_side[KNIGHT].count() * KNIGHT_VALUE;
  utility += my_side[PAWN].count() * PAWN_VALUE;

  // all of their pieces
  utility -= their_side[QUEEN].count() * QUEEN_VALUE;
  utility -= their_side[ROOK].count() * ROOK_VALUE;
  utility -= their_side[BISHOP].count() * BISHOP_VALUE;
  utility -= their_side[KNIGHT].count() * KNIGHT_VALUE;
  utility -= their_side[PAWN].count() * PAWN_VALUE;

  return utility;
}

// places black and white pieces according to current board state
void Chessboard::readBoard(vector<BasicPiece> blk_pieces, vector<BasicPiece> wht_pieces)
{
  // for all black pieces
  for (int i = 0; i < blk_pieces.size(); i++)
  {
    if (blk_pieces[i].type == "King")
      black[KING][blk_pieces[i].index] = 1;
    else if (blk_pieces[i].type == "Queen")
      black[QUEEN][blk_pieces[i].index] = 1;
    else if (blk_pieces[i].type == "Rook")
      black[ROOK][blk_pieces[i].index] = 1;
    else if (blk_pieces[i].type == "Bishop")
      black[BISHOP][blk_pieces[i].index] = 1;
    else if (blk_pieces[i].type == "Knight")
      black[KNIGHT][blk_pieces[i].index] = 1;
    else if (blk_pieces[i].type == "Pawn")
      black[PAWN][blk_pieces[i].index] = 1;
  }

  // for all white pieces
  for (int i = 0; i < wht_pieces.size(); i++)
  {
    if (wht_pieces[i].type == "King")
      white[KING][wht_pieces[i].index] = 1;
    else if (wht_pieces[i].type == "Queen")
      white[QUEEN][wht_pieces[i].index] = 1;
    else if (wht_pieces[i].type == "Rook")
      white[ROOK][wht_pieces[i].index] = 1;
    else if (wht_pieces[i].type == "Bishop")
      white[BISHOP][wht_pieces[i].index] = 1;
    else if (wht_pieces[i].type == "Knight")
      white[KNIGHT][wht_pieces[i].index] = 1;
    else if (wht_pieces[i].type == "Pawn")
      white[PAWN][wht_pieces[i].index] = 1;
  }

  // all pieces
  b_pieces = black[KING] | black[QUEEN] | black[ROOK]
    | black[BISHOP] | black[KNIGHT] | black[PAWN];
  w_pieces = white[KING] | white[QUEEN] | white[ROOK]
    | white[BISHOP] | white[KNIGHT] | white[PAWN];
  all_pieces = b_pieces | w_pieces;

  return;
}

// returns all valid king moves given the current location of all pieces
bitset<BOARD_SIZE> Chessboard::getKingMoves(const string color, const int i, const AttackPiece& a)
{
  if (color == BLACK) // black
    return a.attacking_king[i] & ~b_pieces;
  else // white
    return a.attacking_king[i] & ~w_pieces;
}

// returns all valid queen moves given the current location of all pieces
bitset<BOARD_SIZE> Chessboard::getQueenMoves(const string color, const int i, const AttackPiece& a)
{
  // return valid_moves;
  return getRookMoves(color, i, a) | getBishopMoves(color, i, a);
}

// returns all valid rook moves given the current location of all pieces
bitset<BOARD_SIZE> Chessboard::getRookMoves(const string color, const int i, const AttackPiece& a)
{
  bitset<BOARD_SIZE> my_side;
  bitset<BOARD_SIZE> valid_moves;
  int slider; // moves along the entire straight line the piece can move

  if (color == BLACK) // black
    my_side = b_pieces;
  else // white
    my_side = w_pieces;

  valid_moves = a.attacking_rook[i] & ~my_side;

  // all moves N
  slider = i + HEIGHT_WIDTH;
  while (slider < BOARD_SIZE)
  {
    // if it finds an invalid move or a piece occupies a square on the path
    if (valid_moves[slider] == 0 || all_pieces[slider] == 1)
    {
      // clear all "valid" moves that exist after the invalid/blocked square
      slider += HEIGHT_WIDTH;
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
    if (valid_moves[slider] == 0 || all_pieces[slider] == 1)
    {
      // clear all "valid" moves that exist after the invalid/blocked square
      slider -= HEIGHT_WIDTH;
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
  while ((slider % HEIGHT_WIDTH != 0) && (slider < BOARD_SIZE))
  {
    // if it finds an invalid move or a piece occupies a square on the path
    if (valid_moves[slider] == 0 || all_pieces[slider] == 1)
    {
      // clear all "valid" moves that exist after the invalid/blocked square
      slider += LEFT_RIGHT;
      while ((slider % HEIGHT_WIDTH != 0) && (slider < BOARD_SIZE))
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
  while ((slider % HEIGHT_WIDTH != RIGHT_EDGE) && (slider >= 0))
  {
    // if it finds an invalid move or a piece occupies a square on the path
    if (valid_moves[slider] == 0 || all_pieces[slider] == 1)
    {
      // clear all "valid" moves that exist after the invalid/blocked square
      slider -= LEFT_RIGHT;
      while ((slider % HEIGHT_WIDTH != RIGHT_EDGE) && (slider >= 0))
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

// returns all valid bishop moves given the current location of all pieces
bitset<BOARD_SIZE> Chessboard::getBishopMoves(const string color, const int i, const AttackPiece& a)
{
  bitset<BOARD_SIZE> pieces = all_pieces;
  bitset<BOARD_SIZE> my_side;
  bitset<BOARD_SIZE> valid_moves;
  int slider; // moves along the entire straight line the piece can move

  if (color == BLACK) // black
    my_side = b_pieces;
  else // white
    my_side = w_pieces;

  valid_moves = a.attacking_bishop[i] & ~my_side;

  // all moves NE
  slider = i + NE_SW_DIAGONAL;
  while ((slider < BOARD_SIZE) && (slider % HEIGHT_WIDTH != 0))
  {
    // if it finds an invalid move or a piece occupies a square on the path
    if (valid_moves[slider] == 0 || all_pieces[slider] == 1)
    {
      // clear all "valid" moves that exist after the invalid/blocked square
      slider += NE_SW_DIAGONAL;
      while ((slider < BOARD_SIZE) && (slider % HEIGHT_WIDTH != 0))
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
  while ((slider >= 0) && (slider % HEIGHT_WIDTH != 0))
  {
    // if it finds an invalid move or a piece occupies a square on the path
    if (valid_moves[slider] == 0 || all_pieces[slider] == 1)
    {
      // clear all "valid" moves that exist after the invalid/blocked square
      slider -= NW_SE_DIAGONAL;
      while ((slider >= 0) && (slider % HEIGHT_WIDTH != 0))
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
  while ((slider < BOARD_SIZE) && (slider % HEIGHT_WIDTH != RIGHT_EDGE))
  {
    // if it finds an invalid move or a piece occupies a square on the path
    if (valid_moves[slider] == 0 || all_pieces[slider] == 1)
    {
      // clear all "valid" moves that exist after the invalid/blocked square
      slider += NW_SE_DIAGONAL;
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
  while ((slider >= 0) && (slider % HEIGHT_WIDTH != RIGHT_EDGE))
  {
    // if it finds an invalid move or a piece occupies a square on the path
    if (valid_moves[slider] == 0 || all_pieces[slider] == 1)
    {
      // clear all "valid" moves that exist after the invalid/blocked square
      slider -= NE_SW_DIAGONAL;
      while ((slider >= 0) && (slider % HEIGHT_WIDTH != RIGHT_EDGE))
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
bitset<BOARD_SIZE> Chessboard::getKnightMoves(const string color, const int i, const AttackPiece& a)
{
  if (color == BLACK) // black
    return a.attacking_knight[i] & ~b_pieces;
  else // white
    return a.attacking_knight[i] & ~w_pieces;
}

// returns all valid pawn moves (no attacks) given the current location of all pieces
bitset<BOARD_SIZE> Chessboard::getPawnMoves(const string color, const int i, const AttackPiece& a)
{
  bitset<BOARD_SIZE> valid_moves;
  int slider = i;

  if (color == BLACK) // black
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

// returns all valid pawn attacks (no moves) given the current location of all pieces
bitset<BOARD_SIZE> Chessboard::getPawnAttacks(const string color, const int i, const AttackPiece& a)
{
  if (color == BLACK) // black
    return a.attacking_b_pawn[i] & ~b_pieces;
  else // white
    return a.attacking_w_pawn[i] & ~w_pieces;
}
