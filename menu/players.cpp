#include <iostream> // debug
#include <fstream>
#include <algorithm>

#include "../app.h"
#include "players.h"

using namespace std;

#define ROWS 5
#define COLS 4
#define PAGE (ROWS*COLS)

namespace dartpunk::menu
{

// PlayerCount /////////////////////////////////////////////////////////////////

  PlayerCount::PlayerCount () :
    Selection {},
    icon {"images/player.png"}
  {
  }

  void PlayerCount::render (App * app) const
  {
    for (uint8_t y = 0; y < 2; ++y)
      for (uint8_t x = 0; x < 2; ++x)
      {
        uint8_t n = 2*y + x + 1;

        Rect r {64*x, 32*y, 64, 32};
        const Color & c = App::COLORS [n-1];
        bool selected = (n == selection);

        if (selected)
          app->draw (r, c);

        Blender b = selected ? blend::ALPHA : blend::ModAlpha (c);

        for (uint8_t i = 0; i < n; ++i)
        {
          Point dst {r.x + 32 - 5*n + 10*i, r.y + 8};
          app->draw (icon, {0, 0, 9, 16}, dst, b);
        }
      }
  }

// PlayerEditor ////////////////////////////////////////////////////////////////

  PlayerEditor::PlayerEditor () :
    Selection {},
    color {},
    name {}
  {
  }

  void PlayerEditor::init (uint8_t c, const std::string & n)
  {
    selection = 0;
    color = c;
    name = n;
  }

  void PlayerEditor::select (uint8_t k)
  {
    switch (k)
    {
      case 1: if (selection >  0) --selection; break;
      case 2: if (selection < 64) ++selection; break;
      case 3: if (! name.empty ()) name.pop_back (); break;
      case 4: name += KEYBOARD [selection]; break;
    }
  }

  PlayerEditor::operator string () const
  {
    return name;
  }

  void PlayerEditor::render (App * app) const
  {
    const Color & c = App::COLORS [color];
    app->draw (name, {64 - 4 * name.size (), 2}, 1, blend::ModAlpha (c));

    for (uint8_t y = 0; y < 5; ++y)
      for (uint8_t x = 0; x < 13; ++x)
      {
        uint8_t k = y*13 + x;
        bool selected = (k == selection);

        if (selected)
          app->draw ({64 - 13*5/2 + x*5, 14 + y*8, 5, 8}, c);

        app->draw (string {KEYBOARD [k]}, {64 - 13*5/2 + x*5, 14 + y*8, 5, 8}, {1, 1}, 0);
      }

    app->draw ("<",   {1+0,  64 - 6}, 0, blend::ModAlpha (App::COLORS[0]));
    app->draw (">",   {1+16, 64 - 6}, 0, blend::ModAlpha (App::COLORS[1]));
    app->draw ("DEL", {1+32, 64 - 6}, 0, blend::ModAlpha (App::COLORS[2]));
    app->draw ("OK",  {1+64, 64 - 6}, 0, blend::ModAlpha (App::COLORS[3]));
  }

  const char PlayerEditor::KEYBOARD []
  {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '-', ' '
  };

// Players /////////////////////////////////////////////////////////////////////

  Players::Players () :
    Selection {},
    state {State::COUNT},
    count {},
    editor {},
    config {},
    result {}
  {
    static const auto p = [] (const string & name) {
      return Player {name, 0};
    };

    ifstream ifs ("names.txt");
    for (string line; getline (ifs, line);)
      config.push_back (p (line));

    static const vector<string> CONFIG {"P1", "P2", "P3", "P4"};
    if (config.size () < 4)
      transform (CONFIG.begin () + config.size (), CONFIG.end (), back_inserter (config), p);
  }

  uint8_t Players::find_prev (uint8_t k0) const
  {
    for (uint8_t k = k0; k > 0; --k)
      if (config[k-1].state == 0) return k-1;

    return k0;
  }

  uint8_t Players::find_next (uint8_t k0) const
  {
    for (uint8_t k = k0; k < config.size () - 1; ++k)
      if (config[k+1].state == 0) return k+1;

    return k0;
  }

