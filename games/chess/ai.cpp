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

  // assume they've moved
  castle.king_moved = true;
  castle.king_rook_moved = true;
  castle.queen_rook_moved = true;

  // actually check castling
  initializeCastling();
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
  cout << endl << "Start " << player->color << "'s turn! Here's the current board:" << endl;
  print_current_board();
  cout << endl;

  // gather pieces from MegaMiner framework
  vector<BasicPiece> black_pieces;
  vector<BasicPiece> white_pieces;

  // current board setup; based on black_pieces and white_pieces
  findPieces(black_pieces, white_pieces);
  Chessboard board;
  board.readBoard(black_pieces, white_pieces);
  bitset<BOARD_SIZE> attacked = getAttacked(player->opponent->color, board, attack);

  // generate new moves
  vector<PieceToMove> movable_pieces;
  vector<NewState> possible_states;

  // check if en passant is possible
  string last_move;
  bool can_en_passant = checkEnPassant(last_move);

  // get the current location of the king
  int king_location = getKingLocation();

  // check whether king has moved
  if (player->color == BLACK && king_location != B_KING)
    castle.king_moved = true;

  if (player->color == WHITE && king_location != W_KING)
    castle.king_moved = true;

  // find all movable pieces and their possible moves (don't worry about check)
  findMovablePieces(movable_pieces, board, attacked, king_location, last_move, can_en_passant);

  // for all movable pieces, find valid moves
  findMoves(king_location, black_pieces, white_pieces, movable_pieces, possible_states);

  // pick random state
  int rand_state = rand() % possible_states.size();
  PieceToMove rand_piece = getRandomPiece(rand_state, movable_pieces, possible_states);

  // update castling
  updateCastlingAbility(possible_states[rand_state]);

  // send the move to the game server
  makeMove(possible_states[rand_state].current_index,
    possible_states[rand_state].new_index, rand_piece.piece_moves);

  cout << "Time Remaining: " << player->time_remaining << " ns" << endl;

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

// this will initialize castling ability based on game's starting FEN
void AI::initializeCastling()
{
  int whitespace = 0;
  int i = game->fen.size() - 1;
  while (whitespace < CASTLE_WHITESPACE)
  {
    if (game->fen[i] == ' ')
      whitespace++;

    i--;
  }

  // now we're looking at castling capability
  for (int c = i; c > i - CASTLE_FEN; c--)
  {
    if (player->color == BLACK)
    {
      if (game->fen[c] == 'q')
      {
        castle.queen_rook_moved = false;
        castle.king_moved = false;
      }

      if (game->fen[c] == 'k')
      {
        castle.king_rook_moved = false;
        castle.king_moved = false;
      }
    }

    else // my player is white
    {
      if (game->fen[c] == 'Q')
      {
        castle.queen_rook_moved = false;
        castle.king_moved = false;
      }

      if (game->fen[c] == 'K')
      {
        castle.king_rook_moved = false;
        castle.king_moved = false;
      }
    }
  }

  return;
}

// this returns true if en passant is possible; returns false otherwise
bool AI::checkEnPassant(string& last_move)
{
  bool can_en_passant = false;

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

  else // opponent hasn't moved yet
  {
    if (game->fen[game->fen.size() - EN_PASSANT_FEN] != '-')
      can_en_passant = true;
  }

  return can_en_passant;
}

// this returns the king's index
int AI::getKingLocation()
{
  for (auto piece : player->pieces)
  {
    if (piece->type == "King")
    {
      return getIndex(piece->rank, piece->file);
    }
  }
}

// copy all piece information into two vectors (which will then generate bitboards)
void AI::findPieces(vector<BasicPiece>& black_pieces, vector<BasicPiece>& white_pieces)
{
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

  return;
}

