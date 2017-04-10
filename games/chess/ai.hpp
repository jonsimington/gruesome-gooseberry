#ifndef GAMES_CHESS_AI_HPP
#define GAMES_CHESS_AI_HPP

#include "impl/chess.hpp"
#include "game.hpp"
#include "game_object.hpp"
#include "move.hpp"
#include "piece.hpp"
#include "player.hpp"

#include "../../joueur/src/base_ai.hpp"
#include "../../joueur/src/attr_wrapper.hpp"

// You can add additional #includes here
#include "attack_piece.h"
#include "chessboard.h"
#include <chrono>
#include <queue>
#include <unordered_map>
using namespace std;

namespace cpp_client
{

namespace chess
{

/// <summary>
/// This is the header file for building your Chess AI
/// </summary>
class AI : public Base_ai
{
public:
    /// <summary>
    /// This is a reference to the Game object itself, it contains all the information about the current game
    /// </summary>
    Game game;

    /// <summary>
    /// This is a pointer to your AI's player. This AI class is not a player, but it should command this Player.
    /// </summary>
    Player player;

    // You can add additional class variables here.
    AttackPiece attack; // every possible move for any piece on any square
    CastleCheck castle; // whether or not castling is possible
    unordered_map<string, int> history_table;

    /// <summary>
    /// This returns your AI's name to the game server.
    /// Replace the string name.
    /// </summary>
    /// <returns>The name of your AI.</returns>
    virtual std::string get_name() const override;

    /// <summary>
    /// This is automatically called when the game first starts, once the game objects are created
    /// </summary>
    virtual void start() override;

    /// <summary>
    /// This is automatically called when the game ends.
    /// </summary>
    /// <param name="won">true if you won, false otherwise</param>
    /// <param name="reason">An explanation for why you either won or lost</param>
    virtual void ended(bool won, const std::string& reason) override;

    /// <summary>
    /// This is automatically called the game (or anything in it) updates
    /// </summary>
    virtual void game_updated() override;

    /// <summary>
    /// This is called every time it is this AI.player's turn.
    /// </summary>
    /// <returns>Represents if you want to end your turn. True means end your turn, False means to keep your turn going and re-call this function.</returns>
    bool run_turn();

    /// <summary>
    ///  Prints the current board using pretty ASCII art
    /// </summary>
    /// <remarks>
    /// Note: you can delete this function if you wish
    /// </remarks>
    void print_current_board();

    // my functions
    void generateMoves();
    void initializeCastling();
    bool initializeEnPassant();
    int initializeKingLocation(const vector<BasicPiece>& pieces);
    void initializePieces(vector<BasicPiece>& black_pieces, vector<BasicPiece>& white_pieces);
    void findMovablePieces(const string color, vector<PieceToMove>& movable_pieces,
      Chessboard& board, const bitset<BOARD_SIZE>& attacked,
      const int king_location, const int en_passant_square);
    void findMoves(const string color, const int king_location, const Chessboard& board,
      vector<PieceToMove>& moves, priority_queue<State, vector<State>, greater<State>>& states);
    State minimax(priority_queue<State, vector<State>, greater<State>>& states, const int depth);
    int minimaxValue(State& state, const int depth, int alpha, int beta, bool max);
    void updateCastlingAbility(const int current_index);
    bool drawSetup();
    void makeMove(const int old_index, const int new_index);

    // ####################
    // Don't edit these!
    // ####################
    /// \cond FALSE
    virtual std::string invoke_by_name(const std::string& name,
                                       const std::unordered_map<std::string, Any>& args) override;
    virtual void set_game(Base_game* ptr) override;
    virtual void set_player(std::shared_ptr<Base_object> obj) override;
    virtual void print_win_loss_info() override;
    /// \endcond
    // ####################
    // Don't edit these!
    // ####################

};

} // CHESS

} // cpp_client

#endif // GAMES_CHESS_AI_HPP
