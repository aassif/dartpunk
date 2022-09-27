#include "../app.h"
#include "count-up.h"

using namespace std;

namespace ed900::game
{

  CountUp::CountUp (const vector<string> & names) :
    Game {names.size (), 8},
    players {}
  {
    for (auto & n : names)
      players.push_back ({n, 0});
  }

  void CountUp::dart (uint8_t v, uint8_t m)
  {
    players[state.player].score += v * m;
  }

  void CountUp::cancel ()
  {
  }

  bool CountUp::is_finished () const
  {
    return state.is_finished ();
  }

  vector<Game::Score> CountUp::scores () const
  {
    return players;
  }

  void CountUp::render (App * app) const
  {
    render_progress (app);
    render_scores (app, true);
    render_message (app);
  }

} // ed900::game

