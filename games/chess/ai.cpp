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
std::string AI::get_name() const
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
void AI::ended(bool won, const std::string& reason)
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

  if (player->color == "Black")
  {
    for (int i = 0; i < BOARD_SIZE; i++)
    {
      if (board.black[KING][i] == 1)
      {
        // cout << board.getKingMoves(BLACK, i, attack) << " B K\t" << i << endl;
        piece_to_move.piece_moves = board.getKingMoves(BLACK, i, attack);

        if (piece_to_move.piece_moves.count() > 0)
        {
          piece_to_move.piece_rank = getRank(i);
          piece_to_move.piece_file = getFile(i);
          valid_pieces.push_back(piece_to_move);
        }
      }

      if (board.black[QUEEN][i] == 1)
      {
        // cout << board.getQueenMoves(BLACK, i, attack) << " B Q\t" << i << endl;
        piece_to_move.piece_moves = board.getQueenMoves(BLACK, i, attack);

        if (piece_to_move.piece_moves.count() > 0)
        {
          piece_to_move.piece_rank = getRank(i);
          piece_to_move.piece_file = getFile(i);
          valid_pieces.push_back(piece_to_move);
        }
      }

      if (board.black[ROOKS][i] == 1)
      {
        // cout << board.getRookMoves(BLACK, i, attack) << " B R\t" << i << endl;
        piece_to_move.piece_moves = board.getRookMoves(BLACK, i, attack);

        if (piece_to_move.piece_moves.count() > 0)
        {
          piece_to_move.piece_rank = getRank(i);
          piece_to_move.piece_file = getFile(i);
          valid_pieces.push_back(piece_to_move);
        }
      }

      if (board.black[BISHOPS][i] == 1)
      {
        // cout << board.getBishopMoves(BLACK, i, attack) << " B B\t" << i << endl;
        piece_to_move.piece_moves = board.getBishopMoves(BLACK, i, attack);

        if (piece_to_move.piece_moves.count() > 0)
        {
          piece_to_move.piece_rank = getRank(i);
          piece_to_move.piece_file = getFile(i);
          valid_pieces.push_back(piece_to_move);
        }
      }

      if (board.black[KNIGHTS][i] == 1)
      {
        // cout << board.getKnightMoves(BLACK, i, attack) << " B N\t" << i << endl;
        piece_to_move.piece_moves = board.getKnightMoves(BLACK, i, attack);

        if (piece_to_move.piece_moves.count() > 0)
        {
          piece_to_move.piece_rank = getRank(i);
          piece_to_move.piece_file = getFile(i);
          valid_pieces.push_back(piece_to_move);
        }
      }

      if (board.black[PAWNS][i] == 1)
      {
        // cout << board.getPawnMoves(BLACK, i, attack) << " B P\t" << i << endl;
        piece_to_move.piece_moves = board.getPawnMoves(BLACK, i, attack);

        if (can_en_passant)
        {
          int index = getIndex(last_move[1] + 1, last_move[0] + "");

          if (attack.moving_b_pawn[i][index] == 1)
            piece_to_move.piece_moves[index] = 1;
        }

        if (piece_to_move.piece_moves.count() > 0)
        {
          piece_to_move.piece_rank = getRank(i);
          piece_to_move.piece_file = getFile(i);
          valid_pieces.push_back(piece_to_move);
        }
      }
    }
  }

  else // my color is white
  {
    for (int i = 0; i < BOARD_SIZE; i++)
    {
      if (board.white[KING][i] == 1)
      {
        // cout << board.getKingMoves(WHITE, i, attack) << " W K\t" << i << endl;
        piece_to_move.piece_moves = board.getKingMoves(WHITE, i, attack);

        if (piece_to_move.piece_moves.count() > 0)
        {
          piece_to_move.piece_rank = getRank(i);
          piece_to_move.piece_file = getFile(i);
          valid_pieces.push_back(piece_to_move);
        }
      }

      if (board.white[QUEEN][i] == 1)
      {
        // cout << board.getQueenMoves(WHITE, i, attack) << " W Q\t" << i << endl;
        piece_to_move.piece_moves = board.getQueenMoves(WHITE, i, attack);

        if (piece_to_move.piece_moves.count() > 0)
        {
          piece_to_move.piece_rank = getRank(i);
          piece_to_move.piece_file = getFile(i);
          valid_pieces.push_back(piece_to_move);
        }
      }

      if (board.white[ROOKS][i] == 1)
      {
          // cout << board.getRookMoves(WHITE, i, attack) << " W R\t" << i << endl;
          piece_to_move.piece_moves = board.getRookMoves(WHITE, i, attack);

          if (piece_to_move.piece_moves.count() > 0)
          {
            piece_to_move.piece_rank = getRank(i);
            piece_to_move.piece_file = getFile(i);
            valid_pieces.push_back(piece_to_move);
          }
      }

      if (board.white[BISHOPS][i] == 1)
      {
        // cout << board.getBishopMoves(WHITE, i, attack) << " W B\t" << i << endl;
        piece_to_move.piece_moves = board.getBishopMoves(WHITE, i, attack);

        if (piece_to_move.piece_moves.count() > 0)
        {
          piece_to_move.piece_rank = getRank(i);
          piece_to_move.piece_file = getFile(i);
          valid_pieces.push_back(piece_to_move);
        }
      }

      if (board.white[KNIGHTS][i] == 1)
      {
        // cout << board.getKnightMoves(WHITE, i, attack) << " W N\t" << i << endl;
        piece_to_move.piece_moves = board.getKnightMoves(WHITE, i, attack);

        if (piece_to_move.piece_moves.count() > 0)
        {
          piece_to_move.piece_rank = getRank(i);
          piece_to_move.piece_file = getFile(i);
          valid_pieces.push_back(piece_to_move);
        }
      }

      if (board.white[PAWNS][i] == 1)
      {
        // cout << board.getPawnMoves(WHITE, i, attack) << " W P\t" << i << endl;
        piece_to_move.piece_moves = board.getPawnMoves(WHITE, i, attack);

        if (can_en_passant)
        {
          int index = getIndex(last_move[1] - 1, last_move[0] + "");

          if (attack.moving_w_pawn[i][index] == 1)
            piece_to_move.piece_moves[index] = 1;
        }

        if (piece_to_move.piece_moves.count() > 0)
        {
          piece_to_move.piece_rank = getRank(i);
          piece_to_move.piece_file = getFile(i);
          valid_pieces.push_back(piece_to_move);
        }
      }
    }
  }

  // for (int i = 0; i < valid_pieces.size(); i++)
  // {
  //   cout << valid_pieces[i].piece_moves << endl
  //     << valid_pieces[i].piece_rank << " " << valid_pieces[i].piece_file << endl;
  // }

  // if king in check, fix it
  // otherwise
  int r = rand() % valid_pieces.size();
  PieceToMove rand_piece = valid_pieces[r];
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
        {
          cout << "\t" << getFile(i) << getRank(i) << endl;
        }
      }

      piece->move(rand_file, rand_rank);
      break;
    }
  }

  // cout << endl;
  // cout << "BLACK------------" << endl;
  // for (int i = 0; i < NUM_TYPES; i++)
  //   cout << board.black[i] << endl;
  //
  // cout << board.b_pieces << endl << endl;
  //
  // cout << "WHITE------------" << endl;
  // for (int i = 0; i < NUM_TYPES; i++)
  //   cout << board.white[i] << endl;
  //
  // cout << board.w_pieces << endl << endl;
  // cout << board.all_pieces << endl;
  // cout << endl << endl << endl;

  // Here is where you'll want to code your AI.

  // We've provided sample code that:
  //    1) prints the board to the console
  //    2) prints the opponent's last move to the console
  //    3) prints how much time remaining this AI has to calculate moves
  //    4) makes a random (and probably invalid) move.

  // 1) print the board to the console
  print_current_board();

  // 2) print the opponent's last move to the console
  if(game->moves.size() > 0)
  {
    std::cout << "Opponent's Last Move: '" << game->moves[game->moves.size() - 1]->san << "'" << std::endl;
  }

  // // 3) print how much time remaining this AI has to calculate moves
  std::cout << "Time Remaining: " << player->time_remaining << " ns" << std::endl;

  // 4) make a random (and probably invalid) move.
  // chess::Piece random_piece = player->pieces[rand() % player->pieces.size()];
  // std::string random_file(1, 'a' + rand() % 8);
  // int random_rank = (rand() % 8) + 1;
  // random_piece->move(random_file, random_rank);

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
    std::string str = "";
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
      str += std::to_string(rank);
      str += " |";
      // fill in all the files with pieces at the current rank
      for(int file_offset = 0; file_offset < 8; file_offset++)
      {
        std::string file(1, 'a' + file_offset); // start at a, with with file offset increasing the char;
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

    std::cout << str << std::endl;
  }
}

// You can add additional methods here for your AI to call

} // chess

} // cpp_client
