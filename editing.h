#pragma once
#include "headers.h"
#include "mesh.h"
#include "primitives.h"




namespace edit
{
	struct input_field;

	std::string input;
	std::string hist = "";

	input_field *focus;

	struct input_field
	{
		input_field() {};
		void draw_field(sf::RenderWindow &window, sf::Font font, bool &did_select)
		{
			if (is_button)
			{
				
				sf::Text text(content, font);;
				if (button_val)
				{
					text.setString((char)(5847u));
					text.setFillColor(sf::Color::Red);
				}
				else
				{
					text.setString(((char)150));
					text.setFillColor(color);
				}

				if (pause && this->dynamic)
				{
					if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
					{
						float x_ = sf::Mouse::getPosition(window).x;
						float y_ = sf::Mouse::getPosition(window).y;
						float w = text.getLocalBounds().width;
						float h = text.getLocalBounds().height;

						if (default_width > w) w = default_width;
						if (default_height > h) h = default_height;

						if (x_ >= this->x && x <= this->x + w && y_ >= this->y && y_ <= this->y + h && !button_toggle)
						{
							button_val = !button_val;
							edit::focus = nullptr;
							this->content = "";
							did_select = true;
						}
						button_toggle = true;
					}
					else
					{
						button_toggle = false;
						did_select = true;
					}
				}
				text.setPosition(x, y);
				text.setCharacterSize(size);
				window.draw(text);
			}
			else
			{
				sf::Text text(content, font);

				if (pause && this->dynamic)
				{
					if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
					{
						float x_ = sf::Mouse::getPosition(window).x;
						float y_ = sf::Mouse::getPosition(window).y;
						float w = text.getLocalBounds().width;
						float h = text.getLocalBounds().height;

						if (default_width > w) w = default_width;
						if (default_height > h) h = default_height;

						if (x_ >= this->x && x <= this->x + w && y_ >= this->y && y_ <= this->y + h)
						{
							//if(edit::focus)
							//	edit::focus->is_selected = false;
							edit::focus = this;
							this->content = "";
							//this->is_selected = true;
							did_select = true;
						}
					}
					else
					{
						did_select = true;
					}
				}

				if (edit::focus == this)
				{
					text.setFillColor(sf::Color::Red);
				}
				else
				{
					text.setFillColor(color);
				}
				text.setPosition(x, y);
				text.setCharacterSize(size);
				window.draw(text);
			}
		}
		//std::string title;s
		std::string content;
		int x = 0, y = 0, size = 10;
		int default_width = 10;
		int default_height = 10;
		bool is_selected = false;
		sf::Color color = sf::Color(255, 255, 255, 255);
		bool dynamic = false;
		bool is_button = false;
		bool button_val = false;
		bool button_toggle = false;
	};

	input_field pos_x;
	input_field pos_y;
	input_field pos_z;

	input_field siz_x;
	input_field siz_y;
	input_field siz_z;

	input_field rot_x;
	input_field rot_y;
	input_field rot_z;

	input_field col_r;
	input_field col_g;
	input_field col_b;

	input_field on_grd;
	input_field grd_size;

