// AI
// This is where you build your AI

#include "ai.hpp"
using namespace std;

// You can add #includes here for your AI.

namespace cpp_client
{

namespace chess
{

/// <summary>
/// This returns your AI's name to the game server.
/// Replace the string name.
/// </summary>
/// <returns>The name of your AI.</returns>
string AI::get_name() const
{
  // REPLACE WITH YOUR TEAM NAME!
  return "david-crow";
}

/// <summary>
/// This is automatically called when the game first starts, once the game objects are created
/// </summary>
void AI::start()
{
  // This is a good place to initialize any variables
  srand(time(NULL));
  attack.generateAttacks();
  king_rook_moved = false;
  queen_rook_moved = false;
  king_moved = false;
}

/// <summary>
/// This is automatically called when the game (or anything in it) updates
/// </summary>
void AI::game_updated()
{
  // If a function you call triggers an update this will be called before it returns.
}

/// <summary>
/// This is automatically called when the game ends.
/// </summary>
/// <param name="won">true if you won, false otherwise</param>
/// <param name="reason">An explanation for why you either won or lost</param>
void AI::ended(bool won, const string& reason)
{
  // You can do any cleanup of your AI here.  The program ends when this function returns.
}

/// <summary>
/// This is called every time it is this AI.player's turn.
/// </summary>
/// <returns>Represents if you want to end your turn. True means end your turn, False means to keep your turn going and re-call this function.</returns>
bool AI::run_turn()
{
  cout << endl << "Start turn! Here's the current board:" << endl;
  print_current_board();

  // gather pieces from MegaMiner framework
  vector<BasicPiece> black_pieces;
  vector<BasicPiece> white_pieces;

  // current board setup; based on black_pieces and white_pieces
  Chessboard board;
  int king_location;
  bitset<BOARD_SIZE> attacked;

  // generate new moves
  vector<PieceToMove> movable_pieces;
  vector<NewState> possible_states;

  // get board setup
  for (auto piece : game->pieces)
  {
    BasicPiece p;
    if (piece->owner->color == "Black")
    {
      p.type = piece->type;
      p.index = getIndex(piece->rank, piece->file);
      black_pieces.push_back(p);
    }

    else
    {
      p.type = piece->type;
      p.index = getIndex(piece->rank, piece->file);
      white_pieces.push_back(p);
    }
  }

  // generate bitboard from piece setup
  board.readBoard(black_pieces, white_pieces);
  attacked = getAttacked(player->opponent->color, board, attack);

  // cout << player->opponent->color << " -- their attacks:" << endl;
  // printBoard(attacked);
  // cout << player->color << " -- my attacks:" << endl;
  // printBoard(getAttacked(player->color, board, attack));

  // check if en passant is possible
  bool can_en_passant = false;
  string last_move;
  if (game->moves.size() > 0)
  {
    last_move = game->moves[game->moves.size() - 1]->san;

    // if the opponent just moved a pawn
    if (last_move.size() == PAWN_NOTATION)
    {
      // if the opponent advanced a pawn two squares from the start location
      if (abs(game->moves[game->moves.size() - 1]->from_rank - last_move[1]) == EN_PASSANT)
        can_en_passant = true;
    }
  }

  // get the current location of the king
  for (auto piece : player->pieces)
  {
    if (piece->type == "King")
    {
      king_location = getIndex(piece->rank, piece->file);
      break;
    }
  }

  // find all movable pieces and their possible moves (don't worry about check)
  for (auto piece : player->pieces)
  {
    PieceToMove piece_to_move;
    int i = getIndex(piece->rank, piece->file);

    if (piece->type == "King")
    {
      piece_to_move.piece_moves = board.getKingMoves(player->color, i, attack);

      // check if castling possible
      int left;
      int right;

      // king-side
      if (!king_moved && !king_rook_moved && !isAttacked(attacked, king_location))
      {
        bool can_castle = true;

        if (player->color == BLACK)
          right = B_ROOK_2;
        else
          right = king_location + CASTLE;

        for (int i = king_location + LEFT_RIGHT; i <= right; i++)
        {
          if (isAttacked(attacked, i) || board.all_pieces[i] == 1)
            can_castle = false;
        }

        if (can_castle)
          {piece_to_move.piece_moves[king_location + CASTLE];cout << "king castle!" << endl;}
      }

      // queen-side
      if (!king_moved && !queen_rook_moved && !isAttacked(attacked, king_location))
      {
        bool can_castle = true;

        if (player->color == BLACK)
          left = B_ROOK_1;
        else
          left = king_location - CASTLE;

        for (int i = left; i < king_location; i++)
        {
          if (isAttacked(attacked, i) || board.all_pieces[i] == 1)
            can_castle = false;
        }

        if (can_castle)
            {piece_to_move.piece_moves[king_location - CASTLE];cout << "queen castle!" << endl;}
      }
    }

    else if (piece->type == "Queen")
      piece_to_move.piece_moves = board.getQueenMoves(player->color, i, attack);
    else if (piece->type == "Rook")
      piece_to_move.piece_moves = board.getRookMoves(player->color, i, attack);
    else if (piece->type == "Bishop")
      piece_to_move.piece_moves = board.getBishopMoves(player->color, i, attack);
    else if (piece->type == "Knight")
      piece_to_move.piece_moves = board.getKnightMoves(player->color, i, attack);
    else // pawn
    {
      piece_to_move.piece_moves = board.getPawnMoves(player->color, i, attack);

      if (can_en_passant)
      {
        int index;

        if (player->color == BLACK)
        {
          index = getIndex(last_move[1] - 1, last_move[0] + "");

          if (attack.attacking_b_pawn[i][index] == 1)
            piece_to_move.piece_moves[index] = 1;
        }

        else // my color is white
        {
          index = getIndex(last_move[1] + 1, last_move[0] + "");

          if (attack.attacking_w_pawn[i][index] == 1)
            piece_to_move.piece_moves[index] = 1;
        }
      }
    }

    // if the piece can move to any other squares
    if (piece_to_move.piece_moves.count() > 0)
    {
      piece_to_move.piece_rank = getRank(i);
      piece_to_move.piece_file = getFile(i);
      piece_to_move.piece_type = piece->type;
      movable_pieces.push_back(piece_to_move);
    }
  }

  // for all movable pieces
  for (int i = 0; i < movable_pieces.size(); i++)
  {
    // number of pseudo-legal squares the piece can move to
    int num_moves = movable_pieces[i].piece_moves.count();
    int current_index = getIndex(movable_pieces[i].piece_rank, movable_pieces[i].piece_file);
    int new_index = 0;

    // while there are still moves to look at
    while (num_moves != 0)
    {
      int location = king_location;

      // if we found a square it can move to
      if (movable_pieces[i].piece_moves[new_index] == 1)
      {
        // create a new state and move piece i to square new_index
        NewState state;
        vector<BasicPiece> new_black = black_pieces;
        vector<BasicPiece> new_white = white_pieces;

        if (player->color == BLACK)
        {
          // for each black piece
          for (int b = 0; b < new_black.size(); b++)
          {
            // found the piece we want to move
            if (new_black[b].index == current_index)
            {
              // if we're moving the king, update its location
              if (new_black[b].type == "King")
                location = new_index;

              // move the piece to its new index
              new_black[b].index = new_index;
              break;
            }
          }

          // if there was an opposing piece on new_index, capture it
          for (int del = 0; del < new_white.size(); del++)
          {
            if (new_white[del].index == new_index)
              new_white[del].type = '\0';
          }
        }

        else // my color is white
        {
          // for each white piece
          for (int w = 0; w < new_white.size(); w++)
          {
            // found the piece we want to move
            if (new_white[w].index == current_index)
            {
              // if we're moving the king, update its location
              if (new_white[w].type == "King")
                location = new_index;

              // move the piece to its new index
              new_white[w].index = new_index;
              break;
            }
          }

          // if there was an opposing piece on new_index, capture it
          for (int del = 0; del < new_black.size(); del++)
          {
            if (new_black[del].index == new_index)
              new_black[del].type = '\0';
          }
        }

        // generate new bitboard
        state.board.readBoard(new_black, new_white);
        attacked = getAttacked(player->opponent->color, state.board, attack);

        // if the king isn't checked after this move is completed
        if (!isAttacked(attacked, location))
        {
          state.current_index = current_index;
          state.new_index = new_index;
          possible_states.push_back(state);
        }

        else // piece can't move here
          movable_pieces[i].piece_moves[new_index] == 0;

        num_moves--; // we've accounted for one of the moves
      }

      new_index++; // look at next square on board
    }
  }

  // pick random state
  int rand_state = rand() % possible_states.size();
  PieceToMove rand_piece;

  string from_file = getFile(possible_states[rand_state].current_index);
  int from_rank = getRank(possible_states[rand_state].current_index);
  string to_file = getFile(possible_states[rand_state].new_index);
  int to_rank = getRank(possible_states[rand_state].new_index);

  // get all possible moves
  for (int i = 0; i < movable_pieces.size(); i++)
  {
    int index = getIndex(movable_pieces[i].piece_rank, movable_pieces[i].piece_file);

    if (index == possible_states[rand_state].current_index)
    {
      rand_piece = movable_pieces[i];
      break;
    }
  }

  // update castling
  if (player->color == BLACK)
  {
    if (possible_states[rand_state].current_index == B_ROOK_1)
      queen_rook_moved = true;
    else if (possible_states[rand_state].current_index == B_ROOK_2)
      king_rook_moved = true;
    else if (possible_states[rand_state].current_index == B_KING)
      king_moved = true;
  }

  else // my color is white
  {
    if (possible_states[rand_state].current_index == W_ROOK_1)
      queen_rook_moved = true;
    else if (possible_states[rand_state].current_index == W_ROOK_2)
      king_rook_moved = true;
    else if (possible_states[rand_state].current_index == W_KING)
      king_moved = true;
  }

  for (auto piece : player->pieces)
  {
    if (piece->file == from_file && piece->rank == from_rank)
    {
      cout << piece->type << " on " << from_file
        << from_rank << " can move to:" << endl;

      for (int i = 0; i < rand_piece.piece_moves.size(); i++)
      {
        if (rand_piece.piece_moves[i] == 1)
          cout << "\t" << getFile(i) << getRank(i) << endl;
      }

      // promote to random type
      int random_type = rand() % PROMOTE_TYPES;

      if (random_type == 0)
        piece->move(to_file, to_rank, "Queen");
      else if (random_type == 1)
        piece->move(to_file, to_rank, "Rook");
      else if (random_type == 2)
        piece->move(to_file, to_rank, "Bishop");
      else if (random_type == 3)
        piece->move(to_file, to_rank, "Rook");
      break;
    }
  }

  cout << "Time Remaining: " << player->time_remaining << " ns" << endl;
  if(game->moves.size() > 1)
    cout << "Opponent's Last Move: '" << game->moves[game->moves.size() - 2]->san << "'" << endl;

  return true; // to signify we are done with our turn.
}

/// <summary>
///  Prints the current board using pretty ASCII art
/// </summary>
/// <remarks>
/// Note: you can delete this function if you wish
/// </remarks>
void AI::print_current_board()
{
  for(int rank = 9; rank >= -1; rank--)
  {
    string str = "";
    if(rank == 9 || rank == 0) // then the top or bottom of the board
    {
      str = "   +------------------------+";
    }
    else if(rank == -1) // then show the ranks
    {
      str = "     a  b  c  d  e  f  g  h";
    }
    else // board
    {
      str += " ";
      str += to_string(rank);
      str += " |";
      // fill in all the files with pieces at the current rank
      for(int file_offset = 0; file_offset < 8; file_offset++)
      {
        string file(1, 'a' + file_offset); // start at a, with with file offset increasing the char;
        chess::Piece current_piece = nullptr;
        for(const auto& piece : game->pieces)
        {
          if(piece->file == file && piece->rank == rank) // then we found the piece at (file, rank)
          {
            current_piece = piece;
            break;
          }
        }

        char code = '.'; // default "no piece";
        if(current_piece != nullptr)
      {
          code = current_piece->type[0];

          if(current_piece->type == "Knight") // 'K' is for "King", we use 'N' for "Knights"
          {
            code = 'N';
          }

          if(current_piece->owner->id == "1") // the second player (black) is lower case. Otherwise it's upppercase already
          {
            code = tolower(code);
          }
        }

        str += " ";
        str += code;
        str += " ";
      }

      str += "|";
    }

    cout << str << endl;
  }
}

// You can add additional methods here for your AI to call

} // chess

} // cpp_client
