#ifndef __ED900_GAME_COUNT_UP__
#define __ED900_GAME_COUNT_UP__

#include "game.h"

namespace ed900::game
{

  class CountUp : public Game
  {
    private:
      std::vector<Score> players;

    private:
      std::vector<Score> scores () const;
      void dart (uint8_t value, uint8_t multiplier);
      void cancel ();

    public:
      CountUp (const std::vector<std::string> & players);
      bool is_finished () const;
      void render (App *) const;
  };

} // ed900::game

#endif // __ED900_GAME_COUNT_UP__

