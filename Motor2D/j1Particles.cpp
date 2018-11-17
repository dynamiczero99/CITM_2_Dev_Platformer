#include <math.h>
#include "j1App.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Particles.h"
#include "j1Audio.h"

#include "j1Collision.h"
#include "j1Object.h"

//#include "SDL/include/SDL_timer.h"

#define MARGIN 20

j1Particles::j1Particles()
{
	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
		active[i] = nullptr;

	name.create("Particles");
}

j1Particles::~j1Particles()
{}

bool j1Particles::Awake(pugi::xml_node& node)
{
	LOG("AWAKING PARTICLES");
	if (node.empty())
		LOG("empty particles node");

	particleNode = node;

	return true;
}

// Load assets
bool j1Particles::Start()
{
	LOG("Loading particles");
	// teleport 01 animation
	LoadAnimation(particleNode.child("animation_teleport01"), teleport01.anim, true);
	// teleport 02 animation
	LoadAnimation(particleNode.child("animation_teleport01"), teleport02.anim, true);

	//load textures and links pointers to -------------
	teleport01_tex = App->tex->LoadTexture(particleNode.child("teleport_texture01").text().as_string());
	teleport02_tex = App->tex->LoadTexture(particleNode.child("teleport_texture02").text().as_string());
	// ------------------------------------------------
	//load and links textures for particles -----------
	teleport01.texture = teleport01_tex;
	teleport02.texture = teleport02_tex;

	//load specific Wavs effects for particles --------
	//App->audio->LoadFx("path");
	// ------------------------------------------------


	return true;
}

// Unload assets
bool j1Particles::CleanUp()
{
	LOG("Unloading particles");

	//unloading graphics
	if(App->tex->UnloadTexture(teleport01_tex))
		teleport01_tex = nullptr;
	if (App->tex->UnloadTexture(teleport02_tex))
		teleport02_tex = nullptr;

	//removing active particles
	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
	{
		if (active[i] != nullptr)
		{
			delete active[i];
			active[i] = nullptr;
		}
	}

	//removing particles FX audio
	App->audio->UnloadSFX();

	return true;
}

// Update: draw background
bool j1Particles::PostUpdate()//float dt)
{
	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
	{
		Particle* p = active[i];

		if (p == nullptr)
			continue;

		if (p->Update(App->GetDt()) == false)
		{
			//if (active[i]->deathParticle != nullptr)
			//	AddParticle(*active[i]->deathParticle, active[i]->position.x, //+ active[i]->impactPosition.x,
			//		active[i]->position.y, COLLIDER_ENEMY_SHOT);

			delete p;
			active[i] = nullptr;

		}
		else if (SDL_GetTicks() >= p->born)
		{

			//p->r = p->anim->GetCurrentFrame(); ---
			//App->render->Blit(graphics, p->position.x, p->position.y, &(p->r)); // ---
			App->render->Blit(p->texture, p->position.x, p->position.y, &(p->anim.GetCurrentFrame()));
			if (p->fx_played == false && p->fx != 0)
			{
				p->fx_played = true;
				// Play particle fx here
				App->audio->PlayFx(p->fx, 0);
			}
		}
	}

	return true;
}

//void ModuleParticles::AddParticle(const Particle& particle, Animation& sourceAnim, int x, int y, Uint32 delay, iPoint speed, Uint32 life, char* name)
void j1Particles::AddParticle(const Particle& particle, int x, int y, COLLIDER_TYPE collider_type, iPoint speed, Uint32 delay)
{
	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
	{
		if (active[i] == nullptr)
		{
			Particle* p = new Particle(particle);
			p->born = SDL_GetTicks() + delay;
			p->position.x = x;
			p->position.y = y;
			if (speed.x != 0 || speed.y != 0) //if we send specific speed, defines it
			{
				p->speed = speed;
			}
			if (collider_type != COLLIDER_NONE) //TODO PARTICLE
				p->collider = App->collision->AddCollider(p->anim.GetCurrentFrame(), collider_type);
			active[i] = p;
			break;
		}
	}
}

