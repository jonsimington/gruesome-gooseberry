//////////////////////////////////////////////////////////////////////
/// @file attack_piece.cpp
/// @author David Crow, CS5400 D
/// @brief This file contains the class implementation for piece attacks
//////////////////////////////////////////////////////////////////////

#include "attack_piece.h"
using namespace std;

// initializes all attacks to 0
AttackPiece::AttackPiece()
{
  for (int i = 0; i < BOARD_SIZE; i++)
  {
    attacking_king[i] = 0;
    attacking_queen[i] = 0;
    attacking_rook[i] = 0;
    attacking_bishop[i] = 0;
    attacking_knight[i] = 0;
    attacking_b_pawn[i] = 0;
    attacking_w_pawn[i] = 0;
    moving_b_pawn[i] = 0;
    moving_w_pawn[i] = 0;
  }
}

// calls all generating functions
void AttackPiece::generateAttacks()
{
  kingAttacks();
  queenAttacks();
  rookAttacks();
  bishopAttacks();
  knightAttacks();
  pawnAttacksMoves();

  return;
}

// generates king's attacks from each square
void AttackPiece::kingAttacks()
{
  for (int i = 0; i < BOARD_SIZE; i++)
  {
    attacking_king[i][i] = 1;

    // if king can move up
    if (i + HEIGHT_WIDTH < BOARD_SIZE)
      attacking_king[i][i + HEIGHT_WIDTH] = 1;

    // if king can move down
    if (0 <= i - HEIGHT_WIDTH)
      attacking_king[i][i - HEIGHT_WIDTH] = 1;

    // if not on the left edge
    if (i % HEIGHT_WIDTH != 0)
    {
      // if king can move left
      if (0 <= i - LEFT_RIGHT)
        attacking_king[i][i - LEFT_RIGHT] = 1;

      // if king can move NW
      if (i + NW_SE_DIAGONAL < BOARD_SIZE)
        attacking_king[i][i + NW_SE_DIAGONAL] = 1;

      // if king can move SW
      if (0 <= i - NE_SW_DIAGONAL)
        attacking_king[i][i - NE_SW_DIAGONAL] = 1;
    }

    // if not on the right edge
    if (i % HEIGHT_WIDTH != RIGHT_EDGE)
    {
      // if king can move right
      if (i + LEFT_RIGHT < BOARD_SIZE)
        attacking_king[i][i + LEFT_RIGHT] = 1;

      // if king can move NE
      if (i + NE_SW_DIAGONAL < BOARD_SIZE)
        attacking_king[i][i + NE_SW_DIAGONAL] = 1;

      // if king can move SE
      if (0 <= i - NW_SE_DIAGONAL)
        attacking_king[i][i - NW_SE_DIAGONAL] = 1;
    }
  }

  return;
}

// generates queen's attacks from each square
void AttackPiece::queenAttacks()
{
  for (int i = 0; i < BOARD_SIZE; i++)
  {
    int north_west = i;
    int south_west = i;
    int north_east = i;
    int south_east = i;

    attacking_queen[i][i] = 1;

    // if it can move NW
    do
    {
      attacking_queen[i][north_west] = 1;
      north_west += NW_SE_DIAGONAL;
    } while (north_west % HEIGHT_WIDTH != RIGHT_EDGE && north_west < BOARD_SIZE);

    // if it can move NE
    do
    {
      attacking_queen[i][north_east] = 1;
      north_east += NE_SW_DIAGONAL;
    } while (north_east % HEIGHT_WIDTH != 0 && north_east < BOARD_SIZE);

    // if it can move SW
    do
    {
      attacking_queen[i][south_west] = 1;
      south_west -= NE_SW_DIAGONAL;
    } while (south_west % HEIGHT_WIDTH != RIGHT_EDGE && south_west >= 0);

    // if it can move SE
    do
    {
      attacking_queen[i][south_east] = 1;
      south_east -= NW_SE_DIAGONAL;
    } while (south_east % HEIGHT_WIDTH != 0 && south_east >= 0);

    // for everthing in its column
    for (int up = i; up < BOARD_SIZE; up += HEIGHT_WIDTH)
      attacking_queen[i][up] = 1;
    for (int down = i; down >= 0; down -= HEIGHT_WIDTH)
      attacking_queen[i][down] = 1;

    int left = i;
    int right = i;

    // attack squares to the left
    do
    {
      attacking_queen[i][left] = 1;
      left--;
    } while (left % HEIGHT_WIDTH != RIGHT_EDGE && left >= 0);

    // attack squares to the right
    do
    {
      attacking_queen[i][right] = 1;
      right++;
    } while (right % HEIGHT_WIDTH != 0 && right < BOARD_SIZE);
  }
  
  return;
}

// generates rook's attacks from each square
void AttackPiece::rookAttacks()
{
  for (int i = 0; i < BOARD_SIZE; i++)
  {
    // for everthing in its column
    for (int up = i; up < BOARD_SIZE; up += HEIGHT_WIDTH)
      attacking_rook[i][up] = 1;
    for (int down = i; down >= 0; down -= HEIGHT_WIDTH)
      attacking_rook[i][down] = 1;

    int left = i;
    int right = i;

    // attack squares to the left
    do
    {
      attacking_rook[i][left] = 1;
      left--;
    } while (left % HEIGHT_WIDTH != RIGHT_EDGE && left >= 0);

    // attack squares to the right
    do
    {
      attacking_rook[i][right] = 1;
      right++;
    } while (right % HEIGHT_WIDTH != 0 && right < BOARD_SIZE);
  }

  return;
}

