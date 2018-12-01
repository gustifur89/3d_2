#pragma once
#include "headers.h"
#include "mesh.h"
#include "editing.h"
#include "reader.h"
#include <thread>
#include "Iso.h"

namespace seg
{

	struct segment;

	struct node 
	{
		enum TYPE 
		{
			JOINT,
			OBJECT,
			WALL_OBJECT,
			ROOF_OBJECT,
			SPAWN
		};

		node() {};

		node(TYPE type_, double x_, double y_, double z_, double dir)
		{
			x = x_;
			y = y_;
			z = z_;
			type = type_;
			direction = dir;
		}

		void add_child(node * node_)
		{
			children.push_back(node_);
			node_->parent = this;
		}

		void add_content(node * node_)
		{
			contents.push_back(node_);
			node_->parent = this;
		}

		double x = 0;
		double y = 0;
		double z = 0;
		TYPE type;
		double direction;
		int depth = 0;
		std::vector<node*> children;
		std::vector<node*> contents;
		sf::Color keyColor = sf::Color(60, 60, 60, 255);//sf::Color(255,255,255,255);
		std::vector<node*> key_nodes;
		std::vector<sf::Color> color_mask;
		node * parent = NULL;
		Door * door = NULL;
		seg::segment * cur_seg = NULL;
	};

	struct map_node : node
	{

		map_node() {};

		map_node(TYPE type_, double x_, double y_, double z_, double dir) : node(type_, x_, y_, z_, dir)
		{

		}

		map_node(node node_) : node(node_.type, node_.x, node_.y, node_.z, node_.direction)
		{
	
		}

		void add_child(map_node * node_)
		{
			children.push_back(node_);
			node_->parent = this;
		}

		void add_content(map_node * node_)
		{
			contents.push_back(node_);
			node_->parent = this;
		}

		std::vector<map_node*> children;
		std::vector<map_node*> contents;
		map_node * parent = NULL;
	};

	//THIS REQUIRES A READ_SEG to be good, so the constructor is bunk.
	//all of the test segments have prime node at 0,0,2;
	//	they are al 4x4x4
	//	just a basic approach. for now.
	//	the algorithm will take the segments, it will generate a map, then export that to a single mesh... 
	//		So the segments are the building blocks, but they need a container for loading in and assembly.
	struct segment 
	{
		//prelimenary types for sorting...   I think it would be best to just tag the important ones (end)
		//	Versitility may be good in the future though.   I'll have specific buckets and general buckets.
		enum TYPE {
			STRAIGHT,
			LEFT,
			RIGHT,
			T,
			CROSS,
			ELEVATOR,
			END,
			ROOM,
			CAP
		};
		double x, y, z;
		double dir;
		mesh::Mesh_c * mesh_ = new mesh::Mesh_c();
		std::vector<node*> nodes;
		TYPE type;
		int chosen_node;
		double lowX, lowY, lowZ, hiX, hiY, hiZ;
	};

	bool read_seg(std::string file_name, seg::segment &seg_)
	{

		//std::cout << file_name<<"\n";
		std::ifstream file;
		file = std::ifstream(file_name);
		if (file.fail())
		{
			return 0;
		}
		std::string line;

		//nodes
		while (1)
		{
			file >> line;
			//tpye of segment
			//	"TYPE" TYPE
			if (line == "FILE")
			{
				file >> line;
				reader::read_ply(line, *seg_.mesh_);
				seg_.mesh_->azimuth = 0;
				seg_.mesh_->pitch = 0;
				seg_.mesh_->roll = 0;
				seg_.mesh_->commit_mesh();
			}

			if (line == "nodes")
			{
				int num_nodes;
				file >> num_nodes;
				for (int i = 0; i < num_nodes; i++)
				{
					file >> line;
					//a node for joining
					// "NODE" X Y Z DIR
					//	note: x,y,z in openGL format, not blender
					if (line == "NODE")
					{

						seg::node * node_ = new seg::node();
						std::string type;
						file >> type;
						if (type == "JOINT")
						{
							node_->type = seg::node::TYPE::JOINT;
						}
						else if (type == "SPAWN")
						{
							node_->type = seg::node::TYPE::SPAWN;
						}
						else if (type == "OBJECT")
						{
							node_->type = seg::node::TYPE::OBJECT;
						}
						else if (type == "WALL_OBJECT")
						{
							node_->type = seg::node::TYPE::WALL_OBJECT;
						}
						else if (type == "ROOF_OBJECT")
						{
							node_->type = seg::node::TYPE::ROOF_OBJECT;
						}
						else
						{
							node_->type = seg::node::TYPE::OBJECT;
						}

						double x, y, z, dir;
						file >> x >> y >> z >> dir;
						node_->x = x;
						node_->y = y;
						node_->z = z;
						node_->direction = dir;
						seg_.nodes.push_back(node_);
					}
				}
			}

			if (line == "TYPE")
			{
				std::string type;
				file >> type;
				if (type == "STRAIGHT")
				{
					seg_.type = seg::segment::TYPE::STRAIGHT;
				}
				else if (type == "RIGHT")
				{
					seg_.type = seg::segment::TYPE::RIGHT;
				}
				else if (type == "LEFT")
				{
					seg_.type = seg::segment::TYPE::LEFT;
				}
				else if (type == "T")
				{
					seg_.type = seg::segment::TYPE::T;
				}
				else if (type == "CROSS")
				{
					seg_.type = seg::segment::TYPE::CROSS;
				}
				else if (type == "END")
				{
					seg_.type = seg::segment::TYPE::END;
				}
				else if (type == "CAP")
				{
					seg_.type = seg::segment::TYPE::CAP;
				}
				else if (type == "ELEVATOR")
				{
					seg_.type = seg::segment::TYPE::ELEVATOR;
				}
				else if (type == "ROOM")
				{
					seg_.type = seg::segment::TYPE::ROOM;
				}
				else
				{
					seg_.type = seg::segment::TYPE::CROSS;
				}
			}

			if (file.eof())
				break;
		}

		file.close();
		return 1;
	}

	double usigned_min(double a, double b)
	{
		return abs(a) < abs(b) ? a : b;
	}

	double angle_subtract(double a, double b)
	{
	//	return usigned_min((a - b) - 360, (a - b));
		return abs((a - b) - 360.0) < abs((a - b)) ? (a - b) - 360.0 : (a - b);
	}

	void calculate_bounds(segment * seg_)
	{
		bool first = true;

		double lx = 0;
		double lz = 0;

		double hx = 0;
		double hz = 0;

		double ly = 0;
		double hy = 0;

		for (int i = 0; i < seg_->mesh_->faces.size(); i++)
		{
			for (int j = 0; j < 3; j++)
			{
				double x = seg_->mesh_->faces[i].vertex[j].x;
				double y = seg_->mesh_->faces[i].vertex[j].y;
				double z = seg_->mesh_->faces[i].vertex[j].z;
				if (first)
				{
					lx = x;
					hx = x;
					ly = y;
					hy = y;
					lz = z;
					hz = z;
					first = false;
				}
				else
				{
					if (x < lx) lx = x;
					if (x > hx) hx = x;
					if (y < ly) ly = y;
					if (y > hy) hy = y;
					if (z < lz) lz = z;
					if (z > hz) hz = z;
				}
			}
		}
		seg_->lowX = lx;
		seg_->lowY = ly;
		seg_->lowZ = lz;
		seg_->hiX = hx;
		seg_->hiY = hy;
		seg_->hiZ = hz;
	}

	double tolerance = 0.05;
	bool test_new_segment(std::vector<segment*> segments, segment *seg_, double tx, double ty, double tz, double angle)
	{
		//return true;
		double s = sin(angle*TO_RAD);
		double c = cos(angle*TO_RAD);
		if (fabs(s) < 0.0001) s = 0;
		if (fabs(c) < 0.0001) c = 0;

		//get the new values.   HOW!!!!
		//	I can't. It should work but doesn't. I dont' know.
		//	I'll try some other stuff.
		/*
		double x0 = seg_->lowX * c - seg_->lowZ * s + tx;
		double x1 = seg_->hiX * c - seg_->lowZ * s + tx;
		double x2 = seg_->lowX * c - seg_->hiZ * s + tx;
		double x3 = seg_->hiX * c - seg_->hiZ * s + tx;

		double z0 = seg_->lowX * s + seg_->lowZ * c + tz;
		double z1 = seg_->hiX * s + seg_->lowZ * c + tz;
		double z2 = seg_->lowX * s + seg_->hiZ * c + tz;
		double z3 = seg_->hiX * s + seg_->hiZ * c + tz;


		double lx = seg_->lowX * c - seg_->lowZ * s + tx;
		double lz = seg_->lowX * s + seg_->lowZ * c + tz;

		double hx = seg_->hiX * c - seg_->hiZ * s + tx;
		double hz = seg_->hiX * s + seg_->hiZ * c + tz;

		double ly = seg_->lowY + ty;
		double hy = seg_->hiY + ty;
		
		if (lx > hx)
		{
			double temp = lx;
			lx = hx;
			hx = temp;
		}
		if (ly > hy)
		{
			double temp = ly;
			ly = hy;
			hy = temp;
		}
		if (lz > hz)
		{
			double temp = lz;
			lz = hz;
			hz = temp;
		}
		
	//	lx = fmin(x0, fmin(x1, fmin(x2, x3)));
	//	lz = fmin(z0, fmin(z1, fmin(z2, z3)));

	//	hx = fmax(x0, fmax(x1, fmax(x2, x3)));
	//	hz = fmax(z0, fmax(z1, fmax(z2, z3)));
		//*/
		

		///*
		segment * add = new segment(*seg_);
		add->mesh_ = new mesh::Mesh_c(*seg_->mesh_);
		mesh::rotate_mesh(add->mesh_, angle, 0, 0);
		bool first = true;

		double lx = 0;
		double lz = 0;

		double hx = 0;
		double hz = 0;

		double ly = seg_->lowY + ty;
		double hy = seg_->hiY + ty;

		for (int i = 0; i < add->mesh_->faces.size(); i++)
		{
			for (int j = 0; j < 3; j++)
			{
				double x = add->mesh_->faces[i].vertex[j].x;
				double z = add->mesh_->faces[i].vertex[j].z;
				if (first)
				{
					lx = x;
					hx = x;
					lz = z;
					hz = z;
					first = false;
				}
				else
				{
					if (x < lx) lx = x;
					if (x > hx) hx = x;
					if (z < lz) lz = z;
					if (z > hz) hz = z;
				}
			}
		}

		lx += tx;
		hx += tx;
		lz += tz;
		hz += tz;

	//	*/

		seg_->lowX = lx;
		seg_->lowY = ly;
		seg_->lowZ = lz;
		seg_->hiX = hx;
		seg_->hiY = hy;
		seg_->hiZ = hz;
		for (int i = 0; i < segments.size(); i++)
		{
			if (lx < segments[i]->hiX - tolerance && hx > segments[i]->lowX + tolerance &&
				ly < segments[i]->hiY - tolerance && hy > segments[i]->lowY + tolerance &&
				lz < segments[i]->hiZ - tolerance && hz > segments[i]->lowZ + tolerance)
			{
				mesh::recolor_mesh(segments[i]->mesh_, 255, 0, 0);
				//std::cout << segments[i]->hiX << " : " << lx << " :: " << (ly < segments[i]->hiY - tolerance && hy > segments[i]->lowY + tolerance) << "--\n";
				std::cout << "x=" << lx << " " << segments[i]->lowX << " : " << hx << " " << segments[i]->hiX << "\n";
				std::cout << "y=" << ly << " " << segments[i]->lowY << " : " << hy << " " << segments[i]->hiY << "\n";
				std::cout << "z=" << lz << " " << segments[i]->lowZ << " : " << hz << " " << segments[i]->hiZ << "\n";
				std::cout << "offender: " << i << " " <<segments[i]->type<<"\n";
				std::cout << "toADD: " << seg_->type << "\n";
				return false;
			}
		}
		return true;
	}

	bool test_new_segment(std::vector<segment*> segments, segment *seg_)
	{
		for (int i = 0; i < segments.size(); i++)
		{
			if (seg_->lowX < segments[i]->hiX - tolerance && seg_->hiX > segments[i]->lowX + tolerance &&
				seg_->lowY < segments[i]->hiY - tolerance && seg_->hiY > segments[i]->lowY + tolerance &&
				seg_->lowZ < segments[i]->hiZ - tolerance && seg_->hiZ > segments[i]->lowZ + tolerance)
			{
				if(settings::color_test)
					mesh::recolor_mesh(segments[i]->mesh_, 255, 0, 0);
				return false;
			}
		}
		return true;
	}

	int get_depth(node * node_)
	{
		/*if (node_->children.size() > 0)
		{
			node_->depth = 0;
			return 0;
		}
		else
		{*/
		int cur_dep = 0;
		for (int i = 0; i < node_->children.size(); i++)
		{
			int test = get_depth(node_->children[i]);
			if (test > cur_dep)
				cur_dep = test;
		}
		cur_dep++;
		node_->depth = cur_dep;
		return cur_dep;
	//	}
	}

	//takes the segment list, assembles a map, and exports to mesh
	//	TODO: add parameters. For now just random. and test parameters
	//	TODO: add a way to add hit boxes to the objects.   Like export two different meshes.
	//			One to display, one for the bsp. The one for bsp has bounding boxes around the objects. Just a simple bounding box.
	//			Display doesn't have bounding boxes.
	bool make_map(std::vector<segment> &segments, std::vector<mesh::Mesh_c *> object_list, std::vector<mesh::Mesh_c *> wall_object_list, std::vector<mesh::Mesh_c *> roof_object_list, mesh::Mesh_c &mesh, std::vector<mesh::Mesh_c *> &out_meshes, std::vector<seg::node *> &spawn_nodes)
	{
		//this is the working array, the one that will get converted to a mesh
		std::vector<segment*> map;
		
		std::vector<mesh::Mesh_c*> map_1;

		std::vector<mesh::Mesh_c*> objects;

	//	mesh::Mesh_c object_collider;
	//	mesh::Mesh_c wall_object_collider;
	//	mesh::Mesh_c roof_object_collider;

		mesh::Mesh_c object_collider;
		reader::read_ply("src/better_segs_meshes/object_collider.ply", object_collider);

		mesh::Mesh_c wall_object_collider;
		reader::read_ply("src/better_segs_meshes/wall_object_collider.ply", wall_object_collider);

		mesh::Mesh_c roof_object_collider;
		reader::read_ply("src/better_segs_meshes/roof_object_collider.ply", roof_object_collider);


		//for organization, we go through the availible segments and sort them by TYPE
		//	ALSO, we get a bucket full of everything besides ENDS, 
		std::vector<segment*> strs;
		std::vector<segment*> rts;
		std::vector<segment*> lfts;
		std::vector<segment*> ts;
		std::vector<segment*> xs;

		std::vector<segment*> els;
		std::vector<segment*> ends;
		std::vector<segment*> caps;
		std::vector<segment*> non_ends;

		std::vector<segment*> halls;
		std::vector<segment*> rooms;

		//sort
		for (int i = 0; i < segments.size(); i++)
		{
			switch (segments[i].type)
			{
			case segment::TYPE::STRAIGHT:
				non_ends.push_back(&segments[i]);
				strs.push_back(&segments[i]);
				halls.push_back(&segments[i]);
				break;
			case segment::TYPE::RIGHT:
				non_ends.push_back(&segments[i]);
				rts.push_back(&segments[i]);
				halls.push_back(&segments[i]);
				break;
			case segment::TYPE::LEFT:
				non_ends.push_back(&segments[i]);
				lfts.push_back(&segments[i]);
				halls.push_back(&segments[i]);
				break;
			case segment::TYPE::T:
				non_ends.push_back(&segments[i]);
				ts.push_back(&segments[i]);
				halls.push_back(&segments[i]);
				break;
			case segment::TYPE::CROSS:
				non_ends.push_back(&segments[i]);
				xs.push_back(&segments[i]);
				halls.push_back(&segments[i]);
				break;
			case segment::TYPE::ELEVATOR:
				non_ends.push_back(&segments[i]);
				els.push_back(&segments[i]);
				break;
			case segment::TYPE::END:
				ends.push_back(&segments[i]);
				rooms.push_back(&segments[i]);
				break;
			case segment::TYPE::ROOM:
				rooms.push_back(&segments[i]);
				non_ends.push_back(&segments[i]);
				break;
			case segment::TYPE::CAP:
				caps.push_back(&segments[i]);
				break;
			}
		}
		
		//node buffer. Maybe a linked list...   i don't know...  I'll just use an iterator.
		std::vector<node*> node_list;
		int it = 0;
		//Start with a dead end. There may be start rooms, but this will work for now.
		segment * start = new segment( *ends[rand() % ends.size()] ) ;	//pick a random end to start with. COPY the data..
		start->x = 0;		//move it to the origin
		start->y = 0;
		start->z = 0;
		start->dir = 0;
		mesh::rotate_mesh(start->mesh_, start->dir, 0, 0);
		mesh::translate_mesh(start->mesh_, start->x, start->y, start->z);
		calculate_bounds(start);
		node * start_node = new node(start->nodes[0]->type, start->nodes[0]->x, start->nodes[0]->y, start->nodes[0]->z, start->nodes[0]->direction);
		map.push_back(start);
		node_list.push_back(start_node);

		//generate map
		int tries = 0;
		int num_rooms = 0;
		while (it < node_list.size()) //keeps working on the nodes, until we are at the last one.
		{
			//current node
			node * c_node = node_list[it];

			if (settings::show_nodes)
			{
				mesh::Mesh_c * n_mesh = new mesh::Mesh_c(*ends[0]->mesh_);
				n_mesh->x = c_node->x;
				n_mesh->z = c_node->z;
				n_mesh->y = 5;
				mesh::resize_mesh(n_mesh, 0.1, 0.1, 0.1);
				mesh::recolor_mesh(n_mesh, 0, 0, 255);
				mesh::rotate_mesh(n_mesh, c_node->direction, 0, 0);
				mesh::translate_mesh(n_mesh, c_node->x, c_node->y + 4, c_node->z);
				map_1.push_back(n_mesh);
			}
			//pick a new segment to add.
			segment * add;
			int pick;
			if (num_rooms >= settings::max_rooms)
			{
				pick = rand() % ends.size();
				add = new segment(*ends[pick]);
				add->mesh_ = new mesh::Mesh_c(*ends[pick]->mesh_);
				std::vector<node*> new_nodes;
				for (int i = 0; i < add->nodes.size(); i++)
				{
					new_nodes.push_back(new node(add->nodes[i]->type, add->nodes[i]->x, add->nodes[i]->y, add->nodes[i]->z, add->nodes[i]->direction));
				}
				add->nodes = new_nodes;
			}
			else
			{
				if (tries >= settings::max_tries / 2)	//if it has tried for half of the time to add a room, try adding a hall.
				{
					pick = rand() % halls.size();
					add = new segment(*halls[pick]);
					add->mesh_ = new mesh::Mesh_c(*halls[pick]->mesh_);
					std::vector<node*> new_nodes;
					for (int i = 0; i < add->nodes.size(); i++)
					{
						new_nodes.push_back(new node(add->nodes[i]->type, add->nodes[i]->x, add->nodes[i]->y, add->nodes[i]->z, add->nodes[i]->direction));
					}
					add->nodes = new_nodes;
				}
				else
				{
					/*
					double choice = random();
					std::cout << choice << "\n";
					if (choice <= settings::hall_fraction)
					{
						pick = rand() % halls.size();
						add = new segment(*halls[pick]);
						add->mesh_ = new mesh::Mesh_c(*halls[pick]->mesh_);
					}
					else
					{
						pick = rand() % rooms.size();
						add = new segment(*rooms[pick]);
						add->mesh_ = new mesh::Mesh_c(*rooms[pick]->mesh_);
					}
					//*/
					pick = rand() % non_ends.size();
					add = new segment(*non_ends[pick]);
					add->mesh_ = new mesh::Mesh_c(*non_ends[pick]->mesh_);
					std::vector<node*> new_nodes;
					for (int i = 0; i < add->nodes.size(); i++)
					{
						new_nodes.push_back(new node(add->nodes[i]->type, add->nodes[i]->x, add->nodes[i]->y, add->nodes[i]->z, add->nodes[i]->direction));
					}
					add->nodes = new_nodes;
				}
			}

			//pick a node to attach to the current node
			int chosen_node = rand() % add->nodes.size();
			node * n_node = add->nodes[chosen_node]; //just the zeroth for now.  nothing fancy. Will need to change stuff if it was different.
			add->chosen_node = chosen_node;
			if (n_node->type != node::JOINT)
			{
				chosen_node = 0;
				node * n_node = add->nodes[chosen_node];
				add->chosen_node = chosen_node;
			}
			
			//attaches the segments together
			//	We find <tx,ty,tz> by <current_x - new_x, ... , ... >
			double tx = c_node->x - n_node->x;
			double ty = c_node->y - n_node->y;
			double tz = c_node->z - n_node->z;

			if (fabs(tx) < 0.001) tx = 0;
			if (fabs(ty) < 0.001) ty = 0;
			if (fabs(tz) < 0.001) tz = 0;
			//move the segment to position
			add->x += tx;
			add->y += ty;
			add->z += tz;
			//still need to do something about direction...   
			//	All nodes start with direction in Reference space. We need to transform to Real space, we need to apply turns and stuff.
			//	So all nodes will start at where they are relative to the mesh. We need to turn the mesh AND the nodes. and update node diretion.
			//	Maybe use angles for node direction, that way we can use math, not switch statements.
			//	We'll just make sure they are all 90° when making it.
			//SO we take 0 as pointing along reference. (Y axis (in blender. Z in openGL),  cannot remember +/-)
			//	then rotate around.
			//std::cout << tx << "  :  " << ty << "  :  " << tz << "\n";
			//Nodes need to rotate so they point in on each other. SO 180°. All nodes point out from the mesh.
			double dif_angle = angle_subtract(c_node->direction, ((int)n_node->direction+180) % 360);

			//this is the angle to rotate.
			add->dir = dif_angle;
			//add it to the map
			
			//move the mesh into the proper position.
			mesh::rotate_mesh(add->mesh_, add->dir, n_node->x, n_node->z);
			mesh::translate_mesh(add->mesh_, add->x, add->y, add->z);
			add->mesh_->reconfigure();
			calculate_bounds(add);

			if (settings::color_test)
			{
				if (map.size() >= num_rooms)
				{
					mesh::recolor_mesh(add->mesh_, 0, 0, 255);
				}
			}
			//we test to see if the mesh fits in the grid
			if (test_new_segment(map, add))
			{
				tries = 0;
				if (add->type == segment::TYPE::ROOM)
					num_rooms++;

				map.push_back(add);
				//then we need to update all the nodes with this change in angle. and position.
				//rotate all the other nodes around this node by the angle.
				//I guess we can just and the rest of the nodes now.
				for (int i = 0; i < add->nodes.size(); i++)
				{
					if (i != chosen_node) 
					{
						//So we update dir.
						add->nodes[i]->direction = (int)(add->nodes[i]->direction + dif_angle) % 360; //rotate the direction by the angle and fix to [0,360)
																									  //then we need to change the position...
																									  //	We first rotate with the angle, then translate.
						double s = sin(dif_angle*TO_RAD);
						double c = cos(dif_angle*TO_RAD);
						if (fabs(s) < 0.0001) s = 0;
						if (fabs(c) < 0.0001) c = 0;
						if (s < -0.99) s = -1;
						if (s > 0.99) s = 1;
						if (c < -0.99) c = -1;
						if (c > 0.99) c = 1;

						//translate point back to origin
						double n_x = add->nodes[i]->x - n_node->x;
						double n_z = add->nodes[i]->z - n_node->z;

						//rotate point
						double xnew = n_x * c - n_z * s;
						double znew = n_x * s + n_z * c;

						//translate point back
						add->nodes[i]->x = xnew + n_node->x;
						add->nodes[i]->z = znew + n_node->z;

						//translate point with Vector from before
						add->nodes[i]->x += tx;
						add->nodes[i]->y += ty;
						add->nodes[i]->z += tz;
						//add it to node list
						switch (add->nodes[i]->type)
						{
						case node::TYPE::JOINT:
							node_list.push_back(add->nodes[i]);
							break;
						case node::TYPE::SPAWN:
							spawn_nodes.push_back(add->nodes[i]);
							break;
						case node::TYPE::OBJECT:
							if (object_list.size() > 0)
							{
								int pick = rand() % object_list.size();
								mesh::Mesh_c * mesh_ = new mesh::Mesh_c(*object_list[pick]);
								mesh_->x = add->nodes[i]->x;
								mesh_->y = add->nodes[i]->y;
								mesh_->z = add->nodes[i]->z;
								mesh_->azimuth = -add->nodes[i]->direction;	//negative because of how the renderer works.   some linear algebra mistake that would be too bothersome to fix now. Maybe fix during optimization.
								mesh_->roll = 0;
								mesh_->pitch = 0;
								out_meshes.push_back(mesh_);
								if (settings::object_collide)
								{
									double lx = 0;
									double ly = 0;
									double lz = 0;
									double hx = 0;
									double hy = 0;
									double hz = 0;

									mesh::calculate_bounds(new mesh::Mesh_c(*object_list[pick]), lx, ly, lz, hx, hy, hz);

									mesh_ = new mesh::Mesh_c(object_collider);
									mesh::set_bounds(mesh_, lx, ly, lz, hx, hy, hz);

									mesh::rotate_mesh(mesh_, add->nodes[i]->direction, 0, 0);
									mesh::translate_mesh(mesh_, add->nodes[i]->x, add->nodes[i]->y, add->nodes[i]->z);
									add->mesh_->reconfigure();

									objects.push_back(mesh_);
								}
								if (settings::show_nodes)
								{
									mesh::Mesh_c * n_mesh = new mesh::Mesh_c(*ends[0]->mesh_);
									mesh::resize_mesh(n_mesh, 0.1, 0.1, 0.1);
									mesh::recolor_mesh(n_mesh, 60, 240, 100);
									mesh::rotate_mesh(n_mesh, add->nodes[i]->direction, n_node->x, n_node->z);
									mesh::translate_mesh(n_mesh, add->nodes[i]->x, add->nodes[i]->y + 4, add->nodes[i]->z);
									map_1.push_back(n_mesh);
								}
							}
							break;
						case node::TYPE::WALL_OBJECT:
							if (wall_object_list.size() > 0)
							{
								int pick = rand() % wall_object_list.size();
								mesh::Mesh_c * mesh_ = new mesh::Mesh_c(*wall_object_list[pick]);
								mesh_->x = add->nodes[i]->x;
								mesh_->y = add->nodes[i]->y;
								mesh_->z = add->nodes[i]->z;
								mesh_->azimuth = 0;//-add->nodes[i]->direction
								mesh_->roll = 0;
								mesh_->pitch = 0;
								mesh::rotate_mesh(mesh_, add->nodes[i]->direction, n_node->x, n_node->z);
								out_meshes.push_back(mesh_);
								if (settings::object_collide)
								{
									double lx = 0;
									double ly = 0;
									double lz = 0;
									double hx = 0;
									double hy = 0;
									double hz = 0;

									mesh::calculate_bounds(new mesh::Mesh_c(*wall_object_list[pick]), lx, ly, lz, hx, hy, hz);

									mesh_ = new mesh::Mesh_c(wall_object_collider);
									mesh::set_bounds(mesh_, lx, ly, lz, hx, hy, hz);

									mesh::rotate_mesh(mesh_, add->nodes[i]->direction, 0, 0);
									mesh::translate_mesh(mesh_, add->nodes[i]->x, add->nodes[i]->y, add->nodes[i]->z);
									add->mesh_->reconfigure();

									objects.push_back(mesh_);
								}
								if (settings::show_nodes)
								{
									mesh::Mesh_c * n_mesh = new mesh::Mesh_c(*ends[0]->mesh_);
									mesh::resize_mesh(n_mesh, 0.1, 0.1, 0.1);
									mesh::recolor_mesh(n_mesh, 60, 240, 100);
									mesh::rotate_mesh(n_mesh, add->nodes[i]->direction, n_node->x, n_node->z);
									mesh::translate_mesh(n_mesh, add->nodes[i]->x, add->nodes[i]->y + 4, add->nodes[i]->z);
									map_1.push_back(n_mesh);
								}
							}
							break;
						case node::TYPE::ROOF_OBJECT:
							if (roof_object_list.size() > 0)
							{
								int pick = rand() % roof_object_list.size();
								mesh::Mesh_c * mesh_ = new mesh::Mesh_c(*roof_object_list[pick]);
								mesh_->x = add->nodes[i]->x;
								mesh_->y = add->nodes[i]->y;
								mesh_->z = add->nodes[i]->z;
								mesh_->azimuth = -add->nodes[i]->direction;
								mesh_->roll = 0;
								mesh_->pitch = 0;
								out_meshes.push_back(mesh_);
								if (settings::object_collide)
								{
									double lx = 0;
									double ly = 0;
									double lz = 0;
									double hx = 0;
									double hy = 0;
									double hz = 0;

									mesh::calculate_bounds(new mesh::Mesh_c(*roof_object_list[pick]), lx, ly, lz, hx, hy, hz);

									mesh_ = new mesh::Mesh_c(roof_object_collider);
									mesh::set_bounds(mesh_, lx, ly, lz, hx, hy, hz);

									mesh::rotate_mesh(mesh_, add->nodes[i]->direction, 0, 0);
									mesh::translate_mesh(mesh_, add->nodes[i]->x, add->nodes[i]->y, add->nodes[i]->z);
									add->mesh_->reconfigure();

									objects.push_back(mesh_);
								}
								if (settings::show_nodes)
								{
									mesh::Mesh_c * n_mesh = new mesh::Mesh_c(*ends[0]->mesh_);
									mesh::resize_mesh(n_mesh, 0.1, 0.1, 0.1);
									mesh::recolor_mesh(n_mesh, 60, 240, 100);
									mesh::rotate_mesh(n_mesh, add->nodes[i]->direction, n_node->x, n_node->z);
									mesh::translate_mesh(n_mesh, add->nodes[i]->x, add->nodes[i]->y + 4, add->nodes[i]->z);
									map_1.push_back(n_mesh);
								}
							}
							break;
						}
					}
					
				}
				//rinse, repeat. 
				it++;
			}
			else
			{
				//delete add;
				tries++;
				if (tries > settings::max_tries)
				{
					//Adds a cap.
					pick = rand() % caps.size();
					add = new segment(*caps[pick]);
					add->mesh_ = new mesh::Mesh_c(*caps[pick]->mesh_);
					if (settings::color_test)
					{
						if (map.size() >= num_rooms)
						{
							mesh::recolor_mesh(add->mesh_, 0, 0, 255);
						}
					}
					add->x = c_node->x - add->nodes[0]->x;
					add->y = c_node->y - add->nodes[0]->y;
					add->z = c_node->z - add->nodes[0]->z;
					add->dir = angle_subtract(c_node->direction, ((int)add->nodes[0]->direction + 180) % 360);
					mesh::rotate_mesh(add->mesh_, add->dir, 0, 0);
					mesh::translate_mesh(add->mesh_, c_node->x - add->nodes[0]->x, c_node->y - add->nodes[0]->y, c_node->z - add->nodes[0]->z);
					calculate_bounds(add);
					map.push_back(add);

					tries = 0;
					it++;
				}
			}
		}
		
		if (settings::remove_hall_blockage)
		{
			std::vector<segment*> map_corrected;
			for (int i = 0; i < map.size(); i++)
			{
				if (map[i]->type == segment::CAP)
				{
					bool unique = true;
					for (int j = 0; j < map.size(); j++)
					{
						if (map[j]->type == segment::CAP)
						{
							if (i != j)
							{
								//	if (map[j]->x == map[i]->x && map[j]->y == map[i]->y && map[j]->z == map[i]->z)
								if (fabs(map[j]->x - map[i]->x) <= 0.001 && fabs(map[j]->y - map[i]->y) <= 0.001 && fabs(map[j]->z - map[i]->z) <= 0.001)
								{
									unique = false;
								}
							}
						}
					}
					if(unique)
						map_corrected.push_back(map[i]);
				}
				else
				{
					map_corrected.push_back(map[i]);
				}
			}
			map = map_corrected;
		}

		//convert map to mesh and export
		for (int i = 0; i < map.size(); i++)
		{
			//so a mesh takes a collection of triangles and then makes a list of values.
			//So I need to just add all the triangles.
			for (int j = 0; j < map[i]->mesh_->faces.size(); j++)
			{
				if (settings::snap_verts_to_grid)
				{
					mesh::snap_face(map[i]->mesh_->faces[j]);
				}
				mesh.faces.push_back(map[i]->mesh_->faces[j]);
			}
			delete map[i]->mesh_;
		}
		
		//add all of the nodes to be visible
		if (settings::show_nodes)
		{
			for (int i = 0; i < map_1.size(); i++)
			{
				for (int j = 0; j < map_1[i]->faces.size(); j++)
				{
					mesh.faces.push_back(map_1[i]->faces[j]);
				}
			}
		}
		
		mesh.s_type == mesh::Mesh_c::TYPE::STATIC;
		mesh.azimuth = 0;
		mesh.pitch = 0;
		mesh.roll = 0;
		mesh.x = 0;
		mesh.y = 0;
		mesh.z = 0;
		mesh.commit_mesh();
		mesh.reconfigure();
		out_meshes.push_back(&mesh);

		for (int i = 0; i < objects.size(); i++)
		{
			for (int j = 0; j < objects[i]->faces.size(); j++)
			{
				if (settings::snap_verts_to_grid)
				{
					mesh::snap_face(objects[i]->faces[j]);
				}
				mesh.faces.push_back(objects[i]->faces[j]);
			}
		}
		mesh.commit_mesh();
		mesh.reconfigure();
		return true;
	}
	
