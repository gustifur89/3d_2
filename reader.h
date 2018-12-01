#pragma once
#include "headers.h"
#include "mesh.h"
#include "primitives.h"
#include "items.h"
//#include "segment.h"

namespace reader
{
	bool read_ply(std::string file_name, mesh::Mesh_c &mesh_)
	{
		//std::cout << file_name<<"\n";
		std::ifstream file;
		file = std::ifstream(file_name);
		if (file.fail())
		{
			return 0;
		}
		std::string line;

		std::vector<mesh::Vertex_c> verts;
		std::vector<mesh::face_c> face_list;

		int vertex_number = 0;
		int face_number = 0;
		int count_v = 0;
		int count_f = 0;
		bool prelim = false;
		bool vertexes = false;
		bool faces = false;

		//0 indexed

		while (1)
		{
			//assumes you get 9 in.
			file >> line;
			if (file.eof()) 
				break;
			if (line == "element")
			{
				std::string type;
				file >> type;
				if (type == "vertex")
				{
					file >> vertex_number;
				}
				if (type == "face")
				{
					file >> face_number;
				}
			}
			else if (line == "end_header")
			{
				vertexes = true;
				break;
			}

		}

		while (1)
		{
			if (vertexes)
			{
				if (count_v == vertex_number)
				{
					//exit
					vertexes = false;
					faces = true;
				}
				else
				{
					//read vertex;
					float x, y, z, nx, ny, nz;
					int r, g, b;
					file >> x >> z >> y >> nx >> nz >> ny;	//y and z switch
					file >> r >> g >> b;
					verts.push_back(mesh::Vertex_c(-x, y, z, -nx, ny, nz, r, g, b));
					count_v++;
					/* / 
					float x, y, z, nx, ny, nz;
					int r, g, b;
					file >> x >> z >> y >> nx >> nz >> ny;	//y and z switch
					file >> r >> g >> b;
					verts.push_back(mesh::Vertex_c(x, y, z, nx, ny, nz, r, g, b));
					count_v++;
					//*/
				}
			}
			else if (faces)
			{
				if (count_f == face_number)
				{
					//exit
					break;
				}
				else
				{
					//read vertex;
					/*int num_v;
					int v1, v2, v3;
					file >> num_v >> v1 >> v2 >> v3;
					face_list.push_back(mesh::face_c(verts[v1], verts[v2], verts[v3]));
					count_f++;*/
					//read vertex;
					int num_v;
					int v1, v2, v3, v4;
					file >> num_v;// >> v1 >> v2 >> v3;
					if (num_v == 3)
					{
						file >> v1 >> v2 >> v3;
						face_list.push_back(mesh::face_c(verts[v1], verts[v2], verts[v3]));
					}
					else if (num_v == 4)
					{
						file >> v1 >> v2 >> v3 >> v4;
						face_list.push_back(mesh::face_c(verts[v1], verts[v2], verts[v3]));
						face_list.push_back(mesh::face_c(verts[v1], verts[v3], verts[v4]));
					}

					count_f++;
				}
			}
		}
		mesh_.faces = face_list;
		mesh_.commit_mesh();
		mesh_.file = file_name;
		mesh_.type = -1;
		file.close();
		return 1;
	}
	/*
	bool read_seg(std::string file_name, seg::segment &seg_ )
	{

		//std::cout << file_name<<"\n";
		std::ifstream file;
		file = std::ifstream(file_name);
		if (file.fail())
		{
			return 0;
		}
		std::string line;

		std::vector<mesh::Vertex_c> verts;
		std::vector<mesh::face_c> face_list;

		int vertex_number = 0;
		int face_number = 0;
		int count_v = 0;
		int count_f = 0;
		bool prelim = false;
		bool vertexes = false;
		bool faces = false;

		//read from file
		while (1)
		{
			//assumes you get 9 in.
			file >> line;
			if (file.eof())
				break;
			if (line == "element")
			{
				std::string type;
				file >> type;
				if (type == "vertex")
				{
					file >> vertex_number;
				}
				if (type == "face")
				{
					file >> face_number;
				}
			}
			else if (line == "end_header")
			{
				vertexes = true;
				break;
			}
		}

		
		//assemble mesh
		double lx, ly, lz, hx, hy, hz;
		while (1)
		{
			if (vertexes)
			{
				if (count_v == vertex_number)
				{
					//exit
					vertexes = false;
					faces = true;
				}
				else
				{
					//read vertex;
					float x, y, z, nx, ny, nz;
					int r, g, b;
					file >> x >> z >> y >> nx >> nz >> ny;	//y and z switch
					file >> r >> g >> b;
					if (count_v == 0)
					{
						lx = x;
						hx = x;
						ly = y;
						hy = y;
						lz = z;
						hz = z;
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
					verts.push_back(mesh::Vertex_c(-x, y, z, -nx, ny, nz, r, g, b));
					count_v++;
				}
			}
			else if (faces)
			{
				if (count_f == face_number)
				{
					//exit
					break;
				}
				else
				{
					//read vertex;
					int num_v;
					int v1, v2, v3, v4;
					file >> num_v;
					if (num_v == 3)
					{
						file >> v1 >> v2 >> v3;
						face_list.push_back(mesh::face_c(verts[v1], verts[v2], verts[v3]));
					}
					else if (num_v == 4)
					{
						file >> v1 >> v2 >> v3 >> v4;
						face_list.push_back(mesh::face_c(verts[v1], verts[v2], verts[v3]));
						face_list.push_back(mesh::face_c(verts[v1], verts[v3], verts[v4]));
					}

					count_f++;
				}
			}
		}

		seg_.mesh_->faces = face_list;
		seg_.mesh_->commit_mesh();
		seg_.mesh_->file = file_name;
	
		//nodes
		while (1)
		{
			file >> line;
			//tpye of segment
			//	"TYPE" TYPE
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
						std::cout << x << " " << y << " " << z << " " << "       k\n";
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
				else if (type == "ELEVATOR")
				{
					seg_.type = seg::segment::TYPE::ELEVATOR;
				}
				else
				{
					seg_.type = seg::segment::TYPE::CROSS;
				}
			}

			if (file.eof())
				break;
		}

		//find lowest and highest in each direction. to get bounding box for spawning, &c.
		seg_.lowX = lx;
		seg_.lowY = ly;
		seg_.lowZ = lz;
		seg_.hiX = hx;
		seg_.hiY = hy;
		seg_.hiZ = hz;

		file.close();
		return 1;
	}
	*/

