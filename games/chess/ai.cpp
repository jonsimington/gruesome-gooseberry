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
  initializePieces(black_pieces, white_pieces);
  Chessboard board;
  board.readBoard(black_pieces, white_pieces);
  bitset<BOARD_SIZE> attacked = getAttacked(player->opponent->color, board, attack);

  // generate new moves
  vector<PieceToMove> movable_pieces;
  vector<State> possible_states;

  // check if en passant is possible
  bool can_en_passant = initializeEnPassant();
  int en_passant_square = -1;

  if (can_en_passant)
  {
    int current_index = getIndex(game->moves[game->moves.size() - 1]->from_rank,
      game->moves[game->moves.size() - 1]->from_file);
    en_passant_square = current_index + HEIGHT_WIDTH * (player->color == BLACK ? 1 : -1);
  }

  // get the current location of the king
  int king_location = initializeKingLocation((player->color == BLACK ? black_pieces : white_pieces));

  // check whether king has moved
  if ((player->color == BLACK && king_location != B_KING) ||
      (player->color == WHITE && king_location != W_KING))
  {
    castle.king_moved = true;
  }

  // find all movable pieces and their possible moves (don't worry about check)
  findMovablePieces(player->color, movable_pieces, board, attacked,
    king_location, en_passant_square);

  // for all movable pieces, find valid moves
  findMoves(player->color, king_location, board, movable_pieces, possible_states);

  State best_move;

  // find the best move with iterative deepening
  // maximum of TIME seconds per move
  int depth = 1;
  auto start = chrono::system_clock::now();

  while (true)
  {
    best_move = minimax(possible_states, depth);
    cout << "MAX: " << best_move.utility << endl;

    auto duration = chrono::duration_cast<chrono::nanoseconds>(chrono::system_clock::now() - start);

    // if we found checkmate or we've spent too long thinking
    if (best_move.checkmate || duration > chrono::nanoseconds(TIME))
    {
      break;
    }

    depth++;
  }

  // update castling
  updateCastlingAbility(best_move.current_index);

  // send the move to the game server
  cout << "From " << getFile(best_move.current_index) << getRank(best_move.current_index)
    << " to " << getFile(best_move.new_index) << getRank(best_move.new_index) << endl;
  makeMove(best_move.current_index, best_move.new_index);
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
  // skip from end of FEN string to the castling section (3 whitespaces)
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
bool AI::initializeEnPassant()
{
  bool can_en_passant = false;

  // if the opponent moved before this turn
  if (game->moves.size() > 0)
  {
    string last_move = game->moves[game->moves.size() - 1]->san;

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
    // en passant section of FEN string
    if (game->fen[game->fen.size() - EN_PASSANT_FEN] != '-')
      can_en_passant = false;
  }

  return can_en_passant;
}

// this returns the king's index
int AI::initializeKingLocation(const vector<BasicPiece>& pieces)
{
  for (auto piece : pieces)
  {
    if (piece.type == "King")
      return piece.index;
  }
}

