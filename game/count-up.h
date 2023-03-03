#ifndef __DARTPUNK_GAME_COUNT_UP__
#define __DARTPUNK_GAME_COUNT_UP__

#include "game.h"

namespace dartpunk::game
{

  class CountUp : public Game
  {
    private:
      typedef std::vector<Score> Players;

    private:
      Players players;
      std::stack<Players> stack;

    private:
      std::vector<Score> scores () const;
      void dart (uint8_t value, uint8_t multiplier);
      void push ();
      void pop ();

    public:
      CountUp (const std::vector<std::string> & players);
      bool is_finished () const;
      void render (App *) const;
  };

} // dartpunk::game

#endif // __DARTPUNK_GAME_COUNT_UP__