  uint8_t Players::find_first () const
  {
    return config[0].state == 0 ? 0 : find_next (0);
  }

  void Players::write () const
  {
    ofstream ofs ("names.txt");
    for (auto & p : config)
      ofs << p.name << endl;
  }

  void Players::select (uint8_t k)
  {
    switch (state)
    {
      case State::COUNT:
        count.select (k);
        break;

      case State::ADD:
      case State::EDIT:
        editor.select (k);
        break;

      case State::SELECT:
        switch (k)
        {
          case 1: selection = find_prev (selection); break;
          case 2: selection = find_next (selection); break;

          case 3:
            editor.init (result.size (), config[selection].name);
            state = State::EDIT;
            break;

          case 4:
            editor.init (result.size ());
            state = State::ADD;
            break;
        }
        break;
    }
  }

  bool Players::confirm ()
  {
    switch (state)
    {
      case State::COUNT:
        if (count.confirm ())
          state = State::SELECT;
        return false;

      case State::ADD:
      {
        string name = editor;
        if (! name.empty ())
        {
          selection = config.size ();
          config.push_back ({name, 0});
          write ();
        }
        state = State::SELECT;
        return confirm ();
      }

      case State::EDIT:
      {
        string name = editor;
        if (! name.empty ())
        {
          config[selection].name = name;
          write ();
        }
        state = State::SELECT;
        return false;
      }

      case State::SELECT:
        config[selection].state = 1 + result.size ();
        result.push_back (selection);
        selection = find_first ();
        return count == result.size ();
    }

    return false;
  }

  bool Players::cancel ()
  {
    switch (state)
    {
      case State::COUNT:
        return true;

      case State::ADD:
      case State::EDIT:
        state = State::SELECT;
        return false;

      case State::SELECT:
        if (! result.empty ())
        {
          uint8_t k = result.back ();
          result.pop_back ();
          config[k].state = 0;
          selection = find_first ();
        }
        else
          state = State::COUNT;
        return false;
    }

    return false;
  }

  Players::operator vector<string> () const
  {
    vector<string> v;
    auto f = [this] (uint8_t k) {return config[k].name;};
    transform (result.begin (), result.end (), back_inserter (v), f);
    return v;
  }

  void Players::render (App * app) const
  {
    switch (state)
    {
      case State::COUNT:
        count.render (app);
        break;

      case State::ADD:
      case State::EDIT:
        editor.render (app);
        break;

      case State::SELECT:
      {
        string title = "Player " + to_string (1 + result.size ());
        const Color & color = App::COLORS [result.size ()];
        app->draw (title, {64 - 4 * title.size (), 2}, 1, blend::ModAlpha (color));

        uint8_t p = selection / PAGE;
        for (uint8_t y = 0; y < ROWS; ++y)
          for (uint8_t x = 0; x < COLS; ++x)
          {
            uint8_t k = PAGE*p + COLS*y + x;
            if (k < config.size ())
            {
              Rect r {32*x, 14+8*y, 32, 8};
              bool selected = (k == selection);

              if (selected)
                app->draw (r, color);

              static const Color WHITE {0xFF, 0xFF, 0xFF, 0xFF};
              Color c = config[k].state != 0 ? App::COLORS [config[k].state - 1] : WHITE;
              app->draw (config[k].name, r, {1, 1}, 0, blend::ModAlpha (c));
            }
          }

        app->draw ("<",    {1+0,  64 - 6}, 0, blend::ModAlpha (App::COLORS[0]));
        app->draw (">",    {1+16, 64 - 6}, 0, blend::ModAlpha (App::COLORS[1]));
        app->draw ("EDIT", {1+32, 64 - 6}, 0, blend::ModAlpha (App::COLORS[2]));
        app->draw ("ADD",  {1+64, 64 - 6}, 0, blend::ModAlpha (App::COLORS[3]));

        uint8_t n = (config.size () + (PAGE-1)) / PAGE;
        if (n > 1)
        {
          string page = to_string (p+1) + "/" + to_string (n);
          app->draw (page, {128 - 4 * page.size (), 64 - 6}, 0);
        }

        break;
      }
    }
  }

} // dartpunk::menu

