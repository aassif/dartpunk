#ifndef __DARTPUNK_GAME_CRICKET__
#define __DARTPUNK_GAME_CRICKET__

#include <array>

#include "game.h"

namespace dartpunk::game
{

  class Cricket : public Game
  {
    public:
      enum Mode
      {
        STANDARD = 0,
        CUTTHROAT = 1,
        RANDOM = 2,
        HIDDEN = 3,
      };

    private:
      class Player : public Score
      {
        public:
          std::array<uint8_t, 7> targets;

        public:
          Player (const std::string &);
      };

      typedef std::vector<Player> Players;

    private:
      Mode mode;
      std::array<uint8_t, 7> targets;
      Players players;
      std::stack<Players> stack;

    private:
      uint16_t best_score () const;
      std::vector<Score> scores () const;
      void dart_score (uint8_t target, uint8_t multiplier);
      void dart (uint8_t value, uint8_t multiplier);
      void push ();
      void pop ();
      void render_columns (App *) const;

    public:
      Cricket (const std::vector<std::string> & players, Mode);
      bool is_finished () const;
      void render (App *) const;

    private:
      static std::array<uint8_t, 7> Targets (Mode);
  };

} // dartpunk::game

#endif // __DARTPUNK_GAME_CRICKET__

