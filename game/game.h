#ifndef __ED900_GAME__
#define __ED900_GAME__

#include <vector>

#include "../event.h"

namespace ed900
{
  class App;
}

namespace ed900::game
{
  class Game
  {
    private:
      class State
      {
        public:
          uint8_t rounds;
          uint8_t round;
          uint8_t players;
          uint8_t player;
          uint8_t dart;

        public:
          State (uint8_t players, uint8_t rounds);
          State & operator++ ();
          bool is_waiting () const;
          bool is_finished () const;
          void next ();
      };

    public:
      struct Score
      {
        std::string name;
        uint16_t score;
      };

    protected:
      State state;
      std::vector<DartEvent> history;

    protected:
      virtual std::vector<Score> scores () const = 0;
      virtual uint16_t best_score () const;
      virtual void dart (uint8_t value, uint8_t multiplier) = 0;
      virtual void cancel () = 0;
      void render_progress (App *) const;
      void render_history (App *) const;
      void render_scores (App *, bool player = false) const;
      void render_message (App *) const;

    public:
      Game (uint8_t players, uint8_t rounds);
      virtual ~Game () = default;
      void dart (const DartEvent &);
      bool button (const ButtonEvent &);
      virtual bool is_finished () const = 0;
      virtual void render (App *) const = 0;

    public:
      static bool HighScore (const Score & s1, const Score & s2);
  };

} // ed900::game

#endif // __ED900_GAME__

