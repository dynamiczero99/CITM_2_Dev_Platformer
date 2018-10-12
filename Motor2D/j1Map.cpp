#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Map.h"
#include "j1Collision.h"
#include <math.h>
#include "j1Scene.h"

j1Map::j1Map() : j1Module(), map_loaded(false)
{
	name.create("map");
}

// Destructor
j1Map::~j1Map()
{}

// Called before render is available
bool j1Map::Awake(pugi::xml_node& config)
{
	LOG("Loading Map Parser");
	bool ret = true;

	folder.create(config.child("folder").child_value());

	return ret;
}

void j1Map::Draw()
{
	if(map_loaded == false)
		return;

	p2List_item<MapLayer*>* layer = data.mapLayers.start;
	while (layer != NULL)
	{
		for (int y = 0; y < data.columns; ++y)
		{
			for (int x = 0; x < data.rows; ++x)
			{
				int tile_id = layer->data->GetArrayPos(x, y);
				if (tile_id > 0)
				{
					TileSet* tileset = GetTilesetFromTileId(tile_id);
					if (tileset != nullptr)
					{
						SDL_Rect r = tileset->GetTileRect(tile_id);
						iPoint pos = MapToWorld(x, y);

						if (tileset->anim != nullptr)
						{
							App->render->Blit(tileset->texture, pos.x, (pos.y - tileset->tile_height) + data.tile_height,
								&tileset->anim->ReturnCurrentFrame(), layer->data->properties.parallaxSpeed);
						}
						else
							App->render->Blit(tileset->texture, pos.x, pos.y, &r, layer->data->properties.parallaxSpeed);
					}
				}
			}
		}
		layer = layer->next;
	}
	// advance animation frames
	for (p2List_item<TileSet*>* tilesets = data.tilesets.start; tilesets != NULL; tilesets = tilesets->next)
	{
		if (tilesets->data->anim != nullptr)
		{
			tilesets->data->anim->GetCurrentFrame();
		}
	}
}

TileSet* j1Map::GetTilesetFromTileId(int id) const
{
	// TODO 3: Complete this method so we pick the right
	// Tileset based on a tile id

	p2List_item<TileSet*>* item = data.tilesets.start;

	while (item != NULL)
	{
		if (item->next == nullptr)
			return item->data;
		else
		{ 
			if (id >= item->data->firstgid && item->next->data->firstgid > id)
			{
				return item->data;
			}
		}

		item = item->next;
	}

	return nullptr;
}

iPoint j1Map::MapToWorld(int column, int row) const
{
	iPoint retVec(0,0);
	switch (data.type) {
	case MapTypes::MAPTYPE_ORTHOGONAL:
		retVec.x = column * data.tile_width;
		retVec.y = row * data.tile_height;
		break;
	case MapTypes::MAPTYPE_ISOMETRIC:
		retVec.x = (column - row) * data.tile_width * 0.5f;
		retVec.y = (column + row) * data.tile_height * 0.5f;
		break;
	default:
		LOG("ERROR: Map type not identified.");
		break;
	}
	
	return retVec;
}


iPoint j1Map::WorldToMap(int x, int y) const
{
	iPoint retVec(0,0);
	switch (data.type) {
	case MapTypes::MAPTYPE_ORTHOGONAL:
		retVec.x = x / data.tile_width;
		retVec.y = y / data.tile_height;
		break;
	case MapTypes::MAPTYPE_ISOMETRIC:
		retVec.y = y / data.tile_height - x / data.tile_width;
		retVec.x = (2 * x) / data.tile_width + retVec.y;
		break;
	default:
		LOG("ERROR: Map type not set.");
		break;
	}
	return retVec;
}

SDL_Rect TileSet::GetTileRect(int id) const
{
	int relative_id = id - firstgid;
	SDL_Rect rect = {0, 0, 0, 0};
	rect.w = tile_width;
	rect.h = tile_height;
	rect.x = margin + ((rect.w + spacing) * (relative_id % columns));
	rect.y = margin + ((rect.h + spacing) * (relative_id / columns));
	return rect;
}