	bool make_map_adv(std::vector<segment> &segments, std::vector<mesh::Mesh_c *> object_list, std::vector<mesh::Mesh_c *> wall_object_list, std::vector<mesh::Mesh_c *> roof_object_list, mesh::Mesh_c *mesh, std::vector<mesh::Mesh_c *> &out_meshes, std::vector<seg::node *> &spawn_nodes, std::vector<Object*> &objects_out)
	{
		//this is the working array, the one that will get converted to a mesh
		std::vector<segment*> map;

		std::vector<mesh::Mesh_c*> map_1;

		std::vector<mesh::Mesh_c*> objects;
		
		//	mesh::Mesh_c object_collider;
		//	mesh::Mesh_c wall_object_collider;
		//	mesh::Mesh_c roof_object_collider;

		mesh::Mesh_c object_collider;
		reader::read_ply("src/better_segs_meshes/object_collider.ply", object_collider);

		mesh::Mesh_c wall_object_collider;
		reader::read_ply("src/better_segs_meshes/wall_object_collider.ply", wall_object_collider);

		mesh::Mesh_c roof_object_collider;
		reader::read_ply("src/better_segs_meshes/roof_object_collider.ply", roof_object_collider);

		//for organization, we go through the availible segments and sort them by TYPE
		//	ALSO, we get a bucket full of everything besides ENDS, 
		std::vector<segment*> strs;
		std::vector<segment*> rts;
		std::vector<segment*> lfts;
		std::vector<segment*> ts;
		std::vector<segment*> xs;

		std::vector<segment*> els;
		std::vector<segment*> ends;
		std::vector<segment*> caps;
		std::vector<segment*> non_ends;

		std::vector<segment*> halls;
		std::vector<segment*> rooms;

		//sort
		for (int i = 0; i < segments.size(); i++)
		{
			switch (segments[i].type)
			{
			case segment::TYPE::STRAIGHT:
				non_ends.push_back(&segments[i]);
				strs.push_back(&segments[i]);
				halls.push_back(&segments[i]);
				break;
			case segment::TYPE::RIGHT:
				non_ends.push_back(&segments[i]);
				rts.push_back(&segments[i]);
				halls.push_back(&segments[i]);
				break;
			case segment::TYPE::LEFT:
				non_ends.push_back(&segments[i]);
				lfts.push_back(&segments[i]);
				halls.push_back(&segments[i]);
				break;
			case segment::TYPE::T:
				non_ends.push_back(&segments[i]);
				ts.push_back(&segments[i]);
				halls.push_back(&segments[i]);
				break;
			case segment::TYPE::CROSS:
				non_ends.push_back(&segments[i]);
				xs.push_back(&segments[i]);
				halls.push_back(&segments[i]);
				break;
			case segment::TYPE::ELEVATOR:
				non_ends.push_back(&segments[i]);
				els.push_back(&segments[i]);
				break;
			case segment::TYPE::END:
				ends.push_back(&segments[i]);
				rooms.push_back(&segments[i]);
				break;
			case segment::TYPE::ROOM:
				rooms.push_back(&segments[i]);
				non_ends.push_back(&segments[i]);
				break;
			case segment::TYPE::CAP:
				caps.push_back(&segments[i]);
				break;
			}
		}

		//node buffer. Maybe a linked list...   i don't know...  I'll just use an iterator.
		std::vector<node*> node_list;
		int it = 0;
		//Start with a dead end. There may be start rooms, but this will work for now.
		segment * start = new segment(*ends[rand() % ends.size()]);	//pick a random end to start with. COPY the data..
		start->x = 0;		//move it to the origin
		start->y = 0;
		start->z = 0;
		start->dir = 0;
		mesh::rotate_mesh(start->mesh_, start->dir, 0, 0);
		mesh::translate_mesh(start->mesh_, start->x, start->y, start->z);
		calculate_bounds(start);

		for (int i = 1; i < start->nodes.size(); i++)
		{
			//add it to node list
			switch (start->nodes[i]->type)
			{
			case node::TYPE::JOINT:
				node_list.push_back(start->nodes[i]);
				break;
			case node::TYPE::SPAWN:
				spawn_nodes.push_back(start->nodes[i]);
				break;
			case node::TYPE::OBJECT:
				if (object_list.size() > 0)
				{
					int pick = rand() % object_list.size();
					mesh::Mesh_c * mesh_ = new mesh::Mesh_c(*object_list[pick]);
					mesh_->x = start->nodes[i]->x;
					mesh_->y = start->nodes[i]->y;
					mesh_->z = start->nodes[i]->z;
					mesh_->azimuth = -start->nodes[i]->direction;	//negative because of how the renderer works.   some linear algebra mistake that would be too bothersome to fix now. Maybe fix during optimization.
					mesh_->roll = 0;
					mesh_->pitch = 0;
					out_meshes.push_back(mesh_);
					if (settings::object_collide)
					{
						double lx = 0;
						double ly = 0;
						double lz = 0;
						double hx = 0;
						double hy = 0;
						double hz = 0;

						mesh::calculate_bounds(new mesh::Mesh_c(*object_list[pick]), lx, ly, lz, hx, hy, hz);

						mesh_ = new mesh::Mesh_c(object_collider);
						mesh::set_bounds(mesh_, lx, ly, lz, hx, hy, hz);

						mesh::rotate_mesh(mesh_, start->nodes[i]->direction, 0, 0);
						mesh::translate_mesh(mesh_, start->nodes[i]->x, start->nodes[i]->y, start->nodes[i]->z);
						start->mesh_->reconfigure();

						objects.push_back(mesh_);
					}
					if (settings::show_nodes)
					{
						mesh::Mesh_c * n_mesh = new mesh::Mesh_c(*ends[0]->mesh_);
						mesh::resize_mesh(n_mesh, 0.1, 0.1, 0.1);
						mesh::recolor_mesh(n_mesh, 60, 240, 100);
						mesh::rotate_mesh(n_mesh, start->nodes[i]->direction, 0, 0);
						mesh::translate_mesh(n_mesh, start->nodes[i]->x, start->nodes[i]->y + 4, start->nodes[i]->z);
						map_1.push_back(n_mesh);
					}
				}
				break;
			case node::TYPE::WALL_OBJECT:
				if (wall_object_list.size() > 0)
				{
					int pick = rand() % wall_object_list.size();
					mesh::Mesh_c * mesh_ = new mesh::Mesh_c(*wall_object_list[pick]);
					mesh_->x = start->nodes[i]->x;
					mesh_->y = start->nodes[i]->y;
					mesh_->z = start->nodes[i]->z;
					mesh_->azimuth = 0;//-add->nodes[i]->direction
					mesh_->roll = 0;
					mesh_->pitch = 0;
					mesh::rotate_mesh(mesh_, start->nodes[i]->direction, 0 , 0);
					out_meshes.push_back(mesh_);
					if (settings::object_collide)
					{
						double lx = 0;
						double ly = 0;
						double lz = 0;
						double hx = 0;
						double hy = 0;
						double hz = 0;

						mesh::calculate_bounds(new mesh::Mesh_c(*wall_object_list[pick]), lx, ly, lz, hx, hy, hz);

						mesh_ = new mesh::Mesh_c(wall_object_collider);
						mesh::set_bounds(mesh_, lx, ly, lz, hx, hy, hz);

						mesh::rotate_mesh(mesh_, start->nodes[i]->direction, 0, 0);
						mesh::translate_mesh(mesh_, start->nodes[i]->x, start->nodes[i]->y, start->nodes[i]->z);
						start->mesh_->reconfigure();

						objects.push_back(mesh_);
					}
					if (settings::show_nodes)
					{
						mesh::Mesh_c * n_mesh = new mesh::Mesh_c(*ends[0]->mesh_);
						mesh::resize_mesh(n_mesh, 0.1, 0.1, 0.1);
						mesh::recolor_mesh(n_mesh, 60, 240, 100);
						mesh::rotate_mesh(n_mesh, start->nodes[i]->direction, 0, 0);
						mesh::translate_mesh(n_mesh, start->nodes[i]->x, start->nodes[i]->y + 4, start->nodes[i]->z);
						map_1.push_back(n_mesh);
					}
				}
				break;
			case node::TYPE::ROOF_OBJECT:
				if (roof_object_list.size() > 0)
				{
					int pick = rand() % roof_object_list.size();
					mesh::Mesh_c * mesh_ = new mesh::Mesh_c(*roof_object_list[pick]);
					mesh_->x = start->nodes[i]->x;
					mesh_->y = start->nodes[i]->y;
					mesh_->z = start->nodes[i]->z;
					mesh_->azimuth = -start->nodes[i]->direction;
					mesh_->roll = 0;
					mesh_->pitch = 0;
					out_meshes.push_back(mesh_);
					if (settings::object_collide)
					{
						double lx = 0;
						double ly = 0;
						double lz = 0;
						double hx = 0;
						double hy = 0;
						double hz = 0;

						mesh::calculate_bounds(new mesh::Mesh_c(*roof_object_list[pick]), lx, ly, lz, hx, hy, hz);

						mesh_ = new mesh::Mesh_c(roof_object_collider);
						mesh::set_bounds(mesh_, lx, ly, lz, hx, hy, hz);

						mesh::rotate_mesh(mesh_, start->nodes[i]->direction, 0, 0);
						mesh::translate_mesh(mesh_, start->nodes[i]->x, start->nodes[i]->y, start->nodes[i]->z);
						start->mesh_->reconfigure();

						objects.push_back(mesh_);
					}
					if (settings::show_nodes)
					{
						mesh::Mesh_c * n_mesh = new mesh::Mesh_c(*ends[0]->mesh_);
						mesh::resize_mesh(n_mesh, 0.1, 0.1, 0.1);
						mesh::recolor_mesh(n_mesh, 60, 240, 100);
						mesh::rotate_mesh(n_mesh, start->nodes[i]->direction, 0, 0);
						mesh::translate_mesh(n_mesh, start->nodes[i]->x, start->nodes[i]->y + 4, start->nodes[i]->z);
						map_1.push_back(n_mesh);
					}
				}
				break;
			}
			
		}

		node * start_node = new node(start->nodes[0]->type, start->nodes[0]->x, start->nodes[0]->y, start->nodes[0]->z, start->nodes[0]->direction);
		map.push_back(start);
		node_list.push_back(start_node);

		


		//generate map
		int tries = 0;
		int num_rooms = 0;
		bool only_one = true;
		while (it < node_list.size()) //keeps working on the nodes, until we are at the last one.
		{
			bool doorr = false;

			if (random() < settings::door_chance && only_one)
			{
				doorr = true;
			}

			if (doorr && it+1 < node_list.size())
			{
				tries = 0;
				std::vector<node*> doorr_node_list;
				node * c_node = node_list[it];
				//if doorr, we add a door here. Then we put a key somewhere else.
				//	then we add a door infront of that and put a key somewhere else. An so on.
				
				//these are all the decent nodes. We can add keys in there.
				for (int i = it+1; i < node_list.size(); i++)
				{
					doorr_node_list.push_back(node_list[i]);
				}
				//we set "it" to this so that it doesn't double draw
				it = node_list.size();
				node_list.push_back(c_node);

				//add the door
				int r = rand() % 100+100;
				int g = rand() % 100+100;
				int b = rand() % 100+100;
				Key * key = new Key("src/pick_ups/key.ply");

				Door * door = new Door(c_node->x, c_node->y, c_node->z, c_node->x, c_node->y + 2.1, c_node->z, 1.0, key, "src/meshes/door.ply" );
				//std::cout << key << "\n";
				door->d_node = reinterpret_cast<size_t>(c_node);
				door->key = key;
				
				mesh::rotate_mesh(door->mesh, c_node->direction, 0, 0);
				door->reconfigure();



				//mesh::translate_mesh(door->mesh, c_node->x, c_node->y, c_node->z);
			//	door->mesh->x = c_node->x;
			//	door->mesh->y = c_node->y;
			//	door->mesh->z = c_node->z;
			//	door->reconfigure();
				//paint them the same color.

				bool unique = true;
				for (int p = 0; p < objects_out.size(); p++)
				{
					if (objects_out[p]->type == Object::DOOR)
					{
						if (door->d_node == static_cast<Door*>(objects_out[p])->d_node)
						{
							unique = false;
						}
					}
				}

				if (unique)
				{
					mesh::recolor_mesh(key->mesh, r, g, b);
					mesh::recolor_mesh(door->mesh, r, g, b);

					out_meshes.push_back(key->mesh);
					out_meshes.push_back(door->mesh);

					objects_out.push_back(key);
					objects_out.push_back(door);
				}
			

				int iter = 0;
				int count = 0;
				int length = rand() % settings::max_key_length;
				bool in_loop = true;
				while(in_loop && iter < doorr_node_list.size() && unique)
				{
					//current node
					node * c_node = doorr_node_list[iter];

					if (settings::show_nodes)
					{
						mesh::Mesh_c * n_mesh = new mesh::Mesh_c(*ends[0]->mesh_);
						n_mesh->x = c_node->x;
						n_mesh->z = c_node->z;
						n_mesh->y = 5;
						mesh::resize_mesh(n_mesh, 0.1, 0.1, 0.1);
						mesh::recolor_mesh(n_mesh, 0, 0, 255);
						mesh::rotate_mesh(n_mesh, c_node->direction, 0, 0);
						mesh::translate_mesh(n_mesh, c_node->x, c_node->y + 4, c_node->z);
						map_1.push_back(n_mesh);
					}
					//pick a new segment to add.
					segment * add;
					int pick;
					if (count >= length)
					{
						pick = rand() % ends.size();
						add = new segment(*ends[pick]);
						add->mesh_ = new mesh::Mesh_c(*ends[pick]->mesh_);
						std::vector<node*> new_nodes;
						for (int i = 0; i < add->nodes.size(); i++)
						{
							new_nodes.push_back(new node(add->nodes[i]->type, add->nodes[i]->x, add->nodes[i]->y, add->nodes[i]->z, add->nodes[i]->direction));
						}
						add->nodes = new_nodes;
					}
					else
					{
						if (tries >= settings::max_tries / 2)	//if it has tried for half of the time to add a room, try adding a hall.
						{
							pick = rand() % halls.size();
							add = new segment(*halls[pick]);
							add->mesh_ = new mesh::Mesh_c(*halls[pick]->mesh_);
							std::vector<node*> new_nodes;
							for (int i = 0; i < add->nodes.size(); i++)
							{
								new_nodes.push_back(new node(add->nodes[i]->type, add->nodes[i]->x, add->nodes[i]->y, add->nodes[i]->z, add->nodes[i]->direction));
							}
							add->nodes = new_nodes;
						}
						else
						{
							/*
							double choice = random();
							std::cout << choice << "\n";
							if (choice <= settings::hall_fraction)
							{
							pick = rand() % halls.size();
							add = new segment(*halls[pick]);
							add->mesh_ = new mesh::Mesh_c(*halls[pick]->mesh_);
							}
							else
							{
							pick = rand() % rooms.size();
							add = new segment(*rooms[pick]);
							add->mesh_ = new mesh::Mesh_c(*rooms[pick]->mesh_);
							}
							//*/
							pick = rand() % non_ends.size();
							add = new segment(*non_ends[pick]);
							add->mesh_ = new mesh::Mesh_c(*non_ends[pick]->mesh_);
							std::vector<node*> new_nodes;
							for (int i = 0; i < add->nodes.size(); i++)
							{
								new_nodes.push_back(new node(add->nodes[i]->type, add->nodes[i]->x, add->nodes[i]->y, add->nodes[i]->z, add->nodes[i]->direction));
							}
							add->nodes = new_nodes;
						}
					}

					//pick a node to attach to the current node
					int chosen_node = rand() % add->nodes.size();
					node * n_node = add->nodes[chosen_node]; //just the zeroth for now.  nothing fancy. Will need to change stuff if it was different.
					add->chosen_node = chosen_node;
					if (n_node->type != node::JOINT)
					{
						chosen_node = 0;
						node * n_node = add->nodes[chosen_node];
						add->chosen_node = chosen_node;
					}

					//attaches the segments together
					//	We find <tx,ty,tz> by <current_x - new_x, ... , ... >
					double tx = c_node->x - n_node->x;
					double ty = c_node->y - n_node->y;
					double tz = c_node->z - n_node->z;

					if (fabs(tx) < 0.001) tx = 0;
					if (fabs(ty) < 0.001) ty = 0;
					if (fabs(tz) < 0.001) tz = 0;
					//move the segment to position
					add->x += tx;
					add->y += ty;
					add->z += tz;
					//still need to do something about direction...   
					//	All nodes start with direction in Reference space. We need to transform to Real space, we need to apply turns and stuff.
					//	So all nodes will start at where they are relative to the mesh. We need to turn the mesh AND the nodes. and update node diretion.
					//	Maybe use angles for node direction, that way we can use math, not switch statements.
					//	We'll just make sure they are all 90° when making it.
					//SO we take 0 as pointing along reference. (Y axis (in blender. Z in openGL),  cannot remember +/-)
					//	then rotate around.
					//std::cout << tx << "  :  " << ty << "  :  " << tz << "\n";
					//Nodes need to rotate so they point in on each other. SO 180°. All nodes point out from the mesh.
					double dif_angle = angle_subtract(c_node->direction, ((int)n_node->direction + 180) % 360);

					//this is the angle to rotate.
					add->dir = dif_angle;
					//add it to the map

					//move the mesh into the proper position.
					mesh::rotate_mesh(add->mesh_, add->dir, n_node->x, n_node->z);
					mesh::translate_mesh(add->mesh_, add->x, add->y, add->z);
					add->mesh_->reconfigure();
					calculate_bounds(add);

					if (settings::color_test)
					{
						if (map.size() >= num_rooms)
						{
							mesh::recolor_mesh(add->mesh_, 0, 0, 255);
						}
					}
					//we test to see if the mesh fits in the grid
					if (test_new_segment(map, add))
					{
						tries = 0;
						if (add->type == segment::TYPE::ROOM)
							num_rooms++;
						count++;

						map.push_back(add);
						//then we need to update all the nodes with this change in angle. and position.
						//rotate all the other nodes around this node by the angle.
						//I guess we can just and the rest of the nodes now.
						for (int i = 0; i < add->nodes.size(); i++)
						{
							if (i != chosen_node)
							{
								//So we update dir.
								add->nodes[i]->direction = (int)(add->nodes[i]->direction + dif_angle) % 360; //rotate the direction by the angle and fix to [0,360)
																											  //then we need to change the position...
																											  //	We first rotate with the angle, then translate.
								double s = sin(dif_angle*TO_RAD);
								double c = cos(dif_angle*TO_RAD);
								if (fabs(s) < 0.0001) s = 0;
								if (fabs(c) < 0.0001) c = 0;
								if (s < -0.99) s = -1;
								if (s > 0.99) s = 1;
								if (c < -0.99) c = -1;
								if (c > 0.99) c = 1;

								//translate point back to origin
								double n_x = add->nodes[i]->x - n_node->x;
								double n_z = add->nodes[i]->z - n_node->z;

								//rotate point
								double xnew = n_x * c - n_z * s;
								double znew = n_x * s + n_z * c;

								//translate point back
								add->nodes[i]->x = xnew + n_node->x;
								add->nodes[i]->z = znew + n_node->z;

								//translate point with Vector from before
								add->nodes[i]->x += tx;
								add->nodes[i]->y += ty;
								add->nodes[i]->z += tz;
								//add it to node list
								switch (add->nodes[i]->type)
								{
								case node::TYPE::JOINT:
									doorr_node_list.push_back(add->nodes[i]);
									break;
								case node::TYPE::SPAWN:
									spawn_nodes.push_back(add->nodes[i]);
									if (count >= length)
									{
										key->mesh->x = add->nodes[i]->x;
										key->mesh->y = add->nodes[i]->y;
										key->mesh->z = add->nodes[i]->z;
										key->mesh->azimuth = -add->nodes[i]->direction;
										key->mesh->roll = 0;
										key->mesh->pitch = 0;
										key->on_ground = false;
										in_loop = false;
									}
									/*if (random() > 0.0)
									{
									holdable * pick_up = new holdable("src/pick_ups/key.ply");
									pick_up->mesh->x = add->nodes[i]->x;
									pick_up->mesh->y = add->nodes[i]->y;
									pick_up->mesh->z = add->nodes[i]->z;
									pick_up->mesh->azimuth = -add->nodes[i]->direction;	//negative because of how the renderer works.   some linear algebra mistake that would be too bothersome to fix now. Maybe fix during optimization.
									pick_up->mesh->roll = 0;
									pick_up->mesh->pitch = 0;
									pick_up->on_ground = false;
									objects_out.push_back(pick_up);
									out_meshes.push_back(pick_up->mesh);
									}*/
									break;
								case node::TYPE::OBJECT:
									if (object_list.size() > 0)
									{
										int pick = rand() % object_list.size();
										mesh::Mesh_c * mesh_ = new mesh::Mesh_c(*object_list[pick]);
										mesh_->x = add->nodes[i]->x;
										mesh_->y = add->nodes[i]->y;
										mesh_->z = add->nodes[i]->z;
										mesh_->azimuth = -add->nodes[i]->direction;	//negative because of how the renderer works.   some linear algebra mistake that would be too bothersome to fix now. Maybe fix during optimization.
										mesh_->roll = 0;
										mesh_->pitch = 0;
										out_meshes.push_back(mesh_);
										if (settings::object_collide)
										{
											double lx = 0;
											double ly = 0;
											double lz = 0;
											double hx = 0;
											double hy = 0;
											double hz = 0;

											mesh::calculate_bounds(new mesh::Mesh_c(*object_list[pick]), lx, ly, lz, hx, hy, hz);

											mesh_ = new mesh::Mesh_c(object_collider);
											mesh::set_bounds(mesh_, lx, ly, lz, hx, hy, hz);

											mesh::rotate_mesh(mesh_, add->nodes[i]->direction, 0, 0);
											mesh::translate_mesh(mesh_, add->nodes[i]->x, add->nodes[i]->y, add->nodes[i]->z);
											add->mesh_->reconfigure();

											objects.push_back(mesh_);
										}
										if (settings::show_nodes)
										{
											mesh::Mesh_c * n_mesh = new mesh::Mesh_c(*ends[0]->mesh_);
											mesh::resize_mesh(n_mesh, 0.1, 0.1, 0.1);
											mesh::recolor_mesh(n_mesh, 60, 240, 100);
											mesh::rotate_mesh(n_mesh, add->nodes[i]->direction, n_node->x, n_node->z);
											mesh::translate_mesh(n_mesh, add->nodes[i]->x, add->nodes[i]->y + 4, add->nodes[i]->z);
											map_1.push_back(n_mesh);
										}
									}
									break;
								case node::TYPE::WALL_OBJECT:
									if (wall_object_list.size() > 0)
									{
										int pick = rand() % wall_object_list.size();
										mesh::Mesh_c * mesh_ = new mesh::Mesh_c(*wall_object_list[pick]);
										mesh_->x = add->nodes[i]->x;
										mesh_->y = add->nodes[i]->y;
										mesh_->z = add->nodes[i]->z;
										mesh_->azimuth = 0;//-add->nodes[i]->direction
										mesh_->roll = 0;
										mesh_->pitch = 0;
										mesh::rotate_mesh(mesh_, add->nodes[i]->direction, n_node->x, n_node->z);
										out_meshes.push_back(mesh_);
										if (settings::object_collide)
										{
											double lx = 0;
											double ly = 0;
											double lz = 0;
											double hx = 0;
											double hy = 0;
											double hz = 0;

											mesh::calculate_bounds(new mesh::Mesh_c(*wall_object_list[pick]), lx, ly, lz, hx, hy, hz);

											mesh_ = new mesh::Mesh_c(wall_object_collider);
											mesh::set_bounds(mesh_, lx, ly, lz, hx, hy, hz);

											mesh::rotate_mesh(mesh_, add->nodes[i]->direction, 0, 0);
											mesh::translate_mesh(mesh_, add->nodes[i]->x, add->nodes[i]->y, add->nodes[i]->z);
											add->mesh_->reconfigure();

											objects.push_back(mesh_);
										}
										if (settings::show_nodes)
										{
											mesh::Mesh_c * n_mesh = new mesh::Mesh_c(*ends[0]->mesh_);
											mesh::resize_mesh(n_mesh, 0.1, 0.1, 0.1);
											mesh::recolor_mesh(n_mesh, 60, 240, 100);
											mesh::rotate_mesh(n_mesh, add->nodes[i]->direction, n_node->x, n_node->z);
											mesh::translate_mesh(n_mesh, add->nodes[i]->x, add->nodes[i]->y + 4, add->nodes[i]->z);
											map_1.push_back(n_mesh);
										}
									}
									break;
								case node::TYPE::ROOF_OBJECT:
									if (roof_object_list.size() > 0)
									{
										int pick = rand() % roof_object_list.size();
										mesh::Mesh_c * mesh_ = new mesh::Mesh_c(*roof_object_list[pick]);
										mesh_->x = add->nodes[i]->x;
										mesh_->y = add->nodes[i]->y;
										mesh_->z = add->nodes[i]->z;
										mesh_->azimuth = -add->nodes[i]->direction;
										mesh_->roll = 0;
										mesh_->pitch = 0;
										out_meshes.push_back(mesh_);
										if (settings::object_collide)
										{
											double lx = 0;
											double ly = 0;
											double lz = 0;
											double hx = 0;
											double hy = 0;
											double hz = 0;

											mesh::calculate_bounds(new mesh::Mesh_c(*roof_object_list[pick]), lx, ly, lz, hx, hy, hz);

											mesh_ = new mesh::Mesh_c(roof_object_collider);
											mesh::set_bounds(mesh_, lx, ly, lz, hx, hy, hz);

											mesh::rotate_mesh(mesh_, add->nodes[i]->direction, 0, 0);
											mesh::translate_mesh(mesh_, add->nodes[i]->x, add->nodes[i]->y, add->nodes[i]->z);
											add->mesh_->reconfigure();

											objects.push_back(mesh_);
										}
										if (settings::show_nodes)
										{
											mesh::Mesh_c * n_mesh = new mesh::Mesh_c(*ends[0]->mesh_);
											mesh::resize_mesh(n_mesh, 0.1, 0.1, 0.1);
											mesh::recolor_mesh(n_mesh, 60, 240, 100);
											mesh::rotate_mesh(n_mesh, add->nodes[i]->direction, n_node->x, n_node->z);
											mesh::translate_mesh(n_mesh, add->nodes[i]->x, add->nodes[i]->y + 4, add->nodes[i]->z);
											map_1.push_back(n_mesh);
										}
									}
									break;
								}
							}

						}
						//rinse, repeat. 
						iter++;
					}
					else
					{
						//delete add;
						tries++;
						if (tries > settings::max_tries)
						{
							//Adds a cap.
							pick = rand() % caps.size();
							add = new segment(*caps[pick]);
							add->mesh_ = new mesh::Mesh_c(*caps[pick]->mesh_);
							if (settings::color_test)
							{
								if (map.size() >= num_rooms)
								{
									mesh::recolor_mesh(add->mesh_, 0, 0, 255);
								}
							}
							add->x = c_node->x - add->nodes[0]->x;
							add->y = c_node->y - add->nodes[0]->y;
							add->z = c_node->z - add->nodes[0]->z;
							add->dir = angle_subtract(c_node->direction, ((int)add->nodes[0]->direction + 180) % 360);
							mesh::rotate_mesh(add->mesh_, add->dir, 0, 0);
							mesh::translate_mesh(add->mesh_, c_node->x - add->nodes[0]->x, c_node->y - add->nodes[0]->y, c_node->z - add->nodes[0]->z);
							calculate_bounds(add);
							map.push_back(add);

							tries = 0;
							iter++;
						}
					}
				}




				for (int i = iter; i < doorr_node_list.size(); i++)
				{
					node_list.push_back(doorr_node_list[i]);
				}

			}
			else
			{
				//current node
				node * c_node = node_list[it];

				if (settings::show_nodes)
				{
					mesh::Mesh_c * n_mesh = new mesh::Mesh_c(*ends[0]->mesh_);
					n_mesh->x = c_node->x;
					n_mesh->z = c_node->z;
					n_mesh->y = 5;
					mesh::resize_mesh(n_mesh, 0.1, 0.1, 0.1);
					mesh::recolor_mesh(n_mesh, 0, 0, 255);
					mesh::rotate_mesh(n_mesh, c_node->direction, 0, 0);
					mesh::translate_mesh(n_mesh, c_node->x, c_node->y + 4, c_node->z);
					map_1.push_back(n_mesh);
				}
				//pick a new segment to add.
				segment * add;
				int pick;
				if (num_rooms >= settings::max_rooms)
				{
					pick = rand() % ends.size();
					add = new segment(*ends[pick]);
					add->mesh_ = new mesh::Mesh_c(*ends[pick]->mesh_);
					std::vector<node*> new_nodes;
					for (int i = 0; i < add->nodes.size(); i++)
					{
						new_nodes.push_back(new node(add->nodes[i]->type, add->nodes[i]->x, add->nodes[i]->y, add->nodes[i]->z, add->nodes[i]->direction));
					}
					add->nodes = new_nodes;
				}
				else
				{
					if (tries >= settings::max_tries / 2)	//if it has tried for half of the time to add a room, try adding a hall.
					{
						pick = rand() % halls.size();
						add = new segment(*halls[pick]);
						add->mesh_ = new mesh::Mesh_c(*halls[pick]->mesh_);
						std::vector<node*> new_nodes;
						for (int i = 0; i < add->nodes.size(); i++)
						{
							new_nodes.push_back(new node(add->nodes[i]->type, add->nodes[i]->x, add->nodes[i]->y, add->nodes[i]->z, add->nodes[i]->direction));
						}
						add->nodes = new_nodes;
					}
					else
					{
						/*
						double choice = random();
						std::cout << choice << "\n";
						if (choice <= settings::hall_fraction)
						{
						pick = rand() % halls.size();
						add = new segment(*halls[pick]);
						add->mesh_ = new mesh::Mesh_c(*halls[pick]->mesh_);
						}
						else
						{
						pick = rand() % rooms.size();
						add = new segment(*rooms[pick]);
						add->mesh_ = new mesh::Mesh_c(*rooms[pick]->mesh_);
						}
						//*/
						pick = rand() % non_ends.size();
						add = new segment(*non_ends[pick]);
						add->mesh_ = new mesh::Mesh_c(*non_ends[pick]->mesh_);
						std::vector<node*> new_nodes;
						for (int i = 0; i < add->nodes.size(); i++)
						{
							new_nodes.push_back(new node(add->nodes[i]->type, add->nodes[i]->x, add->nodes[i]->y, add->nodes[i]->z, add->nodes[i]->direction));
						}
						add->nodes = new_nodes;
					}
				}

				//pick a node to attach to the current node
				int chosen_node = rand() % add->nodes.size();
				node * n_node = add->nodes[chosen_node]; //just the zeroth for now.  nothing fancy. Will need to change stuff if it was different.
				add->chosen_node = chosen_node;
				if (n_node->type != node::JOINT)
				{
					chosen_node = 0;
					node * n_node = add->nodes[chosen_node];
					add->chosen_node = chosen_node;
				}

				//attaches the segments together
				//	We find <tx,ty,tz> by <current_x - new_x, ... , ... >
				double tx = c_node->x - n_node->x;
				double ty = c_node->y - n_node->y;
				double tz = c_node->z - n_node->z;

				if (fabs(tx) < 0.001) tx = 0;
				if (fabs(ty) < 0.001) ty = 0;
				if (fabs(tz) < 0.001) tz = 0;
				//move the segment to position
				add->x += tx;
				add->y += ty;
				add->z += tz;
				//still need to do something about direction...   
				//	All nodes start with direction in Reference space. We need to transform to Real space, we need to apply turns and stuff.
				//	So all nodes will start at where they are relative to the mesh. We need to turn the mesh AND the nodes. and update node diretion.
				//	Maybe use angles for node direction, that way we can use math, not switch statements.
				//	We'll just make sure they are all 90° when making it.
				//SO we take 0 as pointing along reference. (Y axis (in blender. Z in openGL),  cannot remember +/-)
				//	then rotate around.
				//std::cout << tx << "  :  " << ty << "  :  " << tz << "\n";
				//Nodes need to rotate so they point in on each other. SO 180°. All nodes point out from the mesh.
				double dif_angle = angle_subtract(c_node->direction, ((int)n_node->direction + 180) % 360);

				//this is the angle to rotate.
				add->dir = dif_angle;
				//add it to the map

				//move the mesh into the proper position.
				mesh::rotate_mesh(add->mesh_, add->dir, n_node->x, n_node->z);
				mesh::translate_mesh(add->mesh_, add->x, add->y, add->z);
				add->mesh_->reconfigure();
				calculate_bounds(add);

				if (settings::color_test)
				{
					if (map.size() >= num_rooms)
					{
						mesh::recolor_mesh(add->mesh_, 0, 0, 255);
					}
				}
				//we test to see if the mesh fits in the grid
				if (test_new_segment(map, add))
				{
					tries = 0;
					if (add->type == segment::TYPE::ROOM)
						num_rooms++;

					map.push_back(add);
					//then we need to update all the nodes with this change in angle. and position.
					//rotate all the other nodes around this node by the angle.
					//I guess we can just and the rest of the nodes now.
					for (int i = 0; i < add->nodes.size(); i++)
					{
						if (i != chosen_node)
						{
							//So we update dir.
							add->nodes[i]->direction = (int)(add->nodes[i]->direction + dif_angle) % 360; //rotate the direction by the angle and fix to [0,360)
																										  //then we need to change the position...
																										  //	We first rotate with the angle, then translate.
							double s = sin(dif_angle*TO_RAD);
							double c = cos(dif_angle*TO_RAD);
							if (fabs(s) < 0.0001) s = 0;
							if (fabs(c) < 0.0001) c = 0;
							if (s < -0.99) s = -1;
							if (s > 0.99) s = 1;
							if (c < -0.99) c = -1;
							if (c > 0.99) c = 1;

							//translate point back to origin
							double n_x = add->nodes[i]->x - n_node->x;
							double n_z = add->nodes[i]->z - n_node->z;

							//rotate point
							double xnew = n_x * c - n_z * s;
							double znew = n_x * s + n_z * c;

							//translate point back
							add->nodes[i]->x = xnew + n_node->x;
							add->nodes[i]->z = znew + n_node->z;

							//translate point with Vector from before
							add->nodes[i]->x += tx;
							add->nodes[i]->y += ty;
							add->nodes[i]->z += tz;
							//add it to node list
							switch (add->nodes[i]->type)
							{
							case node::TYPE::JOINT:
								node_list.push_back(add->nodes[i]);
								break;
							case node::TYPE::SPAWN:
								spawn_nodes.push_back(add->nodes[i]);
								/*if (random() > 0.0)
								{
									holdable * pick_up = new holdable("src/pick_ups/key.ply");
									pick_up->mesh->x = add->nodes[i]->x;
									pick_up->mesh->y = add->nodes[i]->y;
									pick_up->mesh->z = add->nodes[i]->z;
									pick_up->mesh->azimuth = -add->nodes[i]->direction;	//negative because of how the renderer works.   some linear algebra mistake that would be too bothersome to fix now. Maybe fix during optimization.
									pick_up->mesh->roll = 0;
									pick_up->mesh->pitch = 0;
									pick_up->on_ground = false;
									objects_out.push_back(pick_up);
									out_meshes.push_back(pick_up->mesh);
								}*/
								break;
							case node::TYPE::OBJECT:
								if (object_list.size() > 0)
								{
									int pick = rand() % object_list.size();
									mesh::Mesh_c * mesh_ = new mesh::Mesh_c(*object_list[pick]);
									mesh_->x = add->nodes[i]->x;
									mesh_->y = add->nodes[i]->y;
									mesh_->z = add->nodes[i]->z;
									mesh_->azimuth = -add->nodes[i]->direction;	//negative because of how the renderer works.   some linear algebra mistake that would be too bothersome to fix now. Maybe fix during optimization.
									mesh_->roll = 0;
									mesh_->pitch = 0;
									out_meshes.push_back(mesh_);
									if (settings::object_collide)
									{
										double lx = 0;
										double ly = 0;
										double lz = 0;
										double hx = 0;
										double hy = 0;
										double hz = 0;

										mesh::calculate_bounds(new mesh::Mesh_c(*object_list[pick]), lx, ly, lz, hx, hy, hz);

										mesh_ = new mesh::Mesh_c(object_collider);
										mesh::set_bounds(mesh_, lx, ly, lz, hx, hy, hz);

										mesh::rotate_mesh(mesh_, add->nodes[i]->direction, 0, 0);
										mesh::translate_mesh(mesh_, add->nodes[i]->x, add->nodes[i]->y, add->nodes[i]->z);
										add->mesh_->reconfigure();

										objects.push_back(mesh_);
									}
									if (settings::show_nodes)
									{
										mesh::Mesh_c * n_mesh = new mesh::Mesh_c(*ends[0]->mesh_);
										mesh::resize_mesh(n_mesh, 0.1, 0.1, 0.1);
										mesh::recolor_mesh(n_mesh, 60, 240, 100);
										mesh::rotate_mesh(n_mesh, add->nodes[i]->direction, n_node->x, n_node->z);
										mesh::translate_mesh(n_mesh, add->nodes[i]->x, add->nodes[i]->y + 4, add->nodes[i]->z);
										map_1.push_back(n_mesh);
									}
								}
								break;
							case node::TYPE::WALL_OBJECT:
								if (wall_object_list.size() > 0)
								{
									int pick = rand() % wall_object_list.size();
									mesh::Mesh_c * mesh_ = new mesh::Mesh_c(*wall_object_list[pick]);
									mesh_->x = add->nodes[i]->x;
									mesh_->y = add->nodes[i]->y;
									mesh_->z = add->nodes[i]->z;
									mesh_->azimuth = 0;//-add->nodes[i]->direction
									mesh_->roll = 0;
									mesh_->pitch = 0;
									mesh::rotate_mesh(mesh_, add->nodes[i]->direction, n_node->x, n_node->z);
									out_meshes.push_back(mesh_);
									if (settings::object_collide)
									{
										double lx = 0;
										double ly = 0;
										double lz = 0;
										double hx = 0;
										double hy = 0;
										double hz = 0;

										mesh::calculate_bounds(new mesh::Mesh_c(*wall_object_list[pick]), lx, ly, lz, hx, hy, hz);

										mesh_ = new mesh::Mesh_c(wall_object_collider);
										mesh::set_bounds(mesh_, lx, ly, lz, hx, hy, hz);

										mesh::rotate_mesh(mesh_, add->nodes[i]->direction, 0, 0);
										mesh::translate_mesh(mesh_, add->nodes[i]->x, add->nodes[i]->y, add->nodes[i]->z);
										add->mesh_->reconfigure();

										objects.push_back(mesh_);
									}
									if (settings::show_nodes)
									{
										mesh::Mesh_c * n_mesh = new mesh::Mesh_c(*ends[0]->mesh_);
										mesh::resize_mesh(n_mesh, 0.1, 0.1, 0.1);
										mesh::recolor_mesh(n_mesh, 60, 240, 100);
										mesh::rotate_mesh(n_mesh, add->nodes[i]->direction, n_node->x, n_node->z);
										mesh::translate_mesh(n_mesh, add->nodes[i]->x, add->nodes[i]->y + 4, add->nodes[i]->z);
										map_1.push_back(n_mesh);
									}
								}
								break;
							case node::TYPE::ROOF_OBJECT:
								if (roof_object_list.size() > 0)
								{
									int pick = rand() % roof_object_list.size();
									mesh::Mesh_c * mesh_ = new mesh::Mesh_c(*roof_object_list[pick]);
									mesh_->x = add->nodes[i]->x;
									mesh_->y = add->nodes[i]->y;
									mesh_->z = add->nodes[i]->z;
									mesh_->azimuth = -add->nodes[i]->direction;
									mesh_->roll = 0;
									mesh_->pitch = 0;
									out_meshes.push_back(mesh_);
									if (settings::object_collide)
									{
										double lx = 0;
										double ly = 0;
										double lz = 0;
										double hx = 0;
										double hy = 0;
										double hz = 0;

										mesh::calculate_bounds(new mesh::Mesh_c(*roof_object_list[pick]), lx, ly, lz, hx, hy, hz);

										mesh_ = new mesh::Mesh_c(roof_object_collider);
										mesh::set_bounds(mesh_, lx, ly, lz, hx, hy, hz);

										mesh::rotate_mesh(mesh_, add->nodes[i]->direction, 0, 0);
										mesh::translate_mesh(mesh_, add->nodes[i]->x, add->nodes[i]->y, add->nodes[i]->z);
										add->mesh_->reconfigure();

										objects.push_back(mesh_);
									}
									if (settings::show_nodes)
									{
										mesh::Mesh_c * n_mesh = new mesh::Mesh_c(*ends[0]->mesh_);
										mesh::resize_mesh(n_mesh, 0.1, 0.1, 0.1);
										mesh::recolor_mesh(n_mesh, 60, 240, 100);
										mesh::rotate_mesh(n_mesh, add->nodes[i]->direction, n_node->x, n_node->z);
										mesh::translate_mesh(n_mesh, add->nodes[i]->x, add->nodes[i]->y + 4, add->nodes[i]->z);
										map_1.push_back(n_mesh);
									}
								}
								break;
							}
						}

					}
					//rinse, repeat. 
					it++;
				}
				else
				{
					//delete add;
					tries++;
					if (tries > settings::max_tries)
					{
						//Adds a cap.
						pick = rand() % caps.size();
						add = new segment(*caps[pick]);
						add->mesh_ = new mesh::Mesh_c(*caps[pick]->mesh_);
						if (settings::color_test)
						{
							if (map.size() >= num_rooms)
							{
								mesh::recolor_mesh(add->mesh_, 0, 0, 255);
							}
						}
						add->x = c_node->x - add->nodes[0]->x;
						add->y = c_node->y - add->nodes[0]->y;
						add->z = c_node->z - add->nodes[0]->z;
						add->dir = angle_subtract(c_node->direction, ((int)add->nodes[0]->direction + 180) % 360);
						mesh::rotate_mesh(add->mesh_, add->dir, 0, 0);
						mesh::translate_mesh(add->mesh_, c_node->x - add->nodes[0]->x, c_node->y - add->nodes[0]->y, c_node->z - add->nodes[0]->z);
						calculate_bounds(add);
						map.push_back(add);

						tries = 0;
						it++;
					}
				}
			}
		}

	/*
		while (it < node_list.size()) //keeps working on the nodes, until we are at the last one.
		{
			//current node
			node * c_node = node_list[it];

			if (settings::show_nodes)
			{
				mesh::Mesh_c * n_mesh = new mesh::Mesh_c(*ends[0]->mesh_);
				n_mesh->x = c_node->x;
				n_mesh->z = c_node->z;
				n_mesh->y = 5;
				mesh::resize_mesh(n_mesh, 0.1, 0.1, 0.1);
				mesh::recolor_mesh(n_mesh, 0, 0, 255);
				mesh::rotate_mesh(n_mesh, c_node->direction, 0, 0);
				mesh::translate_mesh(n_mesh, c_node->x, c_node->y + 4, c_node->z);
				map_1.push_back(n_mesh);
			}
			//pick a new segment to add.
			segment * add;
			int pick;
			if (num_rooms >= settings::max_rooms)
			{
				pick = rand() % ends.size();
				add = new segment(*ends[pick]);
				add->mesh_ = new mesh::Mesh_c(*ends[pick]->mesh_);
				std::vector<node*> new_nodes;
				for (int i = 0; i < add->nodes.size(); i++)
				{
					new_nodes.push_back(new node(add->nodes[i]->type, add->nodes[i]->x, add->nodes[i]->y, add->nodes[i]->z, add->nodes[i]->direction));
				}
				add->nodes = new_nodes;
			}
			else
			{
				if (tries >= settings::max_tries / 2)	//if it has tried for half of the time to add a room, try adding a hall.
				{
					pick = rand() % halls.size();
					add = new segment(*halls[pick]);
					add->mesh_ = new mesh::Mesh_c(*halls[pick]->mesh_);
					std::vector<node*> new_nodes;
					for (int i = 0; i < add->nodes.size(); i++)
					{
						new_nodes.push_back(new node(add->nodes[i]->type, add->nodes[i]->x, add->nodes[i]->y, add->nodes[i]->z, add->nodes[i]->direction));
					}
					add->nodes = new_nodes;
				}
				else
				{
					pick = rand() % non_ends.size();
					add = new segment(*non_ends[pick]);
					add->mesh_ = new mesh::Mesh_c(*non_ends[pick]->mesh_);
					std::vector<node*> new_nodes;
					for (int i = 0; i < add->nodes.size(); i++)
					{
						new_nodes.push_back(new node(add->nodes[i]->type, add->nodes[i]->x, add->nodes[i]->y, add->nodes[i]->z, add->nodes[i]->direction));
					}
					add->nodes = new_nodes;
				}
			}

			//pick a node to attach to the current node
			int chosen_node = rand() % add->nodes.size();
			node * n_node = add->nodes[chosen_node]; //just the zeroth for now.  nothing fancy. Will need to change stuff if it was different.
			add->chosen_node = chosen_node;
			if (n_node->type != node::JOINT)
			{
				chosen_node = 0;
				node * n_node = add->nodes[chosen_node];
				add->chosen_node = chosen_node;
			}

			//attaches the segments together
			//	We find <tx,ty,tz> by <current_x - new_x, ... , ... >
			double tx = c_node->x - n_node->x;
			double ty = c_node->y - n_node->y;
			double tz = c_node->z - n_node->z;

			if (fabs(tx) < 0.001) tx = 0;
			if (fabs(ty) < 0.001) ty = 0;
			if (fabs(tz) < 0.001) tz = 0;
			//move the segment to position
			add->x += tx;
			add->y += ty;
			add->z += tz;
			//still need to do something about direction...   
			//	All nodes start with direction in Reference space. We need to transform to Real space, we need to apply turns and stuff.
			//	So all nodes will start at where they are relative to the mesh. We need to turn the mesh AND the nodes. and update node diretion.
			//	Maybe use angles for node direction, that way we can use math, not switch statements.
			//	We'll just make sure they are all 90° when making it.
			//SO we take 0 as pointing along reference. (Y axis (in blender. Z in openGL),  cannot remember +/-)
			//	then rotate around.
			//std::cout << tx << "  :  " << ty << "  :  " << tz << "\n";
			//Nodes need to rotate so they point in on each other. SO 180°. All nodes point out from the mesh.
			double dif_angle = angle_subtract(c_node->direction, ((int)n_node->direction + 180) % 360);

			//this is the angle to rotate.
			add->dir = dif_angle;
			//add it to the map

			//move the mesh into the proper position.
			mesh::rotate_mesh(add->mesh_, add->dir, n_node->x, n_node->z);
			mesh::translate_mesh(add->mesh_, add->x, add->y, add->z);
			add->mesh_->reconfigure();
			calculate_bounds(add);

			if (settings::color_test)
			{
				if (map.size() >= num_rooms)
				{
					mesh::recolor_mesh(add->mesh_, 0, 0, 255);
				}
			}
			//we test to see if the mesh fits in the grid
			if (test_new_segment(map, add))
			{
				tries = 0;
				if (add->type == segment::TYPE::ROOM)
					num_rooms++;

				map.push_back(add);
				//then we need to update all the nodes with this change in angle. and position.
				//rotate all the other nodes around this node by the angle.
				//I guess we can just and the rest of the nodes now.
				for (int i = 0; i < add->nodes.size(); i++)
				{
					if (i != chosen_node)
					{
						//So we update dir.
						add->nodes[i]->direction = (int)(add->nodes[i]->direction + dif_angle) % 360; //rotate the direction by the angle and fix to [0,360)
																									  //then we need to change the position...
																									  //	We first rotate with the angle, then translate.
						double s = sin(dif_angle*TO_RAD);
						double c = cos(dif_angle*TO_RAD);
						if (fabs(s) < 0.0001) s = 0;
						if (fabs(c) < 0.0001) c = 0;
						if (s < -0.99) s = -1;
						if (s > 0.99) s = 1;
						if (c < -0.99) c = -1;
						if (c > 0.99) c = 1;

						//translate point back to origin
						double n_x = add->nodes[i]->x - n_node->x;
						double n_z = add->nodes[i]->z - n_node->z;

						//rotate point
						double xnew = n_x * c - n_z * s;
						double znew = n_x * s + n_z * c;

						//translate point back
						add->nodes[i]->x = xnew + n_node->x;
						add->nodes[i]->z = znew + n_node->z;

						//translate point with Vector from before
						add->nodes[i]->x += tx;
						add->nodes[i]->y += ty;
						add->nodes[i]->z += tz;
						//add it to node list
						switch (add->nodes[i]->type)
						{
						case node::TYPE::JOINT:
							node_list.push_back(add->nodes[i]);
							break;
						case node::TYPE::SPAWN:
							spawn_nodes.push_back(add->nodes[i]);
							if (random() > 0.0)
							{
								holdable * pick_up = new holdable("src/pick_ups/key.ply");
								pick_up->mesh->x = add->nodes[i]->x;
								pick_up->mesh->y = add->nodes[i]->y;
								pick_up->mesh->z = add->nodes[i]->z;
								pick_up->mesh->azimuth = -add->nodes[i]->direction;	//negative because of how the renderer works.   some linear algebra mistake that would be too bothersome to fix now. Maybe fix during optimization.
								pick_up->mesh->roll = 0;
								pick_up->mesh->pitch = 0;
								pick_up->on_ground = false;
								objects_out.push_back(pick_up);
								out_meshes.push_back(pick_up->mesh);
							}
							break;
						case node::TYPE::OBJECT:
							if (object_list.size() > 0)
							{
								int pick = rand() % object_list.size();
								mesh::Mesh_c * mesh_ = new mesh::Mesh_c(*object_list[pick]);
								mesh_->x = add->nodes[i]->x;
								mesh_->y = add->nodes[i]->y;
								mesh_->z = add->nodes[i]->z;
								mesh_->azimuth = -add->nodes[i]->direction;	//negative because of how the renderer works.   some linear algebra mistake that would be too bothersome to fix now. Maybe fix during optimization.
								mesh_->roll = 0;
								mesh_->pitch = 0;
								out_meshes.push_back(mesh_);
								if (settings::object_collide)
								{
									double lx = 0;
									double ly = 0;
									double lz = 0;
									double hx = 0;
									double hy = 0;
									double hz = 0;

									mesh::calculate_bounds(new mesh::Mesh_c(*object_list[pick]), lx, ly, lz, hx, hy, hz);

									mesh_ = new mesh::Mesh_c(object_collider);
									mesh::set_bounds(mesh_, lx, ly, lz, hx, hy, hz);

									mesh::rotate_mesh(mesh_, add->nodes[i]->direction, 0, 0);
									mesh::translate_mesh(mesh_, add->nodes[i]->x, add->nodes[i]->y, add->nodes[i]->z);
									add->mesh_->reconfigure();

									objects.push_back(mesh_);
								}
								if (settings::show_nodes)
								{
									mesh::Mesh_c * n_mesh = new mesh::Mesh_c(*ends[0]->mesh_);
									mesh::resize_mesh(n_mesh, 0.1, 0.1, 0.1);
									mesh::recolor_mesh(n_mesh, 60, 240, 100);
									mesh::rotate_mesh(n_mesh, add->nodes[i]->direction, n_node->x, n_node->z);
									mesh::translate_mesh(n_mesh, add->nodes[i]->x, add->nodes[i]->y + 4, add->nodes[i]->z);
									map_1.push_back(n_mesh);
								}
							}
							break;
						case node::TYPE::WALL_OBJECT:
							if (wall_object_list.size() > 0)
							{
								int pick = rand() % wall_object_list.size();
								mesh::Mesh_c * mesh_ = new mesh::Mesh_c(*wall_object_list[pick]);
								mesh_->x = add->nodes[i]->x;
								mesh_->y = add->nodes[i]->y;
								mesh_->z = add->nodes[i]->z;
								mesh_->azimuth = 0;//-add->nodes[i]->direction
								mesh_->roll = 0;
								mesh_->pitch = 0;
								mesh::rotate_mesh(mesh_, add->nodes[i]->direction, n_node->x, n_node->z);
								out_meshes.push_back(mesh_);
								if (settings::object_collide)
								{
									double lx = 0;
									double ly = 0;
									double lz = 0;
									double hx = 0;
									double hy = 0;
									double hz = 0;

									mesh::calculate_bounds(new mesh::Mesh_c(*wall_object_list[pick]), lx, ly, lz, hx, hy, hz);

									mesh_ = new mesh::Mesh_c(wall_object_collider);
									mesh::set_bounds(mesh_, lx, ly, lz, hx, hy, hz);

									mesh::rotate_mesh(mesh_, add->nodes[i]->direction, 0, 0);
									mesh::translate_mesh(mesh_, add->nodes[i]->x, add->nodes[i]->y, add->nodes[i]->z);
									add->mesh_->reconfigure();

									objects.push_back(mesh_);
								}
								if (settings::show_nodes)
								{
									mesh::Mesh_c * n_mesh = new mesh::Mesh_c(*ends[0]->mesh_);
									mesh::resize_mesh(n_mesh, 0.1, 0.1, 0.1);
									mesh::recolor_mesh(n_mesh, 60, 240, 100);
									mesh::rotate_mesh(n_mesh, add->nodes[i]->direction, n_node->x, n_node->z);
									mesh::translate_mesh(n_mesh, add->nodes[i]->x, add->nodes[i]->y + 4, add->nodes[i]->z);
									map_1.push_back(n_mesh);
								}
							}
							break;
						case node::TYPE::ROOF_OBJECT:
							if (roof_object_list.size() > 0)
							{
								int pick = rand() % roof_object_list.size();
								mesh::Mesh_c * mesh_ = new mesh::Mesh_c(*roof_object_list[pick]);
								mesh_->x = add->nodes[i]->x;
								mesh_->y = add->nodes[i]->y;
								mesh_->z = add->nodes[i]->z;
								mesh_->azimuth = -add->nodes[i]->direction;
								mesh_->roll = 0;
								mesh_->pitch = 0;
								out_meshes.push_back(mesh_);
								if (settings::object_collide)
								{
									double lx = 0;
									double ly = 0;
									double lz = 0;
									double hx = 0;
									double hy = 0;
									double hz = 0;

									mesh::calculate_bounds(new mesh::Mesh_c(*roof_object_list[pick]), lx, ly, lz, hx, hy, hz);

									mesh_ = new mesh::Mesh_c(roof_object_collider);
									mesh::set_bounds(mesh_, lx, ly, lz, hx, hy, hz);

									mesh::rotate_mesh(mesh_, add->nodes[i]->direction, 0, 0);
									mesh::translate_mesh(mesh_, add->nodes[i]->x, add->nodes[i]->y, add->nodes[i]->z);
									add->mesh_->reconfigure();

									objects.push_back(mesh_);
								}
								if (settings::show_nodes)
								{
									mesh::Mesh_c * n_mesh = new mesh::Mesh_c(*ends[0]->mesh_);
									mesh::resize_mesh(n_mesh, 0.1, 0.1, 0.1);
									mesh::recolor_mesh(n_mesh, 60, 240, 100);
									mesh::rotate_mesh(n_mesh, add->nodes[i]->direction, n_node->x, n_node->z);
									mesh::translate_mesh(n_mesh, add->nodes[i]->x, add->nodes[i]->y + 4, add->nodes[i]->z);
									map_1.push_back(n_mesh);
								}
							}
							break;
						}
					}

				}
				//rinse, repeat. 
				it++;
			}
			else
			{
				//delete add;
				tries++;
				if (tries > settings::max_tries)
				{
					//Adds a cap.
					pick = rand() % caps.size();
					add = new segment(*caps[pick]);
					add->mesh_ = new mesh::Mesh_c(*caps[pick]->mesh_);
					if (settings::color_test)
					{
						if (map.size() >= num_rooms)
						{
							mesh::recolor_mesh(add->mesh_, 0, 0, 255);
						}
					}
					add->x = c_node->x - add->nodes[0]->x;
					add->y = c_node->y - add->nodes[0]->y;
					add->z = c_node->z - add->nodes[0]->z;
					add->dir = angle_subtract(c_node->direction, ((int)add->nodes[0]->direction + 180) % 360);
					mesh::rotate_mesh(add->mesh_, add->dir, 0, 0);
					mesh::translate_mesh(add->mesh_, c_node->x - add->nodes[0]->x, c_node->y - add->nodes[0]->y, c_node->z - add->nodes[0]->z);
					calculate_bounds(add);
					map.push_back(add);

					tries = 0;
					it++;
				}
			}
		}
*/
		if (settings::remove_hall_blockage)
		{
			std::vector<segment*> map_corrected;
			for (int i = 0; i < map.size(); i++)
			{
				if (map[i]->type == segment::CAP)
				{
					bool unique = true;
					for (int j = 0; j < map.size(); j++)
					{
						if (map[j]->type == segment::CAP)
						{
							if (i != j)
							{
								//	if (map[j]->x == map[i]->x && map[j]->y == map[i]->y && map[j]->z == map[i]->z)
								if (fabs(map[j]->x - map[i]->x) <= 0.001 && fabs(map[j]->y - map[i]->y) <= 0.001 && fabs(map[j]->z - map[i]->z) <= 0.001)
								{
									unique = false;
								}
							}
						}
					}
					if (unique)
						map_corrected.push_back(map[i]);
				}
				else
				{
					map_corrected.push_back(map[i]);
				}
			}
			map = map_corrected;
		}

		//convert map to mesh and export
		for (int i = 0; i < map.size(); i++)
		{
			//so a mesh takes a collection of triangles and then makes a list of values.
			//So I need to just add all the triangles.
			for (int j = 0; j < map[i]->mesh_->faces.size(); j++)
			{
				if (settings::snap_verts_to_grid)
				{
					mesh::snap_face(map[i]->mesh_->faces[j]);
				}
				mesh->faces.push_back(map[i]->mesh_->faces[j]);
			}
			delete map[i]->mesh_;
		}

		//add all of the nodes to be visible
		if (settings::show_nodes)
		{
			for (int i = 0; i < map_1.size(); i++)
			{
				for (int j = 0; j < map_1[i]->faces.size(); j++)
				{
					mesh->faces.push_back(map_1[i]->faces[j]);
				}
			}
		}


		mesh::Mesh_c * mesh_rr = new mesh::Mesh_c(*mesh);

		mesh_rr->s_type == mesh::Mesh_c::TYPE::DYNAMIC;
		mesh_rr->azimuth = 0;
		mesh_rr->pitch = 0;
		mesh_rr->roll = 0;
		mesh_rr->x = 0;
		mesh_rr->y = 0;
		mesh_rr->z = 0;
		mesh_rr->commit_mesh();
		mesh_rr->reconfigure();
		out_meshes.push_back(mesh_rr);

		for (int i = 0; i < objects.size(); i++)
		{
			for (int j = 0; j < objects[i]->faces.size(); j++)
			{
				if (settings::snap_verts_to_grid)
				{
					mesh::snap_face(objects[i]->faces[j]);
				}
				mesh->faces.push_back(objects[i]->faces[j]);
			}
		}

		mesh->s_type == mesh::Mesh_c::TYPE::STATIC;
		mesh->azimuth = 0;
		mesh->pitch = 0;
		mesh->roll = 0;
		mesh->x = 0;
		mesh->y = 0;
		mesh->z = 0;
		mesh->commit_mesh();
		mesh->reconfigure();
		return true;
	}

