//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef HEADER_SUPERTUX_SUPERTUX_WORLD_HPP
#define HEADER_SUPERTUX_SUPERTUX_WORLD_HPP

#include <squirrel.h>
#include <string>
#include <vector>

class PlayerStatus;

class World
{
public:
  static World* current()
  {
    return current_;
  }

private:
  static World* current_;

public:
  World();
  ~World();

  void set_savegame_filename(const std::string& filename);
  void load(const std::string& filename);

  void save_state();
  void load_state();

  unsigned int get_num_levels() const;

  const std::string& get_level_filename(unsigned int i) const;
  const std::string& get_basedir() const;
  const std::string& get_title() const;
  /** returns player status */
  PlayerStatus* get_player_status() const { return player_status.get(); }


  void run();

private:
  std::vector<std::string> levels;
  std::string basedir;
  std::string savegame_filename;
  /// squirrel table that saves persistent state (about the world)
  HSQOBJECT state_table;
  HSQOBJECT world_thread;
  std::string title;
  std::string description;
  std::auto_ptr<PlayerStatus> player_status;

public:
  bool hide_from_contribs;
  bool is_levelset;
};

#endif

/* EOF */
