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
  Chessboard board;
  vector<BasicPiece> black_pieces;
  vector<BasicPiece> white_pieces;
  vector<NewState> possible_states;

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

  board.readBoard(black_pieces, white_pieces);

  AttackPiece attack;
  attack.generateAttacks();

  int num_moves = 0;
  int num_pieces = player->pieces.size();
  bitset<BOARD_SIZE> potential_pieces = 0;
  vector<PieceToMove> valid_pieces;
  PieceToMove piece_to_move;
  string last_move;
  bool can_en_passant = false;
  int king_location;
  bool in_check = false;
  string my_color;
  string their_color;
  bitset<BOARD_SIZE> attacked = 0;
  bitset<BOARD_SIZE> king_check = 0;

  if (player->color == "Black")
  {
    my_color = BLACK;
    their_color = WHITE;
  }

  else
  {
    my_color = WHITE;
    their_color = BLACK;
  }

  if (game->moves.size() > 0)
  {
    last_move = game->moves[game->moves.size() - 1]->san;

    // if the opponent just moved a pawn
    if (last_move.size() == PAWN_NOTATION)
    {
      // if the opponent advanced a pawn two squares from the start location
      if (abs(game->moves[game->moves.size() - 1]->from_rank - last_move[1]) == EN_PASSANT)
      {
        can_en_passant = true;
      }
    }
  }

  for (auto piece : player->pieces)
  {
    if (piece->type == "King")
      king_location = getIndex(piece->rank, piece->file);
  }

  for (auto piece : player->opponent->pieces)
  {
    int i = getIndex(piece->rank, piece->file);

    if (piece->type == "King")
      attacked |= board.getKingMoves(their_color, i, attack);
    else if (piece->type == "Queen")
      attacked |= board.getQueenMoves(their_color, i, attack);
    else if (piece->type == "Rook")
      attacked |= board.getRookMoves(their_color, i, attack);
    else if (piece->type == "Bishop")
      attacked |= board.getBishopMoves(their_color, i, attack);
    else if (piece->type == "Knight")
      attacked |= board.getKnightMoves(their_color, i, attack);
    else if (piece->type == "Pawn")
      attacked |= board.getPawnAttacks(their_color, i, attack);
  }

  if (attacked[king_location] == 1)
    in_check = true;

  for (auto piece : player->pieces)
  {
    int i = getIndex(piece->rank, piece->file);

    if (piece->type == "King")
    {
      if (attacked[getIndex(piece->rank, piece->file)] == 1)
        in_check = true;

      piece_to_move.piece_moves = board.getKingMoves(my_color, i, attack) & ~attacked;

      if (piece_to_move.piece_moves.count() > 0)
      {
        piece_to_move.piece_rank = getRank(i);
        piece_to_move.piece_file = getFile(i);
        piece_to_move.piece_type = "King";
        valid_pieces.push_back(piece_to_move);
      }
    }

    else if (piece->type == "Queen")
    {
      piece_to_move.piece_moves = board.getQueenMoves(my_color, i, attack);

      if (piece_to_move.piece_moves.count() > 0)
      {
        piece_to_move.piece_rank = getRank(i);
        piece_to_move.piece_file = getFile(i);
        piece_to_move.piece_type = "Queen";
        valid_pieces.push_back(piece_to_move);
      }
    }

    else if (piece->type == "Rook")
    {
      piece_to_move.piece_moves = board.getRookMoves(my_color, i, attack);

      if (piece_to_move.piece_moves.count() > 0)
      {
        piece_to_move.piece_rank = getRank(i);
        piece_to_move.piece_file = getFile(i);
        piece_to_move.piece_type = "Rook";
        valid_pieces.push_back(piece_to_move);
      }
    }

    else if (piece->type == "Bishop")
    {
      piece_to_move.piece_moves = board.getBishopMoves(my_color, i, attack);

      if (piece_to_move.piece_moves.count() > 0)
      {
        piece_to_move.piece_rank = getRank(i);
        piece_to_move.piece_file = getFile(i);
        piece_to_move.piece_type = "Bishop";
        valid_pieces.push_back(piece_to_move);
      }
    }

    else if (piece->type == "Knight")
    {
      piece_to_move.piece_moves = board.getKnightMoves(my_color, i, attack);

      if (piece_to_move.piece_moves.count() > 0)
      {
        piece_to_move.piece_rank = getRank(i);
        piece_to_move.piece_file = getFile(i);
        piece_to_move.piece_type = "Knight";
        valid_pieces.push_back(piece_to_move);
      }
    }

    else if (piece->type == "Pawn")
    {
      piece_to_move.piece_moves = board.getPawnMoves(my_color, i, attack);

      if (can_en_passant)
      {
        int index = getIndex(last_move[1] + 1, last_move[0] + "");

        if (attack.attacking_b_pawn[i][index] == 1)
          piece_to_move.piece_moves[index] = 1;
      }

      if (piece_to_move.piece_moves.count() > 0)
      {
        piece_to_move.piece_rank = getRank(i);
        piece_to_move.piece_file = getFile(i);
        piece_to_move.piece_type = "Pawn";
        valid_pieces.push_back(piece_to_move);
      }
    }
  }

  for (int i = 0; i < valid_pieces.size(); i++)
  {
    int num_moves = valid_pieces[i].piece_moves.count();
    int j = 0;

    while (num_moves != 0)
    {
      if (valid_pieces[i].piece_moves[j] == 1)
      {
        NewState state;
        Chessboard new_board;
        vector<BasicPiece> new_black = black_pieces;
        vector<BasicPiece> new_white = white_pieces;

        if (my_color == BLACK)
        {
          int current_index = getIndex(valid_pieces[i].piece_rank, valid_pieces[i].piece_file);

          for (int k = 0; k < new_black.size(); k++)
          {
            // found the piece we want to move
            if (new_black[k].index == current_index)
            {
              // move the piece to its new index
              new_black[k].index = j;
              break;
            }
          }
        }

        else // my color is white
        {
          int current_index = getIndex(valid_pieces[i].piece_rank, valid_pieces[i].piece_file);

          for (int k = 0; k < new_black.size(); k++)
          {
            // found the piece we want to move
            if (new_white[k].index == current_index)
            {
              // move the piece to its new index
              new_white[k].index = j;
              break;
            }
          }
        }

        state.board.readBoard(new_black, new_white);
        state.piece = valid_pieces[i];
        possible_states.push_back(state);
        num_moves--;
      }

      j++;
    }
  }

  int i = 0;
  while (i < possible_states.size())
  {
    int king_location = -1;

    if (my_color == BLACK)
    {
      for (int j = 0; j < BOARD_SIZE; j++)
      {
        if (possible_states[i].board.black[KING][j] == 1)
        {
          king_location = possible_states[i].board.black[KING][j];
          break;
        }
      }

      if (isChecked(king_location, possible_states[i].board))
      {
        possible_states.erase(possible_states.begin() + i);
        i--; // cancel out the increment that comes later
      }
    }

    else // my color is white
    {
      for (int j = 0; j < BOARD_SIZE; j++)
      {
        if (possible_states[i].board.white[KING][j] == 1)
        {
          king_location = possible_states[i].board.white[KING][j];
          break;
        }
      }

      if (isChecked(king_location, possible_states[i].board))
      {
        possible_states.erase(possible_states.begin() + i);
        i--; // cancel out the increment that comes later
      }
    }

    i++;
  }

  // else // not in check
  {
    int r = rand() % possible_states.size();
    PieceToMove rand_piece = possible_states[r].piece;
    int i = rand() % rand_piece.piece_moves.count() + 1;
    int rand_move = -1;

    while (i != 0)
    {
      rand_move++;

      if (rand_piece.piece_moves[rand_move] == 1)
        i--;
    }

    int rand_rank = getRank(rand_move);
    string rand_file = getFile(rand_move);

    for (auto piece : player->pieces)
    {
      if (piece->file == rand_piece.piece_file && piece->rank == rand_piece.piece_rank)
      {
        cout << endl << piece->type << " on " << rand_piece.piece_file
          << piece->rank << " can move to:" << endl;

        for (int i = 0; i < rand_piece.piece_moves.size(); i++)
        {
          if (rand_piece.piece_moves[i] == 1)
            cout << "\t" << getFile(i) << getRank(i) << endl;
        }

        // default promote to Queen
        piece->move(rand_file, rand_rank, "Queen");
        break;
      }
    }
  }

  print_current_board();
  cout << "Time Remaining: " << player->time_remaining << " ns" << endl;
  if(game->moves.size() > 0)
    cout << "Opponent's Last Move: '" << game->moves[game->moves.size() - 1]->san << "'" << endl;

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
