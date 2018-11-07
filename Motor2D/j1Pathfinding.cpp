#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1PathFinding.h"

j1PathFinding::j1PathFinding() : j1Module(), map(NULL), last_path(DEFAULT_PATH_LENGTH), width(0), height(0)
{
	name.create("pathfinding");
}

// Destructor
j1PathFinding::~j1PathFinding()
{
	RELEASE_ARRAY(map);
}

// Called before quitting
bool j1PathFinding::CleanUp()
{
	LOG("Freeing pathfinding library");

	last_path.Clear();
	RELEASE_ARRAY(map);
	return true;
}

// Sets up the walkability map
void j1PathFinding::SetMap(uint width, uint height, uchar* data)
{
	this->width = width;
	this->height = height;

	RELEASE_ARRAY(map);
	map = new uchar[width*height];
	memcpy(map, data, width*height);
}

// Utility: return true if pos is inside the map boundaries
bool j1PathFinding::CheckBoundaries(const iPoint& pos) const
{
	return (pos.x >= 0 && pos.x <= (int)width &&
		pos.y >= 0 && pos.y <= (int)height);
}

// Utility: returns true is the tile is walkable
bool j1PathFinding::IsWalkable(const iPoint& pos) const
{
	uchar t = GetTileAt(pos);
	return t != INVALID_WALK_CODE && t > 0;
}

// Utility: return the walkability value of a tile
uchar j1PathFinding::GetTileAt(const iPoint& pos) const
{
	if (CheckBoundaries(pos))
		return map[(pos.y*width) + pos.x];

	return INVALID_WALK_CODE;
}

// To request all tiles involved in the last generated path
const p2DynArray<iPoint>* j1PathFinding::GetLastPath() const
{
	return &last_path;
}

// PathList ------------------------------------------------------------------------
// Looks for a node in this list and returns it's list node or NULL
// ---------------------------------------------------------------------------------
const p2List_item<PathNode>* PathList::Find(const iPoint& point) const
{
	p2List_item<PathNode>* item = list.start;
	while (item)
	{
		if (item->data.pos == point)
			return item;
		item = item->next;
	}
	return NULL;
}

// PathList ------------------------------------------------------------------------
// Returns the Pathnode with lowest score in this list or NULL if empty
// ---------------------------------------------------------------------------------
p2List_item<PathNode>* PathList::GetNodeLowestScore() const
{
	p2List_item<PathNode>* ret = NULL;
	int min = 65535;

	p2List_item<PathNode>* item = list.end;
	while (item)
	{
		if (item->data.GetF() < min)
		{
			min = item->data.GetF();
			ret = item;
		}
		item = item->prev;
	}
	return ret;
}

// PathNode -------------------------------------------------------------------------
// Convenient constructors
// ----------------------------------------------------------------------------------
PathNode::PathNode() : g(-1), h(-1), pos(-1, -1), parent(NULL)
{}

PathNode::PathNode(int g, int h, const iPoint& pos, PathNode* parent) : g(g), h(h), pos(pos), parent(parent)
{}

PathNode::PathNode(const PathNode& node) : g(node.g), h(node.h), pos(node.pos), parent(node.parent)
{}

// PathNode -------------------------------------------------------------------------
// Calculates this tile score
// ----------------------------------------------------------------------------------
int PathNode::GetF() const
{
	return g + h;
}

// PathNode -------------------------------------------------------------------------
// Calculate the F for a specific destination tile
// ----------------------------------------------------------------------------------
void PathNode::SetGandH(const iPoint& destination)
{
	g = parent->g + COST_TO_MOVE;
	h = pos.DistanceTo(destination);
}

void PathNode::FindWalkableAdjacents(PathList & pathList, const iPoint destination)
{
	iPoint cell;
	// north
	cell.create(pos.x, pos.y + 1);
	if (App->pathfinding->IsWalkable(cell))
		pathList.list.add(PathNode(g + COST_TO_MOVE, cell.DistanceTo(destination), cell, this));

	// east
	cell.create(pos.x + 1, pos.y);
	if (App->pathfinding->IsWalkable(cell))
		pathList.list.add(PathNode(g + COST_TO_MOVE, cell.DistanceTo(destination), cell, this));

	// south
	cell.create(pos.x, pos.y - 1);
	if (App->pathfinding->IsWalkable(cell))
		pathList.list.add(PathNode(g + COST_TO_MOVE, cell.DistanceTo(destination), cell, this));

	// west
	cell.create(pos.x - 1, pos.y);
	if (App->pathfinding->IsWalkable(cell))
		pathList.list.add(PathNode(g + COST_TO_MOVE, cell.DistanceTo(destination), cell, this));
}

// ----------------------------------------------------------------------------------
// Actual A* algorithm: return number of steps in the creation of the path or -1 ----
// ----------------------------------------------------------------------------------
int j1PathFinding::CreatePath(const iPoint& origin, const iPoint& destination)
{
	if (!IsWalkable(origin) || !IsWalkable(destination)) {
		LOG("Invalid origin or destination: Origin or destination are not walkable.");
		return -1;
	}

	PathList openList;
	PathList closedList;
	PathNode originNode(0, origin.DistanceTo(destination), origin, nullptr);
	openList.list.add(originNode);

	while (openList.list.count() > 0) {
		p2List_item<PathNode> * lowestNode = openList.GetNodeLowestScore();

		PathNode * currNode = &closedList.list.add(lowestNode->data)->data;
		openList.list.del(lowestNode);

		if (currNode->pos == destination) {
			last_path.Clear();
			PathNode * pathIterator = currNode;
			while (pathIterator != nullptr && pathIterator->pos != origin) {
				last_path.PushBack(pathIterator->pos);
				pathIterator = pathIterator->parent;
			}
			last_path.Flip();
			LOG("Succesful path: The algorithm has found a path from the origin(%i, %i) to the destination(%i, %i)", origin.x, origin.y, destination.x, destination.y);
			return last_path.GetCapacity();
		}

		PathList adjacentNodes;
		currNode->FindWalkableAdjacents(adjacentNodes, destination);

		for (p2List_item<PathNode>* adjacentNodeIterator = adjacentNodes.list.start; adjacentNodeIterator != nullptr; adjacentNodeIterator = adjacentNodeIterator->next) {
			if (closedList.Find(adjacentNodeIterator->data.pos) != NULL) {
				continue;
			}
			p2List_item<PathNode>* duplicateNode = (p2List_item<PathNode>*)openList.Find(adjacentNodeIterator->data.pos);
			if (duplicateNode == NULL) {
				openList.list.add(adjacentNodeIterator->data);
			}
			else if (adjacentNodeIterator->data.g < duplicateNode->data.g) {
				duplicateNode->data.g = adjacentNodeIterator->data.g;
				duplicateNode->data.parent = currNode;
			}
		}
	}

	LOG("Invalid path: The algorithm has extended to all the possible nodes and hasn't found a path to the destination.");
	return -1;
}

