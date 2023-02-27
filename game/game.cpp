#include <algorithm>
#include <iostream>

#include "game.h"
#include "../app.h"

using namespace std;

namespace ed900::game
{

  Game::State::State (uint8_t p, uint8_t r) :
    rounds {r},
    round {0},
    players {p},
    player {0},
    darts {}
  {
  }
    
  Game::State & Game::State::operator+= (const DartEvent & e)
  {
    darts.push_back (e);
    return *this;
  }

  bool Game::State::is_waiting () const
  {
    return darts.size () >= 3;
  }

  bool Game::State::is_finished () const
  {
    return round >= rounds || (round == rounds-1 && player == players-1 && is_waiting ());
  }

  void Game::State::next ()
  {
    darts.clear ();
    ++player;
    if (player == players)
    {
      ++round;
      player = 0;
    }
  }

  Game::Game (uint8_t players, uint8_t rounds) :
    state {players, rounds},
    stack {}
  {
  }

  uint16_t Game::best_score () const
  {
    vector<Score> s = scores ();
    return max_element (s.begin (), s.end (), HighScore)->score;
  }

  void Game::dart (const DartEvent & e)
  {
    if (! state.is_waiting () && ! is_finished ())
    {
      stack.push (state);
      state += e;

      push ();
      dart (e.value, e.multiplier);
    }
  }

  bool Game::button (const ButtonEvent & e)
  {
    switch (e.button)
    {
      case Button::NEXT:
      {
        if (is_finished ())
          return true;

        stack.push (state);
        push ();
        for (uint8_t k = state.darts.size (); k < 3; ++k)
          dart (0, 0);

        state.next ();
        return false;
      }

      case Button::CANCEL:
        if (! stack.empty ())
        {
          pop ();

          state = stack.top ();
          stack.pop ();
        }
        return false;

      default:
        return false;
    }
  }

  void Game::render_progress (App * app) const
  {
    static Color ACTIVE {192, 192, 192, 255};
    static Color INACTIVE {64, 64, 64, 255};

    uint8_t r = state.round + 1;
    uint8_t n = state.rounds;

    for (uint8_t k = 0; k < r; ++k)
      app->draw (Rect {64 - n + 2*k, 0, 1, 1}, ACTIVE);

    for (uint8_t k = r; k < n; ++k)
      app->draw (Rect {64 - n + 2*k, 0, 1, 1}, INACTIVE);
  }

  void Game::render_darts (App * app) const
  {
    uint8_t n = min<uint8_t> (3, state.darts.size ());

    for (uint8_t i = 0; i < n; ++i)
      app->draw (state.darts [i].text (true), {0, 23 + 6 * i}, 0);
  }

  void Game::render_scores (App * app, bool player) const
  {
    static const int W = 6*4 + 1;
    static const int H = 6 + 8 + 1;

    static const Rect R [] =
    {
      {0,     0,    W, H},
      {128-W, 0,    W, H},
      {0,     64-H, W, H},
      {128-W, 64-H, W, H}
    };

    vector<Score> S = this->scores ();
    for (uint8_t i = 0; i < S.size (); ++i)
    {
      const Score & s = S [i];
      const Rect & r = R [i];
      const Color & c = App::COLORS [i];
      bool active = (i == state.player);

      if (active)
        app->draw (r, c);

      Blender b = active ? blend::ALPHA : blend::ModAlpha (c);
      app->draw (s.name, r, {1, 1}, 0, b);
      string score = to_string (s.score);
      size_t n = score.size ();
      if (n <= 3)
        app->draw (score, r, {1 + (3 - n) * 8, 7}, 1, b);
      else
        app->draw (score, r, {1 + (6 - n) * 4, 9}, 0, b);
    }

    if (player)
    {
      const Score & s = S [state.player];
      string score = to_string (s.score);
      size_t n = score.size ();
      app->draw (score, {64 - 16 * n, 32 - 16 + 1}, 2);
    }
  }

  void Game::render_message (App * app) const
  {
    auto curtain = [app] (const string & text) {
      static const Rect R {0, 0, 128, 64};
      static const Color C {0, 0, 0, 192};
      app->draw (R, C, blend::ALPHA);
      uint8_t n = text.size ();
      app->draw (text, {64 - n*16/2, 18}, 3);
    };

    if (is_finished ())
    {
      vector<Score> S = scores ();
      uint16_t m = best_score ();

      // Winner(s).
      vector<uint8_t> W;
      for (uint8_t k = 0; k < S.size (); ++k)
        if (S[k].score == m) W.push_back (k);

      if (W.size () == 1)
        curtain (S[W[0]].name);
      else
        curtain ("DRAW");
    }
    else
      if (state.is_waiting ())
        curtain ("NEXT");
  }

  bool Game::HighScore (const Score & s1, const Score & s2)
  {
    return s1.score < s2.score;
  }

} // ed900::game