// copy all piece information into two vectors (which will then generate bitboards)
void AI::initializePieces(vector<BasicPiece>& black_pieces, vector<BasicPiece>& white_pieces)
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
void AI::findMovablePieces(const string color, vector<PieceToMove>& movable_pieces,
  Chessboard& board, const bitset<BOARD_SIZE>& attacked,
  const int king_location, const int en_passant_square)
{
  // king movements
  {
    PieceToMove piece_to_move;
    piece_to_move.piece_moves = board.getKingMoves(color, king_location, attack);

    // check if castling possible
    int left;
    int right;

    // king-side
    // if the pieces haven't moved and the king isn't in check
    if (!castle.king_moved && !castle.king_rook_moved && !isAttacked(attacked, king_location))
    {
      bool can_castle = true;

      if (color == BLACK)
        right = B_ROOK_RIGHT;
      else
        right = king_location + W_ROOK_RIGHT;

      // if squares between king and rook are attacked or blocked
      for (int i = king_location + LEFT_RIGHT; i <= right; i++)
      {
        if (isAttacked(attacked, i) || board.all_pieces[i] == 1)
          can_castle = false;
      }

      if (can_castle)
        piece_to_move.piece_moves[king_location + CASTLE] = 1;
    }

    // queen-side
    // if the pieces haven't moved and the king isn't in check
    if (!castle.king_moved && !castle.queen_rook_moved && !isAttacked(attacked, king_location))
    {
      bool can_castle = true;

      if (color == BLACK)
        left = B_ROOK_LEFT + LEFT_RIGHT;
      else
        left = W_ROOK_LEFT + LEFT_RIGHT;

      // if squares between king and rook are attacked or blocked
      for (int i = left; i < king_location; i++)
      {
        if (isAttacked(attacked, i) || board.all_pieces[i] == 1)
          can_castle = false;
      }

      if (can_castle)
        piece_to_move.piece_moves[king_location - CASTLE] = 1;
    }

    // if the piece has possible moves
    if (piece_to_move.piece_moves.count() > 0)
    {
      piece_to_move.piece_rank = getRank(king_location);
      piece_to_move.piece_file = getFile(king_location);
      piece_to_move.piece_type = "King";
      movable_pieces.push_back(piece_to_move);
    }
  }

  // queen movements
  for (int i = 0; i < BOARD_SIZE; i++)
  {
    if ((color == BLACK && board.black[QUEEN][i] == 1) ||
      (color == WHITE && board.white[QUEEN][i] == 1))
    {
      PieceToMove piece_to_move;
      piece_to_move.piece_moves = board.getQueenMoves(color, i, attack);

      // if the piece has possible moves
      if (piece_to_move.piece_moves.count() > 0)
      {
        piece_to_move.piece_rank = getRank(i);
        piece_to_move.piece_file = getFile(i);
        piece_to_move.piece_type = "Queen";
        movable_pieces.push_back(piece_to_move);
      }
    }
  }

  // rook movements
  for (int i = 0; i < BOARD_SIZE; i++)
  {
    if ((color == BLACK && board.black[ROOK][i] == 1) ||
      (color == WHITE && board.white[ROOK][i] == 1))
    {
      PieceToMove piece_to_move;
      piece_to_move.piece_moves = board.getRookMoves(color, i, attack);

      // if the piece has possible moves
      if (piece_to_move.piece_moves.count() > 0)
      {
        piece_to_move.piece_rank = getRank(i);
        piece_to_move.piece_file = getFile(i);
        piece_to_move.piece_type = "Rook";
        movable_pieces.push_back(piece_to_move);
      }
    }
  }

  // bishop movements
  for (int i = 0; i < BOARD_SIZE; i++)
  {
    if ((color == BLACK && board.black[BISHOP][i] == 1) ||
      (color == WHITE && board.white[BISHOP][i] == 1))
    {
      PieceToMove piece_to_move;
      piece_to_move.piece_moves = board.getBishopMoves(color, i, attack);

      // if the piece has possible moves
      if (piece_to_move.piece_moves.count() > 0)
      {
        piece_to_move.piece_rank = getRank(i);
        piece_to_move.piece_file = getFile(i);
        piece_to_move.piece_type = "Bishop";
        movable_pieces.push_back(piece_to_move);
      }
    }
  }

  // knight movements
  for (int i = 0; i < BOARD_SIZE; i++)
  {
    if ((color == BLACK && board.black[KNIGHT][i] == 1) ||
      (color == WHITE && board.white[KNIGHT][i] == 1))
    {
      PieceToMove piece_to_move;
      piece_to_move.piece_moves = board.getKnightMoves(color, i, attack);

      // if the piece has possible moves
      if (piece_to_move.piece_moves.count() > 0)
      {
        piece_to_move.piece_rank = getRank(i);
        piece_to_move.piece_file = getFile(i);
        piece_to_move.piece_type = "Knight";
        movable_pieces.push_back(piece_to_move);
      }
    }
  }

  // pawn movements
  for (int i = 0; i < BOARD_SIZE; i++)
  {
    if ((color == BLACK && board.black[PAWN][i] == 1) ||
      (color == WHITE && board.white[PAWN][i] == 1))
    {
      PieceToMove piece_to_move;
      piece_to_move.piece_moves = board.getPawnMoves(color, i, attack);

      // en passant
      if (en_passant_square != -1)
      {
        if ((color == BLACK && attack.attacking_b_pawn[i][en_passant_square] == 1) ||
          (color == WHITE && attack.attacking_w_pawn[i][en_passant_square] == 1))
        {
          piece_to_move.piece_moves[en_passant_square] = 1;
        }
      }

      // if the piece has possible moves
      if (piece_to_move.piece_moves.count() > 0)
      {
        piece_to_move.piece_rank = getRank(i);
        piece_to_move.piece_file = getFile(i);
        piece_to_move.piece_type = "Pawn";
        movable_pieces.push_back(piece_to_move);
      }
    }
  }

  return;
}

