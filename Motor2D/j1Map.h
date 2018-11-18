#ifndef __j1MAP_H__
#define __j1MAP_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1Module.h"
#include "p2Animation.h"

#define MAX_OBJECTGROUP_COLLIDERS 100

struct Collider;

struct Levels
{
	p2SString name;
};

struct Properties
{
	bool draw = true;
	bool navigation = false;
	int testValue = 0;
	float parallaxSpeed = 1.0f; // default value
	// music
	p2SString music_name;
	p2SString fx_name;

	inline bool GetDraw() const
	{
		return draw;
	}
};

struct PlayerData
{
	p2SString name;
	Properties properties;
	int x, y = 0;
};

struct MapObjects
{
	p2SString name;
	uint id = 0;
	Collider* colliders[MAX_OBJECTGROUP_COLLIDERS] = { nullptr };
	Properties properties;

	~MapObjects()
	{
		//RELEASE_ARRAY(colliders[MAX_OBJECTGROUP_COLLIDERS]);
	}
};

// ----------------------------------------------------
//INFO:
//- Width & Height are always calculated in pixels
//- Columns & Rows are always calculated in number of tiles
struct MapLayer
{
	p2SString name;
	uint columns = 0u;
	uint rows = 0u;
	uint* tileArray = nullptr;
	Properties properties;
	//float parallaxSpeed = 1.0f; // default

	MapLayer() : tileArray(NULL)
	{}

	~MapLayer()
	{
		RELEASE(tileArray);
	}

	inline uint GetArrayPos(int column, int row) const
	{
		return tileArray[(columns*row) + column];
	}
};

// ----------------------------------------------------
struct TileSet
{
	SDL_Rect GetTileRect(int id) const;

	p2SString			name;
	int					firstgid;
	int					margin;
	int					spacing;
	int					tile_width;
	int					tile_height;
	SDL_Texture*		texture;
	int					tex_width;
	int					tex_height;
	int					columns;
	int					rows;
	int					offset_x;
	int					offset_y;
	Animation*			anim = nullptr;
	float				animSpeedFactor = 0.15f; // TODO, implement this custom propierty from tileset of animation
};

enum class MapTypes
{
	MAPTYPE_UNKNOWN = 0,
	MAPTYPE_ORTHOGONAL,
	MAPTYPE_ISOMETRIC,
	MAPTYPE_STAGGERED
};
// ----------------------------------------------------
struct MapData
{
	int					columns;
	int					rows;
	int					tile_width;
	int					tile_height;
	SDL_Color			background_color;
	MapTypes			type;
	p2List<TileSet*>	tilesets;
	p2List<MapLayer*>	mapLayers;
	p2List<MapObjects*> mapObjects;
	p2List<Levels*>		levels;
	uint				numLevels = 0; // counter for num levels
	p2SString			loadedLevel;

	Properties properties;
	
};

// ----------------------------------------------------
class j1Map : public j1Module
{
public:

	j1Map();

	// Destructor
	virtual ~j1Map();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called each loop iteration
	void Draw();

	// Called before quitting
	bool CleanUp();

	// Load new map
	bool Load(const char* path);

	// Reset current map
	bool Reset();

	//bool Load(pugi::xml_node& node);
	//bool Save(pugi::xml_node& node) const;

	// Coordinate translation methods
	iPoint MapToWorld(int x, int y) const;
	iPoint WorldToMap(int x, int y) const;
	bool CreateWalkabilityMap(int& width, int& height, uchar** buffer) const;
	int GetTileWidth() const;

private:

	bool LoadMap();
	bool LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadLayer(pugi::xml_node& node, MapLayer* layer);
	bool LoadMapColliders(pugi::xml_node& node);//, MapObjects* obj);
	bool LoadProperties(pugi::xml_node& node, Properties& properties);
	bool LoadGameObjects(pugi::xml_node& node);

	TileSet* GetTilesetFromTileId(int id) const;

	bool loadFromSaveGame = false;

public:

	MapData data;
	PlayerData playerData;
	bool				map_loaded;
	bool showNavLayer = false;

private:

	pugi::xml_document	map_file;
	p2SString			folder;
};

#endif // __j1MAP_H__