	bool make_map_adv_2(std::vector<segment> &segments, std::vector<mesh::Mesh_c *> object_list, std::vector<mesh::Mesh_c *> wall_object_list, std::vector<mesh::Mesh_c *> roof_object_list, mesh::Mesh_c *mesh, std::vector<mesh::Mesh_c *> &out_meshes, std::vector<seg::node *> &spawn_nodes, std::vector<Object*> &objects_out)
	{
		//this is the working array, the one that will get converted to a mesh

		std::vector<segment*> map;

		std::vector<mesh::Mesh_c*> map_1;

		std::vector<mesh::Mesh_c*> objects;

		//	mesh::Mesh_c object_collider;
		//	mesh::Mesh_c wall_object_collider;
		//	mesh::Mesh_c roof_object_collider;

		mesh::Mesh_c object_collider;
		reader::read_ply("src/better_segs_meshes/object_collider.ply", object_collider);

		mesh::Mesh_c wall_object_collider;
		reader::read_ply("src/better_segs_meshes/wall_object_collider.ply", wall_object_collider);

		mesh::Mesh_c roof_object_collider;
		reader::read_ply("src/better_segs_meshes/roof_object_collider.ply", roof_object_collider);

		//for organization, we go through the availible segments and sort them by TYPE
		//	ALSO, we get a bucket full of everything besides ENDS, 
		std::vector<segment*> strs;
		std::vector<segment*> rts;
		std::vector<segment*> lfts;
		std::vector<segment*> ts;
		std::vector<segment*> xs;

		std::vector<segment*> els;
		std::vector<segment*> ends;
		std::vector<segment*> caps;
		std::vector<segment*> non_ends;

		std::vector<segment*> halls;
		std::vector<segment*> rooms;

		//sort
		for (int i = 0; i < segments.size(); i++)
		{
			switch (segments[i].type)
			{
			case segment::TYPE::STRAIGHT:
				non_ends.push_back(&segments[i]);
				strs.push_back(&segments[i]);
				halls.push_back(&segments[i]);
				break;
			case segment::TYPE::RIGHT:
				non_ends.push_back(&segments[i]);
				rts.push_back(&segments[i]);
				halls.push_back(&segments[i]);
				break;
			case segment::TYPE::LEFT:
				non_ends.push_back(&segments[i]);
				lfts.push_back(&segments[i]);
				halls.push_back(&segments[i]);
				break;
			case segment::TYPE::T:
				non_ends.push_back(&segments[i]);
				ts.push_back(&segments[i]);
				halls.push_back(&segments[i]);
				break;
			case segment::TYPE::CROSS:
				non_ends.push_back(&segments[i]);
				xs.push_back(&segments[i]);
				halls.push_back(&segments[i]);
				break;
			case segment::TYPE::ELEVATOR:
				non_ends.push_back(&segments[i]);
				els.push_back(&segments[i]);
				break;
			case segment::TYPE::END:
				ends.push_back(&segments[i]);
				rooms.push_back(&segments[i]);
				break;
			case segment::TYPE::ROOM:
				rooms.push_back(&segments[i]);
				non_ends.push_back(&segments[i]);
				break;
			case segment::TYPE::CAP:
				caps.push_back(&segments[i]);
				break;
			}
			
		}

		//node buffer. Maybe a linked list...   i don't know...  I'll just use an iterator.
		std::vector<node*> node_list;
		int it = 0;
		//Start with a dead end. There may be start rooms, but this will work for now.
		segment * start = new segment(*ends[rand() % ends.size()]);	//pick a random end to start with. COPY the data..
		start->x = 0;		//move it to the origin
		start->y = 0;
		start->z = 0;
		start->dir = 0;
		mesh::rotate_mesh(start->mesh_, start->dir, 0, 0);
		mesh::translate_mesh(start->mesh_, start->x, start->y, start->z);
		calculate_bounds(start);

		node * start_node = new node(start->nodes[0]->type, start->nodes[0]->x, start->nodes[0]->y, start->nodes[0]->z, start->nodes[0]->direction);
		map.push_back(start);
		node_list.push_back(start_node);

		//map_node * node_map = new map_node(*start_node);

		for (int i = 1; i < start->nodes.size(); i++)
		{
			//add it to node list
			switch (start->nodes[i]->type)
			{
			case node::TYPE::JOINT:
				node_list.push_back(start->nodes[i]);
				break;
			case node::TYPE::SPAWN:
				spawn_nodes.push_back(start->nodes[i]);
				break;
			case node::TYPE::OBJECT:
				if (object_list.size() > 0)
				{
					int pick = rand() % object_list.size();
					mesh::Mesh_c * mesh_ = new mesh::Mesh_c(*object_list[pick]);
					mesh_->x = start->nodes[i]->x;
					mesh_->y = start->nodes[i]->y;
					mesh_->z = start->nodes[i]->z;
					mesh_->azimuth = -start->nodes[i]->direction;	//negative because of how the renderer works.   some linear algebra mistake that would be too bothersome to fix now. Maybe fix during optimization.
					mesh_->roll = 0;
					mesh_->pitch = 0;
					out_meshes.push_back(mesh_);
					if (settings::object_collide)
					{
						double lx = 0;
						double ly = 0;
						double lz = 0;
						double hx = 0;
						double hy = 0;
						double hz = 0;

						mesh::calculate_bounds(new mesh::Mesh_c(*object_list[pick]), lx, ly, lz, hx, hy, hz);

						mesh_ = new mesh::Mesh_c(object_collider);
						mesh::set_bounds(mesh_, lx, ly, lz, hx, hy, hz);

						mesh::rotate_mesh(mesh_, start->nodes[i]->direction, 0, 0);
						mesh::translate_mesh(mesh_, start->nodes[i]->x, start->nodes[i]->y, start->nodes[i]->z);
						start->mesh_->reconfigure();

						objects.push_back(mesh_);
					}
					if (settings::show_nodes)
					{
						mesh::Mesh_c * n_mesh = new mesh::Mesh_c(*ends[0]->mesh_);
						mesh::resize_mesh(n_mesh, 0.1, 0.1, 0.1);
						mesh::recolor_mesh(n_mesh, 60, 240, 100);
						mesh::rotate_mesh(n_mesh, start->nodes[i]->direction, 0, 0);
						mesh::translate_mesh(n_mesh, start->nodes[i]->x, start->nodes[i]->y + 4, start->nodes[i]->z);
						map_1.push_back(n_mesh);
					}
				}
				break;
			case node::TYPE::WALL_OBJECT:
				if (wall_object_list.size() > 0)
				{
					int pick = rand() % wall_object_list.size();
					mesh::Mesh_c * mesh_ = new mesh::Mesh_c(*wall_object_list[pick]);
					mesh_->x = start->nodes[i]->x;
					mesh_->y = start->nodes[i]->y;
					mesh_->z = start->nodes[i]->z;
					mesh_->azimuth = 0;//-add->nodes[i]->direction
					mesh_->roll = 0;
					mesh_->pitch = 0;
					mesh::rotate_mesh(mesh_, start->nodes[i]->direction, 0, 0);
					out_meshes.push_back(mesh_);
					if (settings::object_collide)
					{
						double lx = 0;
						double ly = 0;
						double lz = 0;
						double hx = 0;
						double hy = 0;
						double hz = 0;

						mesh::calculate_bounds(new mesh::Mesh_c(*wall_object_list[pick]), lx, ly, lz, hx, hy, hz);

						mesh_ = new mesh::Mesh_c(wall_object_collider);
						mesh::set_bounds(mesh_, lx, ly, lz, hx, hy, hz);

						mesh::rotate_mesh(mesh_, start->nodes[i]->direction, 0, 0);
						mesh::translate_mesh(mesh_, start->nodes[i]->x, start->nodes[i]->y, start->nodes[i]->z);
						start->mesh_->reconfigure();

						objects.push_back(mesh_);
					}
					if (settings::show_nodes)
					{
						mesh::Mesh_c * n_mesh = new mesh::Mesh_c(*ends[0]->mesh_);
						mesh::resize_mesh(n_mesh, 0.1, 0.1, 0.1);
						mesh::recolor_mesh(n_mesh, 60, 240, 100);
						mesh::rotate_mesh(n_mesh, start->nodes[i]->direction, 0, 0);
						mesh::translate_mesh(n_mesh, start->nodes[i]->x, start->nodes[i]->y + 4, start->nodes[i]->z);
						map_1.push_back(n_mesh);
					}
				}
				break;
			case node::TYPE::ROOF_OBJECT:
				if (roof_object_list.size() > 0)
				{
					int pick = rand() % roof_object_list.size();
					mesh::Mesh_c * mesh_ = new mesh::Mesh_c(*roof_object_list[pick]);
					mesh_->x = start->nodes[i]->x;
					mesh_->y = start->nodes[i]->y;
					mesh_->z = start->nodes[i]->z;
					mesh_->azimuth = -start->nodes[i]->direction;
					mesh_->roll = 0;
					mesh_->pitch = 0;
					out_meshes.push_back(mesh_);
					if (settings::object_collide)
					{
						double lx = 0;
						double ly = 0;
						double lz = 0;
						double hx = 0;
						double hy = 0;
						double hz = 0;

						mesh::calculate_bounds(new mesh::Mesh_c(*roof_object_list[pick]), lx, ly, lz, hx, hy, hz);

						mesh_ = new mesh::Mesh_c(roof_object_collider);
						mesh::set_bounds(mesh_, lx, ly, lz, hx, hy, hz);

						mesh::rotate_mesh(mesh_, start->nodes[i]->direction, 0, 0);
						mesh::translate_mesh(mesh_, start->nodes[i]->x, start->nodes[i]->y, start->nodes[i]->z);
						start->mesh_->reconfigure();

						objects.push_back(mesh_);
					}
					if (settings::show_nodes)
					{
						mesh::Mesh_c * n_mesh = new mesh::Mesh_c(*ends[0]->mesh_);
						mesh::resize_mesh(n_mesh, 0.1, 0.1, 0.1);
						mesh::recolor_mesh(n_mesh, 60, 240, 100);
						mesh::rotate_mesh(n_mesh, start->nodes[i]->direction, 0, 0);
						mesh::translate_mesh(n_mesh, start->nodes[i]->x, start->nodes[i]->y + 4, start->nodes[i]->z);
						map_1.push_back(n_mesh);
					}
				}
				break;
			}
			if (start->nodes[i]->type == node::TYPE::JOINT)
			{
				start_node->children.push_back(start->nodes[i]);
			}
			else
			{
				start_node->contents.push_back(start->nodes[i]);
			}
		}

		
		/*
		if (start->nodes[i]->type == node::TYPE::JOINT)
		{
			node_map->children.push_back(new map_node(*start->nodes[i]));
		}
		else
		{
			node_map->contents.push_back(new map_node(*start->nodes[i]));
		}
		*/



		//generate map
		int tries = 0;
		int num_rooms = 0;
		bool only_one = true;
		while (it < node_list.size()) //keeps working on the nodes, until we are at the last one.
		{

			//current node
			node * c_node = node_list[it];

			if (settings::show_nodes)
			{
				mesh::Mesh_c * n_mesh = new mesh::Mesh_c(*ends[0]->mesh_);
				n_mesh->x = c_node->x;
				n_mesh->z = c_node->z;
				n_mesh->y = 5;
				mesh::resize_mesh(n_mesh, 0.1, 0.1, 0.1);
				mesh::recolor_mesh(n_mesh, 0, 0, 255);
				mesh::rotate_mesh(n_mesh, c_node->direction, 0, 0);
				mesh::translate_mesh(n_mesh, c_node->x, c_node->y + 4, c_node->z);
				map_1.push_back(n_mesh);
			}
			//pick a new segment to add.
			segment * add;
			int pick;
			if (num_rooms >= settings::max_rooms)
			{
				pick = rand() % ends.size();
				add = new segment(*ends[pick]);
				add->mesh_ = new mesh::Mesh_c(*ends[pick]->mesh_);
				std::vector<node*> new_nodes;
				for (int i = 0; i < add->nodes.size(); i++)
				{
					new_nodes.push_back(new node(add->nodes[i]->type, add->nodes[i]->x, add->nodes[i]->y, add->nodes[i]->z, add->nodes[i]->direction));
				}
				add->nodes = new_nodes;
			}
			else
			{
				if (tries >= settings::max_tries / 2)	//if it has tried for half of the time to add a room, try adding a hall.
				{
					pick = rand() % halls.size();
					add = new segment(*halls[pick]);
					add->mesh_ = new mesh::Mesh_c(*halls[pick]->mesh_);
					std::vector<node*> new_nodes;
					for (int i = 0; i < add->nodes.size(); i++)
					{
						new_nodes.push_back(new node(add->nodes[i]->type, add->nodes[i]->x, add->nodes[i]->y, add->nodes[i]->z, add->nodes[i]->direction));
					}
					add->nodes = new_nodes;
				}
				else
				{
					/*
					double choice = random();
					std::cout << choice << "\n";
					if (choice <= settings::hall_fraction)
					{
					pick = rand() % halls.size();
					add = new segment(*halls[pick]);
					add->mesh_ = new mesh::Mesh_c(*halls[pick]->mesh_);
					}
					else
					{
					pick = rand() % rooms.size();
					add = new segment(*rooms[pick]);
					add->mesh_ = new mesh::Mesh_c(*rooms[pick]->mesh_);
					}
					//*/
					pick = rand() % non_ends.size();
					add = new segment(*non_ends[pick]);
					add->mesh_ = new mesh::Mesh_c(*non_ends[pick]->mesh_);
					std::vector<node*> new_nodes;
					for (int i = 0; i < add->nodes.size(); i++)
					{
						new_nodes.push_back(new node(add->nodes[i]->type, add->nodes[i]->x, add->nodes[i]->y, add->nodes[i]->z, add->nodes[i]->direction));
					}
					add->nodes = new_nodes;
				}
			}

			//pick a node to attach to the current node
			int chosen_node = rand() % add->nodes.size();
			node * n_node = add->nodes[chosen_node]; //just the zeroth for now.  nothing fancy. Will need to change stuff if it was different.
			add->chosen_node = chosen_node;
			if (n_node->type != node::JOINT)
			{
				chosen_node = 0;
				node * n_node = add->nodes[chosen_node];
				add->chosen_node = chosen_node;
			}

			//attaches the segments together
			//	We find <tx,ty,tz> by <current_x - new_x, ... , ... >
			double tx = c_node->x - n_node->x;
			double ty = c_node->y - n_node->y;
			double tz = c_node->z - n_node->z;

			if (fabs(tx) < 0.001) tx = 0;
			if (fabs(ty) < 0.001) ty = 0;
			if (fabs(tz) < 0.001) tz = 0;
			//move the segment to position
			add->x += tx;
			add->y += ty;
			add->z += tz;
			//still need to do something about direction...   
			//	All nodes start with direction in Reference space. We need to transform to Real space, we need to apply turns and stuff.
			//	So all nodes will start at where they are relative to the mesh. We need to turn the mesh AND the nodes. and update node diretion.
			//	Maybe use angles for node direction, that way we can use math, not switch statements.
			//	We'll just make sure they are all 90° when making it.
			//SO we take 0 as pointing along reference. (Y axis (in blender. Z in openGL),  cannot remember +/-)
			//	then rotate around.
			//std::cout << tx << "  :  " << ty << "  :  " << tz << "\n";
			//Nodes need to rotate so they point in on each other. SO 180°. All nodes point out from the mesh.
			double dif_angle = angle_subtract(c_node->direction, ((int)n_node->direction + 180) % 360);

			//this is the angle to rotate.
			add->dir = dif_angle;
			//add it to the map

			//move the mesh into the proper position.
			mesh::rotate_mesh(add->mesh_, add->dir, n_node->x, n_node->z);
			mesh::translate_mesh(add->mesh_, add->x, add->y, add->z);
			add->mesh_->reconfigure();
			calculate_bounds(add);

			if (settings::color_test)
			{
				if (map.size() >= num_rooms)
				{
					mesh::recolor_mesh(add->mesh_, 0, 0, 255);
				}
			}
			//we test to see if the mesh fits in the grid
			if (test_new_segment(map, add))
			{
				tries = 0;
				if (add->type == segment::TYPE::ROOM)
					num_rooms++;

				map.push_back(add);
				//then we need to update all the nodes with this change in angle. and position.
				//rotate all the other nodes around this node by the angle.
				//I guess we can just and the rest of the nodes now.
				for (int i = 0; i < add->nodes.size(); i++)
				{
					//add all nodes to the map
					//add it to node map
					if (add->nodes[i]->type == node::TYPE::JOINT)
					{
						c_node->children.push_back(add->nodes[i]);
					}
					else
					{
						c_node->contents.push_back(add->nodes[i]);
					}

					if (i != chosen_node)
					{
						//So we update dir.
						add->nodes[i]->direction = (int)(add->nodes[i]->direction + dif_angle) % 360; //rotate the direction by the angle and fix to [0,360)
																										//then we need to change the position...
																										//	We first rotate with the angle, then translate.
						double s = sin(dif_angle*TO_RAD);
						double c = cos(dif_angle*TO_RAD);
						if (fabs(s) < 0.0001) s = 0;
						if (fabs(c) < 0.0001) c = 0;
						if (s < -0.99) s = -1;
						if (s > 0.99) s = 1;
						if (c < -0.99) c = -1;
						if (c > 0.99) c = 1;

						//translate point back to origin
						double n_x = add->nodes[i]->x - n_node->x;
						double n_z = add->nodes[i]->z - n_node->z;

						//rotate point
						double xnew = n_x * c - n_z * s;
						double znew = n_x * s + n_z * c;

						//translate point back
						add->nodes[i]->x = xnew + n_node->x;
						add->nodes[i]->z = znew + n_node->z;

						//translate point with Vector from before
						add->nodes[i]->x += tx;
						add->nodes[i]->y += ty;
						add->nodes[i]->z += tz;
						//add it to node list
						switch (add->nodes[i]->type)
						{
						case node::TYPE::JOINT:
							node_list.push_back(add->nodes[i]);
							break;
						case node::TYPE::SPAWN:
							spawn_nodes.push_back(add->nodes[i]);
							/*if (random() > 0.0)
							{
							holdable * pick_up = new holdable("src/pick_ups/key.ply");
							pick_up->mesh->x = add->nodes[i]->x;
							pick_up->mesh->y = add->nodes[i]->y;
							pick_up->mesh->z = add->nodes[i]->z;
							pick_up->mesh->azimuth = -add->nodes[i]->direction;	//negative because of how the renderer works.   some linear algebra mistake that would be too bothersome to fix now. Maybe fix during optimization.
							pick_up->mesh->roll = 0;
							pick_up->mesh->pitch = 0;
							pick_up->on_ground = false;
							objects_out.push_back(pick_up);
							out_meshes.push_back(pick_up->mesh);
							}*/
							break;
						case node::TYPE::OBJECT:
							if (object_list.size() > 0)
							{
								int pick = rand() % object_list.size();
								mesh::Mesh_c * mesh_ = new mesh::Mesh_c(*object_list[pick]);
								mesh_->x = add->nodes[i]->x;
								mesh_->y = add->nodes[i]->y;
								mesh_->z = add->nodes[i]->z;
								mesh_->azimuth = -add->nodes[i]->direction;	//negative because of how the renderer works.   some linear algebra mistake that would be too bothersome to fix now. Maybe fix during optimization.
								mesh_->roll = 0;
								mesh_->pitch = 0;
								out_meshes.push_back(mesh_);
								if (settings::object_collide)
								{
									double lx = 0;
									double ly = 0;
									double lz = 0;
									double hx = 0;
									double hy = 0;
									double hz = 0;

									mesh::calculate_bounds(new mesh::Mesh_c(*object_list[pick]), lx, ly, lz, hx, hy, hz);

									mesh_ = new mesh::Mesh_c(object_collider);
									mesh::set_bounds(mesh_, lx, ly, lz, hx, hy, hz);

									mesh::rotate_mesh(mesh_, add->nodes[i]->direction, 0, 0);
									mesh::translate_mesh(mesh_, add->nodes[i]->x, add->nodes[i]->y, add->nodes[i]->z);
									add->mesh_->reconfigure();

									objects.push_back(mesh_);
								}
								if (settings::show_nodes)
								{
									mesh::Mesh_c * n_mesh = new mesh::Mesh_c(*ends[0]->mesh_);
									mesh::resize_mesh(n_mesh, 0.1, 0.1, 0.1);
									mesh::recolor_mesh(n_mesh, 60, 240, 100);
									mesh::rotate_mesh(n_mesh, add->nodes[i]->direction, n_node->x, n_node->z);
									mesh::translate_mesh(n_mesh, add->nodes[i]->x, add->nodes[i]->y + 4, add->nodes[i]->z);
									map_1.push_back(n_mesh);
								}
							}
							break;
						case node::TYPE::WALL_OBJECT:
							if (wall_object_list.size() > 0)
							{
								int pick = rand() % wall_object_list.size();
								mesh::Mesh_c * mesh_ = new mesh::Mesh_c(*wall_object_list[pick]);
								mesh_->x = add->nodes[i]->x;
								mesh_->y = add->nodes[i]->y;
								mesh_->z = add->nodes[i]->z;
								mesh_->azimuth = 0;//-add->nodes[i]->direction
								mesh_->roll = 0;
								mesh_->pitch = 0;
								mesh::rotate_mesh(mesh_, add->nodes[i]->direction, n_node->x, n_node->z);
								out_meshes.push_back(mesh_);
								if (settings::object_collide)
								{
									double lx = 0;
									double ly = 0;
									double lz = 0;
									double hx = 0;
									double hy = 0;
									double hz = 0;

									mesh::calculate_bounds(new mesh::Mesh_c(*wall_object_list[pick]), lx, ly, lz, hx, hy, hz);

									mesh_ = new mesh::Mesh_c(wall_object_collider);
									mesh::set_bounds(mesh_, lx, ly, lz, hx, hy, hz);

									mesh::rotate_mesh(mesh_, add->nodes[i]->direction, 0, 0);
									mesh::translate_mesh(mesh_, add->nodes[i]->x, add->nodes[i]->y, add->nodes[i]->z);
									add->mesh_->reconfigure();

									objects.push_back(mesh_);
								}
								if (settings::show_nodes)
								{
									mesh::Mesh_c * n_mesh = new mesh::Mesh_c(*ends[0]->mesh_);
									mesh::resize_mesh(n_mesh, 0.1, 0.1, 0.1);
									mesh::recolor_mesh(n_mesh, 60, 240, 100);
									mesh::rotate_mesh(n_mesh, add->nodes[i]->direction, n_node->x, n_node->z);
									mesh::translate_mesh(n_mesh, add->nodes[i]->x, add->nodes[i]->y + 4, add->nodes[i]->z);
									map_1.push_back(n_mesh);
								}
							}
							break;
						case node::TYPE::ROOF_OBJECT:
							if (roof_object_list.size() > 0)
							{
								int pick = rand() % roof_object_list.size();
								mesh::Mesh_c * mesh_ = new mesh::Mesh_c(*roof_object_list[pick]);
								mesh_->x = add->nodes[i]->x;
								mesh_->y = add->nodes[i]->y;
								mesh_->z = add->nodes[i]->z;
								mesh_->azimuth = -add->nodes[i]->direction;
								mesh_->roll = 0;
								mesh_->pitch = 0;
								out_meshes.push_back(mesh_);
								if (settings::object_collide)
								{
									double lx = 0;
									double ly = 0;
									double lz = 0;
									double hx = 0;
									double hy = 0;
									double hz = 0;

									mesh::calculate_bounds(new mesh::Mesh_c(*roof_object_list[pick]), lx, ly, lz, hx, hy, hz);

									mesh_ = new mesh::Mesh_c(roof_object_collider);
									mesh::set_bounds(mesh_, lx, ly, lz, hx, hy, hz);

									mesh::rotate_mesh(mesh_, add->nodes[i]->direction, 0, 0);
									mesh::translate_mesh(mesh_, add->nodes[i]->x, add->nodes[i]->y, add->nodes[i]->z);
									add->mesh_->reconfigure();

									objects.push_back(mesh_);
								}
								if (settings::show_nodes)
								{
									mesh::Mesh_c * n_mesh = new mesh::Mesh_c(*ends[0]->mesh_);
									mesh::resize_mesh(n_mesh, 0.1, 0.1, 0.1);
									mesh::recolor_mesh(n_mesh, 60, 240, 100);
									mesh::rotate_mesh(n_mesh, add->nodes[i]->direction, n_node->x, n_node->z);
									mesh::translate_mesh(n_mesh, add->nodes[i]->x, add->nodes[i]->y + 4, add->nodes[i]->z);
									map_1.push_back(n_mesh);
								}
							}
							break;
						}
					}

				}
				//rinse, repeat. 
				it++;
			}
			else
			{
				//delete add;
				tries++;
				if (tries > settings::max_tries)
				{
					//Adds a cap.
					pick = rand() % caps.size();
					add = new segment(*caps[pick]);
					add->mesh_ = new mesh::Mesh_c(*caps[pick]->mesh_);
					if (settings::color_test)
					{
						if (map.size() >= num_rooms)
						{
							mesh::recolor_mesh(add->mesh_, 0, 0, 255);
						}
					}
					add->x = c_node->x - add->nodes[0]->x;
					add->y = c_node->y - add->nodes[0]->y;
					add->z = c_node->z - add->nodes[0]->z;
					add->dir = angle_subtract(c_node->direction, ((int)add->nodes[0]->direction + 180) % 360);
					mesh::rotate_mesh(add->mesh_, add->dir, 0, 0);
					mesh::translate_mesh(add->mesh_, c_node->x - add->nodes[0]->x, c_node->y - add->nodes[0]->y, c_node->z - add->nodes[0]->z);
					calculate_bounds(add);
					map.push_back(add);

					tries = 0;
					it++;
				}
			}
			
		}


		//start_node is the map
		//so now we make the doors. Maybe rank the paths. Go down each and count how long it is.
		//recursion.
		
		/*
		while (it < node_list.size()) //keeps working on the nodes, until we are at the last one.
		{
		//current node
		node * c_node = node_list[it];

		if (settings::show_nodes)
		{
		mesh::Mesh_c * n_mesh = new mesh::Mesh_c(*ends[0]->mesh_);
		n_mesh->x = c_node->x;
		n_mesh->z = c_node->z;
		n_mesh->y = 5;
		mesh::resize_mesh(n_mesh, 0.1, 0.1, 0.1);
		mesh::recolor_mesh(n_mesh, 0, 0, 255);
		mesh::rotate_mesh(n_mesh, c_node->direction, 0, 0);
		mesh::translate_mesh(n_mesh, c_node->x, c_node->y + 4, c_node->z);
		map_1.push_back(n_mesh);
		}
		//pick a new segment to add.
		segment * add;
		int pick;
		if (num_rooms >= settings::max_rooms)
		{
		pick = rand() % ends.size();
		add = new segment(*ends[pick]);
		add->mesh_ = new mesh::Mesh_c(*ends[pick]->mesh_);
		std::vector<node*> new_nodes;
		for (int i = 0; i < add->nodes.size(); i++)
		{
		new_nodes.push_back(new node(add->nodes[i]->type, add->nodes[i]->x, add->nodes[i]->y, add->nodes[i]->z, add->nodes[i]->direction));
		}
		add->nodes = new_nodes;
		}
		else
		{
		if (tries >= settings::max_tries / 2)	//if it has tried for half of the time to add a room, try adding a hall.
		{
		pick = rand() % halls.size();
		add = new segment(*halls[pick]);
		add->mesh_ = new mesh::Mesh_c(*halls[pick]->mesh_);
		std::vector<node*> new_nodes;
		for (int i = 0; i < add->nodes.size(); i++)
		{
		new_nodes.push_back(new node(add->nodes[i]->type, add->nodes[i]->x, add->nodes[i]->y, add->nodes[i]->z, add->nodes[i]->direction));
		}
		add->nodes = new_nodes;
		}
		else
		{
		pick = rand() % non_ends.size();
		add = new segment(*non_ends[pick]);
		add->mesh_ = new mesh::Mesh_c(*non_ends[pick]->mesh_);
		std::vector<node*> new_nodes;
		for (int i = 0; i < add->nodes.size(); i++)
		{
		new_nodes.push_back(new node(add->nodes[i]->type, add->nodes[i]->x, add->nodes[i]->y, add->nodes[i]->z, add->nodes[i]->direction));
		}
		add->nodes = new_nodes;
		}
		}

		//pick a node to attach to the current node
		int chosen_node = rand() % add->nodes.size();
		node * n_node = add->nodes[chosen_node]; //just the zeroth for now.  nothing fancy. Will need to change stuff if it was different.
		add->chosen_node = chosen_node;
		if (n_node->type != node::JOINT)
		{
		chosen_node = 0;
		node * n_node = add->nodes[chosen_node];
		add->chosen_node = chosen_node;
		}

		//attaches the segments together
		//	We find <tx,ty,tz> by <current_x - new_x, ... , ... >
		double tx = c_node->x - n_node->x;
		double ty = c_node->y - n_node->y;
		double tz = c_node->z - n_node->z;

		if (fabs(tx) < 0.001) tx = 0;
		if (fabs(ty) < 0.001) ty = 0;
		if (fabs(tz) < 0.001) tz = 0;
		//move the segment to position
		add->x += tx;
		add->y += ty;
		add->z += tz;
		//still need to do something about direction...
		//	All nodes start with direction in Reference space. We need to transform to Real space, we need to apply turns and stuff.
		//	So all nodes will start at where they are relative to the mesh. We need to turn the mesh AND the nodes. and update node diretion.
		//	Maybe use angles for node direction, that way we can use math, not switch statements.
		//	We'll just make sure they are all 90° when making it.
		//SO we take 0 as pointing along reference. (Y axis (in blender. Z in openGL),  cannot remember +/-)
		//	then rotate around.
		//std::cout << tx << "  :  " << ty << "  :  " << tz << "\n";
		//Nodes need to rotate so they point in on each other. SO 180°. All nodes point out from the mesh.
		double dif_angle = angle_subtract(c_node->direction, ((int)n_node->direction + 180) % 360);

		//this is the angle to rotate.
		add->dir = dif_angle;
		//add it to the map

		//move the mesh into the proper position.
		mesh::rotate_mesh(add->mesh_, add->dir, n_node->x, n_node->z);
		mesh::translate_mesh(add->mesh_, add->x, add->y, add->z);
		add->mesh_->reconfigure();
		calculate_bounds(add);

		if (settings::color_test)
		{
		if (map.size() >= num_rooms)
		{
		mesh::recolor_mesh(add->mesh_, 0, 0, 255);
		}
		}
		//we test to see if the mesh fits in the grid
		if (test_new_segment(map, add))
		{
		tries = 0;
		if (add->type == segment::TYPE::ROOM)
		num_rooms++;

		map.push_back(add);
		//then we need to update all the nodes with this change in angle. and position.
		//rotate all the other nodes around this node by the angle.
		//I guess we can just and the rest of the nodes now.
		for (int i = 0; i < add->nodes.size(); i++)
		{
		if (i != chosen_node)
		{
		//So we update dir.
		add->nodes[i]->direction = (int)(add->nodes[i]->direction + dif_angle) % 360; //rotate the direction by the angle and fix to [0,360)
		//then we need to change the position...
		//	We first rotate with the angle, then translate.
		double s = sin(dif_angle*TO_RAD);
		double c = cos(dif_angle*TO_RAD);
		if (fabs(s) < 0.0001) s = 0;
		if (fabs(c) < 0.0001) c = 0;
		if (s < -0.99) s = -1;
		if (s > 0.99) s = 1;
		if (c < -0.99) c = -1;
		if (c > 0.99) c = 1;

		//translate point back to origin
		double n_x = add->nodes[i]->x - n_node->x;
		double n_z = add->nodes[i]->z - n_node->z;

		//rotate point
		double xnew = n_x * c - n_z * s;
		double znew = n_x * s + n_z * c;

		//translate point back
		add->nodes[i]->x = xnew + n_node->x;
		add->nodes[i]->z = znew + n_node->z;

		//translate point with Vector from before
		add->nodes[i]->x += tx;
		add->nodes[i]->y += ty;
		add->nodes[i]->z += tz;
		//add it to node list
		switch (add->nodes[i]->type)
		{
		case node::TYPE::JOINT:
		node_list.push_back(add->nodes[i]);
		break;
		case node::TYPE::SPAWN:
		spawn_nodes.push_back(add->nodes[i]);
		if (random() > 0.0)
		{
		holdable * pick_up = new holdable("src/pick_ups/key.ply");
		pick_up->mesh->x = add->nodes[i]->x;
		pick_up->mesh->y = add->nodes[i]->y;
		pick_up->mesh->z = add->nodes[i]->z;
		pick_up->mesh->azimuth = -add->nodes[i]->direction;	//negative because of how the renderer works.   some linear algebra mistake that would be too bothersome to fix now. Maybe fix during optimization.
		pick_up->mesh->roll = 0;
		pick_up->mesh->pitch = 0;
		pick_up->on_ground = false;
		objects_out.push_back(pick_up);
		out_meshes.push_back(pick_up->mesh);
		}
		break;
		case node::TYPE::OBJECT:
		if (object_list.size() > 0)
		{
		int pick = rand() % object_list.size();
		mesh::Mesh_c * mesh_ = new mesh::Mesh_c(*object_list[pick]);
		mesh_->x = add->nodes[i]->x;
		mesh_->y = add->nodes[i]->y;
		mesh_->z = add->nodes[i]->z;
		mesh_->azimuth = -add->nodes[i]->direction;	//negative because of how the renderer works.   some linear algebra mistake that would be too bothersome to fix now. Maybe fix during optimization.
		mesh_->roll = 0;
		mesh_->pitch = 0;
		out_meshes.push_back(mesh_);
		if (settings::object_collide)
		{
		double lx = 0;
		double ly = 0;
		double lz = 0;
		double hx = 0;
		double hy = 0;
		double hz = 0;

		mesh::calculate_bounds(new mesh::Mesh_c(*object_list[pick]), lx, ly, lz, hx, hy, hz);

		mesh_ = new mesh::Mesh_c(object_collider);
		mesh::set_bounds(mesh_, lx, ly, lz, hx, hy, hz);

		mesh::rotate_mesh(mesh_, add->nodes[i]->direction, 0, 0);
		mesh::translate_mesh(mesh_, add->nodes[i]->x, add->nodes[i]->y, add->nodes[i]->z);
		add->mesh_->reconfigure();

		objects.push_back(mesh_);
		}
		if (settings::show_nodes)
		{
		mesh::Mesh_c * n_mesh = new mesh::Mesh_c(*ends[0]->mesh_);
		mesh::resize_mesh(n_mesh, 0.1, 0.1, 0.1);
		mesh::recolor_mesh(n_mesh, 60, 240, 100);
		mesh::rotate_mesh(n_mesh, add->nodes[i]->direction, n_node->x, n_node->z);
		mesh::translate_mesh(n_mesh, add->nodes[i]->x, add->nodes[i]->y + 4, add->nodes[i]->z);
		map_1.push_back(n_mesh);
		}
		}
		break;
		case node::TYPE::WALL_OBJECT:
		if (wall_object_list.size() > 0)
		{
		int pick = rand() % wall_object_list.size();
		mesh::Mesh_c * mesh_ = new mesh::Mesh_c(*wall_object_list[pick]);
		mesh_->x = add->nodes[i]->x;
		mesh_->y = add->nodes[i]->y;
		mesh_->z = add->nodes[i]->z;
		mesh_->azimuth = 0;//-add->nodes[i]->direction
		mesh_->roll = 0;
		mesh_->pitch = 0;
		mesh::rotate_mesh(mesh_, add->nodes[i]->direction, n_node->x, n_node->z);
		out_meshes.push_back(mesh_);
		if (settings::object_collide)
		{
		double lx = 0;
		double ly = 0;
		double lz = 0;
		double hx = 0;
		double hy = 0;
		double hz = 0;

		mesh::calculate_bounds(new mesh::Mesh_c(*wall_object_list[pick]), lx, ly, lz, hx, hy, hz);

		mesh_ = new mesh::Mesh_c(wall_object_collider);
		mesh::set_bounds(mesh_, lx, ly, lz, hx, hy, hz);

		mesh::rotate_mesh(mesh_, add->nodes[i]->direction, 0, 0);
		mesh::translate_mesh(mesh_, add->nodes[i]->x, add->nodes[i]->y, add->nodes[i]->z);
		add->mesh_->reconfigure();

		objects.push_back(mesh_);
		}
		if (settings::show_nodes)
		{
		mesh::Mesh_c * n_mesh = new mesh::Mesh_c(*ends[0]->mesh_);
		mesh::resize_mesh(n_mesh, 0.1, 0.1, 0.1);
		mesh::recolor_mesh(n_mesh, 60, 240, 100);
		mesh::rotate_mesh(n_mesh, add->nodes[i]->direction, n_node->x, n_node->z);
		mesh::translate_mesh(n_mesh, add->nodes[i]->x, add->nodes[i]->y + 4, add->nodes[i]->z);
		map_1.push_back(n_mesh);
		}
		}
		break;
		case node::TYPE::ROOF_OBJECT:
		if (roof_object_list.size() > 0)
		{
		int pick = rand() % roof_object_list.size();
		mesh::Mesh_c * mesh_ = new mesh::Mesh_c(*roof_object_list[pick]);
		mesh_->x = add->nodes[i]->x;
		mesh_->y = add->nodes[i]->y;
		mesh_->z = add->nodes[i]->z;
		mesh_->azimuth = -add->nodes[i]->direction;
		mesh_->roll = 0;
		mesh_->pitch = 0;
		out_meshes.push_back(mesh_);
		if (settings::object_collide)
		{
		double lx = 0;
		double ly = 0;
		double lz = 0;
		double hx = 0;
		double hy = 0;
		double hz = 0;

		mesh::calculate_bounds(new mesh::Mesh_c(*roof_object_list[pick]), lx, ly, lz, hx, hy, hz);

		mesh_ = new mesh::Mesh_c(roof_object_collider);
		mesh::set_bounds(mesh_, lx, ly, lz, hx, hy, hz);

		mesh::rotate_mesh(mesh_, add->nodes[i]->direction, 0, 0);
		mesh::translate_mesh(mesh_, add->nodes[i]->x, add->nodes[i]->y, add->nodes[i]->z);
		add->mesh_->reconfigure();

		objects.push_back(mesh_);
		}
		if (settings::show_nodes)
		{
		mesh::Mesh_c * n_mesh = new mesh::Mesh_c(*ends[0]->mesh_);
		mesh::resize_mesh(n_mesh, 0.1, 0.1, 0.1);
		mesh::recolor_mesh(n_mesh, 60, 240, 100);
		mesh::rotate_mesh(n_mesh, add->nodes[i]->direction, n_node->x, n_node->z);
		mesh::translate_mesh(n_mesh, add->nodes[i]->x, add->nodes[i]->y + 4, add->nodes[i]->z);
		map_1.push_back(n_mesh);
		}
		}
		break;
		}
		}

		}
		//rinse, repeat.
		it++;
		}
		else
		{
		//delete add;
		tries++;
		if (tries > settings::max_tries)
		{
		//Adds a cap.
		pick = rand() % caps.size();
		add = new segment(*caps[pick]);
		add->mesh_ = new mesh::Mesh_c(*caps[pick]->mesh_);
		if (settings::color_test)
		{
		if (map.size() >= num_rooms)
		{
		mesh::recolor_mesh(add->mesh_, 0, 0, 255);
		}
		}
		add->x = c_node->x - add->nodes[0]->x;
		add->y = c_node->y - add->nodes[0]->y;
		add->z = c_node->z - add->nodes[0]->z;
		add->dir = angle_subtract(c_node->direction, ((int)add->nodes[0]->direction + 180) % 360);
		mesh::rotate_mesh(add->mesh_, add->dir, 0, 0);
		mesh::translate_mesh(add->mesh_, c_node->x - add->nodes[0]->x, c_node->y - add->nodes[0]->y, c_node->z - add->nodes[0]->z);
		calculate_bounds(add);
		map.push_back(add);

		tries = 0;
		it++;
		}
		}
		}
		*/
		if (settings::remove_hall_blockage)
		{
			std::vector<segment*> map_corrected;
			for (int i = 0; i < map.size(); i++)
			{
				if (map[i]->type == segment::CAP)
				{
					bool unique = true;
					for (int j = 0; j < map.size(); j++)
					{
						if (map[j]->type == segment::CAP)
						{
							if (i != j)
							{
								//	if (map[j]->x == map[i]->x && map[j]->y == map[i]->y && map[j]->z == map[i]->z)
								if (fabs(map[j]->x - map[i]->x) <= 0.001 && fabs(map[j]->y - map[i]->y) <= 0.001 && fabs(map[j]->z - map[i]->z) <= 0.001)
								{
									unique = false;
								}
							}
						}
					}
					if (unique)
						map_corrected.push_back(map[i]);
				}
				else
				{
					map_corrected.push_back(map[i]);
				}
			}
			map = map_corrected;
		}


		//assign depths to each node.
		get_depth(start_node);

		//get the size of the map
		int map_size = map.size();
		int num_doors = ceil(map_size*settings::door_fraction);
		int biggest_depth = start_node->depth;
		node * working_node = start_node;
		for (int i = 0; i < num_doors; i++)
		{
			//Have room doors, and section doors. room doors close off rooms (depth<=2). 
			int door_type = rand() % 2;
			int to_depth = 0;
			switch (door_type)
			{
			case 0:
				to_depth = 2;
				break;
			case 1:
				to_depth = rand() % (biggest_depth - settings::min_door_placement) + settings::min_door_placement;
				break;
			}

		
			working_node = start_node;
			std::cout << working_node->children.size() << "\n";
			//work down the tree to pick a node that is at the given depth. TODO: add more variation
			while (working_node->depth > to_depth)
			{
				if (working_node->children.size() == 0)
					break;
				std::cout << working_node << "\n";
				//get child candidates.
				std::vector<node*> cand_nodes;
				for (int j = 0; j < working_node->children.size(); j++)
				{
					if (working_node->children[j]->depth >= to_depth)
					{
						cand_nodes.push_back(working_node->children[j]);
					}
				}
				//randomly pick a child
				int pick = rand() % cand_nodes.size();
				working_node = cand_nodes[pick];
			}
	
			node * door_node = working_node;


			//add the door
			int r = rand() % 100 + 100;
			int g = rand() % 100 + 100;
			int b = rand() % 100 + 100;
			Key * key = new Key("src/pick_ups/key.ply");

			Door * door = new Door(working_node->x, working_node->y, working_node->z, working_node->x, working_node->y + 2.1, working_node->z, 1.0, key, "src/meshes/door.ply");
			//std::cout << key << "\n";
			door->d_node = reinterpret_cast<size_t>(working_node);
			door->key = key;

			mesh::rotate_mesh(door->mesh, working_node->direction, 0, 0);
			door->reconfigure();

			bool unique = true;
			for (int p = 0; p < objects_out.size(); p++)
			{
				if (objects_out[p]->type == Object::DOOR)
				{
					if (door->d_node == static_cast<Door*>(objects_out[p])->d_node)
					{
						unique = false;
					}
				}
			}

			if (unique)
			{
				mesh::recolor_mesh(key->mesh, r, g, b);
				mesh::recolor_mesh(door->mesh, r, g, b);

				out_meshes.push_back(key->mesh);
				out_meshes.push_back(door->mesh);

				objects_out.push_back(key);
				objects_out.push_back(door);
			}
			

			//place the key
			//go back
			int back_up = rand() % to_depth;
			for (int j = 0; j < back_up; j++)
			{
				if (working_node->parent != NULL)
				{
					working_node = working_node->parent;
				}
			}

			//traverse tree
			int step = rand() % (int)(1.6*to_depth);
			while (true)
			{
				if (working_node->children.size() == 0)
				{
					for (int j = 0; j < back_up; j++)
					{
						if (working_node->parent != NULL)
						{
							working_node = working_node->parent;
						}
					}
					step = rand() % (int)(1.6*to_depth);
				}
				if (working_node == door_node)
				{
					for (int j = 0; j < back_up; j++)
					{
						if (working_node->parent != NULL)
						{
							working_node = working_node->parent;
						}
					}
					step = rand() % (int)(1.6*to_depth);
				}

				if (step <= 40)
				{
					if (working_node->contents.size() > 0)
					{
						int count = 0;
						for (int j = 0; j < working_node->contents.size(); j++)
						{
							if (working_node->contents[j]->type == node::TYPE::SPAWN)
								count++;
						}
						if (count != 0)
							break;
					}
				}

				if (step == 0)
				{
					for (int j = 0; j < back_up; j++)
					{
						if (working_node->parent != NULL)
						{
							working_node = working_node->parent;
						}
					}
					step = rand() % (int)(1.6*to_depth);
				}

				//randomly pick a child
				if (working_node->children.size() == 0)
				{
					working_node = working_node->parent;
				}
				else
				{
					int pick = rand() % working_node->children.size();
					working_node = working_node->children[pick];
				}
				step--;
			}

			//place key
			int pick = 0;
			std::vector<node*> pick_nodes;
			for (int j = 0; j < working_node->contents.size(); j++)
			{
				if (working_node->contents[j]->type == node::TYPE::SPAWN)
					pick_nodes.push_back(working_node->contents[j]);
			}
			
			node * chosen_node = working_node->contents[rand() % working_node->contents.size()];//pick_nodes[rand() % pick_nodes.size()];

			key->mesh->x = chosen_node->x;
			key->mesh->y = chosen_node->y;
			key->mesh->z = chosen_node->z;
			key->mesh->azimuth = -chosen_node->direction;
			key->mesh->roll = 0;
			key->mesh->pitch = 0;
			key->on_ground = false;


		}




		//convert map to mesh and export
		for (int i = 0; i < map.size(); i++)
		{
			//so a mesh takes a collection of triangles and then makes a list of values.
			//So I need to just add all the triangles.
			for (int j = 0; j < map[i]->mesh_->faces.size(); j++)
			{
				if (settings::snap_verts_to_grid)
				{
					mesh::snap_face(map[i]->mesh_->faces[j]);
				}
				mesh->faces.push_back(map[i]->mesh_->faces[j]);
			}
			delete map[i]->mesh_;
		}

		

		//add all of the nodes to be visible
		if (settings::show_nodes)
		{
			for (int i = 0; i < map_1.size(); i++)
			{
				for (int j = 0; j < map_1[i]->faces.size(); j++)
				{
					mesh->faces.push_back(map_1[i]->faces[j]);
				}
			}
		}


		mesh::Mesh_c * mesh_rr = new mesh::Mesh_c(*mesh);

		mesh_rr->s_type == mesh::Mesh_c::TYPE::DYNAMIC;
		mesh_rr->azimuth = 0;
		mesh_rr->pitch = 0;
		mesh_rr->roll = 0;
		mesh_rr->x = 0;
		mesh_rr->y = 0;
		mesh_rr->z = 0;
		mesh_rr->commit_mesh();
		mesh_rr->reconfigure();
		out_meshes.push_back(mesh_rr);

		for (int i = 0; i < objects.size(); i++)
		{
			for (int j = 0; j < objects[i]->faces.size(); j++)
			{
				if (settings::snap_verts_to_grid)
				{
					mesh::snap_face(objects[i]->faces[j]);
				}
				mesh->faces.push_back(objects[i]->faces[j]);
			}
		}

		mesh->s_type == mesh::Mesh_c::TYPE::STATIC;
		mesh->azimuth = 0;
		mesh->pitch = 0;
		mesh->roll = 0;
		mesh->x = 0;
		mesh->y = 0;
		mesh->z = 0;
		mesh->commit_mesh();
		mesh->reconfigure();
		return true;
	}