// Called before quitting
bool j1Map::CleanUp()
{
	LOG("Unloading map");

	// Remove all tilesets
	p2List_item<TileSet*>* item;
	item = data.tilesets.start;

	while(item != NULL)
	{
		if (item->data->texture != nullptr)
		{
			App->tex->UnloadTexture(item->data->texture);
			item->data->texture = nullptr;
		}

		RELEASE(item->data->anim);
		RELEASE(item->data);
		
	item = item->next;
	}
	data.tilesets.clear();

	// Remove all layers
	p2List_item<MapLayer*>* item2;
	item2 = data.mapLayers.start;

	while(item2 != NULL)
	{
		RELEASE(item2->data);
		item2 = item2->next;
	}
	data.mapLayers.clear();

	// remove all colliders/object data
	p2List_item<MapObjects*>* objectItem;
	objectItem = data.mapObjects.start;

	while (objectItem != NULL)
	{
		for (uint i = 0; i < MAX_OBJECTGROUP_COLLIDERS; ++i)
		{
			if (objectItem->data->colliders[i] != nullptr)
			{
				objectItem->data->colliders[i]->to_delete = true;
				objectItem->data->colliders[i] = nullptr;
			}
		}
		RELEASE(objectItem->data);
		objectItem = objectItem->next;
	}
	data.mapObjects.clear();

	// Clean up the pugui tree
	map_file.reset();

	return true;
}