	bool read_animation(std::string file_name, std::vector<mesh::Mesh_c*> &frames)
	{
		std::ifstream file;
		file = std::ifstream(file_name);
		if (file.fail())
		{
			return 0;
		}
		std::string line;
		int num_frames = 0;

		file >> line;
		if (line == "frames")
		{
			file >> num_frames;
		}

		for (int i = 0; i < num_frames; i++)
		{
			file >> line;
			mesh::Mesh_c * mesh = new mesh::Mesh_c();
			if(!read_ply(line, *mesh))
				std::cout << "failed\n";
			frames.push_back(mesh);
		}

		file.close();
		return true;
	}

	bool read_ply(std::string file_name, mesh_p::Mesh_c &mesh_)
	{
		//std::cout << file_name<<"\n";
		std::ifstream file;
		file = std::ifstream(file_name);

		std::string line;

		std::vector<mesh_p::Vertex_c*> verts;
		std::vector<mesh_p::face_c> face_list;

		int vertex_number = 0;
		int face_number = 0;
		int count_v = 0;
		int count_f = 0;
		bool prelim = false;
		bool vertexes = false;
		bool faces = false;

		//0 indexed

		while (1)
		{
			//assumes you get 9 in.
			file >> line;
			if (file.eof())
				break;
			if (line == "element")
			{
				std::string type;
				file >> type;
				if (type == "vertex")
				{
					file >> vertex_number;
				}
				if (type == "face")
				{
					file >> face_number;
				}
			}
			else if (line == "end_header")
			{
				vertexes = true;
				break;
			}

		}

		while (1)
		{
			if (vertexes)
			{
				if (count_v == vertex_number)
				{
					//exit
					vertexes = false;
					faces = true;
				}
				else
				{
					//read vertex;
					float x, y, z, nx, ny, nz;
					int r, g, b;
					file >> x >> z >> y >> nx >> nz >> ny;	//y and z switch
					file >> r >> g >> b;
					verts.push_back(new mesh_p::Vertex_c(x, y, -z, nx, ny, -nz, r, g, b));
					count_v++;
				}
			}
			else if (faces)
			{
				if (count_f == face_number)
				{
					//exit
					break;
				}
				else
				{
					//read vertex;
					int num_v;
					int v1, v2, v3, v4;
					file >> num_v;// >> v1 >> v2 >> v3;
					if (num_v == 3)
					{
						file >> v1 >> v2 >> v3;
						face_list.push_back(mesh_p::face_c(*verts[v1], *verts[v2],*verts[v3]));
					}
					else if (num_v == 4)
					{
						file >> v1 >> v2 >> v3 >> v4;
						face_list.push_back(mesh_p::face_c(*verts[v1], *verts[v2], *verts[v3]));
						face_list.push_back(mesh_p::face_c(*verts[v1], *verts[v4], *verts[v3]));
					}
					
					count_f++;
				}
			}
		}
	//	std::cout << "in here?\n";
		mesh_.faces = face_list;
	//	std::cout << "is it?\n";
		mesh_.commit_mesh();
	//	std::cout << "committ mesh??\n";
		mesh_.file = file_name;
	//std::cout << "nruh?\n";
		mesh_.type = -1;
		file.close();
		return 1;
	}

