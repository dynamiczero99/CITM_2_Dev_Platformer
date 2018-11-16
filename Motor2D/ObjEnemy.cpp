#include "ObjEnemy.h"
#include "j1App.h"
#include "j1Map.h"
#include "j1Object.h"
#include "ObjPlayer.h"
#include "p2Point.h"

ObjEnemy::ObjEnemy(fPoint & position, int index) : GameObject(position, index)
{

}

bool ObjEnemy::isPlayerInTileRange(const uint range) const
{
	// translate to map coords
	iPoint thisPos = App->map->WorldToMap((int)position.x, (int)position.y);
	iPoint playerPos = App->map->WorldToMap((int)App->object->player->position.x, (int)App->object->player->position.y);

	return (thisPos.DistanceManhattan(playerPos) < range);
}