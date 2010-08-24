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

#include "scripting/functions.hpp"

#include "audio/sound_manager.hpp"
#include "math/random_generator.hpp"
#include "object/camera.hpp"
#include "object/player.hpp"
#include "physfs/ifile_stream.hpp"
#include "supertux/fadeout.hpp"
#include "supertux/game_session.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "supertux/screen_manager.hpp"
#include "supertux/sector.hpp"
#include "supertux/shrinkfade.hpp"
#include "supertux/textscroller.hpp"
#include "supertux/tile.hpp"
#include "supertux/world.hpp"
#include "util/gettext.hpp"
#include "worldmap/tux.hpp"

#include "scripting/squirrel_util.hpp"
#include "scripting/time_scheduler.hpp"

namespace scripting {

SQInteger display(HSQUIRRELVM vm)
{
  Console::output << squirrel2string(vm, -1) << std::endl;
  return 0;
}

void print_stacktrace(HSQUIRRELVM vm)
{
  print_squirrel_stack(vm);
}

SQInteger get_current_thread(HSQUIRRELVM vm)
{
  sq_pushobject(vm, vm_to_object(vm));
  return 1;
}

void wait(HSQUIRRELVM vm, float seconds)
{
  TimeScheduler::instance->schedule_thread(vm, game_time + seconds);
}

void wait_for_screenswitch(HSQUIRRELVM vm)
{
  g_screen_manager->waiting_threads.add(vm);
}

void exit_screen()
{
  g_screen_manager->exit_screen();
}

void fadeout_screen(float seconds)
{
  g_screen_manager->set_screen_fade(new FadeOut(seconds));
}

void shrink_screen(float dest_x, float dest_y, float seconds)
{
  g_screen_manager->set_screen_fade(new ShrinkFade(Vector(dest_x, dest_y), seconds));
}

void abort_screenfade()
{
  g_screen_manager->set_screen_fade(NULL);
}

std::string translate(const std::string& text)
{
  return dictionary_manager->get_dictionary().translate(text);
}

void display_text_file(const std::string& filename)
{
  g_screen_manager->push_screen(new TextScroller(filename));
}

void load_worldmap(const std::string& filename)
{
  using namespace worldmap;

  if(World::current() == NULL)
    throw std::runtime_error("Can't start WorldMap without active world.");

  g_screen_manager->push_screen(new WorldMap(filename, World::current()->get_player_status()));
}

void load_level(const std::string& filename)
{
  if(GameSession::current() == NULL)
    throw std::runtime_error("Can't start level without active level.");

  g_screen_manager->push_screen(new GameSession(filename, GameSession::current()->get_player_status()));
}

void import(HSQUIRRELVM vm, const std::string& filename)
{
  IFileStream in(filename);

  if(SQ_FAILED(sq_compile(vm, squirrel_read_char, &in,
                          filename.c_str(), SQTrue)))
    throw SquirrelError(vm, "Couldn't parse script");

  sq_pushroottable(vm);
  if(SQ_FAILED(sq_call(vm, 1, SQFalse, SQTrue))) {
    sq_pop(vm, 1);
    throw SquirrelError(vm, "Couldn't execute script");
  }
  sq_pop(vm, 1);
}

void debug_collrects(bool enable)
{
  Sector::show_collrects = enable;
}

void debug_show_fps(bool enable)
{
  g_config->show_fps = enable;
}

void debug_draw_solids_only(bool enable)
{
  Sector::draw_solids_only = enable;
}

void debug_draw_editor_images(bool enable)
{
  Tile::draw_editor_images = enable;
}

void debug_worldmap_ghost(bool enable)
{
  using namespace worldmap;

  if(WorldMap::current() == NULL)
    throw std::runtime_error("Can't change ghost mode without active WorldMap");

  WorldMap::current()->get_tux()->set_ghost_mode(enable);
}

void save_state()
{
  using namespace worldmap;

  if(World::current() == NULL || WorldMap::current() == NULL)
    throw std::runtime_error("Can't save state without active World");

  WorldMap::current()->save_state();
  World::current()->save_state();
}

void update_worldmap()
{
  using namespace worldmap;

  if(WorldMap::current() == NULL)
    throw std::runtime_error("Can't update Worldmap: none active");

  WorldMap::current()->load_state();
}

// not added to header, function to only be used by others
// in this file
bool validate_sector_player()
{
  if (Sector::current() == 0)
  {
    log_info << "No current sector." << std::endl;
    return false;
  }

  if (Sector::current()->player == 0)
  {
    log_info << "No player." << std::endl;
    return false;
  }
  return true;
}

void play_music(const std::string& filename)
{
  sound_manager->play_music(filename);
}

void play_sound(const std::string& filename)
{
  sound_manager->play(filename);
}

void grease()
{
  if (!validate_sector_player()) return;
  ::Player* tux = Sector::current()->player; // scripting::Player != ::Player
  tux->get_physic().set_velocity_x(tux->get_physic().get_velocity_x()*3);
}

void invincible()
{
  if (!validate_sector_player()) return;
  ::Player* tux = Sector::current()->player;
  tux->invincible_timer.start(10000);
}

void ghost()
{
  if (!validate_sector_player()) return;
  ::Player* tux = Sector::current()->player;
  tux->set_ghost_mode(true);
}

void mortal()
{
  if (!validate_sector_player()) return;
  ::Player* tux = Sector::current()->player;
  tux->invincible_timer.stop();
  tux->set_ghost_mode(false);
}

void restart()
{
  if (GameSession::current() == 0)
  {
    log_info << "No game session" << std::endl;
    return;
  }
  GameSession::current()->restart_level();
}

void whereami()
{
  if (!validate_sector_player()) return;
  ::Player* tux = Sector::current()->player;
  log_info << "You are at x " << tux->get_pos().x << ", y " << tux->get_pos().y << std::endl;
}

void gotoend()
{
  if (!validate_sector_player()) return;
  ::Player* tux = Sector::current()->player;
  tux->move(Vector(
              (Sector::current()->get_width()) - (SCREEN_WIDTH*2), 0));
  Sector::current()->camera->reset(
    Vector(tux->get_pos().x, tux->get_pos().y));
}

void camera()
{
  if (!validate_sector_player()) return;
  log_info << "Camera is at " << Sector::current()->camera->get_translation().x << "," << Sector::current()->camera->get_translation().y << std::endl;
}

void set_gamma(float gamma) {
  SDL_SetGamma(gamma, gamma, gamma);
}

void quit()
{
  g_screen_manager->quit();
}

int rand()
{
  return gameRandom.rand();
}

void set_game_speed(float speed)
{
  ::g_game_speed = speed;
}

void record_demo(const std::string& filename)
{
  if (GameSession::current() == 0)
  {
    log_info << "No game session" << std::endl;
    return;
  }
  GameSession::current()->restart_level();
  GameSession::current()->record_demo(filename);
}

void play_demo(const std::string& filename)
{
  if (GameSession::current() == 0)
  {
    log_info << "No game session" << std::endl;
    return;
  }
  // Reset random seed
  g_config->random_seed = GameSession::current()->get_demo_random_seed(filename);
  g_config->random_seed = gameRandom.srand(g_config->random_seed);
  GameSession::current()->restart_level();
  GameSession::current()->play_demo(filename);
}

}

/* EOF */