// Load new map
bool j1Map::Load(const char* file_name)
{
	bool ret = true;
	p2SString tmp("%s%s", folder.GetString(), file_name);

	pugi::xml_parse_result result = map_file.load_file(tmp.GetString());

	if(result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", file_name, result.description());
		ret = false;
	}

	// Load general info ----------------------------------------------
	if(ret == true)
	{
		ret = LoadMap();
	}

	// Load all tilesets info ----------------------------------------------
	pugi::xml_node tileset;
	for(tileset = map_file.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
	{
		TileSet* set = new TileSet();

		if(ret == true)
		{
			ret = LoadTilesetDetails(tileset, set);
		}

		if(ret == true)
		{
			ret = LoadTilesetImage(tileset, set);
		}

		data.tilesets.add(set);
	}

	// Load layer info ----------------------------------------------
	pugi::xml_node layer;
	for(layer = map_file.child("map").child("layer"); layer && ret; layer = layer.next_sibling("layer"))
	{
		MapLayer* lay = new MapLayer();

		ret = LoadLayer(layer, lay);

		if(ret == true)
			data.mapLayers.add(lay);
	}

	// Load objects/scene colliders -----------------------------------------
	pugi::xml_node objectGroup;
	for (objectGroup = map_file.child("map").child("group"); objectGroup && ret; objectGroup = objectGroup.next_sibling("group"))
	{
		p2SString tmp(objectGroup.attribute("name").as_string());
		//MapObjects* obj = new MapObjects();

		if (tmp == "Colliders")
		{
			//for(pugi::xml_node collidersGroup = objectGroup.child("objectgroup"))
			ret = LoadMapColliders(objectGroup);//, obj);
			LOG("loading Map colliders");
		}
		else if (tmp == "Player")
		{
			// TODO, check latest handout
			pugi::xml_node player = objectGroup.child("objectgroup").child("object");
			playerData.name = player.attribute("name").as_string();
			playerData.x = player.attribute("x").as_int();
			playerData.y = player.attribute("y").as_int();

			// load custom properties
			LoadProperties(objectGroup.child("objectgroup"), playerData.properties);
		}

		//if (ret == true)
			//data.mapObjects.add(obj);
	}

	if(ret == true)
	{
		LOG("Successfully parsed map XML file: %s", file_name);
		LOG("width: %d height: %d", data.columns, data.rows);
		LOG("tile_width: %d tile_height: %d", data.tile_width, data.tile_height);

		p2List_item<TileSet*>* item = data.tilesets.start;
		while(item != NULL)
		{
			TileSet* s = item->data;
			LOG("Tileset ----");
			LOG("name: %s firstgid: %d", s->name.GetString(), s->firstgid);
			LOG("tile width: %d tile height: %d", s->tile_width, s->tile_height);
			LOG("spacing: %d margin: %d", s->spacing, s->margin);
			item = item->next;
		}

		p2List_item<MapLayer*>* item_layer = data.mapLayers.start;
		while(item_layer != NULL)
		{
			MapLayer* l = item_layer->data;
			LOG("Layer ----");
			LOG("name: %s", l->name.GetString());
			LOG("tile width: %d tile height: %d", l->columns, l->rows);
			item_layer = item_layer->next;
		}
	}

	map_loaded = ret;

	return ret;
}

// Load map general properties
bool j1Map::LoadMap()
{
	bool ret = true;
	pugi::xml_node map = map_file.child("map");

	if(map == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'map' tag.");
		ret = false;
	}
	else
	{
		data.columns = map.attribute("width").as_int();
		data.rows = map.attribute("height").as_int();
		data.tile_width = map.attribute("tilewidth").as_int();
		data.tile_height = map.attribute("tileheight").as_int();
		p2SString bg_color(map.attribute("backgroundcolor").as_string());

		data.background_color.r = 0;
		data.background_color.g = 0;
		data.background_color.b = 0;
		data.background_color.a = 0;

		if(bg_color.Length() > 0)
		{
			p2SString red, green, blue;
			bg_color.SubString(1, 2, red);
			bg_color.SubString(3, 4, green);
			bg_color.SubString(5, 6, blue);

			int v = 0;

			sscanf_s(red.GetString(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.r = v;

			sscanf_s(green.GetString(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.g = v;

			sscanf_s(blue.GetString(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.b = v;
		}

		p2SString orientation(map.attribute("orientation").as_string());

		if(orientation == "orthogonal")
		{
			data.type = MapTypes::MAPTYPE_ORTHOGONAL;
		}
		else if(orientation == "isometric")
		{
			data.type = MapTypes::MAPTYPE_ISOMETRIC;
		}
		else if(orientation == "staggered")
		{
			data.type = MapTypes::MAPTYPE_STAGGERED;
		}
		else
		{
			data.type = MapTypes::MAPTYPE_UNKNOWN;
		}

		// load custom properties from map data
		pugi::xml_node propertiesNode = map.child("properties");

		if (propertiesNode == NULL)
		{
			LOG("Map without custom properties");
		}
		else
		{
			LoadProperties(map, data.properties);
		}

	}

	return ret;
}

bool j1Map::LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;

	set->name.create(tileset_node.attribute("name").as_string());
	set->firstgid = tileset_node.attribute("firstgid").as_int();
	set->tile_width = tileset_node.attribute("tilewidth").as_int();
	set->tile_height = tileset_node.attribute("tileheight").as_int();
	set->margin = tileset_node.attribute("margin").as_int();
	set->spacing = tileset_node.attribute("spacing").as_int();

	pugi::xml_node offset = tileset_node.child("tileoffset");
	if(offset != NULL)
	{
		set->offset_x = offset.attribute("x").as_int();
		set->offset_y = offset.attribute("y").as_int();
	}
	else
	{
		set->offset_x = 0;
		set->offset_y = 0;
	}

	return ret;
}

bool j1Map::LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	pugi::xml_node image = tileset_node.child("image");

	if(image == NULL)
	{
		LOG("Error parsing tileset xml file: Cannot find 'image' tag.");
		ret = false;
	}
	else
	{
		set->texture = App->tex->LoadTexture(PATH(folder.GetString(), image.attribute("source").as_string()));
		int w, h;
		SDL_QueryTexture(set->texture, NULL, NULL, &w, &h);
		set->tex_width = image.attribute("width").as_int();

		if(set->tex_width <= 0)
		{
			set->tex_width = w;
		}

		set->tex_height = image.attribute("height").as_int();

		if(set->tex_height <= 0)
		{
			set->tex_height = h;
		}

		set->columns = set->tex_width / set->tile_width;
		set->rows = set->tex_height / set->tile_height;
	}

	//Loading animation
	//Currently each tileset can only hold one animation - tiled map editor restriction
	if (tileset_node.child("tile").child("animation")) {
		set->anim = new Animation;
		
		for (pugi::xml_node frame_node = tileset_node.child("tile").child("animation").child("frame"); frame_node; frame_node = frame_node.next_sibling()) {
			set->anim->PushBack(set->GetTileRect(frame_node.attribute("tileid").as_int() + set->firstgid));
		}
		pugi::xml_node speed_node = tileset_node.child("tile").child("animation").child("frame");
		set->anim->speed = speed_node.attribute("duration").as_float() * set->animSpeedFactor;
	}

	return ret;
}

bool j1Map::LoadLayer(pugi::xml_node& node, MapLayer* layer)
{
	bool ret = true;

	layer->name = node.attribute("name").as_string();
	layer->columns = node.attribute("width").as_int();
	layer->rows = node.attribute("height").as_int();
	LoadProperties(node, layer->properties);
	pugi::xml_node layer_data = node.child("data");

	if(layer_data == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'layer/data' tag.");
		ret = false;
		RELEASE(layer);
	}
	else
	{
		layer->tileArray = new uint[layer->columns*layer->rows];
		memset(layer->tileArray, 0, layer->columns*layer->rows);

		int i = 0;
		for(pugi::xml_node tile = layer_data.child("tile"); tile; tile = tile.next_sibling("tile"))
		{
			layer->tileArray[i++] = tile.attribute("gid").as_int(0);
		}
	}

	// check custom properties from layer - test functionality // TODO, implement latest handout to do this
	/*pugi::xml_node properties = node.child("properties");
	if (properties == NULL)
	{
		LOG("This layer doesnt have custom properties defined");
	}
	else
	{
		layer->parallaxSpeed = properties.child("property").attribute("value").as_float();
		LOG("%f", layer->parallaxSpeed);
	}*/

	return ret;
}

bool j1Map::LoadMapColliders(pugi::xml_node& node)//, MapObjects* obj)
{
	bool ret = true;

	SDL_Rect colliderRect;
	
	// iterate all objectgroups
	for (pugi::xml_node objectGroup = node.child("objectgroup"); objectGroup && ret; objectGroup = objectGroup.next_sibling("objectgroup"))
	{
		p2SString tmp = objectGroup.attribute("name").as_string();
		MapObjects* newObject = new MapObjects();

		newObject->name = tmp.GetString();

		// Load custom properties
		LoadProperties(objectGroup, newObject->properties);

		//bool counted = false;

		// iterate all objects
		int i = 0; // to allocate i colliders at once on new MapObject pointer for map reference?, not implemented yet

		for (pugi::xml_node object = objectGroup.child("object"); object; object = object.next_sibling("object"))
		{
			// count the num of objects
			/*if (!counted)
			{
				for (pugi::xml_node objectCounter = objectGroup.child("object"); object; object = object.next_sibling("object"))
				{
					++i;
				}
				counted = true;
			}*/

			colliderRect.x = object.attribute("x").as_int(0);
			colliderRect.y = object.attribute("y").as_int(0);
			colliderRect.h = object.attribute("height").as_int(0);
			colliderRect.w = object.attribute("width").as_int(0);
			// create collider type of
			if(tmp == "Platforms")
				newObject->colliders[i] = App->collision->AddCollider(colliderRect, COLLIDER_WALL, this);
			else if (tmp == "Floor")
				newObject->colliders[i] = App->collision->AddCollider(colliderRect, COLLIDER_WALL, this); // reference collider type for test

			// increments counter
			++i;
		}
		
		// add object to list
		data.mapObjects.add(newObject);
	}

	return ret;
}

bool j1Map::Reset()
{
	map_loaded = false;
	App->scene->Disable();
	App->scene->Enable();

	if(CleanUp())
		return true;

	return false;
}

// Load a group of properties from a node and fill a list with it
bool j1Map::LoadProperties(pugi::xml_node& node, Properties& properties)
{
	bool ret = true;

	// TODO 6: Fill in the method to fill the custom properties from 
	// an xml_node
	pugi::xml_node propertiesNode = node.child("properties");

	if (propertiesNode == NULL)
	{
		LOG("properties not found");
		ret = false;
	}
	else
	{
		properties.draw = propertiesNode.find_child_by_attribute("name", "Draw").attribute("value").as_bool(true);
		properties.navigation = propertiesNode.find_child_by_attribute("name", "Navigation").attribute("value").as_bool(true);
		properties.testValue = propertiesNode.find_child_by_attribute("name", "testValue").attribute("value").as_int(0);
		properties.parallaxSpeed = propertiesNode.find_child_by_attribute("name", "parallaxSpeed").attribute("value").as_float(1.0f);
	}

	return ret;
}