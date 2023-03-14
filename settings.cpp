#include <utility> // std::pair

#include "settings.h"

#include "game/count-up.h"
#include "game/x01.h"
#include "game/cricket.h"

namespace dartpunk
{

  Settings::Settings () :
    state {State::PLAYERS},
    players {},
    game {{"CountUp", "X01", "Cricket", "TicTacToe"}},
    x01_start {{"301", "501", "701", "901"}},
    x01_options {},
    cricket {{"Standard", "CutThroat", "Random", "Hidden"}}
  {
  }

  menu::Abstract * Settings::menu ()
  {
    switch (state)
    {
      case State::PLAYERS     : return &players;
      case State::GAME        : return &game;
      case State::X01_START   : return &x01_start;
      case State::X01_OPTIONS : return &x01_options;
      case State::CRICKET     : return &cricket;
      default                 : return nullptr;
    }
  }

  void Settings::select (uint8_t k)
  {
    menu::Abstract * m = menu ();
    if (m) m->select (k);
  }

  game::Game * Settings::confirm ()
  {
    switch (state)
    {
      case State::PLAYERS:
        if (players.confirm ())
          state = State::GAME;
        break;

      case State::GAME:
        switch (game)
        {
          case 1: return new game::CountUp {players};
          case 2: state = State::X01_START;   break;
          case 3: state = State::CRICKET;     break;
          //case 4: state = State::TIC_TAC_TOE; break;
        }
        break;

      case State::X01_START:
        if (x01_start)
          state = State::X01_OPTIONS;
        break;

      case State::X01_OPTIONS:
      {
        static const uint16_t S [] {0, 301, 501, 701, 901};
        uint16_t start = S [x01_start];
        std::pair<bool, bool> o {x01_options};
        return new game::X01 {players, start, o.first, o.second};
      }

      case State::CRICKET:
        switch (cricket)
        {
          case 1: return new game::Cricket {players, game::Cricket::STANDARD};
          case 2: return new game::Cricket {players, game::Cricket::CUTTHROAT};
          case 3: return new game::Cricket {players, game::Cricket::RANDOM};
          case 4: return new game::Cricket {players, game::Cricket::HIDDEN};
        }
        break;
    }

    return nullptr;
  }

  void Settings::cancel ()
  {
    switch (state)
    {
      case State::PLAYERS:
        players.cancel ();
        break;

      case State::GAME:
        state = State::PLAYERS;
        players.cancel ();
        break;

      case State::X01_START:
        state = State::GAME;
        break;

      case State::X01_OPTIONS:
        state = State::X01_START;
        break;

      case State::CRICKET:
        state = State::GAME;
        break;
    }
  }

  void Settings::render (App * app)
  {
    menu::Abstract * m = menu ();
    if (m) m->render (app);
  }

} // dartpunk