	/*
	void cmd(std::string str, std::vector<mesh::Mesh_c *> &meshes)
	{
		std::string::size_type loc = str.find(" ", 0);
		if (loc != std::string::npos)
		{
			if (str.substr(0, loc) == "spawn")
			{
				if (str.substr(loc, str.size()) == " cube")
				{
					spawn_primitive(meshes, PRIM_CUBE, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1);
				}
				if (str.substr(loc, str.size()) == " cyl")
				{
					spawn_primitive(meshes, PRIM_CYL, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1);
				}
				if (str.substr(loc, str.size()) == " ramp")
				{
					spawn_primitive(meshes, PRIM_RAMP, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1);
				}
				if (str.substr(loc, str.size()) == " sphere")
				{
					spawn_primitive(meshes, PRIM_SPHERE, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1);
				}
				if (str.substr(loc, str.size()) == " dome")
				{
					spawn_primitive(meshes, PRIM_DOME, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1);
				}
				if (str.substr(loc, str.size()) == " cone")
				{
					spawn_primitive(meshes, PRIM_CONE, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1);
				}
			}
			if (str.substr(0, loc) == "scale"|| str.substr(0, loc) == "resize")
			{
				std::string::size_type locx = str.find(" ", loc + 1);
				std::string::size_type locy = str.find(" ", locx + 1);
				float x = std::atof(str.substr(loc + 1, locx).c_str());
				float z = std::atof(str.substr(locx + 1, locy).c_str());
				float y = std::atof(str.substr(locy + 1, str.size()).c_str());
				if (editing)
					mesh::resize_mesh(editing, x, y, z);
			}
			if (str.substr(0, loc) == "move")
			{
				std::string::size_type locx = str.find(" ", loc + 1);
				std::string::size_type locy = str.find(" ", locx + 1);
				float x = std::atof(str.substr(loc + 1, locx).c_str());
				float y = std::atof(str.substr(locx + 1, locy).c_str());
				float z = std::atof(str.substr(locy + 1, str.size()).c_str());
				if (editing)
				{
					editing->x += -x;
					editing->y += z;
					editing->z += y;
				}
			}
			if (str.substr(0, loc) == "set")
			{
				std::string::size_type locx = str.find(" ", loc + 1);
				std::string::size_type locy = str.find(" ", locx + 1);
				float x = std::atof(str.substr(loc + 1, locx).c_str());
				float y = std::atof(str.substr(locx + 1, locy).c_str());
				float z = std::atof(str.substr(locy + 1, str.size()).c_str());
				if (editing)
				{
					editing->x = x;
					editing->y = z;
					editing->z = y;
				}
			}
			if (str.substr(0, loc) == "rot"|| str.substr(0, loc) == "rotate")
			{
				std::string::size_type locx = str.find(" ", loc + 1);
				std::string::size_type locy = str.find(" ", locx + 1);
				float x = std::atof(str.substr(loc + 1, locx).c_str());
				float y = std::atof(str.substr(locx + 1, locy).c_str());
				float z = std::atof(str.substr(locy + 1, str.size()).c_str());
				if (editing)
				{
					editing->pitch = x;
					editing->azimuth = z;
					editing->roll = y;
				}
			}
			if (str.substr(0, loc) == "pick")
			{
				float i = std::stoi(str.substr(loc, str.size()));
				if (i < 0)
					i = 0;
				if (i >= meshes.size())
					i = meshes.size() - 1;
				editing = meshes[i];
			}
			if (str.substr(0, loc) == "color")
			{
				std::string::size_type locx = str.find(" ", loc + 1);
				std::string::size_type locy = str.find(" ", locx + 1);
				float x = std::atof(str.substr(loc + 1, locx).c_str());
				float y = std::atof(str.substr(locx + 1, locy).c_str());
				float z = std::atof(str.substr(locy + 1, str.size()).c_str());
				if (editing)
				{
					mesh::recolor_mesh(editing, x, y, z);
				}
			}
			if (str.substr(0, loc) == "load")
			{
				std::string file_to_load = str.substr(loc + 1, str.size());
				mesh::Mesh_c * new_m = new mesh::Mesh_c();
				reader::read_ply(file_to_load, *new_m);
				new_m->x = 0;
				new_m->y = 0;
				new_m->z = 0;
				new_m->roll = 0;
				new_m->pitch = 0;
				new_m->azimuth = 0;
				meshes.push_back(new_m);
				editing = new_m;
			}
			if (str.substr(0, loc) == "delete")
			{
				int index = 0;
				for (int i = 0; i < meshes.size(); i++)
				{
					if (meshes[i] == editing)
					{
						index = i;
					}
				}
				meshes.erase(meshes.begin() + index);
				editing = nullptr;
			}
			if (str.substr(0, loc) == "load_edit")
			{
				std::string file_to_load = str.substr(loc + 1, str.size());
				reader::load_mp(file_to_load, meshes);
			}
		}
	}
	*/

