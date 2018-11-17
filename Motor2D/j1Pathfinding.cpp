#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1PathFinding.h"
#include "Brofiler/Brofiler.h"

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
	h = pos.DistanceManhattan(destination);
}

void PathNode::FindWalkableAdjacents(PathList & pathList, const iPoint destination)
{
	iPoint cell;
	// north
	cell.create(pos.x, pos.y + 1);
	if (App->pathfinding->IsWalkable(cell))
		pathList.list.add(PathNode(g + COST_TO_MOVE, cell.DistanceManhattan(destination), cell, this));

	// east
	cell.create(pos.x + 1, pos.y);
	if (App->pathfinding->IsWalkable(cell))
		pathList.list.add(PathNode(g + COST_TO_MOVE, cell.DistanceManhattan(destination), cell, this));

	// south
	cell.create(pos.x, pos.y - 1);
	if (App->pathfinding->IsWalkable(cell))
		pathList.list.add(PathNode(g + COST_TO_MOVE, cell.DistanceManhattan(destination), cell, this));

	// west
	cell.create(pos.x - 1, pos.y);
	if (App->pathfinding->IsWalkable(cell))
		pathList.list.add(PathNode(g + COST_TO_MOVE, cell.DistanceManhattan(destination), cell, this));

	//// diagonals test
	//// top left
	//cell.create(pos.x - 1, pos.y - 1);
	//if (App->pathfinding->IsWalkable(cell))
	//	pathList.list.add(PathNode(g + COST_TO_MOVE + 1, cell.DistanceManhattan(destination), cell, this));
	//// top right
	//cell.create(pos.x + 1, pos.y - 1);
	//if (App->pathfinding->IsWalkable(cell))
	//	pathList.list.add(PathNode(g + COST_TO_MOVE + 1, cell.DistanceManhattan(destination), cell, this));
	//// bottom left
	//cell.create(pos.x - 1, pos.y + 1);
	//if (App->pathfinding->IsWalkable(cell))
	//	pathList.list.add(PathNode(g + COST_TO_MOVE + 1, cell.DistanceManhattan(destination), cell, this));
	//// bottom right
	//cell.create(pos.x + 1, pos.y + 1);
	//if (App->pathfinding->IsWalkable(cell))
	//	pathList.list.add(PathNode(g + COST_TO_MOVE + 1, cell.DistanceManhattan(destination), cell, this));

}

