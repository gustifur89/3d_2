#pragma once
#include "headers.h"
#include "editing.h"
#include "segment.h"
#include <filesystem>
#define world_ std::vector<mesh::Mesh_c *>

namespace builder
{
	void build_level(std::string file_name, world_ &meshes)
	{
		std::ifstream file;
		file = std::ifstream(file_name);
		std::string line;
		std::string line_2;
		std::string line_3;
		while (1)
		{
			std::getline(file, line);
			edit::cmd(line, meshes);
			if (file.eof())
				break;
			/*file >> line;
			if (file.eof())
				break;
			if (line == "load")
			{

				file >> line_2;
				line_3 = line + " " + line_2;
				std::cout << line_3 << "\n";
				edit::cmd(line_3, meshes);
			}
			else if (line == "load_edit")
			{

				file >> line_2;
				line_3 = line + " " + line_2;
				std::cout << line_3 << "\n";
				edit::cmd(line_3, meshes);
			}*/

		}
	}
	
	void load_segments(std::string file_name, std::vector<seg::segment> &segments)
	{
		for (auto & p : std::experimental::filesystem::directory_iterator(file_name))
		{
			seg::segment seg_;
			if (seg::read_seg(p.path().string(), seg_))
				segments.push_back(seg_);
		}
	}

	void load_objects(std::string file_name, std::vector<mesh::Mesh_c *> &object_list)
	{
		for (auto & p : std::experimental::filesystem::directory_iterator(file_name))
		{
			mesh::Mesh_c * mesh_ = new mesh::Mesh_c();
			if (reader::read_ply(p.path().string(), *mesh_))
				object_list.push_back(mesh_);
		}
	}

	/*void build_level(std::string file_name, std::vector<mesh_p::Mesh *> &meshes)
	{
		std::ifstream file;
		file = std::ifstream(file_name);
		std::string line;
		std::string line_2;
		std::string line_3;
		while (1)
		{
			std::getline(file, line);
			edit::cmd(line, meshes);
			if (file.eof())
				break;
		}
	}*/
}