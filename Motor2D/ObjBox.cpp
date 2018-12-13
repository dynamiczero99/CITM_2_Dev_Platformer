#include "ObjBox.h"
#include "j1Object.h"
#include "p2Point.h"
#include "j1Render.h"
#include "j1Collision.h"
#include "j1App.h"
#include "p2Animation.h"
#include "PugiXml/src/pugixml.hpp"
#include "p2Log.h"

ObjBox::ObjBox(fPoint &position, int index, pugi::xml_node &box_node, int objectID) : GameObject(position, index, objectID) {
	velocity = fPoint(0.0f, 0.0f);
	acceleration = fPoint(0.0f, 0.0f);

	animTileWidth = box_node.child("animation").attribute("tile_width").as_uint();
	animTileHeight = box_node.child("animation").attribute("tile_height").as_uint();
	LoadAnimation(box_node.child("animation").child("inactive_animation"), inactiveAnim);
	LoadAnimation(box_node.child("animation").child("active_animation"), activeAnim);
	maxFallSpeed = box_node.child("maximum_fall_velocity").text().as_float();
	gravity = TileToPixel(box_node.child("gravity").text().as_float());
	currAnim = &inactiveAnim;
	pivot = Pivot(PivotV::bottom, PivotH::middle);
	iPoint colPos = GetRectPos(pivot, (int)position.x, (int)position.y, animTileWidth, animTileHeight);
	SDL_Rect colRect;
	colRect.w = animTileWidth;
	colRect.h = animTileHeight;
	colRect.x = colPos.x;
	colRect.y = colPos.y;
	col = App->collision->AddCollider(colRect, COLLIDER_TYPE::COLLIDER_BOX, this);

	LOG("object id");
}

bool ObjBox::OnDestroy() {
	App->collision->DeleteCollider(col);
	return true;
}

void ObjBox::MarkObject(bool mark)
{
	if (mark) {
		currAnim = &activeAnim;
	}
	else {
		currAnim = &inactiveAnim;
	}
}

bool ObjBox::Update(float dt) {
	fPoint temp = position;
	acceleration.y = gravity;
	velocity = velocity + acceleration * dt;
	position = position + velocity * dt;
	LimitFallSpeed(dt);
	iPoint colPos = GetRectPos(pivot, position.x, position.y, animTileWidth, animTileHeight);
	col->SetPos(colPos.x, colPos.y);
	
	return true;
}

bool ObjBox::PostUpdate() {
	iPoint blitPos = GetRectPos(pivot, position.x, position.y, animTileWidth, animTileHeight);
	App->render->Blit(App->object->robotTilesetTex, blitPos.x, blitPos.y, &currAnim->GetCurrentFrame());
	return true;
}

void ObjBox::OnCollision(Collider * c1, Collider * c2) {
	if (c2->type == COLLIDER_WALL || c2->type == COLLIDER_BOX || c2->type == COLLIDER_GLASS) {
		uint dist[(uint)dir::max];
		dist[(uint)dir::invalid] = INT_MAX;
		dist[(uint)dir::left] = c2->rect.GetRight() - c1->rect.GetLeft();
		dist[(uint)dir::right] = c1->rect.GetRight() - c2->rect.GetLeft();
		dist[(uint)dir::down] = c1->rect.GetBottom() - c2->rect.GetTop();
		dist[(uint)dir::up] = c2->rect.GetBottom() - c1->rect.GetTop();
		dir nearestDir = dir::invalid;
		for (int i = 1; i < (int)dir::max; ++i) {
			if (dist[i] < dist[(uint)nearestDir]) {
				nearestDir =  (dir)i;
			}
		}
		switch (nearestDir) {
		case dir::down:
			position.y = c2->rect.GetTop();
			velocity.y = 0;
			acceleration.y = 0;
			break;
		case dir::left:
			position.x = c2->rect.GetRight() + c1->rect.w / 2;
			velocity.x = 0;
			break;
		case dir::right:
			position.x = c2->rect.GetLeft() - c1->rect.w / 2;
			velocity.x = 0;
			break;
		case dir::up:
			//position.y = c2->rect.GetBottom() + c1->rect.h;
			//velocity.y = 0;
			break;
		default:
			LOG("Error checking box collsion");
			break;
		}
		iPoint colPos = GetRectPos(pivot, (int)position.x, (int)position.y, c1->rect.w, c1->rect.h);
		c1->SetPos(colPos.x, colPos.y);
	}
}

bool ObjBox::Load(pugi::xml_node& node)
{

	return true;
}

bool ObjBox::Save(pugi::xml_node& node) const
{
	LOG("Saving obj box");

	pugi::xml_node boxNode = node.append_child("Box");

	boxNode.append_attribute("x") = position.x;
	boxNode.append_attribute("y") = position.y;
	boxNode.append_attribute("velocity_x") = velocity.x;
	boxNode.append_attribute("velocity_y") = velocity.y;
	boxNode.append_attribute("id") = objectID;
	if(currAnim == &activeAnim)
		boxNode.append_attribute("isMarked") = true;


	return true;
}

bool ObjBox::OnTriggerEnter()
{
	//LOG("called");
	velocity.y = - 500.0F;
	return true;
}

bool ObjBox::OnTriggerExit()
{
	//LOG("exit trigger");
	return true;
}