	void color_mask_nodes(node * node_, sf::Color color)
	{
		node_->keyColor = color;
		node_->color_mask.push_back(color);
		for (int i = 0; i < node_->children.size(); i++)
		{
			color_mask_nodes(node_->children[i], color);
		}
	}

	node * make_map_adv_3(std::vector<segment> &segments, std::vector<mesh::Mesh_c *> object_list, std::vector<mesh::Mesh_c *> wall_object_list, std::vector<mesh::Mesh_c *> roof_object_list, mesh::Mesh_c *mesh, std::vector<mesh::Mesh_c *> &out_meshes, std::vector<seg::node *> &spawn_nodes, std::vector<Object*> &objects_out)
	{
		//this is the working array, the one that will get converted to a mesh
		std::vector<segment*> map;

		std::vector<mesh::Mesh_c*> map_1;

		std::vector<mesh::Mesh_c*> objects;

		//	mesh::Mesh_c object_collider;
		//	mesh::Mesh_c wall_object_collider;
		//	mesh::Mesh_c roof_object_collider;

		mesh::Mesh_c object_collider;
		reader::read_ply("src/better_segs_meshes/object_collider.ply", object_collider);

		mesh::Mesh_c wall_object_collider;
		reader::read_ply("src/better_segs_meshes/wall_object_collider.ply", wall_object_collider);

		mesh::Mesh_c roof_object_collider;
		reader::read_ply("src/better_segs_meshes/roof_object_collider.ply", roof_object_collider);

		//for organization, we go through the availible segments and sort them by TYPE
		//	ALSO, we get a bucket full of everything besides ENDS, 
		std::vector<segment*> strs;
		std::vector<segment*> rts;
		std::vector<segment*> lfts;
		std::vector<segment*> ts;
		std::vector<segment*> xs;

		std::vector<segment*> els;
		std::vector<segment*> ends;
		std::vector<segment*> caps;
		std::vector<segment*> non_ends;

		std::vector<segment*> halls;
		std::vector<segment*> rooms;


		


		//sort
		for (int i = 0; i < segments.size(); i++)
		{
			switch (segments[i].type)
			{
			case segment::TYPE::STRAIGHT:
				non_ends.push_back(&segments[i]);
				strs.push_back(&segments[i]);
				halls.push_back(&segments[i]);
				break;
			case segment::TYPE::RIGHT:
				non_ends.push_back(&segments[i]);
				rts.push_back(&segments[i]);
				halls.push_back(&segments[i]);
				break;
			case segment::TYPE::LEFT:
				non_ends.push_back(&segments[i]);
				lfts.push_back(&segments[i]);
				halls.push_back(&segments[i]);
				break;
			case segment::TYPE::T:
				non_ends.push_back(&segments[i]);
				ts.push_back(&segments[i]);
				halls.push_back(&segments[i]);
				break;
			case segment::TYPE::CROSS:
				non_ends.push_back(&segments[i]);
				xs.push_back(&segments[i]);
				halls.push_back(&segments[i]);
				break;
			case segment::TYPE::ELEVATOR:
				non_ends.push_back(&segments[i]);
				els.push_back(&segments[i]);
				break;
			case segment::TYPE::END:
				ends.push_back(&segments[i]);
				rooms.push_back(&segments[i]);
				break;
			case segment::TYPE::ROOM:
				rooms.push_back(&segments[i]);
				non_ends.push_back(&segments[i]);
				break;
			case segment::TYPE::CAP:
				caps.push_back(&segments[i]);
				break;
			}
		}

		//node buffer. Maybe a linked list...   i don't know...  I'll just use an iterator.
		std::vector<node*> node_list;
		int it = 0;
		//Start with a dead end. There may be start rooms, but this will work for now.
		segment * start = new segment(*ends[rand() % ends.size()]);	//pick a random end to start with. COPY the data..
		start->x = 0;		//move it to the origin
		start->y = 0;
		start->z = 0;
		start->dir = 0;
		mesh::rotate_mesh(start->mesh_, start->dir, 0, 0);
		mesh::translate_mesh(start->mesh_, start->x, start->y, start->z);
		calculate_bounds(start);
		


		node * start_node = new node(start->nodes[0]->type, start->nodes[0]->x, start->nodes[0]->y, start->nodes[0]->z, start->nodes[0]->direction);
		map.push_back(start);
		node_list.push_back(start_node);

		start_node->cur_seg = start;

		node * node_map = start_node;

		for (int i = 1; i < start->nodes.size(); i++)
		{
			//add it to node list
			switch (start->nodes[i]->type)
			{
			case node::TYPE::JOINT:
				node_list.push_back(start->nodes[i]);
				node_map->add_child(start->nodes[i]);
				break;
			case node::TYPE::SPAWN:
				spawn_nodes.push_back(start->nodes[i]);
				node_map->add_content(start->nodes[i]);
				break;
			case node::TYPE::OBJECT:
				node_map->add_content(start->nodes[i]);
				if (object_list.size() > 0)
				{
					int pick = rand() % object_list.size();
					mesh::Mesh_c * mesh_ = new mesh::Mesh_c(*object_list[pick]);
					mesh_->x = start->nodes[i]->x;
					mesh_->y = start->nodes[i]->y;
					mesh_->z = start->nodes[i]->z;
					mesh_->azimuth = -start->nodes[i]->direction;	//negative because of how the renderer works.   some linear algebra mistake that would be too bothersome to fix now. Maybe fix during optimization.
					mesh_->roll = 0;
					mesh_->pitch = 0;
					out_meshes.push_back(mesh_);
					if (settings::object_collide)
					{
						double lx = 0;
						double ly = 0;
						double lz = 0;
						double hx = 0;
						double hy = 0;
						double hz = 0;

						mesh::calculate_bounds(new mesh::Mesh_c(*object_list[pick]), lx, ly, lz, hx, hy, hz);

						mesh_ = new mesh::Mesh_c(object_collider);
						mesh::set_bounds(mesh_, lx, ly, lz, hx, hy, hz);

						mesh::rotate_mesh(mesh_, start->nodes[i]->direction, 0, 0);
						mesh::translate_mesh(mesh_, start->nodes[i]->x, start->nodes[i]->y, start->nodes[i]->z);
						start->mesh_->reconfigure();

						objects.push_back(mesh_);
					}
					if (settings::show_nodes)
					{
						mesh::Mesh_c * n_mesh = new mesh::Mesh_c(*ends[0]->mesh_);
						mesh::resize_mesh(n_mesh, 0.1, 0.1, 0.1);
						mesh::recolor_mesh(n_mesh, 60, 240, 100);
						mesh::rotate_mesh(n_mesh, start->nodes[i]->direction, 0, 0);
						mesh::translate_mesh(n_mesh, start->nodes[i]->x, start->nodes[i]->y + 4, start->nodes[i]->z);
						map_1.push_back(n_mesh);
					}
				}
				break;
			case node::TYPE::WALL_OBJECT:
				node_map->add_content(start->nodes[i]);
				if (wall_object_list.size() > 0)
				{
					int pick = rand() % wall_object_list.size();
					mesh::Mesh_c * mesh_ = new mesh::Mesh_c(*wall_object_list[pick]);
					mesh_->x = start->nodes[i]->x;
					mesh_->y = start->nodes[i]->y;
					mesh_->z = start->nodes[i]->z;
					mesh_->azimuth = 0;//-add->nodes[i]->direction
					mesh_->roll = 0;
					mesh_->pitch = 0;
					mesh::rotate_mesh(mesh_, start->nodes[i]->direction, 0, 0);
					out_meshes.push_back(mesh_);
					if (settings::object_collide)
					{
						double lx = 0;
						double ly = 0;
						double lz = 0;
						double hx = 0;
						double hy = 0;
						double hz = 0;

						mesh::calculate_bounds(new mesh::Mesh_c(*wall_object_list[pick]), lx, ly, lz, hx, hy, hz);

						mesh_ = new mesh::Mesh_c(wall_object_collider);
						mesh::set_bounds(mesh_, lx, ly, lz, hx, hy, hz);

						mesh::rotate_mesh(mesh_, start->nodes[i]->direction, 0, 0);
						mesh::translate_mesh(mesh_, start->nodes[i]->x, start->nodes[i]->y, start->nodes[i]->z);
						start->mesh_->reconfigure();

						objects.push_back(mesh_);
					}
					if (settings::show_nodes)
					{
						mesh::Mesh_c * n_mesh = new mesh::Mesh_c(*ends[0]->mesh_);
						mesh::resize_mesh(n_mesh, 0.1, 0.1, 0.1);
						mesh::recolor_mesh(n_mesh, 60, 240, 100);
						mesh::rotate_mesh(n_mesh, start->nodes[i]->direction, 0, 0);
						mesh::translate_mesh(n_mesh, start->nodes[i]->x, start->nodes[i]->y + 4, start->nodes[i]->z);
						map_1.push_back(n_mesh);
					}
				}
				break;
			case node::TYPE::ROOF_OBJECT:
				node_map->add_content(start->nodes[i]);
				if (roof_object_list.size() > 0)
				{
					int pick = rand() % roof_object_list.size();
					mesh::Mesh_c * mesh_ = new mesh::Mesh_c(*roof_object_list[pick]);
					mesh_->x = start->nodes[i]->x;
					mesh_->y = start->nodes[i]->y;
					mesh_->z = start->nodes[i]->z;
					mesh_->azimuth = -start->nodes[i]->direction;
					mesh_->roll = 0;
					mesh_->pitch = 0;
					out_meshes.push_back(mesh_);
					if (settings::object_collide)
					{
						double lx = 0;
						double ly = 0;
						double lz = 0;
						double hx = 0;
						double hy = 0;
						double hz = 0;

						mesh::calculate_bounds(new mesh::Mesh_c(*roof_object_list[pick]), lx, ly, lz, hx, hy, hz);

						mesh_ = new mesh::Mesh_c(roof_object_collider);
						mesh::set_bounds(mesh_, lx, ly, lz, hx, hy, hz);

						mesh::rotate_mesh(mesh_, start->nodes[i]->direction, 0, 0);
						mesh::translate_mesh(mesh_, start->nodes[i]->x, start->nodes[i]->y, start->nodes[i]->z);
						start->mesh_->reconfigure();

						objects.push_back(mesh_);
					}
					if (settings::show_nodes)
					{
						mesh::Mesh_c * n_mesh = new mesh::Mesh_c(*ends[0]->mesh_);
						mesh::resize_mesh(n_mesh, 0.1, 0.1, 0.1);
						mesh::recolor_mesh(n_mesh, 60, 240, 100);
						mesh::rotate_mesh(n_mesh, start->nodes[i]->direction, 0, 0);
						mesh::translate_mesh(n_mesh, start->nodes[i]->x, start->nodes[i]->y + 4, start->nodes[i]->z);
						map_1.push_back(n_mesh);
					}
				}
				break;
			}

		}

		//generate map
		int tries = 0;
		int num_rooms = 0;
		bool only_one = true;
		while (it < node_list.size()) //keeps working on the nodes, until we are at the last one.
		{

			//current node
			node * c_node = node_list[it];

			if (settings::show_nodes)
			{
				mesh::Mesh_c * n_mesh = new mesh::Mesh_c(*ends[0]->mesh_);
				n_mesh->x = c_node->x;
				n_mesh->z = c_node->z;
				n_mesh->y = 5;
				mesh::resize_mesh(n_mesh, 0.1, 0.1, 0.1);
				mesh::recolor_mesh(n_mesh, 0, 0, 255);
				mesh::rotate_mesh(n_mesh, c_node->direction, 0, 0);
				mesh::translate_mesh(n_mesh, c_node->x, c_node->y + 4, c_node->z);
				map_1.push_back(n_mesh);
			}
			//pick a new segment to add.
			segment * add;
			int pick;
			if (num_rooms >= settings::max_rooms)
			{
				pick = rand() % ends.size();
				add = new segment(*ends[pick]);
				add->mesh_ = new mesh::Mesh_c(*ends[pick]->mesh_);
				std::vector<node*> new_nodes;
				for (int i = 0; i < add->nodes.size(); i++)
				{
					new_nodes.push_back(new node(add->nodes[i]->type, add->nodes[i]->x, add->nodes[i]->y, add->nodes[i]->z, add->nodes[i]->direction));
				}
				add->nodes = new_nodes;
			}
			else
			{
				if (tries >= settings::max_tries / 2)	//if it has tried for half of the time to add a room, try adding a hall.
				{
					pick = rand() % halls.size();
					add = new segment(*halls[pick]);
					add->mesh_ = new mesh::Mesh_c(*halls[pick]->mesh_);
					std::vector<node*> new_nodes;
					for (int i = 0; i < add->nodes.size(); i++)
					{
						new_nodes.push_back(new node(add->nodes[i]->type, add->nodes[i]->x, add->nodes[i]->y, add->nodes[i]->z, add->nodes[i]->direction));
					}
					add->nodes = new_nodes;
				}
				else
				{
					/*
					double choice = random();
					std::cout << choice << "\n";
					if (choice <= settings::hall_fraction)
					{
					pick = rand() % halls.size();
					add = new segment(*halls[pick]);
					add->mesh_ = new mesh::Mesh_c(*halls[pick]->mesh_);
					}
					else
					{
					pick = rand() % rooms.size();
					add = new segment(*rooms[pick]);
					add->mesh_ = new mesh::Mesh_c(*rooms[pick]->mesh_);
					}
					//*/
					pick = rand() % non_ends.size();
					add = new segment(*non_ends[pick]);
					add->mesh_ = new mesh::Mesh_c(*non_ends[pick]->mesh_);
					std::vector<node*> new_nodes;
					for (int i = 0; i < add->nodes.size(); i++)
					{
						new_nodes.push_back(new node(add->nodes[i]->type, add->nodes[i]->x, add->nodes[i]->y, add->nodes[i]->z, add->nodes[i]->direction));
					}
					add->nodes = new_nodes;
				}
			}

			//pick a node to attach to the current node
			int chosen_node = rand() % add->nodes.size();
			node * n_node = add->nodes[chosen_node]; //just the zeroth for now.  nothing fancy. Will need to change stuff if it was different.
			add->chosen_node = chosen_node;
			if (n_node->type != node::JOINT)
			{
				chosen_node = 0;
				node * n_node = add->nodes[chosen_node];
				add->chosen_node = chosen_node;
			}

			//attaches the segments together
			//	We find <tx,ty,tz> by <current_x - new_x, ... , ... >
			double tx = c_node->x - n_node->x;
			double ty = c_node->y - n_node->y;
			double tz = c_node->z - n_node->z;

			if (fabs(tx) < 0.001) tx = 0;
			if (fabs(ty) < 0.001) ty = 0;
			if (fabs(tz) < 0.001) tz = 0;
			//move the segment to position
			add->x += tx;
			add->y += ty;
			add->z += tz;
			//still need to do something about direction...   
			//	All nodes start with direction in Reference space. We need to transform to Real space, we need to apply turns and stuff.
			//	So all nodes will start at where they are relative to the mesh. We need to turn the mesh AND the nodes. and update node diretion.
			//	Maybe use angles for node direction, that way we can use math, not switch statements.
			//	We'll just make sure they are all 90° when making it.
			//SO we take 0 as pointing along reference. (Y axis (in blender. Z in openGL),  cannot remember +/-)
			//	then rotate around.
			//std::cout << tx << "  :  " << ty << "  :  " << tz << "\n";
			//Nodes need to rotate so they point in on each other. SO 180°. All nodes point out from the mesh.
			double dif_angle = angle_subtract(c_node->direction, ((int)n_node->direction + 180) % 360);

			//this is the angle to rotate.
			add->dir = dif_angle;
			//add it to the map

			//move the mesh into the proper position.
			mesh::rotate_mesh(add->mesh_, add->dir, n_node->x, n_node->z);
			mesh::translate_mesh(add->mesh_, add->x, add->y, add->z);
			add->mesh_->reconfigure();
			calculate_bounds(add);

			if (settings::color_test)
			{
				if (map.size() >= num_rooms)
				{
					mesh::recolor_mesh(add->mesh_, 0, 0, 255);
				}
			}
			//we test to see if the mesh fits in the grid
			if (test_new_segment(map, add))
			{
				tries = 0;
				if (add->type == segment::TYPE::ROOM)
					num_rooms++;
				c_node->cur_seg = add;

				map.push_back(add);
				//then we need to update all the nodes with this change in angle. and position.
				//rotate all the other nodes around this node by the angle.
				//I guess we can just and the rest of the nodes now.
				for (int i = 0; i < add->nodes.size(); i++)
				{
					if (i != chosen_node)
					{
						//So we update dir.
						add->nodes[i]->direction = (int)(add->nodes[i]->direction + dif_angle) % 360; //rotate the direction by the angle and fix to [0,360)
																										//then we need to change the position...
																										//	We first rotate with the angle, then translate.
						double s = sin(dif_angle*TO_RAD);
						double c = cos(dif_angle*TO_RAD);
						if (fabs(s) < 0.0001) s = 0;
						if (fabs(c) < 0.0001) c = 0;
						if (s < -0.99) s = -1;
						if (s > 0.99) s = 1;
						if (c < -0.99) c = -1;
						if (c > 0.99) c = 1;

						//translate point back to origin
						double n_x = add->nodes[i]->x - n_node->x;
						double n_z = add->nodes[i]->z - n_node->z;

						//rotate point
						double xnew = n_x * c - n_z * s;
						double znew = n_x * s + n_z * c;

						//translate point back
						add->nodes[i]->x = xnew + n_node->x;
						add->nodes[i]->z = znew + n_node->z;

						//translate point with Vector from before
						add->nodes[i]->x += tx;
						add->nodes[i]->y += ty;
						add->nodes[i]->z += tz;
						//add it to node list
						switch (add->nodes[i]->type)
						{
						case node::TYPE::JOINT:
							node_list.push_back(add->nodes[i]);
							c_node->add_child(add->nodes[i]);
							break;
						case node::TYPE::SPAWN:
							spawn_nodes.push_back(add->nodes[i]);
							c_node->add_content(add->nodes[i]);
							/*if (random() > 0.0)
							{
							holdable * pick_up = new holdable("src/pick_ups/key.ply");
							pick_up->mesh->x = add->nodes[i]->x;
							pick_up->mesh->y = add->nodes[i]->y;
							pick_up->mesh->z = add->nodes[i]->z;
							pick_up->mesh->azimuth = -add->nodes[i]->direction;	//negative because of how the renderer works.   some linear algebra mistake that would be too bothersome to fix now. Maybe fix during optimization.
							pick_up->mesh->roll = 0;
							pick_up->mesh->pitch = 0;
							pick_up->on_ground = false;
							objects_out.push_back(pick_up);
							out_meshes.push_back(pick_up->mesh);
							}*/
							break;
						case node::TYPE::OBJECT:
							c_node->add_content(add->nodes[i]);
							if (object_list.size() > 0)
							{
								int pick = rand() % object_list.size();
								mesh::Mesh_c * mesh_ = new mesh::Mesh_c(*object_list[pick]);
								mesh_->x = add->nodes[i]->x;
								mesh_->y = add->nodes[i]->y;
								mesh_->z = add->nodes[i]->z;
								mesh_->azimuth = -add->nodes[i]->direction;	//negative because of how the renderer works.   some linear algebra mistake that would be too bothersome to fix now. Maybe fix during optimization.
								mesh_->roll = 0;
								mesh_->pitch = 0;
								out_meshes.push_back(mesh_);
								if (settings::object_collide)
								{
									double lx = 0;
									double ly = 0;
									double lz = 0;
									double hx = 0;
									double hy = 0;
									double hz = 0;

									mesh::calculate_bounds(new mesh::Mesh_c(*object_list[pick]), lx, ly, lz, hx, hy, hz);

									mesh_ = new mesh::Mesh_c(object_collider);
									mesh::set_bounds(mesh_, lx, ly, lz, hx, hy, hz);

									mesh::rotate_mesh(mesh_, add->nodes[i]->direction, 0, 0);
									mesh::translate_mesh(mesh_, add->nodes[i]->x, add->nodes[i]->y, add->nodes[i]->z);
									add->mesh_->reconfigure();

									objects.push_back(mesh_);
								}
								if (settings::show_nodes)
								{
									mesh::Mesh_c * n_mesh = new mesh::Mesh_c(*ends[0]->mesh_);
									mesh::resize_mesh(n_mesh, 0.1, 0.1, 0.1);
									mesh::recolor_mesh(n_mesh, 60, 240, 100);
									mesh::rotate_mesh(n_mesh, add->nodes[i]->direction, n_node->x, n_node->z);
									mesh::translate_mesh(n_mesh, add->nodes[i]->x, add->nodes[i]->y + 4, add->nodes[i]->z);
									map_1.push_back(n_mesh);
								}
							}
							break;
						case node::TYPE::WALL_OBJECT:
							c_node->add_content(add->nodes[i]);
							if (wall_object_list.size() > 0)
							{
								int pick = rand() % wall_object_list.size();
								mesh::Mesh_c * mesh_ = new mesh::Mesh_c(*wall_object_list[pick]);
								mesh_->x = add->nodes[i]->x;
								mesh_->y = add->nodes[i]->y;
								mesh_->z = add->nodes[i]->z;
								mesh_->azimuth = 0;//-add->nodes[i]->direction
								mesh_->roll = 0;
								mesh_->pitch = 0;
								mesh::rotate_mesh(mesh_, add->nodes[i]->direction, n_node->x, n_node->z);
								out_meshes.push_back(mesh_);
								if (settings::object_collide)
								{
									double lx = 0;
									double ly = 0;
									double lz = 0;
									double hx = 0;
									double hy = 0;
									double hz = 0;

									mesh::calculate_bounds(new mesh::Mesh_c(*wall_object_list[pick]), lx, ly, lz, hx, hy, hz);

									mesh_ = new mesh::Mesh_c(wall_object_collider);
									mesh::set_bounds(mesh_, lx, ly, lz, hx, hy, hz);

									mesh::rotate_mesh(mesh_, add->nodes[i]->direction, 0, 0);
									mesh::translate_mesh(mesh_, add->nodes[i]->x, add->nodes[i]->y, add->nodes[i]->z);
									add->mesh_->reconfigure();

									objects.push_back(mesh_);
								}
								if (settings::show_nodes)
								{
									mesh::Mesh_c * n_mesh = new mesh::Mesh_c(*ends[0]->mesh_);
									mesh::resize_mesh(n_mesh, 0.1, 0.1, 0.1);
									mesh::recolor_mesh(n_mesh, 60, 240, 100);
									mesh::rotate_mesh(n_mesh, add->nodes[i]->direction, n_node->x, n_node->z);
									mesh::translate_mesh(n_mesh, add->nodes[i]->x, add->nodes[i]->y + 4, add->nodes[i]->z);
									map_1.push_back(n_mesh);
								}
							}
							break;
						case node::TYPE::ROOF_OBJECT:
							c_node->add_content(add->nodes[i]);
							if (roof_object_list.size() > 0)
							{
								int pick = rand() % roof_object_list.size();
								mesh::Mesh_c * mesh_ = new mesh::Mesh_c(*roof_object_list[pick]);
								mesh_->x = add->nodes[i]->x;
								mesh_->y = add->nodes[i]->y;
								mesh_->z = add->nodes[i]->z;
								mesh_->azimuth = -add->nodes[i]->direction;
								mesh_->roll = 0;
								mesh_->pitch = 0;
								out_meshes.push_back(mesh_);
								if (settings::object_collide)
								{
									double lx = 0;
									double ly = 0;
									double lz = 0;
									double hx = 0;
									double hy = 0;
									double hz = 0;

									mesh::calculate_bounds(new mesh::Mesh_c(*roof_object_list[pick]), lx, ly, lz, hx, hy, hz);

									mesh_ = new mesh::Mesh_c(roof_object_collider);
									mesh::set_bounds(mesh_, lx, ly, lz, hx, hy, hz);

									mesh::rotate_mesh(mesh_, add->nodes[i]->direction, 0, 0);
									mesh::translate_mesh(mesh_, add->nodes[i]->x, add->nodes[i]->y, add->nodes[i]->z);
									add->mesh_->reconfigure();

									objects.push_back(mesh_);
								}
								if (settings::show_nodes)
								{
									mesh::Mesh_c * n_mesh = new mesh::Mesh_c(*ends[0]->mesh_);
									mesh::resize_mesh(n_mesh, 0.1, 0.1, 0.1);
									mesh::recolor_mesh(n_mesh, 60, 240, 100);
									mesh::rotate_mesh(n_mesh, add->nodes[i]->direction, n_node->x, n_node->z);
									mesh::translate_mesh(n_mesh, add->nodes[i]->x, add->nodes[i]->y + 4, add->nodes[i]->z);
									map_1.push_back(n_mesh);
								}
							}
							break;
						}
					}

				}
				//rinse, repeat. 
				it++;
			}
			else
			{
				//delete add;
				tries++;
				if (tries > settings::max_tries)
				{
					//Adds a cap.
					pick = rand() % caps.size();
					add = new segment(*caps[pick]);
					add->mesh_ = new mesh::Mesh_c(*caps[pick]->mesh_);
					if (settings::color_test)
					{
						if (map.size() >= num_rooms)
						{
							mesh::recolor_mesh(add->mesh_, 0, 0, 255);
						}
					}
					add->x = c_node->x - add->nodes[0]->x;
					add->y = c_node->y - add->nodes[0]->y;
					add->z = c_node->z - add->nodes[0]->z;
					add->dir = angle_subtract(c_node->direction, ((int)add->nodes[0]->direction + 180) % 360);
					mesh::rotate_mesh(add->mesh_, add->dir, 0, 0);
					mesh::translate_mesh(add->mesh_, c_node->x - add->nodes[0]->x, c_node->y - add->nodes[0]->y, c_node->z - add->nodes[0]->z);
					calculate_bounds(add);
					map.push_back(add);

					tries = 0;
					it++;
				}
			}
			
		}



		//ADD DOORS AND WORK WITH NODES
		//Now with map generated, we move to adding doors and fun shit.
		//We shouldn't add objects until this stage. Go through a few times. Add
		get_depth(start_node);
		int num_doors = settings::max_rooms / 2;
		int biggest_depth = start_node->depth;
	//	std::cout << biggest_depth << "\n";
		node * working_node = start_node;

		int doors_added = 0;

		//NEW::
		//add doors.
		//THEN add keys.
		//Move keys down, and keep them out of color_masked areas. But once it's placed, Add red color_mask (+ red inherited color mask) to the color you placed in. 
		//		That way, if red is placed in blue, Blue would be color_masked to blue AND red. So it makes it guarenteed solvable.


		int door_redundecy_count = 0;
		for (int i = 0; i < num_doors; i++)
		{
			working_node = start_node;
			int depth_to = rand() % (settings::max_door_placement - settings::min_door_placement) + settings::min_door_placement;
	//		std::cout << depth_to << "\n";
			//Go down and place doors.
			while (true)
			{
				if (working_node->depth > depth_to)
				{
					std::vector<node*> deep_enough;
					for (int j = 0; j < working_node->children.size(); j++)
					{
						if (working_node->children[j]->depth >= depth_to)
							deep_enough.push_back(working_node->children[j]);
					}
					int chosen_child = rand() % deep_enough.size();
					working_node = deep_enough[chosen_child];
				}
				else
				{
					break;
				}
			}
			//now that we are deep enough, add the door.
			sf::Color door_color = HSV_RGB(((double)i / num_doors)*360.0, 0.8, 0.9);//HSV_RGB(random() * 360.0, 0.8, 0.9);
			
			if (working_node->door != NULL)
			{
				//std::cout << "hey!!!!\n";
				i--;
				door_redundecy_count++;
				if (door_redundecy_count > 10)
					break;
				//for safety. So we don't go into an infinite loop;
				continue;
			}			

			Key * key = new Key("src/pick_ups/key.ply");

			Door * door = new Door(working_node->x, working_node->y, working_node->z, working_node->x, working_node->y + 2.1, working_node->z, 1.0, key, "src/meshes/door.ply");
			//std::cout << key << "\n";
			working_node->door = door;
			door->d_node = reinterpret_cast<size_t>(working_node);
			door->key = key;

			mesh::rotate_mesh(door->mesh, working_node->direction, 0, 0);
			door->reconfigure();

			mesh::recolor_mesh(key->mesh, door_color.r, door_color.g, door_color.b);
			mesh::recolor_mesh(door->mesh, door_color.r, door_color.g, door_color.b);

			out_meshes.push_back(key->mesh);
			out_meshes.push_back(door->mesh);

			objects_out.push_back(key);
			objects_out.push_back(door);

			color_mask_nodes(working_node, door_color);
			
			//Move up. Between one and MAX nodes away. then we'll move back down.
			int range = biggest_depth - settings::max_door_placement - settings::key_min_buffer;
			if (range <= 0) range = 1;

			int depth_up = rand() % (range) + settings::max_door_placement + settings::key_min_buffer;
			depth_to = rand() % ((int) fmin(depth_up, settings::max_key_placement) - settings::min_key_placement) + settings::min_key_placement;

			//Move back up
			while (true)
			{
				if (working_node->parent && working_node->depth < depth_up)
				{
					working_node = working_node->parent;
				}
				else
				{
					break;
				}
			}

			if(settings::reset_key_placement)
				working_node = start_node;


			//Move back down and place the key
			while (true)
			{
				if (working_node->depth > depth_to)
				{
					std::vector<node*> non_blocked_nodes;
					for (int j = 0; j < working_node->children.size(); j++)
					{
						bool non_blocked = true;
						for (int k = 0; k < working_node->children[j]->color_mask.size(); k++)
						{
							if (working_node->children[j]->color_mask[k] == door_color)
								non_blocked = false;
						}
						if (non_blocked)
							non_blocked_nodes.push_back(working_node->children[j]);
					}
					
					//TODO: have it redo the loop.
					if (non_blocked_nodes.size() == 0)
						break;
					
					int chosen_child = rand() % non_blocked_nodes.size();
					working_node = non_blocked_nodes[chosen_child];
				}
				else
				{
					break;
				}
			}

			
			key->mesh->x = working_node->x;
			key->mesh->y = working_node->y + 1;
			key->mesh->z = working_node->z;
			key->mesh->azimuth = -working_node->direction;
			key->mesh->roll = 0;
			key->mesh->pitch = 0;
			key->on_ground = false;
			node * key_node = new node(*working_node);
			key_node->z += 1;
			key_node->x += 1;
			key_node->keyColor = door_color;
			working_node->key_nodes.push_back(key_node);			
		}

		if (settings::remove_hall_blockage)
		{
			std::vector<segment*> map_corrected;
			for (int i = 0; i < map.size(); i++)
			{
				if (map[i]->type == segment::CAP)
				{
					bool unique = true;
					for (int j = 0; j < map.size(); j++)
					{
						if (map[j]->type == segment::CAP)
						{
							if (i != j)
							{
								//	if (map[j]->x == map[i]->x && map[j]->y == map[i]->y && map[j]->z == map[i]->z)
								if (fabs(map[j]->x - map[i]->x) <= 0.001 && fabs(map[j]->y - map[i]->y) <= 0.001 && fabs(map[j]->z - map[i]->z) <= 0.001)
								{
									unique = false;
								}
							}
						}
					}
					if (unique)
						map_corrected.push_back(map[i]);
				}
				else
				{
					map_corrected.push_back(map[i]);
				}
			}
			map = map_corrected;
		}

		//convert map to mesh and export
		for (int i = 0; i < map.size(); i++)
		{
			//so a mesh takes a collection of triangles and then makes a list of values.
			//So I need to just add all the triangles.
			for (int j = 0; j < map[i]->mesh_->faces.size(); j++)
			{
				if (settings::snap_verts_to_grid)
				{
					mesh::snap_face(map[i]->mesh_->faces[j]);
				}
				mesh->faces.push_back(map[i]->mesh_->faces[j]);
			}
			delete map[i]->mesh_;
		}

		//add all of the nodes to be visible
		if (settings::show_nodes)
		{
			for (int i = 0; i < map_1.size(); i++)
			{
				for (int j = 0; j < map_1[i]->faces.size(); j++)
				{
					mesh->faces.push_back(map_1[i]->faces[j]);
				}
			}
		}


		mesh::Mesh_c * mesh_rr = new mesh::Mesh_c(*mesh);

		mesh_rr->s_type == mesh::Mesh_c::TYPE::DYNAMIC;
		mesh_rr->azimuth = 0;
		mesh_rr->pitch = 0;
		mesh_rr->roll = 0;
		mesh_rr->x = 0;
		mesh_rr->y = 0;
		mesh_rr->z = 0;
		mesh_rr->commit_mesh();
		mesh_rr->reconfigure();
		out_meshes.push_back(mesh_rr);

		for (int i = 0; i < objects.size(); i++)
		{
			for (int j = 0; j < objects[i]->faces.size(); j++)
			{
				if (settings::snap_verts_to_grid)
				{
					mesh::snap_face(objects[i]->faces[j]);
				}
				mesh->faces.push_back(objects[i]->faces[j]);
			}
		}

		mesh->s_type == mesh::Mesh_c::TYPE::STATIC;
		mesh->azimuth = 0;
		mesh->pitch = 0;
		mesh->roll = 0;
		mesh->x = 0;
		mesh->y = 0;
		mesh->z = 0;
		mesh->commit_mesh();
		mesh->reconfigure();
		return node_map;
	}

