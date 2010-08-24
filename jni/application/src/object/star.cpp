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

#include "object/player.hpp"
#include "object/star.hpp"

static const float INITIALJUMP = -400;
static const float STAR_SPEED = 150;
static const float JUMPSTAR_SPEED = -300;

Star::Star(const Vector& pos, Direction direction) :
  MovingSprite(pos, "images/powerups/star/star.sprite", LAYER_OBJECTS, COLGROUP_MOVING),
  physic()
{
  physic.set_velocity((direction == LEFT) ? -STAR_SPEED : STAR_SPEED, INITIALJUMP);
}

void
Star::update(float elapsed_time)
{
  movement = physic.get_movement(elapsed_time);
}

void
Star::collision_solid(const CollisionHit& hit)
{
  if(hit.bottom) {
    physic.set_velocity_y(JUMPSTAR_SPEED);
  } else if(hit.top) {
    physic.set_velocity_y(0);
  } else if(hit.left || hit.right) {
    physic.set_velocity_x(-physic.get_velocity_x());
  }
}

HitResponse
Star::collision(GameObject& other, const CollisionHit& )
{
  Player* player = dynamic_cast<Player*> (&other);
  if(player) {
    player->make_invincible();
    remove_me();
    return ABORT_MOVE;
  }

  return FORCE_MOVE;
}

/* EOF */