	int find_vertex(std::vector<mesh::Vertex_c> vert_list, mesh::Vertex_c vert)
	{
		for (int i = 0; i < vert_list.size(); i++)
		{
			if (vert == vert_list[i])
				return i;
		}
		return -1;
	}

	void export_map_mesh(std::vector<mesh::Mesh_c *> meshes)
	{
		std::ofstream file;
		std::string file_name_str = "test_m.ply";
		std::cout << "File name:\n";                                                                
		std::cin >> file_name_str;
		file.open(file_name_str);
		file << file_name_str << "\n";
		std::vector<mesh::Vertex_c> verts;
		std::vector<mesh::face_c> faces;
		int face_num = 0;
		int vert_count = 0;
		for (int i = 0; i < meshes.size(); i++)
		{
			//i is the mesh
			for (int j = 0; j < meshes[i]->faces.size(); j++)
			{
				//j is the face
				for (int k = 0; k < 3; k++)
				{
					//k is the vertex
					//check and add.
					if (find_vertex(verts, mesh::transform_vertex(meshes[i]->faces[j].vertex[k], meshes[i]->x, meshes[i]->y, meshes[i]->z, meshes[i]->azimuth, meshes[i]->pitch, meshes[i]->roll)) == -1)
						verts.push_back(mesh::transform_vertex(meshes[i]->faces[j].vertex[k], meshes[i]->x, meshes[i]->y, meshes[i]->z, meshes[i]->azimuth, meshes[i]->pitch, meshes[i]->roll));
					vert_count++;
			//		std::cout << vert_count << "\n";
				}
				face_num++;
			}
		}
		//we only need to add the stuff we care about...
		file << "element vertex " << verts.size() << "\n";
		file << "element face " << face_num << "\n";
		file << "end_header" << "\n";
		//print it out.
		for (int i = 0; i < verts.size(); i++)
		{
			//x y z nx ny nz r g b
			file << verts[i].x << " " << -verts[i].z << " " << verts[i].y << " " << verts[i].nx << " " << -verts[i].nz << " " << verts[i].ny << " " << verts[i].r << " " << verts[i].g << " " << verts[i].b << "\n";
		}
		int test = 0;
		//now the faces
		for (int i = 0; i < meshes.size(); i++)
		{
			//i is the mesh
			for (int j = 0; j < meshes[i]->faces.size(); j++)
			{
				//j is the face
				file << "3 " << find_vertex(verts, mesh::transform_vertex(meshes[i]->faces[j].vertex[0], meshes[i]->x, meshes[i]->y, meshes[i]->z, meshes[i]->azimuth, meshes[i]->pitch, meshes[i]->roll)) << " " << find_vertex(verts, mesh::transform_vertex(meshes[i]->faces[j].vertex[1], meshes[i]->x, meshes[i]->y, meshes[i]->z, meshes[i]->azimuth, meshes[i]->pitch, meshes[i]->roll)) << " " << find_vertex(verts, mesh::transform_vertex(meshes[i]->faces[j].vertex[2], meshes[i]->x, meshes[i]->y, meshes[i]->z, meshes[i]->azimuth, meshes[i]->pitch, meshes[i]->roll)) << "\n";
				test++;
		//		std::cout << test << "\n";
			}
		}
		file.close();
		//std::cout << "saved " << file_name_str << "\n";
	}