// this identfies all pieces with at least one pseudo-legal move
void AI::findMovablePieces(vector<PieceToMove>& movable_pieces,
  Chessboard& board, const bitset<BOARD_SIZE>& attacked, const int king_location,
  const string last_move, const bool can_en_passant)
{
  for (auto piece : player->pieces)
  {
    int i = getIndex(piece->rank, piece->file); // location of piece
    PieceToMove piece_to_move;

    if (piece->type == "King")
    {
      piece_to_move.piece_moves = board.getKingMoves(player->color, i, attack);

      // check if castling possible
      int left;
      int right;

      // king-side
      // if the pieces haven't moved and the king isn't in check
      if (!castle.king_moved && !castle.king_rook_moved && !isAttacked(attacked, king_location))
      {
        bool can_castle = true;

        if (player->color == BLACK)
          right = B_ROOK_RIGHT;
        else
          right = king_location + CASTLE;

        // if squares between king and rook are attacked or blocked
        for (int i = king_location + LEFT_RIGHT; i <= right; i++)
        {
          if (isAttacked(attacked, i) || board.all_pieces[i] == 1)
            can_castle = false;
        }

        if (can_castle)
          piece_to_move.piece_moves[king_location + CASTLE];
      }

      // queen-side
      // if the pieces haven't moved and the king isn't in check
      if (!castle.king_moved && !castle.queen_rook_moved && !isAttacked(attacked, king_location))
      {
        bool can_castle = true;

        if (player->color == BLACK)
          left = B_ROOK_LEFT + LEFT_RIGHT;
        else
          left = king_location - CASTLE;

        // if squares between king and rook are attacked or blocked
        for (int i = left; i < king_location; i++)
        {
          if (isAttacked(attacked, i) || board.all_pieces[i] == 1)
            can_castle = false;
        }

        if (can_castle)
          piece_to_move.piece_moves[king_location - CASTLE];
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

        // figure out which index is the en passant square
        if (player->color == BLACK)
        {
          index = getIndex(last_move[1] - 1, last_move[0] + "");

          if (attack.attacking_b_pawn[i][index] == 1)
            piece_to_move.piece_moves[index] = 1;
        }

        // figure out which index is the en passant square
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

  return;
}

// this filters out all illegal moves from all movable pieces
void AI::findMoves(const int king_location,
  const vector<BasicPiece>& black, const vector<BasicPiece>& white,
  vector<PieceToMove>& moves, vector<NewState>& states)
{
  // for all movable pieces
  for (int i = 0; i < moves.size(); i++)
  {
    // number of pseudo-legal squares the piece can move to
    int num_moves = moves[i].piece_moves.count();
    int current_index = getIndex(moves[i].piece_rank, moves[i].piece_file);
    int new_index = 0;

    // while there are still moves to look at
    while (num_moves != 0)
    {
      int location = king_location;

      // if we found a square it can move to
      if (moves[i].piece_moves[new_index] == 1)
      {
        // create a new state and move piece i to square new_index
        NewState state;
        vector<BasicPiece> new_black = black;
        vector<BasicPiece> new_white = white;

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
        bitset<BOARD_SIZE> attacked = getAttacked(player->opponent->color, state.board, attack);

        // if the king isn't checked after this move is completed
        if (!isAttacked(attacked, location))
        {
          state.current_index = current_index;
          state.new_index = new_index;
          states.push_back(state);
        }

        else // piece can't move here
          moves[i].piece_moves[new_index] == 0;

        num_moves--; // we've accounted for one of the moves
      }

      new_index++; // look at next square on board
    }
  }

  return;
}

// this will pick a random piece and a random location to move the piece to
PieceToMove AI::getRandomPiece(const int random,
  const vector<PieceToMove>& moves, const vector<NewState>& states)
{
  PieceToMove rand_piece;

  // get all completely valid moves for this piece
  for (int i = 0; i < moves.size(); i++)
  {
    int index = getIndex(moves[i].piece_rank, moves[i].piece_file);

    if (index == states[random].current_index)
    {
      rand_piece = moves[i];
      break;
    }
  }

  return rand_piece;
}

// if the last piece moved affects castling ability, update variables
void AI::updateCastlingAbility(const NewState& state)
{
  if (player->color == BLACK)
  {
    if (state.current_index == B_ROOK_LEFT)
      castle.queen_rook_moved = true;
    else if (state.current_index == B_ROOK_RIGHT)
      castle.king_rook_moved = true;
    else if (state.current_index == B_KING)
      castle.king_moved = true;
  }

  else // my color is white
  {
    if (state.current_index == W_ROOK_LEFT)
      castle.queen_rook_moved = true;
    else if (state.current_index == W_ROOK_RIGHT)
      castle.king_rook_moved = true;
    else if (state.current_index == W_KING)
      castle.king_moved = true;
  }

  return;
}

// this function will send the move to the game server and print all valid moves
void AI::makeMove(const int old_index, const int new_index, bitset<BOARD_SIZE> moves)
{
  // gather all location info
  string from_file = getFile(old_index);
  int from_rank = getRank(old_index);
  string to_file = getFile(new_index);
  int to_rank = getRank(new_index);

  // for all pieces
  for (auto piece : player->pieces)
  {
    // if piece is the one we intend to move
    if (piece->file == from_file && piece->rank == from_rank)
    {
      cout << piece->type << " on " << from_file << from_rank << " can move to:" << endl;

      // output all completely valid moves
      for (int i = 0; i < moves.size(); i++)
      {
        if (moves[i] == 1)
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
        piece->move(to_file, to_rank, "Knight");
      break;
    }
  }

  return;
}

} // chess

} // cpp_client