	void cmd(std::string str, std::vector<mesh::Mesh_c * > &meshes)
	{
		std::string::size_type loc = str.find(" ", 0);
		if (loc != std::string::npos||true)
		{
			if (str.substr(0, loc) == "spawn")
			{
				if (str.substr(loc, str.size()) == " cube")
				{
					spawn_primitive(meshes, PRIM_CUBE, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1);
				}
				if (str.substr(loc, str.size()) == " cyl")
				{
					spawn_primitive(meshes, PRIM_CYL, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1);
				}
				if (str.substr(loc, str.size()) == " ramp")
				{
					spawn_primitive(meshes, PRIM_RAMP, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1);
				}
				if (str.substr(loc, str.size()) == " sphere")
				{
					spawn_primitive(meshes, PRIM_SPHERE, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1);
				}
				if (str.substr(loc, str.size()) == " dome")
				{
					spawn_primitive(meshes, PRIM_DOME, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1);
				}
				if (str.substr(loc, str.size()) == " cone")
				{
					spawn_primitive(meshes, PRIM_CONE, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1);
				}
			}
			if (str.substr(0, loc) == "scale" || str.substr(0, loc) == "resize")
			{
				std::string::size_type locx = str.find(" ", loc + 1);
				std::string::size_type locy = str.find(" ", locx + 1);
				float x = std::atof(str.substr(loc + 1, locx).c_str());
				float z = std::atof(str.substr(locx + 1, locy).c_str());
				float y = std::atof(str.substr(locy + 1, str.size()).c_str());
				if (editing)
					mesh::resize_mesh(editing, x, y, z);
			}
			if (str.substr(0, loc) == "move")
			{
				std::string::size_type locx = str.find(" ", loc + 1);
				std::string::size_type locy = str.find(" ", locx + 1);
				float x = std::atof(str.substr(loc + 1, locx).c_str());
				float y = std::atof(str.substr(locx + 1, locy).c_str());
				float z = std::atof(str.substr(locy + 1, str.size()).c_str());
				if (editing)
				{
					editing->x += -x;
					editing->y += z;
					editing->z += y;
				}
			}
			if (str.substr(0, loc) == "set")
			{
				std::string::size_type locx = str.find(" ", loc + 1);
				std::string::size_type locy = str.find(" ", locx + 1);
				float x = std::atof(str.substr(loc + 1, locx).c_str());
				float y = std::atof(str.substr(locx + 1, locy).c_str());
				float z = std::atof(str.substr(locy + 1, str.size()).c_str());
				if (editing)
				{
					editing->x = x;
					editing->y = z;
					editing->z = y;
				}
			}
			if (str.substr(0, loc) == "rot" || str.substr(0, loc) == "rotate")
			{
				std::string::size_type locx = str.find(" ", loc + 1);
				std::string::size_type locy = str.find(" ", locx + 1);
				float x = std::atof(str.substr(loc + 1, locx).c_str());
				float y = std::atof(str.substr(locx + 1, locy).c_str());
				float z = std::atof(str.substr(locy + 1, str.size()).c_str());
				if (editing)
				{
					editing->pitch = x;
					editing->azimuth = z;
					editing->roll = y;
				}
			}
			if (str.substr(0, loc) == "pick")
			{
				float i = std::stoi(str.substr(loc, str.size()));
				if (i < 0)
					i = 0;
				if (i >= meshes.size())
					i = meshes.size() - 1;
				editing = meshes[i];
			}
			if (str.substr(0, loc) == "color")
			{
				std::string::size_type locx = str.find(" ", loc + 1);
				std::string::size_type locy = str.find(" ", locx + 1);
				float x = std::atof(str.substr(loc + 1, locx).c_str());
				float y = std::atof(str.substr(locx + 1, locy).c_str());
				float z = std::atof(str.substr(locy + 1, str.size()).c_str());
				if (editing)
				{
					mesh::recolor_mesh(editing, x, y, z);
				}
			}
			if (str.substr(0, loc) == "load")
			{
				std::string file_to_load = str.substr(loc + 1, str.size());
				mesh::Mesh_c * new_m = new mesh::Mesh_c();
				reader::read_ply(file_to_load, *new_m);
				new_m->x = 0;
				new_m->y = 0;
				new_m->z = 0;
				new_m->roll = 0;
				new_m->pitch = 0;
				new_m->azimuth = 0;
				meshes.push_back(new_m);
				editing = new_m;
			}
			if (str.substr(0, loc) == "delete")
			{
				int index = 0;
				for (int i = 0; i < meshes.size(); i++)
				{
					if (meshes[i] == editing)
					{
						index = i;
					}
				}
				meshes.erase(meshes.begin() + index);
				editing = nullptr;
			}
			if (str.substr(0, loc) == "load_edit")
			{
			//	std::string file_to_load = str.substr(loc + 1, str.size());
			//	reader::load_mp(file_to_load, meshes);
			}
			if (str.substr(0, loc) == "set_type_static")
			{
				std::cout << ":bot";
				editing->s_type = mesh::Mesh_c::TYPE::STATIC;
			}
}
	}


