#include <algorithm>

#include "../app.h"
#include "x01.h"

using namespace std;

namespace ed900::game
{

  X01::Player::Player (const string & name, uint16_t start) :
    Score {name, start},
    busted {start}
  {
  }

  X01::X01 (const vector<string> & names, uint16_t start, bool in, bool out) :
    Game {names.size (), 15},
    start {start},
    double_in {in},
    double_out {out},
    players {}
  {
    for (auto & n : names)
      players.push_back ({n, start});
  }

  uint16_t X01::best_score () const
  {
    return min_element (players.begin (), players.end (), HighScore)->score;
  }

  void X01::dart (uint8_t v, uint8_t m)
  {
    // Dart value.
    uint8_t d = v * m;

    // Active player.
    Player & p = players [state.player];

    auto discard = [] {};

    auto score = [&] () {
      p.score -= d;
      if (state.dart == 2)
        p.busted = p.score;
    };

    auto bust = [&] ()
    {
      p.score = p.busted;
      state.dart = 2;
    };

    if (p.score == start)
      return (double_in && m != 2) ? discard () : score ();

    if (p.score > d + 1)
      return score ();

    if (p.score == d + 1)
      return double_out ? bust () : score ();

    if (p.score == d)
      return (double_out && m != 2) ? bust () : score ();

    if (p.score < d)
      return bust ();
  }

  void X01::cancel ()
  {
  }

  bool X01::is_finished () const
  {
    auto player_has_finished = [this] (const Player & p) {return p.score == 0;};
    return state.is_finished () || any_of (players.begin (), players.end (), player_has_finished);
  }

  vector<Game::Score> X01::scores () const
  {
    return vector<Score> {players.begin (), players.end ()};
  }

  void X01::render (App * app) const
  {
    render_progress (app);
    render_scores (app, true);
    render_message (app);
  }

} // ed900::game