	double h_color(double y)
	{
		return  fmod((y * 3.0) * 255, 255.5);
	}

	void drawNode(sf::RenderWindow * window, node * node_, double scale, bool colorMode, bool D3, iso::mat3 projection)
	{
		//draw circle for this node

		//Get it's location given the projections;
		iso::pointdef * thisPt = new iso::pointdef(node_->x, node_->z, node_->y);

		iso::applyMatrix(projection, thisPt);

		sf::CircleShape dot(1.0*scale, 16);
		
		//std::cout << node_->cur_seg << "\n";
		
		
		dot.setOrigin(1.0*scale, 1.0*scale);
		if (D3)
			dot.setPosition(thisPt->projX, thisPt->projY);
		else
			dot.setPosition(node_->x*scale, node_->z*scale);

		double y_color = h_color(node_->y);
		if (colorMode)
			dot.setFillColor(HSV_RGB(fmod(node_->y*25.0 + 720.0, 360.0), 0.8, 0.9));
		else
		{
			dot.setFillColor(node_->keyColor);
		}

		//Draw Aux nodes
		if (node_->color_mask.size() > 1)
		{
			for (int i = node_->color_mask.size() - 1; i >= 1; i--)
				//	for (int i = 0; i < node_->color_mask.size(); i--)
			{
				sf::CircleShape dotA(1.0*scale, 16);
				dotA.setOrigin(1.0*scale, 1.0*scale);

				iso::pointdef * otherPt = new iso::pointdef(node_->x + 0.2*i, node_->z + 0.2*i, node_->y);
				iso::applyMatrix(projection, otherPt);

				if (D3)
					dotA.setPosition(thisPt->projX + 0.2*i, thisPt->projY + 0.2*i);
				//dotA.setPosition(node_->x*scale + scale*0.2*i, node_->z*scale + scale*0.2*i);
				else
					dotA.setPosition(node_->x*scale + scale*0.2*i, node_->z*scale + scale*0.2*i);
				//	dotA.setPosition(node_->x*scale + scale*0.2*i, node_->z*scale + scale*0.2*i);
				dotA.setFillColor(node_->color_mask[i - 1]);
				window->draw(dotA);
				delete otherPt;
			}
		}

		window->draw(dot);

		//Children Nodes
		for (int i = 0; i < node_->children.size(); i++)
		{
			//draw lines together.
			sf::VertexArray lines(sf::LinesStrip, 2);


			if (D3)
				lines[0].position = sf::Vector2f(thisPt->projX, thisPt->projY);
			else
				lines[0].position = sf::Vector2f(node_->x*scale, node_->z*scale);

			if (colorMode)
				lines[0].color = HSV_RGB(fmod(node_->y*25.0 + 720.0, 360.0), 0.8, 0.9);
			else
				lines[0].color = node_->keyColor;


			iso::pointdef * otherPt = new iso::pointdef(node_->children[i]->x, node_->children[i]->z, node_->children[i]->y);
			iso::applyMatrix(projection, otherPt);
			//lines[1].position = sf::Vector2f(node_->children[i]->x*scale, node_->children[i]->z*scale);

			if (D3)
				lines[1].position = sf::Vector2f(otherPt->projX, otherPt->projY);
			else
				lines[1].position = sf::Vector2f(node_->children[i]->x*scale, node_->children[i]->z*scale);


			if (colorMode)
				lines[1].color = HSV_RGB(fmod(node_->children[i]->y*25.0 + 720.0, 360.0), 0.8, 0.9);
			else
				lines[1].color = node_->children[i]->keyColor;
			window->draw(lines);
			//make those draw.
			drawNode(window, node_->children[i], scale, colorMode, D3, projection);
			delete otherPt;
		}

		//Contents
		if (settings::map_show_content)
		{
			for (int i = 0; i < node_->contents.size(); i++)
			{
				sf::CircleShape dot(0.5*scale, 6);
				dot.setOrigin(0.5*scale, 0.5*scale);
				dot.setFillColor(sf::Color(20, 30, 160, 255));


				iso::pointdef * otherPt = new iso::pointdef(node_->contents[i]->x, node_->contents[i]->z, node_->contents[i]->y);
				iso::applyMatrix(projection, otherPt);
				//lines[1].position = sf::Vector2f(node_->children[i]->x*scale, node_->children[i]->z*scale);

				if (D3)
					dot.setPosition(otherPt->projX, otherPt->projY);
				else
					dot.setPosition(node_->contents[i]->x*scale, node_->contents[i]->z*scale);

				window->draw(dot);

				//draw lines together.
				sf::VertexArray lines(sf::LinesStrip, 2);

				if (D3)
					lines[0].position = sf::Vector2f(thisPt->projX, thisPt->projY);
				else
					lines[0].position = sf::Vector2f(node_->x*scale, node_->z*scale);


				if (colorMode)
					lines[0].color = HSV_RGB(fmod(node_->y*25.0 + 720.0, 360.0), 0.8, 0.9);
				else
					lines[0].color = node_->keyColor;


				//lines[1].position = sf::Vector2f(node_->contents[i]->x*scale, node_->contents[i]->z*scale);
				if (D3)
					lines[1].position = sf::Vector2f(otherPt->projX, otherPt->projY);
				else
					lines[1].position = sf::Vector2f(node_->contents[i]->x*scale, node_->contents[i]->z*scale);
				lines[1].color = sf::Color(20, 30, 160, 255);
				window->draw(lines);
				delete otherPt;
			}
		}
		// dot.setPointCount(4);
		//dot.setFillColor(node_->contents[i]->keyColor);

		//keys
		for (int i = node_->key_nodes.size()-1; i >= 0; i--)
		{
			sf::CircleShape dot(0.5*scale, 4);
			dot.setOrigin(0.5*scale, 0.5*scale);
			dot.setFillColor(node_->key_nodes[i]->keyColor);
			
			
			iso::pointdef * otherPt = new iso::pointdef(node_->key_nodes[i]->x, node_->key_nodes[i]->z, node_->key_nodes[i]->y);
			iso::applyMatrix(projection, otherPt);
			
			//dot.setPosition(node_->key_nodes[i]->x*scale + i*0.2*scale  , node_->key_nodes[i]->z*scale + i*0.2*scale);
			if (D3)
				dot.setPosition(otherPt->projX + i*0.2*scale, otherPt->projY + i*0.2*scale);
			else
				dot.setPosition(node_->key_nodes[i]->x*scale + i*0.2*scale, node_->key_nodes[i]->z*scale + i*0.2*scale);


			window->draw(dot);

			//draw lines together.
			sf::VertexArray lines(sf::LinesStrip, 2);
			
			if (D3)
				lines[0].position = sf::Vector2f(thisPt->projX, thisPt->projY);
			else
				lines[0].position = sf::Vector2f(node_->x*scale, node_->z*scale);


			if (colorMode)
				lines[0].color = HSV_RGB(fmod(node_->y*25.0 + 720.0, 360.0), 0.8, 0.9);
			else
				lines[0].color = node_->key_nodes[i]->keyColor;
			
			
			//lines[1].position = sf::Vector2f(node_->key_nodes[i]->x*scale, node_->key_nodes[i]->z*scale);
			if (D3)
				lines[1].position = sf::Vector2f(otherPt->projX, otherPt->projY);
			else
				lines[1].position = sf::Vector2f(node_->key_nodes[i]->x*scale, node_->key_nodes[i]->z*scale);

			
			lines[1].color = sf::Color(20, 30, 160, 255);
			window->draw(lines);
			delete otherPt;
		}

		delete thisPt;
	}