	void text_edit(sf::Event event, std::vector<mesh::Mesh_c * > &meshes)
	{
		if (pause)
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return))
			{
				cmd(edit::input, meshes);
				edit::hist = "\n" + edit::input + edit::hist;
				edit::input = "";
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::BackSpace))
			{
				if (settings::use_console)
				{
					if (edit::input.size() > 0)
						edit::input.pop_back();
				}
				else
				{
					if (edit::focus)
					{
						if (edit::focus->content.size() > 0)
							edit::focus->content.pop_back();
					}
				}
			}
			else
			{
				char a = event.text.unicode;
				if (settings::use_console)
					edit::input += a;
				else
				{
					if (edit::focus)
						edit::focus->content += a;
				}
			}
		}
	}

	float o_mousex = 0.0;
	float o_mousey = 0.0;
	float edit_speed = 40.0;
	bool on_grid = true;
	float grid_size = 1.0;

	bool select_toggle = false;


	/*
	void dynamic_edit(std::vector<mesh::Mesh_c *> &meshes, sf::RenderWindow &window, camera_pos &cam)
	{
		if (pause)
		{
			if (editing)
			{
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z))
				{
					//move on z..
					//find z projection of object..
					editing->z += signum(sf::Mouse::getPosition(window).x - o_mousex - sf::Mouse::getPosition(window).y + o_mousey) *    sqrt((sf::Mouse::getPosition(window).x - o_mousex)*(sf::Mouse::getPosition(window).x - o_mousex) + (sf::Mouse::getPosition(window).y - o_mousey)*(sf::Mouse::getPosition(window).y - o_mousey))  * (edit_speed / 100.0);
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::X))
				{
					//move on x
					editing->x += signum(sf::Mouse::getPosition(window).x - o_mousex - sf::Mouse::getPosition(window).y + o_mousey) *    sqrt((sf::Mouse::getPosition(window).x - o_mousex)*(sf::Mouse::getPosition(window).x - o_mousex) + (sf::Mouse::getPosition(window).y - o_mousey)*(sf::Mouse::getPosition(window).y - o_mousey))  * (edit_speed / 100.0);
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Y))
				{
					//move on y
					editing->y += signum(sf::Mouse::getPosition(window).x - o_mousex - sf::Mouse::getPosition(window).y + o_mousey) *    sqrt((sf::Mouse::getPosition(window).x - o_mousex)*(sf::Mouse::getPosition(window).x - o_mousex) + (sf::Mouse::getPosition(window).y - o_mousey)*(sf::Mouse::getPosition(window).y - o_mousey))  * (edit_speed / 100.0);
				}
				else
				{
					if (on_grid)
					{
						editing->x = grid_size * round(editing->x / grid_size);
						editing->y = grid_size * round(editing->y / grid_size);
						editing->z = grid_size * round(editing->z / grid_size);
					}
				}


				if (&pos_x != focus)
					pos_x.content = std::to_string(editing->x);
				else
					if (pos_x.content.size() > 0)
						editing->x = std::atof(pos_x.content.c_str());
				if (&pos_y != focus)
					pos_y.content = std::to_string(editing->y);
				else
					if (pos_y.content.size() > 0)
						editing->y = std::atof(pos_y.content.c_str());
				if (&pos_z != focus)
					pos_z.content = std::to_string(editing->z);
				else
					if (pos_z.content.size() > 0)
						editing->z = std::atof(pos_z.content.c_str());




				if (editing->re_color)
				{
					if (&col_r == focus)
					{
						if (col_r.content.size() > 0 && std::atoi(col_r.content.c_str()) != editing->r)
						{
							mesh::recolor_mesh(editing, std::atoi(col_r.content.c_str()), editing->g, editing->b);
						}
					}
					if (&col_g == focus)
					{
						if (col_g.content.size() > 0 && std::atoi(col_g.content.c_str()) != editing->g)
						{
							mesh::recolor_mesh(editing, editing->r, std::atoi(col_g.content.c_str()), editing->b);
						}
					}
					if (&col_b == focus)
					{
						if (col_b.content.size() > 0 && std::atoi(col_b.content.c_str()) != editing->b)
						{
							mesh::recolor_mesh(editing, editing->r, editing->g, std::atoi(col_b.content.c_str()));
						}
					}
				}

				if (&col_r != focus)
					col_r.content = std::to_string(editing->r);

				if (&col_g != focus)
					col_g.content = std::to_string(editing->g);

				if (&col_b != focus)
					col_b.content = std::to_string(editing->b);






				if (&siz_x == focus)
				{
					if (siz_x.content.size() > 0)
					{
						mesh::resize_mesh(editing, std::atof(siz_x.content.c_str()), editing->height, editing->depth);
					}
				}
				if (&siz_y == focus)
				{
					if (siz_y.content.size() > 0)
					{
						mesh::resize_mesh(editing, editing->width, std::atof(siz_y.content.c_str()), editing->depth);
					}
				}
				if (&siz_z == focus)
				{
					if (siz_z.content.size() > 0)
					{
						mesh::resize_mesh(editing, editing->width, editing->height, std::atof(siz_z.content.c_str()));
					}
				}
				
				if (&siz_x != focus)
					siz_x.content = std::to_string(editing->width);
				
				if (&siz_y != focus)
					siz_y.content = std::to_string(editing->height);
				
				if (&siz_z != focus)
					siz_z.content = std::to_string(editing->depth);
				
				if (&rot_x != focus)
					rot_x.content = std::to_string(editing->pitch);
				else
					if (rot_x.content.size() > 0)
						editing->pitch = std::atof(rot_x.content.c_str());
				if (&rot_y != focus)
					rot_y.content = std::to_string(editing->azimuth);
				else
					if (rot_y.content.size() > 0)
						editing->azimuth = std::atof(rot_y.content.c_str());
				if (&rot_z != focus)
					rot_z.content = std::to_string(editing->roll);
				else
					if (rot_z.content.size() > 0)
						editing->roll = std::atof(rot_z.content.c_str());




			//	col_r.content = std::to_string( editing->faces[0].vertex[0].r );
		//		col_g.content = std::to_string( editing->faces[0].vertex[0].g );
		//		col_b.content = std::to_string( editing->faces[0].vertex[0].b );
			}
			else
			{
			
				//std::atof(pos_x.content.c_str());
				
			}
			o_mousex = sf::Mouse::getPosition(window).x;
			o_mousey = sf::Mouse::getPosition(window).y;
			
			if(grd_size.content.size() > 0)
				grid_size = std::atof(grd_size.content.c_str());
			
			if (on_grd.button_val)
			{
				on_grid = 1;
			}
			else
			{
				on_grid = 0;
			}
	


			//for selections...

			if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
			{
				int select_depth = 50.f;
				float select_resolution = 0.05;


				if (!select_toggle)
				{
					float m_xr = (sf::Mouse::getPosition(window).x - settings::view_port_width / 2)/settings::view_port_width;
					float m_yr = (sf::Mouse::getPosition(window).y - settings::view_port_height / 2)/settings::view_port_height;
					float min_z = 1.f;
					float d_theta = settings::fov_x * m_xr ;
					float d_phi = settings::fov_y * m_yr;
					float theta = ( cam.azimuth - d_theta ) * (PI / 180.f);			
					float phi = ( cam.pitch - d_phi ) * (PI / 180.f);

					theta += PI;

					for (float b = 0; b < select_depth; b += select_resolution)
					{
						//checks the stuff.... fun; bounding box...
						float x_ = cam.x + b * sin(theta) * cos(phi);
						float y_ = cam.y + b * sin(phi);
						float z_ = cam.z + b * cos(theta) * cos(phi);
						for (int i = 0; i < meshes.size(); i++)
						{
							mesh::Mesh_c mesh_ = *meshes[i];
							//bounding box...
							if (x_ < mesh_.x + mesh_.width && x_ > mesh_.x - mesh_.width && y_ < mesh_.y + mesh_.height && y_ > mesh_.y - mesh_.height && z_ < mesh_.z + mesh_.depth && z_ > mesh_.z - mesh_.depth)
							{
								//Select that boy!
								editing = meshes[i];mom
								break;
//								mesh::recolor_mesh(meshes[i], 120, 240, 130);
							}
						}
					}
				}
				select_toggle = true;
			}
			else
			{
				select_toggle = false;
			}
			

		}
	}
	*/
	void edit_input()
	{
		while (1)
		{
			//std::cout << input << "\n";
		}
	}
}
