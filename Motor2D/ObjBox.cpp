#include "ObjBox.h"
#include "j1Object.h"
#include "p2Point.h"
#include "j1Render.h"
#include "j1Collision.h"
#include "j1App.h"
#include "p2Animation.h"
#include "PugiXml/src/pugixml.hpp"

ObjBox::ObjBox(fPoint position, int index, pugi::xml_node &box_node) : Gameobject(position, index) {
	animTileWidth = box_node.child("animation").attribute("tile_width").as_uint();
	animTileHeight = box_node.child("animation").attribute("tile_height").as_uint();
	LoadAnimation(box_node.child("animation").child("inactive_animation").child("sprite"), inactiveAnim);
	inactiveAnim.speed = box_node.child("animation").child("inactive_animation").attribute("speed").as_float();
	LoadAnimation(box_node.child("animation").child("active_animation").child("sprite"), activeAnim);
	inactiveAnim.speed = box_node.child("animation").child("active_animation").attribute("speed").as_float();
	currAnim = &inactiveAnim;
	iPoint colPos = GetPosFromPivot(pivot::bottom_middle, (int)position.x, (int)position.y, animTileWidth, animTileHeight);
	SDL_Rect colRect;
	colRect.w = animTileWidth;
	colRect.h = animTileHeight;
	colRect.x = colPos.x;
	colRect.y = colPos.y;
	collider = App->collision->AddCollider(colRect, COLLIDER_TYPE::COLLIDER_BOX, this);
}

bool ObjBox::OnDestroy() {
	App->collision->DeleteCollider(collider);
	return true;
}

bool ObjBox::Update() {
	//Move the block down
	return true;
}

bool ObjBox::PostUpdate() {
	iPoint blitPos = GetPosFromPivot(pivot::bottom_middle, position.x, position.y, animTileWidth, animTileHeight);
	App->render->Blit(App->object->robotTex, blitPos.x, blitPos.y, &currAnim->GetCurrentFrame());
	return true;
}

void ObjBox::OnCollision(Collider * c1, Collider * c2) {
	//Same collision as player
}