// generates bishop's attacks from each square
void AttackPiece::bishopAttacks()
{
  for (int i = 0; i < BOARD_SIZE; i++)
  {
    int north_west = i;
    int south_west = i;
    int north_east = i;
    int south_east = i;

    attacking_bishop[i][i] = 1;

    // if it can move NW
    do
    {
      attacking_bishop[i][north_west] = 1;
      north_west += NW_SE_DIAGONAL;
    } while (north_west % HEIGHT_WIDTH != RIGHT_EDGE && north_west < BOARD_SIZE);

    // if it can move NE
    do
    {
      attacking_bishop[i][north_east] = 1;
      north_east += NE_SW_DIAGONAL;
    } while (north_east % HEIGHT_WIDTH != 0 && north_east < BOARD_SIZE);

    // if it can move SW
    do
    {
      attacking_bishop[i][south_west] = 1;
      south_west -= NE_SW_DIAGONAL;
    } while (south_west % HEIGHT_WIDTH != RIGHT_EDGE && south_west >= 0);

    // if it can move SE
    do
    {
      attacking_bishop[i][south_east] = 1;
      south_east -= NW_SE_DIAGONAL;
    } while (south_east % HEIGHT_WIDTH != 0 && south_east >= 0);
  }
  return;
}

// generates knight's attacks from each square
void AttackPiece::knightAttacks()
{
  for (int i = 0; i < BOARD_SIZE; i++)
  {
    attacking_knight[i][i] = 1;

    // if not on left edge
    if (i % HEIGHT_WIDTH != 0)
    {
      // if knight can move 2 down 1 left
      if (i - KNIGHT_MOVES[0] >= 0)
        attacking_knight[i][i - KNIGHT_MOVES[0]] = 1;

      // if knight can move 2 up 1 left
      if (i + KNIGHT_MOVES[1] < BOARD_SIZE)
        attacking_knight[i][i + KNIGHT_MOVES[1]] = 1;

      // if not in file B
      if (i % HEIGHT_WIDTH != 1)
      {
        // if knight can move 2 left 1 down
        if (i - KNIGHT_MOVES[2] >= 0)
          attacking_knight[i][i - KNIGHT_MOVES[2]] = 1;

        // if knight can move 2 left 1 up
        if (i + KNIGHT_MOVES[3] < BOARD_SIZE)
          attacking_knight[i][i + KNIGHT_MOVES[3]] = 1;
      }
    }

    // if not on right edge
    if (i % HEIGHT_WIDTH != RIGHT_EDGE)
    {
      // if knight can move 2 up 1 right
      if (i + KNIGHT_MOVES[0] < BOARD_SIZE)
        attacking_knight[i][i + KNIGHT_MOVES[0]] = 1;

      // if knight can move 2 down 1 right
      if (i - KNIGHT_MOVES[1] >= 0)
        attacking_knight[i][i - KNIGHT_MOVES[1]] = 1;

      // if not in file G
      if (i % HEIGHT_WIDTH != RIGHT_EDGE - 1)
      {
        // if knight can move 2 right 1 up
        if (i + KNIGHT_MOVES[2] < BOARD_SIZE)
          attacking_knight[i][i + KNIGHT_MOVES[2]] = 1;

        // if knight can move 2 right 1 down
        if (i - KNIGHT_MOVES[3] >= 0)
          attacking_knight[i][i - KNIGHT_MOVES[3]] = 1;
      }
    }
  }

  return;
}

// generates pawn's attacks and moves from each square
void AttackPiece::pawnAttacksMoves()
{
  for (int i = 0; i < BOARD_SIZE; i++)
  {
    attacking_b_pawn[i][i] = 1;
    attacking_w_pawn[i][i] = 1;

    // if not on the left edge
    if (i % HEIGHT_WIDTH != 0)
    {
      // if black pawn can move SW
      if (0 <= i - NE_SW_DIAGONAL)
        attacking_b_pawn[i][i - NE_SW_DIAGONAL] = 1;

      // if white pawn can move NW
      if (i + NW_SE_DIAGONAL < BOARD_SIZE)
        attacking_w_pawn[i][i + NW_SE_DIAGONAL] = 1;
    }

    // if not on the right edge
    if (i % HEIGHT_WIDTH != RIGHT_EDGE)
    {
      // if black pawn can move SE
      if (0 <= i - NW_SE_DIAGONAL)
        attacking_b_pawn[i][i - NW_SE_DIAGONAL] = 1;

      // if white pawn can move NE
      if (i + NE_SW_DIAGONAL < BOARD_SIZE)
        attacking_w_pawn[i][i + NE_SW_DIAGONAL] = 1;
    }

    // black pawn forward two
    if (B_PAWN_LEFT <= i && i <= B_PAWN_RIGHT)
      moving_b_pawn[i][i - HEIGHT_WIDTH * PAWN_START] = 1;

    // black pawn forward one
    if (i - HEIGHT_WIDTH >= 0)
      moving_b_pawn[i][i - HEIGHT_WIDTH] = 1;

    // white pawn forward two
    if (W_PAWN_LEFT <= i && i <= W_PAWN_RIGHT)
      moving_w_pawn[i][i + HEIGHT_WIDTH * PAWN_START] = 1;

    // white pawn forward one
    if (i + HEIGHT_WIDTH < BOARD_SIZE)
      moving_w_pawn[i][i + HEIGHT_WIDTH] = 1;
  }

  return;
}
