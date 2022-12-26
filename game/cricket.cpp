#include <algorithm>
#include <random>

#include "../app.h"
#include "cricket.h"

using namespace std;

namespace ed900::game
{

  array<uint8_t, 7> Cricket::Targets (Mode m)
  {
    switch (m)
    {
      case STANDARD:
      case CUTTHROAT:
        return {20, 19, 18, 17, 16, 15, 25};

      case RANDOM:
      case HIDDEN:
      {
        array<uint8_t, 20> t20;
        iota (t20.begin (), t20.end (), 1);

        random_device rd;
        default_random_engine g {rd ()};
        shuffle (t20.begin (), t20.end (), g);

        array<uint8_t, 6> t6;
        copy_n (t20.begin (), 6, t6.begin ());
        sort (t6.begin (), t6.end (), greater<uint8_t> ());
        return {t6[0], t6[1], t6[2], t6[3], t6[4], t6[5], 25};
      }
    }

    return array<uint8_t, 7> {};
  }

  Cricket::Player::Player (const string & n) :
    Score {n, 0},
    targets {}
  {
  }

  Cricket::Cricket (const vector<string> & names, Mode m) :
    Game {names.size (), 20},
    mode {m},
    targets {Targets (m)},
    players {}
  {
    for (auto & n : names)
      players.push_back (Player {n});
  }

  vector<Game::Score> Cricket::scores () const
  {
    return vector<Score> {players.begin (), players.end ()};
  }

  uint16_t Cricket::best_score () const
  {
    auto [min, max] = minmax_element (players.begin (), players.end (), HighScore);
    return mode == CUTTHROAT ? min->score : max->score;
  }

  void Cricket::dart_score (uint8_t target, uint8_t multiplier)
  {
    auto target_is_open = [target] (const Player & p) {
      return p.targets[target] >= 3u;
    };

    switch (mode)
    {
      case STANDARD:
      case RANDOM:
      case HIDDEN:
        if (! all_of (players.begin (), players.end (), target_is_open))
          players[state.player].score += targets[target] * multiplier;
        break;

      case CUTTHROAT:
        for (auto & p : players)
          if (! target_is_open (p))
            p.score += targets[target] * multiplier;
        break;
    }
  }

  void Cricket::dart (uint8_t v, uint8_t m)
  {
    auto f = find (targets.begin (), targets.end (), v);
    if (f != targets.end ())
    {
      uint8_t p = state.player;
      uint8_t t = distance (targets.begin (), f);
      if (players[p].targets[t] < 3u)
      {
        uint8_t d = min<uint8_t> (3 - players[p].targets[t], m);
        players[p].targets[t] += d;
        dart_score (t, m - d);
      }
      else
        dart_score (t, m);
    }
  }

  void Cricket::cancel ()
  {
  }

  bool Cricket::is_finished () const
  {
    auto player_has_finished = [this] (const Player & p) {
      auto target_is_open = [] (uint8_t t) {return t >= 3u;};
      return all_of (p.targets.begin (), p.targets.end (), target_is_open) && p.score == best_score ();
    };

    return state.is_finished () || any_of (players.begin (), players.end (), player_has_finished);
  }

  void Cricket::render_columns (App * app) const
  {
    for (uint8_t i = 0; i < 6; ++i)
    {
      string item = to_string (targets [i]);
      app->draw (item, 64 - (targets[i] < 10 ? 4 : 8), 4 + i*8, 1);
    }

    app->draw ("BULL", 64-8, 4 + 6*8 + 1, 0);

    static const Rect R [] =
    {
      {30,       3, 9, 57},
      {42,       3, 9, 57},
      {128-42-9, 3, 9, 57},
      {128-30-9, 3, 9, 57}
    };

    static const uint8_t P [][4] =
    {
      {},
      {1},         // 1 player
      {1, 2},      // 2 players
      {0, 1, 2},   // 3 players
      {0, 1, 2, 3} // 4 players
    };

    size_t p = players.size ();
    for (uint8_t i = 0; i < p; ++i)
    {
      const Rect & r = R [P[p][i]];
      app->draw (r, App::COLORS [i], blend::NONE);
      for (uint8_t j = 0; j < 7; ++j)
      {
        uint8_t n = min<uint8_t> (players[i].targets[j], 3);
        app->draw (to_string (n), r.x + 1, r.y + 8*j + 1, 4);
      }
    }

    auto target_is_closed = [this] (uint8_t target) {
      auto target_is_open = [target] (const Player & p) {return p.targets[target] >= 3u;};
      return all_of (players.begin (), players.end (), target_is_open);
    };

    for (uint8_t j = 0; j < 7;)
      if (target_is_closed (j))
      {
        uint8_t h = 1;
        while (j+h < 7 && target_is_closed (j+h)) ++h;

        uint8_t a = (j   != 0 ? 1 : 0);
        uint8_t b = (j+h != 7 ? a : 0);

        static const Color BLACK {0, 0, 0, 96};
        app->draw (Rect {30, 3+8*j+0, 68, 8*h+1}, BLACK, blend::BLEND);
        app->draw (Rect {30, 3+8*j+a, 68, 8*h-b}, BLACK, blend::BLEND);

        j += h;
      }
      else ++j;
  }

  void Cricket::render (App * app) const
  {
    render_progress (app);
    render_history (app);
    render_scores (app);
    render_columns (app);
    render_message (app);
  }

} // ed900::game

