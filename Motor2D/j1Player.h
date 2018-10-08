#ifndef __j1PLAYER_H__
#define __j1PLAYER_H__

#include "j1Module.h"
#include "p2Point.h"
#include "SDL/include/SDL_render.h"
#include "p2SString.h"

struct SDL_Texture;
struct SDL_Rect;
class Animation;

class j1Player : public j1Module
{
public:
	j1Player();
	// Called before render is available
	bool Awake(pugi::xml_node&);
	// Called before the first frame
	bool Start();
	// Called each loop iteration
	bool PreUpdate();
	// Called each loop iteration
	bool Update(float dt);
	// Called each loop iteration
	bool PostUpdate();
	// Called before quitting
	bool CleanUp();
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;
	inline float pixel_to_tile(uint pixel);

private:
	bool IsStanding();

private:
	//TODO: Set the velocity in the xml

	fPoint position;
	fPoint velocity;
	fPoint acceleration;
	//TODO: Add acceleration (to use incorporate gravity easily)
	SDL_Texture* characterTex;
	Animation* currentAnim;
	Animation* idleAnim;
	Animation* jumpAnim;
	Animation* runAnim;
	Animation* deathAnim;

	float moveSpeed;//(pixels/s)
	float jumpSpeed;//(pixels/s)

	uint tile_size;

	p2SString path;
	SDL_RendererFlip flip;
	float deltaTime = 0;
	Uint32 lastTime = 0;
	float gravity;
};

#endif