	void export_map_edit(std::vector<mesh::Mesh_c *> meshes)
	{		
		std::ofstream file;
		std::string file_name_str = "test_m.ply";
		std::cout << "File name:\n";
		std::cin >> file_name_str;
		file.open(file_name_str);
		file << file_name_str << "\n";
		file << "end_header\n";

		for (int i = 0; i < meshes.size(); i++)
		{	
			//load all the fancy stuff.
			//	type (1)
			//	size (3)
			//	translation (3)
			//	rotation (3)
			//	color (3)
			//	so i f f f f f f f f f i i i
			
			//IF non primitive, make type = -1, and put the file address at the end... then we can load and go...

			//types
			file << meshes[i]->type << " ";
			//size
			file << meshes[i]->width << " " << meshes[i]->height << " " << meshes[i]->depth << " ";
			//translation
			file << meshes[i]->x << " " << meshes[i]->y << " " << meshes[i]->z << " ";
			//rotation
			file << meshes[i]->pitch << " " << meshes[i]->azimuth << " " << meshes[i]->roll << " ";
			//color....        objectionable.... See if they... uhhh.... how do you load in non-primitives... 
			file << meshes[i]->faces[0].vertex[0].r << " " << meshes[i]->faces[0].vertex[0].g << " " << meshes[i]->faces[0].vertex[0].b;
			if (meshes[i]->type == -1)
			{
				file << " " << meshes[i]->file << " ";
			}
			file << "\n";
		}	
		file << -2;
		file.close();
	}		

	void load_mp(std::string file_name, std::vector<mesh::Mesh_c *> &meshes)
	{
		//std::cout << file_name<<"\n";
		std::ifstream file;
		file = std::ifstream(file_name);
		std::string line;
		//0 indexed
		while (1)
		{
			//Do anything in header
			file >> line;
			if (line == "end_header")
			{
				break;
			}

		}

		//reads faces...
		while (1)
		{
			//	type (1)
			//	size (3)
			//	translation (3)
			//	rotation (3)
			//	color (3)
			//	so i f f f f f f f f f i i i

			//IF non primitive, make type = -1, and put the file address at the end... then we can load and go...

			int type;
			float w, h, d, x, y, z, pit, az, rol;
			int r, g, b;
			std::string file_name;
			file >> type;
			if (type == -2)
				break;
			//grab the rest;
			file >> w >> h >> d >> x >> y >> z >> pit >> az >> rol;
			file >> r >> g >> b;
			if (type == -1)
				file >> file_name;


			//then procede
			mesh::Mesh_c * new_m = new mesh::Mesh_c();
			if (type == PRIM_CUBE)
				*new_m = prim_cube;
			else if (type == PRIM_CYL)
				*new_m = prim_cyl;
			else if (type == PRIM_RAMP)
				*new_m = prim_ramp;
			else if (type == PRIM_SPHERE)
				*new_m = prim_sphere;
			else if (type == PRIM_CONE)
				*new_m = prim_cone;
			else if (type == PRIM_DOME)
				*new_m = prim_dome;
			else
			{
				read_ply(file_name, *new_m);
			}
			new_m->type = type;
			new_m->x = x;
			new_m->y = y;
			new_m->z = z;
			new_m->roll = rol;
			new_m->pitch = pit;
			new_m->azimuth = az;
			mesh::resize_mesh(new_m, w, h, d);
			mesh::recolor_mesh(new_m, r, g, b);
			meshes.push_back(new_m);
		}
		file.close();

	}
}			