// -------------------------------------------------------------
void j1Particles::OnCollision(Collider* c1, Collider* c2)
{
	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
	{
		// Always destroy particles that collide
		if (active[i] != nullptr && active[i]->collider == c1)
		{
			//AddParticle(explosion, active[i]->position.x, active[i]->position.y);
			// checks if we are collisioning on walls, or anything else

			if (c2->type == COLLIDER_WALL) // if collider wall...
			{
				if (active[i]->onCollisionWallParticle != nullptr)
				{
					if (active[i]->impactPosition.x != NULL || active[i]->impactPosition.y != NULL)
					{
						AddParticle(*active[i]->onCollisionWallParticle, active[i]->position.x + active[i]->impactPosition.x,
							active[i]->position.y + active[i]->impactPosition.y, COLLIDER_NONE, { 0,0 }, 0);
					}
					else
						AddParticle(*active[i]->onCollisionWallParticle, active[i]->position.x, active[i]->position.y, COLLIDER_NONE, { 0,0 }, 0);
				}
			}
			//if (c2->type != COLLIDER_WALL) // anything collider
			//{
			if (active[i]->onCollisionGeneralParticle != nullptr)
				AddParticle(*active[i]->onCollisionGeneralParticle, active[i]->position.x + active[i]->impactPosition.x,
					active[i]->position.y + active[i]->impactPosition.y, COLLIDER_NONE, { 0,0 }, 0);
			//}

			//active[i]->texture = nullptr;
			delete active[i];
			active[i] = nullptr;
			break;
		}
	}
}

// -------------------------------------------------------------

Particle::Particle()
{
	position.SetToZero();
	speed.SetToZero();
}

Particle::Particle(const Particle& p) :
	anim(p.anim), position(p.position), speed(p.speed), fx(p.fx), born(p.born), life(p.life), texture(p.texture),
	damage(p.damage), onCollisionGeneralParticle(p.onCollisionGeneralParticle), onCollisionWallParticle(p.onCollisionWallParticle),
	impactPosition(p.impactPosition), deathParticle(p.deathParticle)
{}

Particle::~Particle()
{
	if (collider != nullptr)
		App->collision->DeleteCollider(collider);
}

bool Particle::Update(float dt)
{
	bool ret = true;

	if (life > 0)
	{
		if ((SDL_GetTicks() - born) > life)
			ret = false;
	}

	//destroy particles respect camera position margins // TODO PARTICLES
	/*if (position.x > (abs(App->render->camera.x) / SCREEN_SIZE) + SCREEN_WIDTH - MARGIN)
		ret = false;
	else if (position.x < (abs(App->render->camera.x) / SCREEN_SIZE) - MARGIN - 150)
		ret = false;*/

	// destroy particle when animation is finished
	if (!anim.loop && anim.Finished())
		ret = false;

	position.x += speed.x * dt;
	position.y += speed.y * dt;

	if (collider != nullptr)
		collider->SetPos(position.x, position.y);

	return ret;
}

bool j1Particles::LoadAnimation(pugi::xml_node &node, Animation &anim, bool sequential)
{
	if (!sequential)
	{
		anim.speed = node.attribute("speed").as_float();
		for (node = node.child("sprite"); node; node = node.next_sibling("sprite")) {
			SDL_Rect frame;
			frame.x = node.attribute("x").as_int();
			frame.y = node.attribute("y").as_int();
			frame.w = node.attribute("w").as_int();
			frame.h = node.attribute("h").as_int();
			anim.PushBack(frame);
		}
	}
	else
	{
		int tile_width = node.attribute("tile_width").as_int();//48;
		int tile_height = node.attribute("tile_height").as_int();
		int x = node.child("size").attribute("x").as_int();
		int y = node.child("size").attribute("y").as_int();
	
		for (int j = 0; j < y; ++j)
		{
			for (int i = 0; i < x; ++i)
			{
				anim.PushBack({ i * tile_width, j * tile_height, tile_width, tile_height });
			}

		}
		anim.speed = node.attribute("speed").as_float();
		anim.loop = node.attribute("loop").as_bool(true);
	}

	return true;
}