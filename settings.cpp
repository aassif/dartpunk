#include <utility> // std::pair

#include "settings.h"

#include "game/count-up.h"
#include "game/x01.h"
#include "game/cricket.h"

namespace ed900
{

  Settings::Settings (App * a) :
    state {State::PLAYERS},
    players {a},
    game {a, {"CountUp", "X01", "Cricket", "TicTacToe"}},
    x01_start {a, {"301", "501", "701", "901"}},
    x01_options {a},
    cricket {a, {"Standard", "CutThroat", "Random", "Hidden"}}
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
        switch (game.index ())
        {
          case 1: return new game::CountUp (players);
          case 2: state = State::X01_START;   break;
          case 3: state = State::CRICKET;     break;
          //case 4: state = State::TIC_TAC_TOE; break;
        }
        break;

      case State::X01_START:
        if (game.index ())
          state = State::X01_OPTIONS;
        break;

      case State::X01_OPTIONS:
      {
        static const uint16_t S [] = {0, 301, 501, 701, 901};
        uint16_t start = S [x01_start.index ()];
        std::pair<bool, bool> o {x01_options};
        return new game::X01 (players, start, o.first, o.second);
      }

      case State::CRICKET:
        switch (cricket.index ())
        {
          case 1: return new game::Cricket (players, game::Cricket::STANDARD);
          case 2: return new game::Cricket (players, game::Cricket::CUTTHROAT);
          case 3: return new game::Cricket (players, game::Cricket::RANDOM);
          case 4: return new game::Cricket (players, game::Cricket::HIDDEN);
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

  void Settings::render ()
  {
    menu::Abstract * m = menu ();
    if (m) m->render ();
  }

}