// ----------------------------------------------------------------------------------
// Actual A* algorithm: return number of steps in the creation of the path or -1 ----
// ----------------------------------------------------------------------------------
int j1PathFinding::CreatePath(const iPoint& origin, const iPoint& destination)
{
	//BROFILER_FRAME("createThread");
	//BROFILER_THREAD("CreatePath worker");
	BROFILER_CATEGORY("CreatePath", Profiler::Color::Black);

	if (!IsWalkable(origin) || !IsWalkable(destination) || origin == destination) {
		LOG("Invalid origin or destination: Origin or destination are not walkable or are the same.");
		return -1;
	}

	PathList openList;
	PathList closedList;
	openList.list.add(PathNode (0, origin.DistanceManhattan(destination), origin, nullptr));

	while (openList.list.Count() > 0) {
		p2List_item<PathNode> * lowestNode = openList.GetNodeLowestScore();

		PathNode * currNode = &closedList.list.add(lowestNode->data)->data;
		openList.list.del(lowestNode);

		if (currNode->pos == destination) {
			last_path.Clear();
			for (PathNode * pathIterator = currNode; pathIterator != nullptr && pathIterator->pos != origin; pathIterator = pathIterator->parent) {
				last_path.PushBack(pathIterator->pos);
			}
			// adds start node too
			//last_path.PushBack(closedList.list.start->data.pos);
			last_path.Flip();
			LOG("Succesful path: The algorithm has found a path from the origin(%i, %i) to the destination(%i, %i)", origin.x, origin.y, destination.x, destination.y);
			return last_path.Count();
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

int j1PathFinding::CreatePathLand(const iPoint& origin, const iPoint& destination)
{
	BROFILER_CATEGORY("CreatePath", Profiler::Color::Black);
	if (!IsWalkable(origin) || !IsWalkable(destination) || origin == destination) {
		LOG("Invalid origin or destination: Origin or destination are not walkable or are the same.");
		return -1;
	}

	PathList openList;
	PathList closedList;
	openList.list.add(PathNode(0, origin.DistanceManhattan(destination), origin, nullptr));

	while (openList.list.Count() > 0) {
		p2List_item<PathNode> * lowestNode = openList.GetNodeLowestScore();

		PathNode * currNode = &closedList.list.add(lowestNode->data)->data;
		openList.list.del(lowestNode);

		if (currNode->pos == destination) {
			last_path.Clear();
			for (PathNode * pathIterator = currNode; pathIterator != nullptr && pathIterator->pos != origin; pathIterator = pathIterator->parent) {
				last_path.PushBack(pathIterator->pos);
			}
			// adds start node too
			//last_path.PushBack(closedList.list.start->data.pos);
			last_path.Flip();
			LOG("Succesful path: The algorithm has found a path from the origin(%i, %i) to the destination(%i, %i)", origin.x, origin.y, destination.x, destination.y);
			return last_path.Count();
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

void j1PathFinding::EmptyLastPath() {
	//for (lastPath.) {

	//}
}

int j1PathFinding::multiThreadCreatePath(void* data)
{
	BROFILER_FRAME("new thread");
	//BROFILER_THREAD("new worker");
	BROFILER_CATEGORY("worker path", Profiler::Color::Orchid);
	//LOG("NEW THREAD");
	threadData* tdata = (threadData*)data;
	
	if (App->pathfinding->CreatePathLand(tdata->origin, tdata->destination) > 0)
	{
		LOG("origin %i,%i", tdata->origin.x, tdata->origin.y);
		LOG("object index: %i", tdata->index);
		//CreatePath(tdata->origin, tdata->destination);

		tdata->ready = true;

		tdata->CopyLastGeneratedPath();
	}
	else
	{
		LOG("thread finished without reachable path");
		tdata->waitingForPath = false;
	}
	
	return 0;
}

int j1PathFinding::CreateLandPath(const iPoint& origin, const iPoint& destination, int characterTileWidth, int characterTileHeight, int maxCharacterTilesJump)
{
	if (!IsWalkable(origin) || !IsWalkable(destination) || origin == destination) {
		LOG("Invalid origin or destination: Origin or destination are not walkable or are the same.");
		return -1;
	}

	PathList openList;
	PathList closedList;
	openList.list.add(PathNode(0, origin.DistanceManhattan(destination), origin, nullptr));
	p2List_item<PathNode> * startNode = openList.GetNodeLowestScore();
	
	// adds z value to start node
	startNode->data.pos_z = 0;

	// check if the node above is ground or not
	if (!IsWalkable((startNode->data.pos - iPoint(0, -1))))
	{
		startNode->data.jumpLength = 0;
	}
	else
		startNode->data.jumpLength = maxCharacterTilesJump * 2;

	while (openList.list.Count() > 0) 
	{
		p2List_item<PathNode> * lowestNode = openList.GetNodeLowestScore();

		PathNode * currNode = &closedList.list.add(lowestNode->data)->data;
		openList.list.del(lowestNode);
		

		if (currNode->pos == destination) {
			last_path.Clear();
			for (PathNode * pathIterator = currNode; pathIterator != nullptr && pathIterator->pos != origin; pathIterator = pathIterator->parent) {
				last_path.PushBack(pathIterator->pos);
			}
			// adds start node too
			//last_path.PushBack(closedList.list.start->data.pos);
			last_path.Flip();
			LOG("Succesful path: The algorithm has found a path from the origin(%i, %i) to the destination(%i, %i)", origin.x, origin.y, destination.x, destination.y);
			return last_path.Count();
		}

		PathList adjacentNodes;
		currNode->FindWalkableAdjacents(adjacentNodes, destination);

		for (p2List_item<PathNode>* adjacentNodeIterator = adjacentNodes.list.start; adjacentNodeIterator != nullptr; adjacentNodeIterator = adjacentNodeIterator->next)
		{
			bool onGround = false;
			bool atCeiling = false;

			// check if the successor would be on ground
			if (!IsWalkable(adjacentNodeIterator->data.pos + iPoint(0, 1)))
				onGround = true;

			// check if would be at ceiling
			if (!IsWalkable(adjacentNodeIterator->data.pos - iPoint(0, 1)))
				atCeiling = true;

			// calculate jump value -------------
			// get jumpt value from parent
			int jumpLength = currNode->jumpLength;
			int newJumpLength = jumpLength;

			if (onGround)
				newJumpLength = 0;
			else if (atCeiling)
			{
				// if the character needs to fall
				if (adjacentNodeIterator->data.pos.x != currNode->pos.x)
					newJumpLength = MAX(maxCharacterTilesJump * 2 + 1, jumpLength + 1);
				else
					newJumpLength = MAX(maxCharacterTilesJump * 2, jumpLength + 2);
			}
			// calculate jump value on air
			else if (adjacentNodeIterator->data.pos.y > currNode->pos.y)
			{
				if (jumpLength < 2)
					newJumpLength = 3;
				else if (jumpLength % 2 == 0) // if the jumpLength is even
					newJumpLength = jumpLength + 2; // increment by two
				else
					newJumpLength = jumpLength + 1;
			}
			else if (adjacentNodeIterator->data.pos.y < currNode->pos.y)
			{
				if (jumpLength % 2 == 0)
					newJumpLength = MAX(maxCharacterTilesJump * 2, jumpLength + 2);
				else
					newJumpLength = MAX(maxCharacterTilesJump * 2, jumpLength + 1);
			}

			// validating successor
			if (jumpLength % 2 != 0 && currNode->pos.x != adjacentNodeIterator->data.pos.x)
				continue;

			if (jumpLength >= maxCharacterTilesJump * 2 && adjacentNodeIterator->data.pos.y > currNode->pos.y)
				continue;

			if (newJumpLength >= maxCharacterTilesJump * 2 + 6 && 
				adjacentNodeIterator->data.pos.x != currNode->pos.x && (newJumpLength - (maxCharacterTilesJump * 2 + 6)) % 8 != 3)
				continue;

			// calculating cost
			// g
			int newG = currNode->g + adjacentNodeIterator->data.g + newJumpLength / 4;

			// if the node still doesnt has duplicateds routes
			p2List_item<PathNode>* duplicateNode = (p2List_item<PathNode>*)openList.Find(adjacentNodeIterator->data.pos);
			if (duplicateNode == NULL) {
				adjacentNodeIterator->data.jumpLength = newJumpLength;
				openList.list.add(adjacentNodeIterator->data);
			}
			else
			{
				// check if the duplicated has lowest jump value
				int lowestJump = 0;
				bool couldMoveSideways = false;
				// check all duplicates // maybe we need to improve this
				if (duplicateNode->data.jumpLength < adjacentNodeIterator->data.jumpLength)
				{
					lowestJump = duplicateNode->data.jumpLength;
				}
				if (duplicateNode->data.jumpLength % 2 == 0 && duplicateNode->data.jumpLength < maxCharacterTilesJump * 2 + 6)
					couldMoveSideways = true;

				if (lowestJump <= newJumpLength && (newJumpLength % 2 != 0 || newJumpLength >= maxCharacterTilesJump * 2 + 6 || couldMoveSideways))
					continue;
			}

			adjacentNodeIterator->data.g = newG;
			adjacentNodeIterator->data.parent = currNode;
			adjacentNodeIterator->data.jumpLength = newJumpLength;
			openList.list.add(adjacentNodeIterator->data);

		}
	}

	LOG("Invalid path: The algorithm has extended to all the possible nodes and hasn't found a path to the destination.");
	return -1;
}
