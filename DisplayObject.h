#pragma once
#include "headers.h"
#include "mesh.h"

namespace object {

	enum TYPE
	{
		DISPLAY_OBJECT = 0,
		WALL = 1,
		DYNAMIC_OBJECT = 2,
		MOVABLE_WALL = 3,
		OBJECT = 4,
		ENTITY = 5,
		PLAYER = 6
	};

	struct DisplayObject // this is the main object in the game. it is the base.
	{
		DisplayObject(mesh::Mesh mesh_)
		{
			mesh = mesh_;
			type = TYPE::DISPLAY_OBJECT;
		}
		mesh::Mesh mesh;
		position pos;
	public :
		TYPE type;
	};

	struct Wall : DisplayObject
	{
		Wall(mesh::Mesh mesh_) : DisplayObject (mesh_)
		{
			type = TYPE::WALL;
		}
	};

	struct DynamicObject : DisplayObject
	{

	};

	struct MovableWall : DynamicObject
	{

	};

	struct Object : DynamicObject
	{

	};

	struct Entity : DynamicObject
	{

	};

	struct Player : Entity
	{

	};
}