	std::vector<sf::ConvexShape> draw_buffer;
	std::vector<double> depth_buffer;
	
	void flushBuffer(sf::RenderWindow * window, bool useZDepth)
	{
		//order it by z-depth;
		if (useZDepth)
		{
			for (int i = 0; i < draw_buffer.size(); i++)
			{

				for (int j = draw_buffer.size()-1; j > i; j--)
				{
					if (depth_buffer[j] < depth_buffer[j - 1])
					{
						//swap
						double temp = depth_buffer[j];
						depth_buffer[j] = depth_buffer[j - 1];
						depth_buffer[j - 1] = temp;
						sf::ConvexShape tempS = draw_buffer[j];
						draw_buffer[j] = draw_buffer[j-1];
						draw_buffer[j - 1] = tempS;
					}
				}
			}
		}

		for (int i = 0; i < draw_buffer.size(); i++)
		{
			window->draw(draw_buffer[i]);
		}
		draw_buffer.clear();
		depth_buffer.clear();
	}

	void drawFloor(node * node_, bool color, bool drawMulti, iso::mat3 projection, double x_cent, double z_cent)
	{
		double xh = node_->x;
		double xl = node_->x;
		double zh = node_->z;
		double zl = node_->z;
		double y = node_->y;

		if (node_->cur_seg != NULL)
		{
			xh = node_->cur_seg->hiX;
			xl = node_->cur_seg->lowX;
			zh = node_->cur_seg->hiZ;
			zl = node_->cur_seg->lowZ;
			//y = node_->cur_seg->lowY;
		}

		iso::pointdef * pt0 = new iso::pointdef(xl - x_cent, zl - z_cent, y);
		iso::pointdef * pt1 = new iso::pointdef(xl - x_cent, zh - z_cent, y);
		iso::pointdef * pt2 = new iso::pointdef(xh - x_cent, zh - z_cent, y);
		iso::pointdef * pt3 = new iso::pointdef(xh - x_cent, zl - z_cent, y);

		iso::applyMatrix(projection, pt0);
		iso::applyMatrix(projection, pt1);
		iso::applyMatrix(projection, pt2);
		iso::applyMatrix(projection, pt3);
		sf::ConvexShape rect;
		rect.setPointCount(4);
		rect.setPoint(0, sf::Vector2f(pt0->projX, pt0->projY));
		rect.setPoint(1, sf::Vector2f(pt1->projX, pt1->projY));
		rect.setPoint(2, sf::Vector2f(pt2->projX, pt2->projY));
		rect.setPoint(3, sf::Vector2f(pt3->projX, pt3->projY));

		rect.setOrigin(1.0, 1.0);
		//rect.setPosition(thisPt->projX, thisPt->projY);
		if(color)
			rect.setFillColor(node_->keyColor);
		else
			rect.setFillColor(sf::Color(70,70,70,255));

		rect.setOutlineColor(node_->keyColor);
		rect.setOutlineThickness(0.2);

		//std::cout << pt0->projZ << "\t" << pt1->projZ <<"\t"<< pt2->projZ << "\t" << pt3->projZ << "\n";;
		depth_buffer.push_back(0.25 * (pt0->projZ + pt1->projZ + pt2->projZ + pt3->projZ));
		draw_buffer.push_back(rect);
		//Draw Aux nodes
		if (drawMulti && color)
		{
			if (node_->color_mask.size() > 1)
			{
				for (int i = node_->color_mask.size() - 1; i >= 1; i--)
					//		for (int i = 0; i < node_->color_mask.size(); i--)
				{
					/*
					sf::CircleShape dotA(1.0, 16);
					dotA.setOrigin(1.0, 1.0);
					iso::pointdef * otherPt = new iso::pointdef(node_->x + 0.2*i, node_->z + 0.2*i, node_->y);
					iso::applyMatrix(projection, otherPt);
					dotA.setPosition(thisPt->projX + 0.2*i, thisPt->projY + 0.2*i);
					dotA.setFillColor(node_->color_mask[i - 1]);
					*/
					sf::ConvexShape rect2 = rect;
					rect.setFillColor(node_->color_mask[i - 1]);
					rect2.move(0.2*i, 0.2*i);

					draw_buffer.push_back(rect2);
					depth_buffer.push_back(0.25 * (pt0->projZ + pt1->projZ + pt2->projZ + pt3->projZ)+0.002*i);
					//window->draw(rect2);
					//delete otherPt;
				}
			}
		}
		//window->draw(rect);
		//Children Nodes
		for (int i = 0; i < node_->children.size(); i++)
		{
			//draw lines together.
			/*
			sf::VertexArray lines(sf::LinesStrip, 2);
			lines[0].position = sf::Vector2f(thisPt->projX, thisPt->projY);
			lines[0].color = node_->keyColor;

			iso::pointdef * otherPt = new iso::pointdef(node_->children[i]->x, node_->children[i]->z, node_->children[i]->y);
			iso::applyMatrix(projection, otherPt);

			lines[1].position = sf::Vector2f(otherPt->projX, otherPt->projY);
			lines[1].color = node_->children[i]->keyColor;
			window->draw(lines);
			//make those draw.*/
			drawFloor(node_->children[i], color, drawMulti, projection, x_cent, z_cent);
			//delete otherPt;
		}

		delete pt0;
		delete pt1;
		delete pt2;
		delete pt3;
/*
		//Contents
		if (settings::map_show_content)
		{
			for (int i = 0; i < node_->contents.size(); i++)
			{
				sf::CircleShape dotA(0.5, 6);
				dotA.setOrigin(0.5, 0.5);
				dotA.setFillColor(sf::Color(20, 30, 160, 255));

				iso::pointdef * otherPt = new iso::pointdef(node_->contents[i]->x, node_->contents[i]->z, node_->contents[i]->y);
				iso::applyMatrix(projection, otherPt);
				dotA.setPosition(otherPt->projX, otherPt->projY);
				window->draw(dotA);

				//draw lines together.
				sf::VertexArray lines(sf::LinesStrip, 2);

				lines[0].position = sf::Vector2f(thisPt->projX, thisPt->projY);


				lines[0].color = node_->keyColor;
				lines[1].position = sf::Vector2f(otherPt->projX, otherPt->projY);
				lines[1].color = sf::Color(20, 30, 160, 255);
				window->draw(lines);
				delete otherPt;
			}
		}

		//keys
		for (int i = node_->key_nodes.size() - 1; i >= 0; i--)
		{
			sf::CircleShape dotA(0.5, 4);
			dotA.setOrigin(0.5, 0.5);
			dotA.setFillColor(node_->key_nodes[i]->keyColor);


			iso::pointdef * otherPt = new iso::pointdef(node_->key_nodes[i]->x, node_->key_nodes[i]->z, node_->key_nodes[i]->y);
			iso::applyMatrix(projection, otherPt);
			dotA.setPosition(otherPt->projX + i*0.2, otherPt->projY + i*0.2);
			
			window->draw(dotA);

			//draw lines together.
			sf::VertexArray lines(sf::LinesStrip, 2);

			lines[0].position = sf::Vector2f(thisPt->projX, thisPt->projY);
			lines[0].color = node_->key_nodes[i]->keyColor;
			lines[1].position = sf::Vector2f(otherPt->projX, otherPt->projY);
			lines[1].color = sf::Color(20, 30, 160, 255);
			window->draw(lines);
			delete otherPt;
		}
*/
	}