// this filters out all illegal moves from all movable pieces
void AI::findMoves(const string color, const int king_location, const Chessboard& board,
  vector<PieceToMove>& moves, vector<State>& states)
{
  // need to look at moves of my own prediction, not just actually completed moves
  bool near_draw = drawSetup();

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
        State state;
        vector<BasicPiece> new_black;
        vector<BasicPiece> new_white;

        // find all pieces on the bitboard
        for (int i = 0; i < BOARD_SIZE; i++)
        {
          BasicPiece piece;
          piece.index = i;

          if (board.black[KING][i] == 1)
          {
            piece.type = "King";
            new_black.push_back(piece);
          }

          else if (board.black[QUEEN][i] == 1)
          {
            piece.type = "Queen";
            new_black.push_back(piece);
          }

          else if (board.black[ROOK][i] == 1)
          {
            piece.type = "Rook";
            new_black.push_back(piece);
          }

          else if (board.black[BISHOP][i] == 1)
          {
            piece.type = "Bishop";
            new_black.push_back(piece);
          }

          else if (board.black[KNIGHT][i] == 1)
          {
            piece.type = "Knight";
            new_black.push_back(piece);
          }

          else if (board.black[PAWN][i] == 1)
          {
            piece.type = "Pawn";
            new_black.push_back(piece);
          }

          else if (board.white[KING][i] == 1)
          {
            piece.type = "King";
            new_white.push_back(piece);
          }

          else if (board.white[QUEEN][i] == 1)
          {
            piece.type = "Queen";
            new_white.push_back(piece);
          }

          else if (board.white[ROOK][i] == 1)
          {
            piece.type = "Rook";
            new_white.push_back(piece);
          }

          else if (board.white[BISHOP][i] == 1)
          {
            piece.type = "Bishop";
            new_white.push_back(piece);
          }

          else if (board.white[KNIGHT][i] == 1)
          {
            piece.type = "Knight";
            new_white.push_back(piece);
          }

          else if (board.white[PAWN][i] == 1)
          {
            piece.type = "Pawn";
            new_white.push_back(piece);
          }
        }

        if (color == BLACK)
        {
          // for each black piece
          for (int b = 0; b < new_black.size(); b++)
          {
            // found the piece we want to move
            if (new_black[b].index == current_index)
            {
              state.type = new_black[b].type;

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
              new_white[del].index = -1;
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
              state.type = new_white[w].type;

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
              new_black[del].index = -1;
          }
        }

        // generate new bitboard
        state.board.readBoard(new_black, new_white);
        bitset<BOARD_SIZE> attacked =
          getAttacked((color == BLACK ? WHITE : BLACK), state.board, attack);

        // true if this move will cause a simplified draw
        bool will_draw = false;

        // if we're one move from a draw
        if (near_draw)
        {
          string from_file_1 = game->moves[game->moves.size() - DRAW_SHIFT]->from_file;
          string to_file_1 = game->moves[game->moves.size() - DRAW_SHIFT]->to_file;
          int from_rank_1 = game->moves[game->moves.size() - DRAW_SHIFT]->from_rank;
          int to_rank_1 = game->moves[game->moves.size() - DRAW_SHIFT]->to_rank;

          string from_file_2 = getFile(current_index);
          string to_file_2 = getFile(new_index);
          int from_rank_2 = getRank(current_index);
          int to_rank_2 = getRank(new_index);

          // if this move is identical to the move made 4 plys ago
          if (from_file_1 == from_file_2 && to_file_1 == to_file_2
            && from_rank_1 == from_rank_2 && to_rank_1 == to_rank_2)
          {
            will_draw = true;
          }
        }

        // if the king isn't checked after this move is completed and
        // if this move doesn't lead to a simplified draw
        if (!isAttacked(attacked, location) && !will_draw)
        {
          state.checkmate = false;
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

// returns true if one move from a draw; false otherwise
bool AI::drawSetup()
{
  // assume we're not ready for a draw
  bool draw_ready = false;

  // if there have been 7+ moves
  if (game->moves.size() >= DRAW_MOVES)
  {
    // set draw_ready true; make false if draw conditions broken
    draw_ready = true;

    // false if it finds any captures, promotions, or pawn movements
    bool draw_possible = true;

    // for the last seven moves
    for (int i = game->moves.size() - DRAW_MOVES; i < game->moves.size(); i++)
    {
      // if a pawn moved or promoted or a piece was captured
      if (game->moves[i]->promotion != "" || game->moves[i]->piece->type == "Pawn"
        || game->moves[i]->captured != NULL)
      {
        draw_possible = false;
        draw_ready = false;
        break;
      }
    }

    // if there were no pawn movements or pawn promotions or piece captures
    if (draw_possible)
    {
      // start at size - 1; for the last three pairs of moves
      for (int j = 1; j < DRAW_SHIFT; j++)
      {
        // all location information for each move
        string from_file_1 = game->moves[game->moves.size() - j]->from_file;
        string to_file_1 = game->moves[game->moves.size() - j]->to_file;
        int from_rank_1 = game->moves[game->moves.size() - j]->from_rank;
        int to_rank_1 = game->moves[game->moves.size() - j]->to_rank;

        string from_file_2 = game->moves[game->moves.size() - j - DRAW_SHIFT]->from_file;
        string to_file_2 = game->moves[game->moves.size() - j - DRAW_SHIFT]->to_file;
        int from_rank_2 = game->moves[game->moves.size() - j - DRAW_SHIFT]->from_rank;
        int to_rank_2 = game->moves[game->moves.size() - j - DRAW_SHIFT]->to_rank;

        // if the two moves are identical
        if (from_file_1 != from_file_2 || to_file_1 != to_file_2
          || from_rank_1 != from_rank_2 || to_rank_1 != to_rank_2)
        {
          draw_ready = false;
          break;
        }
      } // for last three pairs of moves
    } // if draw conditions met so far
  } // if there have been 7+ moves

  return draw_ready;
}

// this will use IDDLMM to pick which move to make
State AI::minimax(vector<State>& states, const int depth)
{
  State best_move = states[0];
  int best_utility;
  int alpha = INFINITY * -1;
  int beta = INFINITY;

  // initialize utility outside the realm of possibility
  best_utility = MAX_POINTS * -1; // below possible range

  for (auto state : states)
  {
    state.utility = minimaxValue(state, depth - 1, alpha, beta, true);

    // update best move
    if (state.utility > best_utility)
    {
      best_utility = state.utility;
      best_move = state;

      // still outside the realm of possibility
      if (best_utility > CHECKMATE / 2)
      {
        best_move.checkmate = true;
        break;
      }
    }

    // update alpha
    if (state.utility > alpha)
      alpha = state.utility;

    // prune
    if (state.utility >= beta)
    {
      best_move.utility = best_utility;
      return best_move;
    }
  }

  best_move.utility = best_utility;
  return best_move;
}

// returns the max utility if max == true; min utility otherwise
int AI::minimaxValue(State& state, const int depth, int alpha, int beta, bool max)
{
  string color = max ? player->color : player->opponent->color;
  string other_color = max ? player->opponent->color : player->color;

  if (depth == 0)
  {
    if (max)
      return state.board.getUtility(color, attack);
    else
      return state.board.getUtility(color, attack) * -1;
  }

  // generate new moves
  vector<PieceToMove> movable_pieces;
  vector<State> possible_states;
  bitset<BOARD_SIZE> attacked = getAttacked((max ? other_color : color), state.board, attack);

  // get king location
  int king_location;
  for (int i = 0; i < BOARD_SIZE; i++)
  {
    if ((color == BLACK && state.board.black[KING][i] == 1) ||
      (color == WHITE && state.board.white[KING][i] == 1))
    {
      king_location = i;
      break;
    }
  }

  // check if en passant possible
  bool can_en_passant = false;
  int en_passant_square = -1;
  if (state.type == "Pawn")
  {
    // if pawn moved two ranks
    if (abs(state.current_index - state.new_index) == (HEIGHT_WIDTH + HEIGHT_WIDTH))
      can_en_passant = true;
  }

  if (can_en_passant)
    en_passant_square = state.new_index + HEIGHT_WIDTH *
      (other_color == BLACK ? 1 : -1);

  // find all movable pieces and their possible moves (don't worry about check)
  findMovablePieces(color, movable_pieces, state.board,
    attacked, king_location, en_passant_square);

  // for all movable pieces, find valid moves
  findMoves(color, king_location, state.board, movable_pieces, possible_states);

  // if my king has no valid moves and it's checked
  if (state.board.getKingMoves(color, king_location, attack).count() == 0 &&
    isAttacked(attacked, king_location))
  {
    return CHECKMATE * -1;
  }

  // initialize utility outside the realm of possibility
  int best_utility = max ? MAX_POINTS * -1 : MAX_POINTS;

  if (max)
  {
    for (auto state : possible_states)
    {
      int utility = minimaxValue(state, depth - 1, alpha, beta, false);

      // update best move
      if (utility > best_utility)
        best_utility = utility;

      // update alpha
      if (state.utility > alpha)
        alpha = state.utility;

      // prune
      if (state.utility >= beta)
        return best_utility;
    }
  }

  else // min
  {
    for (auto state : possible_states)
    {
      int utility = minimaxValue(state, depth - 1, alpha, beta, true);

      // update best move
      if (utility < best_utility)
        best_utility = utility;

      // update beta
      if (state.utility < beta)
        beta = state.utility;

      // prune
      if (state.utility <= alpha)
        return best_utility;
    }
  }

  return best_utility;
}

// // returns the maximum predicted utility
// int AI::maxValue(State& state, const int depth, int alpha, int beta)
// {
//   if (depth == 0)
//     return state.board.getUtility(player->color, attack);
//
//   // generate new moves
//   vector<PieceToMove> movable_pieces;
//   vector<State> possible_states;
//   bitset<BOARD_SIZE> attacked = getAttacked(player->opponent->color, state.board, attack);
//
//   // get king location
//   int king_location;
//   for (int i = 0; i < BOARD_SIZE; i++)
//   {
//     if ((player->color == BLACK && state.board.black[KING][i] == 1) ||
//       (player->color == WHITE && state.board.white[KING][i] == 1))
//     {
//       king_location = i;
//       break;
//     }
//   }
//
//   // check if en passant possible
//   bool can_en_passant = false;
//   int en_passant_square = -1;
//   if (state.type == "Pawn")
//   {
//     // if pawn moved two ranks
//     if (abs(state.current_index - state.new_index) == (HEIGHT_WIDTH + HEIGHT_WIDTH))
//       can_en_passant = true;
//   }
//
//   if (can_en_passant)
//     en_passant_square = state.new_index + HEIGHT_WIDTH *
//       (player->opponent->color == BLACK ? 1 : -1);
//
//   // find all movable pieces and their possible moves (don't worry about check)
//   findMovablePieces(player->color, movable_pieces, state.board,
//     attacked, king_location, en_passant_square);
//
//   // for all movable pieces, find valid moves
//   findMoves(player->color, king_location, state.board, movable_pieces, possible_states);
//
//   // if my king has no valid moves and it's checked
//   if (state.board.getKingMoves(player->color, king_location, attack).count() == 0 &&
//     isAttacked(attacked, king_location))
//   {
//     return CHECKMATE * -1;
//   }
//
//   // initialize utility outside the realm of possibility
//   int best_utility = MAX_POINTS * -1; // below possible range
//
//   for (auto state : possible_states)
//   {
//     int utility = minValue(state, depth - 1, alpha, beta);
//
//     // update best move
//     if (utility > best_utility)
//       best_utility = utility;
//
//     // update alpha
//     if (state.utility > alpha)
//       alpha = state.utility;
//
//     // prune
//     if (state.utility >= beta)
//       return best_utility;
//   }
//
//   return best_utility;
// }
//
// int AI::minValue(State& state, const int depth, int alpha, int beta)
// {
//   if (depth == 0)
//     return state.board.getUtility(player->opponent->color, attack) * -1;
//
//   // generate new moves
//   vector<PieceToMove> movable_pieces;
//   vector<State> possible_states;
//   bitset<BOARD_SIZE> attacked = getAttacked(player->color, state.board, attack);
//
//   // get king location
//   int king_location;
//   for (int i = 0; i < BOARD_SIZE; i++)
//   {
//     if ((player->opponent->color == BLACK && state.board.black[KING][i] == 1) ||
//       (player->opponent->color == WHITE && state.board.white[KING][i] == 1))
//     {
//       king_location = i;
//       break;
//     }
//   }
//
//   // check if en passant possible
//   bool can_en_passant = false;
//   int en_passant_square = -1;
//   if (state.type == "Pawn")
//   {
//     // if pawn moved two ranks
//     if (abs(state.current_index - state.new_index) == (HEIGHT_WIDTH + HEIGHT_WIDTH))
//       can_en_passant = true;
//   }
//
//   if (can_en_passant)
//     en_passant_square = state.new_index + HEIGHT_WIDTH *
//       (player->color == BLACK ? 1 : -1);
//
//   // find all movable pieces and their possible moves (don't worry about check)
//   findMovablePieces(player->opponent->color, movable_pieces, state.board,
//     attacked, king_location, en_passant_square);
//
//   // for all movable pieces, find valid moves
//   findMoves(player->opponent->color, king_location, state.board, movable_pieces, possible_states);
//
//   // if their king has no valid moves and it's checked
//   if (state.board.getKingMoves(player->opponent->color, king_location, attack).count() == 0 &&
//     isAttacked(attacked, king_location))
//   {
//     return CHECKMATE;
//   }
//
//   // initialize utility outside the realm of possibility
//   int best_utility = MAX_POINTS; // above possible range
//
//   for (auto state : possible_states)
//   {
//     int utility = maxValue(state, depth - 1, alpha, beta);
//
//     // update best move
//     if (utility < best_utility)
//       best_utility = utility;
//
//     // update beta
//     if (state.utility < beta)
//       beta = state.utility;
//
//     // prune
//     if (state.utility <= alpha)
//       return best_utility;
//   }
//
//   return best_utility;
// }

// if the last piece moved affects castling ability, update variables
void AI::updateCastlingAbility(const int current_index)
{
  if (player->color == BLACK)
  {
    if (current_index == B_ROOK_LEFT)
      castle.queen_rook_moved = true;
    else if (current_index == B_ROOK_RIGHT)
      castle.king_rook_moved = true;
    else if (current_index == B_KING)
      castle.king_moved = true;
  }

  else // my color is white
  {
    if (current_index == W_ROOK_LEFT)
      castle.queen_rook_moved = true;
    else if (current_index == W_ROOK_RIGHT)
      castle.king_rook_moved = true;
    else if (current_index == W_KING)
      castle.king_moved = true;
  }

  return;
}

// this function will send the move to the game server and print all valid moves
void AI::makeMove(const int old_index, const int new_index)
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
      piece->move(to_file, to_rank, "Queen");
      break;
    }
  }

  return;
}

} // chess

} // cpp_client
