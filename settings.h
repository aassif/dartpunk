#ifndef __DARTPUNK_SETTINGS__
#define __DARTPUNK_SETTINGS__

#include "menu/players.h"
#include "menu/x01.h"

namespace dartpunk
{

  namespace game
  {
    class Game;
  }

  class Settings
  {
    public:
      enum class State : uint8_t
      {
        PLAYERS = 1,
        GAME = 2,
        COUNT_UP = 3,
        X01_START = 4,
        X01_OPTIONS = 5,
        CRICKET = 6,
        TIC_TAC_TOE = 7,
        HALF_IT = 8
      };

    private:
      State state;
      menu::Players players;
      menu::Menu game;
      menu::Menu x01_start;
      menu::X01 x01_options;
      menu::Menu cricket;

    private:
      menu::Abstract * menu ();

    public:
      Settings ();
      void select (uint8_t);
      game::Game * confirm ();
      void cancel ();
      void render (App *);
  };

} // dartpunk

#endif // __DARTPUNK_SETTINGS__