	double deltaMouseX;
	double deltaMouseY;
	double oldMouseX;
	double oldMouseY;

	void setDeltaMouse() {
		deltaMouseX = sf::Mouse::getPosition().x - oldMouseX;
		oldMouseX = sf::Mouse::getPosition().x;

		deltaMouseY = sf::Mouse::getPosition().y - oldMouseY;
		oldMouseY = sf::Mouse::getPosition().y;
	}

	double theta = 0;
	double phi = 0;
	double scaleF = 1.0;

	void displayNode_loop(node * map)
	{
		sf::RenderWindow * window = new sf::RenderWindow(sf::VideoMode(300, 300), "Map");
		sf::View view;
		view.reset(sf::FloatRect(0, 0, 300, 300));
		view.setCenter(0, 0);
		double viewScale = 1.0;
		// Set its target viewport to be half of the window
		view.setViewport(sf::FloatRect(0.f, 0.f, 1.f, 1.f));
		window->setView(view);
		bool isFocus = true;
		sf::Color clearColor(255, 255, 255, 255);

		double x_cent = 0.0;
		double z_cent = 0.0;
		
		while (window->isOpen())
		{
			//window->setActive();
			sf::Event event;
			while (window->pollEvent(event))
			{
				if (event.type == event.Resized)
				{
					view.reset(sf::FloatRect(0, 0, event.size.width, event.size.height));
					view.setCenter(0, 0);
					viewScale = 1.0;
					view.setViewport(sf::FloatRect(0.f, 0.f, 1.f, 1.f));
					window->setView(view);
				}
				if (event.type == event.GainedFocus)
				{
					isFocus = true;
					oldMouseX = sf::Mouse::getPosition().x;
					oldMouseY = sf::Mouse::getPosition().y;
				}
				if (event.type == event.LostFocus)
					isFocus = false;
				if (event.type == sf::Event::MouseWheelScrolled) 
				{
					if (isFocus)
					{
						viewScale *= event.mouseWheelScroll.delta > 0 ? 0.9 : 1.1;
						view.zoom(event.mouseWheelScroll.delta>0 ? 0.9 : 1.1);
					}
				}
				if (((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape)) || event.type == sf::Event::Closed)
				{
					//exit = true;
					run_game = false;
				}
			}
			if (isFocus)
			{
				setDeltaMouse();

				if (sf::Mouse::isButtonPressed(sf::Mouse::Middle))
				{
					phi -= deltaMouseY;
					theta -= deltaMouseX;
					if (phi > 85.0) phi = 85.0;
					if (phi < 0.0) phi = 0.0;

					if (theta > 90.0) theta = 90.0;
					if (theta < -90.0) theta = -90.0;
				}

				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad0))
				{
					theta = 0.0;
					phi = 0.0;
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad1))
				{
					theta = -30.0;
					phi = 50.0;
				}

