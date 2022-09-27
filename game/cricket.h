#ifndef __ED900_GAME_CRICKET__
#define __ED900_GAME_CRICKET__

#include <array>

#include "game.h"

namespace ed900::game
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

    private:
      Mode mode;
      std::array<uint8_t, 7> targets;
      std::vector<Player> players;

    private:
      uint16_t best_score () const;
      std::vector<Score> scores () const;
      void dart_score (uint8_t target, uint8_t multiplier);
      void dart (uint8_t value, uint8_t multiplier);
      void cancel ();
      void render_columns (App *) const;

    public:
      Cricket (const std::vector<std::string> & players, Mode);
      bool is_finished () const;
      void render (App *) const;

    private:
      static std::array<uint8_t, 7> Targets (Mode);
  };

} // ed900::game

#endif // __ED900_GAME_CRICKET__

