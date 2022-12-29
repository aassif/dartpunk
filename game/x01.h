#ifndef __ED900_GAME_X01__
#define __ED900_GAME_X01__

#include "game.h"

namespace ed900::game
{

  class X01 : public Game
  {
    private:
      class Player : public Score
      {
        public:
          uint16_t busted;

        public:
          Player (const std::string &, uint16_t start);
      };

      typedef std::vector<Player> Players;

    private:
      uint16_t start;
      bool double_in;
      bool double_out;
      Players players;
      std::stack<Players> stack;

    private:
      uint16_t best_score () const;
      std::vector<Score> scores () const;
      void dart (uint8_t value, uint8_t multiplier);
      void push ();
      void pop ();

    public:
      X01 (const std::vector<std::string> & players, uint16_t start, bool double_in, bool double_out);
      bool is_finished () const;
      void render (App *) const;
  };

} // ed900::game

#endif // __ED900_GAME_X01__