				double theta_use = round(theta / 5.0)*5.0 * TO_RAD;
				double phi_use = round(phi / 5.0)*5.0 * TO_RAD;

				if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
				{
					//view.setCenter(view.getCenter().x - deltaMouseX*viewScale, view.getCenter().y - deltaMouseY*viewScale);

					x_cent -= viewScale * ( deltaMouseX * cos(theta_use) + deltaMouseY * sin(theta_use) );
					z_cent -= viewScale * ( deltaMouseY * cos(theta_use) - deltaMouseX * sin(theta_use) );
				}

				iso::mat3 proj;

				iso::mat3 rXMat = iso::rotationX(phi_use);
				iso::mat3 rZMat = iso::rotationZ(theta_use);

				proj = ((proj) * rXMat) * rZMat;

				window->clear(clearColor);
				window->setView(view);
				//drawNode(window, map, 1.0, false, true, proj);
				drawFloor(map, true, false, proj, x_cent, z_cent);
				flushBuffer(window, true);
				//Draw A Compass.
				//Have it in Map and in Game
				window->display();
			}
		}
	}

	void displayNodeMap(node * map)
	{
		/*
		std::thread rendering(render::render_loop, &window, &cam, &world_meshes_c, &hud_meshes_c);
		rendering.detach();
		*/
		std::thread disp(seg::displayNode_loop, map);
		disp.detach();
	}

}




