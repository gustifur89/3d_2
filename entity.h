#pragma once
#include "headers.h"
#include "mesh.h"
#include "bsp.h"
#include "dynamic_object.h"

struct entity
{
	entity() {};
	entity(GLfloat x_, GLfloat y_, GLfloat z_, std::vector < std::vector<mesh::Mesh_g * > > display_mesh_, GLfloat radius_, GLfloat height_)
	{
		x = x_;
		y = y_;
		z = z_;
		display_mesh = display_mesh_;
		radius = radius_;
		height = height_;
		x_vel = 0.f;
		y_vel = 0.f;
		z_vel = 0.f;
	}

	void next_frame()
	{
		frame++;
		frame = frame % display_mesh[animation].size();
	}

	GLfloat * get_current_mesh_array()		//this gets called in the render to get the mesh.
	{
		return &display_mesh[animation][frame]->vertexes[0];
	}
	
	/*
	void add_frame(mesh::Mesh_g * frame)
	{
		display_mesh.push_back(frame);
	}
	void set_frame(mesh::Mesh_g * frame, int index)
	{
		display_mesh[index] = frame;
	}
	void set_frames(std::vector<mesh::Mesh_g *> frames)
	{
		display_mesh = frames;
	}
	*/

	//have movement stuff
	//ARCHIVED BECAUSE IT WORKS, BUT WE CAN DO BETTER
	/*
	void move(bsp_1::node *bsp_map)
	{
		//does the collider movement with the bsp...
		// and entity list. BUT how it interact with entities?...
		/*
		float hit_resolution = 40.f;
		float gravity = 10.f; 
		float epsilon = 0.025;
		float valid_step = 0.1;
		y_vel -= gravity * delta_time;
		GLfloat seg_x = x_vel * delta_time / hit_resolution;
		GLfloat seg_y = y_vel * delta_time / hit_resolution;
		GLfloat seg_z = z_vel * delta_time / hit_resolution;
		
		GLfloat valid_x = x;
		GLfloat valid_y = y;
		GLfloat valid_z = z;
		/*
		//for xz
		for (int b = 0; b <= hit_resolution; b++)
		{
			if (bsp_1::collide(bsp_map, valid_x + seg_x, valid_y, valid_z + seg_z, radius))
			{
				if (bsp_1::collide(bsp_map, valid_x + seg_x, valid_y + valid_step, valid_z + seg_z, radius))
				{
					break;
				}
				else
				{
					valid_x += seg_x;
					valid_y += valid_step;
					valid_z += seg_z;
				}
			}
			else
			{
				valid_x += seg_x;
				valid_z += seg_z;
			}
		}

		//for y
		for (int b = 0; b <= hit_resolution; b++)
		{
			if (bsp_1::collide(bsp_map, valid_x, valid_y + seg_y, valid_z, radius))
			{
				y_vel = 0;
				valid_y += epsilon;
				break;
			}
			else
			{
				valid_y += seg_y;
			}
		}
		* /

		double eff_y_vel = y_vel / settings::hit_resolution;
		double eff_x_vel = x_vel / settings::hit_resolution;
		double eff_z_vel = z_vel / settings::hit_resolution;
		if(abs(y_vel)<settings::max_y_vel)
			y_vel -= settings::gravity*delta_time;

		for (int b = 0; b < settings::hit_resolution; b++)
		{
			double y_ = y;
			y += eff_y_vel*delta_time;
			if (bsp_1::collide(bsp_map, x, y, z, 0.24f, 2.0f, y-height, bsp_1::TYPE::HORIZONTAL))
			{
				y -= eff_y_vel*delta_time;
				if(y_vel < 0)
					has_jump = true;
				y_vel = 0;
			}
			
			x += eff_x_vel*delta_time;
			z += eff_z_vel*delta_time;
			double dx = eff_x_vel*delta_time;
			double dz = eff_z_vel*delta_time;
			mesh::face_c face;
			if ((dx!=0||dz!=0) && bsp_1::collide_r(bsp_map, x, y, z, settings::player_radius, settings::player_height, bsp_1::TYPE::VERTICAL, face))
			{
				//do a quick check to see if it is TYPE::SEMI?
				double angle = 180.0 * acos(abs(face.normal[1])) / PI;
				if (angle > 0.01 && angle < 89)
				{
					if (angle > settings::max_walking_angle)
					{
						x -= dx;
						z -= dz;
					}
					else
					{
						//solve for y given x,z,a,b,c,d
						double a = face.vertex[0].nx;
						double b = face.vertex[0].ny;
						double c = face.vertex[0].nz;
						double d = -(a*face.position[0] + b*face.position[1] + c*face.position[2]);
						double curr_y = -(d + a*(x-dx) + c*(z-dz)) / b;
						//get the y, given face.
						double old_y = y;
						double new_y = -(d + a*x + c*z) / b;
						double delta_y = new_y - curr_y;

						std::cout << curr_y << " " << new_y << "\n";
						y += delta_y;
						if (bsp_1::collide_r(bsp_map, x, y, z, settings::player_radius, settings::player_height, bsp_1::TYPE::VERTICAL, face))
						{
							double angle_2 = 180.0 * acos(abs(face.normal[1])) / PI;
							if (angle_2 > 89)
							{
								//only if you hit a vertical wall now.
								y -= delta_y;
								x -= dx;
								z -= dz;
							}
						}
					}
				}
				else
				{
					y += settings::step_up;
					if (bsp_1::collide_r(bsp_map, x, y, z, settings::player_radius, settings::player_height, bsp_1::TYPE::VERTICAL, face))
					{
						double nx = face.vertex[0].nx;
						double nz = face.vertex[0].nz;
						double wall_theta = atan2(nx, nz);

						double value = dx*nz - dz*nx;
						double r90 = 90.0 * PI / 180.0;

						double dx_0 = value * sin(wall_theta + r90);
						double dz_0 = value * cos(wall_theta + r90);

						//check dot product to determine if you are really pointing into the face. 
						//Don't stop if you are behind it.
						y -= settings::step_up;
						if (nx*dx + nz*dz > 0)
						{
							//	x -= dx;
							//	z -= dz;
							double theta = atan2(dx, dz);
							mesh::face_c other_face;

							float cx_0 = x + sin(theta)*radius;
							float cz_0 = z + cos(theta)*radius;
							if (bsp_1::collide_n(bsp_map, x, y, z, cx_0, y, cz_0, settings::player_radius, settings::player_height, bsp_1::TYPE::VERTICAL, face, other_face))
							{
								double nx_ = other_face.vertex[0].nx;
								double nz_ = other_face.vertex[0].nz;
								double wall_theta = atan2(nx_, nz_);

								double value = dx*nz_ - dz*nx_;
								double dx_0_ = value * sin(wall_theta + r90);
								double dz_0_ = value * cos(wall_theta + r90);
								x += dx_0_;
								z += dz_0_;
								cx_0 += dx_0_;
								cz_0 += dz_0_;
								if (bsp_1::collide_n(bsp_map, x, y, z, cx_0, y, cz_0, settings::player_radius, settings::player_height, bsp_1::TYPE::VERTICAL, face, other_face))
								{
									x -= dx_0_;
									z -= dz_0_;
								}
							}
							/*
							float cx_0 = x + sin(theta + r90)*radius;
							float cz_0 = z + cos(theta + r90)*radius;

							float cx_1 = x + sin(theta - r90)*radius;
							float cz_1 = z + cos(theta - r90)*radius;
							if (bsp_1::collide_n(bsp_map, x, y, z, cx_0, y, cz_0, settings::player_radius, settings::player_height, bsp_1::TYPE::VERTICAL, face, other_face))
							{
							double nx_ = other_face.vertex[0].nx;
							double nz_ = other_face.vertex[0].nz;
							double wall_theta = atan2(nx_, nz_);

							double value = dx*nz_ - dz*nx_;
							double dx_0_ = value * sin(wall_theta + r90);
							double dz_0_ = value * cos(wall_theta + r90);
							x += dx_0_;
							z += dz_0_;
							if (bsp_1::collide_n(bsp_map, x, y, z, settings::player_radius, settings::player_height, bsp_1::TYPE::VERTICAL, face, other_face))
							{
							x -= dx_0_;
							z -= dz_0_;
							}
							}
							else if (bsp_1::collide_n(bsp_map, x, y, z, cx_1, y, cz_1, settings::player_radius, settings::player_height, bsp_1::TYPE::VERTICAL, face, other_face))
							{
							double nx_ = other_face.vertex[0].nx;
							double nz_ = other_face.vertex[0].nz;
							double wall_theta = atan2(nx_, nz_);

							double value = dx*nz_ - dz*nx_;
							double dx_0_ = value * sin(wall_theta + r90);
							double dz_0_ = value * cos(wall_theta + r90);
							x += dx_0_;
							z += dz_0_;
							if (bsp_1::collide_n(bsp_map, x, y, z, settings::player_radius, settings::player_height, bsp_1::TYPE::VERTICAL, face, other_face))
							{
							x -= dx_0_;
							z -= dz_0_;
							}
							}	* /
						}
						else
						{
							x -= dx;
							z -= dz;
							x += dx_0;
							z += dz_0;
							if (bsp_1::collide_r(bsp_map, x, y, z, settings::player_radius, settings::player_height, bsp_1::TYPE::VERTICAL, face))
							{
								x -= dx_0;
								z -= dz_0;
							}
						}
					}
				}
			}
		}
	}
	*/

	double dot(mesh::face_c face1, mesh::face_c face2)
	{
		return face1.vertex[0].nx * face2.vertex[0].nx + face1.vertex[0].ny * face2.vertex[0].ny + face1.vertex[0].nz * face2.vertex[0].nz;
	}

	mesh::face_c get_most_negative_dot(std::vector<mesh::face_c> faces, float x, float y, float z)
	{
		mesh::face_c face = faces[0];
		double most_negative_dot = x*faces[0].vertex[0].nx + y*faces[0].vertex[0].ny + z*faces[0].vertex[0].nz;
		for (int i = 1; i < faces.size(); i++)
		{
			//what is the best face? most negative dot product? sure.
			//calculate dot
			double dot = x*faces[i].vertex[0].nx + y*faces[i].vertex[0].ny + z*faces[i].vertex[0].nz;
			if (dot < most_negative_dot)
			{
				most_negative_dot = dot;
				face = faces[i];
			}
		}
		return face;
	}

	void moveA(bsp_1::node *bsp_map_horz, bsp_1::node *bsp_map_vert)
	{
		bsp_1::node *bsp_map = bsp_map_horz;
	//	bsp_map_vert = bsp_map_horz;
		//does the collider movement with the bsp...
		// and entity list. BUT how it interact with entities?...
		double eff_y_vel = y_vel / settings::hit_resolution;
		double eff_x_vel = x_vel / settings::hit_resolution;
		double eff_z_vel = z_vel / settings::hit_resolution;
		if (abs(y_vel) < settings::max_y_vel)
			y_vel -= settings::gravity*delta_time;

		if (to_move_up > 0 && false) 
		{
			to_move_up -= settings::step_speed*delta_time;
			if (to_move_up <= 0)
			{
				to_move_up = 0;
				last_move_up_face = mesh::face_c();
			}
			y += settings::step_speed*delta_time;
			//if (y > up_y) y = up_y;

		}
	
		bool touched_floor = false;
		
		double d_y = eff_y_vel*delta_time;
		for (int b = 0; b < settings::hit_resolution; b++)
		{
			y += d_y;
			//Horizontal collision walls.
			std::vector<mesh::face_c> horz_faces = bsp_1::get_collide_faces(bsp_map_horz, x, y, z, radius, height, bsp_1::TYPE::HORIZONTAL, 0, 0, 0);
			if (horz_faces.size() > 0)
			{
				//we need to assess the faces... to see if they are all in the same direction.
				double ny = horz_faces[0].vertex[0].ny;
				bool diff = false;
				for (int i = 1; i < horz_faces.size(); i++)
				{
					if (signum(ny) != signum(horz_faces[i].vertex[0].ny))
					{
						//So a face has a different directions...
						diff = true;
					}
				}



				if (true) // If any face is different, then you're probably not hitting the horz properly anyway. Like the side of a floating plat.
				{
					//	y -= eff_y_vel*delta_time;
					if (y_vel <= 0)
					{
						has_jump = true;
					}
					y -= d_y;
					y_vel = 0;
					touched_floor = true;
				//	did_wall_jump = false;
					has_wall_jump = true;
					//frictions...
					if (!is_moving)
					{
						double curr_mag = sqrt(x_vel*x_vel + z_vel*z_vel);
						double alt_mag = curr_mag - settings::friction_ground_accel*delta_time;
						if (alt_mag <= 0)
						{
							x_vel = 0;
							z_vel = 0;
						}
						else
						{
							x_vel = alt_mag * (x_vel / curr_mag);
							z_vel = alt_mag * (z_vel / curr_mag);
						}
					}

					//x_vel -= settings::friction_ground_accel
				}
			}
		}
		//std::cout << to_move_up << "\n";
		
		if (!touched_floor)
		{
			if (!is_moving)
			{
				double curr_mag = sqrt(x_vel*x_vel + z_vel*z_vel);
				double alt_mag = curr_mag - settings::friction_air_accel*delta_time;
				if (alt_mag <= 0)
				{
					x_vel = 0;
					z_vel = 0;
				}
				else
				{
					x_vel = alt_mag * (x_vel / curr_mag);
					z_vel = alt_mag * (z_vel / curr_mag);
				}
			}
		}
		



		eff_x_vel = x_vel / settings::hit_resolution;
		eff_z_vel = z_vel / settings::hit_resolution;
		for (int b = 0; b < settings::hit_resolution; b++)
		{
			double x_temp = x_vel;
			double z_temp = z_vel;

			x += eff_x_vel*delta_time;
			z += eff_z_vel*delta_time;
			double dx = eff_x_vel*delta_time;
			double dz = eff_z_vel*delta_time;

			if ((dx != 0 || dz != 0) || true)
			{
				std::vector<mesh::face_c> vert_faces = bsp_1::get_collide_faces(bsp_map_vert, x, y, z, radius, height, bsp_1::TYPE::VERTICAL, dx, 0, dz);
				if(vert_faces.size() > 0)
				{
					//do a quick check to see if it is TYPE::SEMI?
					//now we pick the face that seems best...   idk. 
					mesh::face_c face = get_most_negative_dot(vert_faces, dx, 0, dz);
					//now that we have the best face...    just regular hit stuff
					if (dot(face, last_vert_wall) != 1)
					{
						
					}
					last_vert_wall = face;
					//check if the face is TYPE::SEMI
					double angle = 180.0 * acos(abs(face.normal[1])) / PI;
					if (angle > 0.01 && angle < 89) // it is TYPE::SEMI
					{
						if (angle > settings::max_walking_angle)
						{
							//It's basically a wall.
							x -= dx;
							z -= dz;
						}
						else
						{
							//solve for y given x,z,a,b,c,d
							double a = face.vertex[0].nx;
							double b = face.vertex[0].ny;
							double c = face.vertex[0].nz;
							double d = -(a*face.position[0] + b*face.position[1] + c*face.position[2]);
							double curr_y = -(d + a*(x - dx) + c*(z - dz)) / b;
							//get the y, given face.
							double old_y = y;
							double new_y = -(d + a*x + c*z) / b;
							double delta_y = new_y - curr_y;

							y += delta_y;
							std::vector<mesh::face_c> after_collide = bsp_1::get_collide_faces(bsp_map_vert, x, y, z, radius, height, bsp_1::TYPE::VERTICAL, face);
							//This is the vertical hit 						
							if (after_collide.size() > 0)
							{
							//	std::cout << after_collide.size() << "poom\n";
								//we have a hit... 
								//check if any are not the floor we currently are on.
								for (int i = 0; i < after_collide.size(); i++)
								{
									//oooh. We are hitting something else...
									mesh::face_c n_face = get_most_negative_dot(after_collide, dx, 0, dz);
									//hit a wall. Go back.
									double nx = n_face.vertex[0].nx;
									double nz = n_face.vertex[0].nz;
									double wall_theta = atan2(nx, nz);

									double value = dx*nz - dz*nx;
									double r90 = 90.0 * PI / 180.0;

									double dx_0 = value * sin(wall_theta + r90);
									double dz_0 = value * cos(wall_theta + r90);

									//check if we are behind the wall
									if (dx*n_face.vertex[0].nx + dz*n_face.vertex[0].nz > 0)
									{
										//We are behind it
										//do nothing
									}
									else
									{
										//So this is a real wall. Move along it.
										x -= dx;
										z -= dz;
										x += dx_0;
										z += dz_0;
										if (bsp_1::get_collide_faces(bsp_map_vert, x, y, z, radius, height, bsp_1::TYPE::VERTICAL, face).size() > 0)
										{
											x -= dx_0;
											z -= dz_0;
										}
									}
									break;
								}
							}
						}
					}
					else
					{
						//But this is a real life wall.
						//now we step up and see again.
						
						//we need to do this more casually. Jumping straight up is lame.
						
						double step = settings::step_up;// / settings::hit_resolution;
						//check face's top and see if it would be within max step.
						double face_y = fmax(face.vertex[0].y, fmax(face.vertex[1].y, face.vertex[2].y));
					//	double old_y = y;
					//	if ((face_y + height) - (y) <= settings::step_up)
						//	y = face_y + height + 0.01;

						//y += step;
					//	std::vector<mesh::face_c> after_collide = bsp_1::get_collide_faces(bsp_map_vert, x, y, z, radius, height, bsp_1::TYPE::VERTICAL, dx, 0, dz);
						std::vector<mesh::face_c> after_collide = bsp_1::get_collide_faces(bsp_map_vert, x, face_y + height + 0.01, z, radius, height, bsp_1::TYPE::VERTICAL, 0, 0, 0);
						if ((face_y + height) - (y) <= settings::step_up && after_collide.size() == 0)
						{
							if (last_move_up_face != face)
							{
								if (face.vertex[0].ny > 0)
								{
									to_move_up += (face_y + height) - (y)+0.08;
									last_move_up_face = face;
								}
							}
							//	up_y = (face_y + height);
						}
						else
//						if (after_collide.size() > 0)
						{
							//if you're still hitting. Just move along the wall;
							//mesh::face_c face = get_most_negative_dot(vert_faces, dx, 0, dz);
						//	y = old_y;
							double nx = face.vertex[0].nx;
							double nz = face.vertex[0].nz;
							double wall_theta = atan2(nx, nz);

							double value = dx*nz - dz*nx;
							double r90 = 90.0 * PI / 180.0;

							double dx_0 = value * sin(wall_theta + r90);
							double dz_0 = value * cos(wall_theta + r90);
							x_temp = dx_0 / delta_time;
							z_temp = dz_0 / delta_time;

							//check if we are behind the wall
							if (dx*face.vertex[0].nx + dz*face.vertex[0].nz > 0)
							{
								//We are behind it
								//do nothing
							}
							else
							{
								//So this is a real wall. Move along it.
								x -= dx;
								z -= dz;
								x += dx_0;
								z += dz_0;
								if (bsp_1::get_collide_faces(bsp_map_vert, x, y, z, radius, height, bsp_1::TYPE::VERTICAL, dx_0, 0, dz_0).size() > 0)
								{
									x -= dx_0;
									z -= dz_0;
								}
								else
								{
									if (!touched_floor)
									{
										y -= d_y*0.6;
										if (has_wall_jump)
										{
											has_wall_jump = false;
											has_jump = true;
										}
									}
									//y_vel = 0;
								}
							}
						}
					}
				}
				else
				{
					if (!touched_floor)
					{
						//has_wall_jump = true;
						//std::cout << "yabba yabba doo\n";
					}
				}
			}
			x_vel = x_temp;
			z_vel = z_temp;
		}
	}

	void move2(bsp_1::node *bsp_map_horz, bsp_1::node *bsp_map_vert)
	{
		bsp_1::node *bsp_map = bsp_map_horz;
		double eff_y_vel = y_vel / settings::hit_resolution;
		double eff_x_vel = x_vel / settings::hit_resolution;
		double eff_z_vel = z_vel / settings::hit_resolution;


		double d_y = eff_y_vel*delta_time;
		for (int b = 0; b < settings::hit_resolution; b++)
		{
			y += d_y;
			//Horizontal collision walls.
			std::vector<mesh::face_c> horz_faces = bsp_1::get_collide_faces(bsp_map_horz, x, y, z, radius, height, bsp_1::TYPE::HORIZONTAL, 0, 0, 0);
			if (horz_faces.size() > 0)
			{
				//we need to assess the faces... to see if they are all in the same direction.
				double ny = horz_faces[0].vertex[0].ny;			
				//std::cout << y_vel << " " << ny << "\n";
				if (signum(y_vel) != signum(ny)  || true)
				{
					if (y_vel < 0)
					{
						has_jump = true;
					}

					y -= d_y;
					y_vel = 0;
				}
			}
		}

		for (int b = 0; b < settings::hit_resolution; b++)
		{
			double x_temp = x_vel;
			double z_temp = z_vel;

			x += eff_x_vel*delta_time;
			z += eff_z_vel*delta_time;
			double dx = eff_x_vel*delta_time;
			double dz = eff_z_vel*delta_time;

			if (dx != 0 || dz != 0 || true)
			{
				//First see if we are hitting any horizontal. This is for stepping up.
				//	We shouldn't be hitting the ground because that was filtered out
				
				std::vector<mesh::face_c> horz_faces = bsp_1::get_collide_faces(bsp_map_horz, x, y, z, radius, height, bsp_1::TYPE::HORIZONTAL, 0, 0, 0);
				if (horz_faces.size() > 0)
				{
					//grab first one.
					mesh::face_c h_face = horz_faces[0];
					if ( h_face.vertex[0].ny > 0 && y - height + settings::step_up > h_face.position[1])
					{
						//if stepping up would make you above it.
						if (bsp_1::get_collide_faces(bsp_map_horz, x, h_face.position[1] + height + settings::epsilon, z, radius, height, bsp_1::TYPE::HORIZONTAL, 0, 0, 0).size() == 0)
						{
							y = h_face.position[1] + height + settings::epsilon;//settings::step_up;
							//std::cout << "step up, bro \n";
						}
					}
				}

				std::vector<mesh::face_c> vert_faces = bsp_1::get_collide_faces(bsp_map_vert, x, y, z, radius, height, bsp_1::TYPE::VERTICAL, dx, 0, dz);
				if (vert_faces.size() > 0)
				{
					//do a quick check to see if it is TYPE::SEMI?   No SEMI's anymore..  just forget those kids.
					//now we pick the face that seems best...   idk. 
					mesh::face_c face = get_most_negative_dot(vert_faces, dx, 0, dz);
					//now that we have the best face...    just regular hit stuff
					
					//But this is a real life wall.
					//now we step up and see again.

					//we need to do this more casually. Jumping straight up is lame. NOT FOR NOW

					//if you're still hitting. Just move along the wall;

					double nx = face.vertex[0].nx;
					double nz = face.vertex[0].nz;
					double wall_theta = atan2(nx, nz);

					double value = dx*nz - dz*nx;
					double r90 = 90.0 * PI / 180.0;

					double dx_0 = value * sin(wall_theta + r90);
					double dz_0 = value * cos(wall_theta + r90);
					x_temp = dx_0 / delta_time;
					z_temp = dz_0 / delta_time;

					//check if we are behind the wall
					if (dx*face.vertex[0].nx + dz*face.vertex[0].nz > 0)
					{
						//We are behind it
						//do nothing
					}
					else
					{
						//So this is a real wall. Move along it.
						x -= dx;
						z -= dz;
						x += dx_0;
						z += dz_0;
						if (bsp_1::get_collide_faces(bsp_map_vert, x, y, z, radius, height, bsp_1::TYPE::VERTICAL, dx_0, 0, dz_0).size() > 0)
						{
							x -= dx_0;
							z -= dz_0;
						}
					}
				}
			}

			/*
			double x_temp = x_vel;
			double z_temp = z_vel;

			x += eff_x_vel*delta_time;
			z += eff_z_vel*delta_time;
			double dx = eff_x_vel*delta_time;
			double dz = eff_z_vel*delta_time;

			if ((dx != 0 || dz != 0) || true)
			{
				std::vector<mesh::face_c> vert_faces = bsp_1::get_collide_faces(bsp_map_vert, x, y, z, radius, height, bsp_1::TYPE::VERTICAL, dx, 0, dz);
				if (vert_faces.size() > 0)
				{
					//do a quick check to see if it is TYPE::SEMI?
					//now we pick the face that seems best...   idk. 
					mesh::face_c face = get_most_negative_dot(vert_faces, dx, 0, dz);
					//now that we have the best face...    just regular hit stuff
					if (dot(face, last_vert_wall) != 1)
					{

					}
					last_vert_wall = face;
					//check if the face is TYPE::SEMI
					double angle = 180.0 * acos(abs(face.normal[1])) / PI;
					if (angle > 0.01 && angle < 89) // it is TYPE::SEMI
					{
						if (angle > settings::max_walking_angle)
						{
							//It's basically a wall.
							x -= dx;
							z -= dz;
						}
						else
						{
							//solve for y given x,z,a,b,c,d
							double a = face.vertex[0].nx;
							double b = face.vertex[0].ny;
							double c = face.vertex[0].nz;
							double d = -(a*face.position[0] + b*face.position[1] + c*face.position[2]);
							double curr_y = -(d + a*(x - dx) + c*(z - dz)) / b;
							//get the y, given face.
							double old_y = y;
							double new_y = -(d + a*x + c*z) / b;
							double delta_y = new_y - curr_y;

							y += delta_y;
							std::vector<mesh::face_c> after_collide = bsp_1::get_collide_faces(bsp_map_vert, x, y, z, radius, height, bsp_1::TYPE::VERTICAL, face);
							//This is the vertical hit 						
							if (after_collide.size() > 0)
							{
								//	std::cout << after_collide.size() << "poom\n";
								//we have a hit... 
								//check if any are not the floor we currently are on.
								for (int i = 0; i < after_collide.size(); i++)
								{
									//oooh. We are hitting something else...
									mesh::face_c n_face = get_most_negative_dot(after_collide, dx, 0, dz);
									//hit a wall. Go back.
									double nx = n_face.vertex[0].nx;
									double nz = n_face.vertex[0].nz;
									double wall_theta = atan2(nx, nz);

									double value = dx*nz - dz*nx;
									double r90 = 90.0 * PI / 180.0;

									double dx_0 = value * sin(wall_theta + r90);
									double dz_0 = value * cos(wall_theta + r90);

									//check if we are behind the wall
									if (dx*n_face.vertex[0].nx + dz*n_face.vertex[0].nz > 0)
									{
										//We are behind it
										//do nothing
									}
									else
									{
										//So this is a real wall. Move along it.
										x -= dx;
										z -= dz;
										x += dx_0;
										z += dz_0;
										if (bsp_1::get_collide_faces(bsp_map_vert, x, y, z, radius, height, bsp_1::TYPE::VERTICAL, face).size() > 0)
										{
											x -= dx_0;
											z -= dz_0;
										}
									}
									break;
								}
							}
						}
					}
					else
					{
						//But this is a real life wall.
						//now we step up and see again.

						//we need to do this more casually. Jumping straight up is lame.

						double step = settings::step_up;// / settings::hit_resolution;
														//check face's top and see if it would be within max step.
						double face_y = fmax(face.vertex[0].y, fmax(face.vertex[1].y, face.vertex[2].y));
						//	double old_y = y;
						//	if ((face_y + height) - (y) <= settings::step_up)
						//	y = face_y + height + 0.01;

						//y += step;
						//	std::vector<mesh::face_c> after_collide = bsp_1::get_collide_faces(bsp_map_vert, x, y, z, radius, height, bsp_1::TYPE::VERTICAL, dx, 0, dz);
						std::vector<mesh::face_c> after_collide = bsp_1::get_collide_faces(bsp_map_vert, x, face_y + height + 0.01, z, radius, height, bsp_1::TYPE::VERTICAL, 0, 0, 0);
						if ((face_y + height) - (y) <= settings::step_up && after_collide.size() == 0)
						{
							if (last_move_up_face != face)
							{
								if (face.vertex[0].ny > 0)
								{
									to_move_up += (face_y + height) - (y)+0.08;
									last_move_up_face = face;
								}
							}
							//	up_y = (face_y + height);
						}
						else
							//						if (after_collide.size() > 0)
						{
							//if you're still hitting. Just move along the wall;
							//mesh::face_c face = get_most_negative_dot(vert_faces, dx, 0, dz);
							//	y = old_y;
							double nx = face.vertex[0].nx;
							double nz = face.vertex[0].nz;
							double wall_theta = atan2(nx, nz);

							double value = dx*nz - dz*nx;
							double r90 = 90.0 * PI / 180.0;

							double dx_0 = value * sin(wall_theta + r90);
							double dz_0 = value * cos(wall_theta + r90);
							x_temp = dx_0 / delta_time;
							z_temp = dz_0 / delta_time;

							//check if we are behind the wall
							if (dx*face.vertex[0].nx + dz*face.vertex[0].nz > 0)
							{
								//We are behind it
								//do nothing
							}
							else
							{
								//So this is a real wall. Move along it.
								x -= dx;
								z -= dz;
								x += dx_0;
								z += dz_0;
								if (bsp_1::get_collide_faces(bsp_map_vert, x, y, z, radius, height, bsp_1::TYPE::VERTICAL, dx_0, 0, dz_0).size() > 0)
								{
									x -= dx_0;
									z -= dz_0;
								}
								else
								{
									if (!true)
									{
										y -= d_y*0.6;
										if (has_wall_jump)
										{
											has_wall_jump = false;
											has_jump = true;
										}
									}
									//y_vel = 0;
								}
							}
						}
					}
				}
				else
				{
					if (!true)
					{
						//has_wall_jump = true;
						//std::cout << "yabba yabba doo\n";
					}
				}
			}
			x_vel = x_temp;
			z_vel = z_temp;
			*/
		}
	}

	std::vector<mesh::face_c> move3(bsp_1::node *bsp_map_horz, bsp_1::node *bsp_map_vert)
	{
		std::vector<mesh::face_c> out_faces;
		bsp_1::node *bsp_map = bsp_map_horz;
		double eff_y_vel = y_vel / settings::hit_resolution;
		double eff_x_vel = x_vel / settings::hit_resolution;
		double eff_z_vel = z_vel / settings::hit_resolution;


		double d_y = eff_y_vel*delta_time;
		for (int b = 0; b < settings::hit_resolution; b++)
		{
			y += d_y;
			//Horizontal collision walls.
			std::vector<mesh::face_c> horz_faces = bsp_1::get_collide_faces(bsp_map_horz, x, y, z, radius, height, bsp_1::TYPE::HORIZONTAL, 0, 0, 0);
			if (horz_faces.size() > 0)
			{
				//we need to assess the faces... to see if they are all in the same direction.
				double ny = horz_faces[0].vertex[0].ny;
				//std::cout << y_vel << " " << ny << "\n";
				if (signum(y_vel) != signum(ny) || true)
				{
					if (y_vel < 0)
					{
						has_jump = true;
					}
					out_faces.push_back(horz_faces[0]);
					y -= d_y;
					y_vel = 0;
				}
			}
		}

		for (int b = 0; b < settings::hit_resolution; b++)
		{
			double x_temp = x_vel;
			double z_temp = z_vel;

			x += eff_x_vel*delta_time;
			z += eff_z_vel*delta_time;
			double dx = eff_x_vel*delta_time;
			double dz = eff_z_vel*delta_time;

			if (dx != 0 || dz != 0 || true)
			{
				//First see if we are hitting any horizontal. This is for stepping up.
				//	We shouldn't be hitting the ground because that was filtered out

				std::vector<mesh::face_c> horz_faces = bsp_1::get_collide_faces(bsp_map_horz, x, y, z, radius, height, bsp_1::TYPE::HORIZONTAL, 0, 0, 0);
				if (horz_faces.size() > 0)
				{
					//grab first one.
					mesh::face_c h_face = horz_faces[0];
					if (h_face.vertex[0].ny > 0 && y - height + settings::step_up > h_face.position[1])
					{
						//if stepping up would make you above it.
						if (bsp_1::get_collide_faces(bsp_map_horz, x, h_face.position[1] + height + settings::epsilon, z, radius, height, bsp_1::TYPE::HORIZONTAL, 0, 0, 0).size() == 0)
						{
							y = h_face.position[1] + height + settings::epsilon;//settings::step_up;
																				//std::cout << "step up, bro \n";
							out_faces.push_back(h_face);
						}
					}
				}

				std::vector<mesh::face_c> vert_faces = bsp_1::get_collide_faces(bsp_map_vert, x, y, z, radius, height, bsp_1::TYPE::VERTICAL, dx, 0, dz);
				if (vert_faces.size() > 0)
				{
					//do a quick check to see if it is TYPE::SEMI?   No SEMI's anymore..  just forget those kids.
					//now we pick the face that seems best...   idk. 
					mesh::face_c face = get_most_negative_dot(vert_faces, dx, 0, dz);
					//now that we have the best face...    just regular hit stuff

					//But this is a real life wall.
					//now we step up and see again.

					//we need to do this more casually. Jumping straight up is lame. NOT FOR NOW

					//if you're still hitting. Just move along the wall;

					double nx = face.vertex[0].nx;
					double nz = face.vertex[0].nz;
					double wall_theta = atan2(nx, nz);

					double value = dx*nz - dz*nx;
					double r90 = 90.0 * PI / 180.0;

					double dx_0 = value * sin(wall_theta + r90);
					double dz_0 = value * cos(wall_theta + r90);
					x_temp = dx_0 / delta_time;
					z_temp = dz_0 / delta_time;

					//check if we are behind the wall
					if (dx*face.vertex[0].nx + dz*face.vertex[0].nz > 0)
					{
						//We are behind it
						//do nothing
					}
					else
					{
						//So this is a real wall. Move along it.
						x -= dx;
						z -= dz;
						x += dx_0;
						z += dz_0;
						out_faces.push_back(face);
						if (bsp_1::get_collide_faces(bsp_map_vert, x, y, z, radius, height, bsp_1::TYPE::VERTICAL, dx_0, 0, dz_0).size() > 0)
						{
							x -= dx_0;
							z -= dz_0;
						}
					}
				}
			}
		}
		return out_faces;
	}
	//the real movement are determinded by the other structs. like player and enemy. That's where the x_vel stuff is. 

	std::vector<mesh::face_c> move4(bsp_1::node *bsp_map)
	{
		std::vector<mesh::face_c> out_faces;
		double eff_y_vel = (y_vel / settings::hit_resolution);
		double eff_x_vel = (x_vel / settings::hit_resolution);
		double eff_z_vel = (z_vel / settings::hit_resolution);

		double max_ny = sin(settings::max_incline*TO_RAD);

		//FOR COLLISIONS:
		// If this doesn't work right,
		// Do three orthagonal projections and see if projected circle interescts. I think that's valid.
		// Could adjust the radius if approriate.



		for (int b = 0; b < settings::hit_resolution; b++)
		{
			double x_temp = x_vel;
			double y_temp = y_vel;
			double z_temp = z_vel;

			x += eff_x_vel*delta_time;
			y += eff_y_vel*delta_time;
			z += eff_z_vel*delta_time;
			double dx = eff_x_vel*delta_time;
			double dy = eff_y_vel*delta_time;
			double dz = eff_z_vel*delta_time;

			if ((dx != 0 || dy != 0 || dz != 0 || true))
			{
				//First see if we are hitting any horizontal. This is for stepping up.
				//	We shouldn't be hitting the ground because that was filtered out

				std::vector<mesh::face_c> faces = bsp_1::get_collide_faces_arbit(bsp_map, x, y, z, radius, dx, dy, dz);
				if (faces.size() > 0)
				{
					//do a quick check to see if it is TYPE::SEMI?   No SEMI's anymore..  just forget those kids.
					//now we pick the face that seems best...   idk. 
					mesh::face_c face = get_most_negative_dot(faces, dx, dy, dz);
					//now that we have the best face...    just regular hit stuff

					//But this is a real life wall.
					//now we step up and see again.

					//we need to do this more casually. Jumping straight up is lame. NOT FOR NOW

					//if you're still hitting. Just move along the wall;

					double nx = face.vertex[0].nx;
					double ny = face.vertex[0].ny;
					double nz = face.vertex[0].nz;

					if (ny > max_ny)
					{
						has_jump = true;
						
						y_vel = 0;
					}
					else
					{
						y -= dy;
					}


					double normal_proj_mag = dx * nx + dy * ny + dz * nz;
					double np_x = normal_proj_mag * nx;
					double np_y = normal_proj_mag * ny;
					double np_z = normal_proj_mag * nz;

					double p_x = dx - np_x;
					double p_y = dy - np_y;
					double p_z = dz - np_z;

					if (dx*nx  + dy*ny + dz*nz > 0)
					{
						//We are behind it
						//do nothing
					}
					else
					{
						//So this is a real wall. Move along it.

						//set back epsilon
						x += nx * settings::epsilon;
						y += ny * settings::epsilon;
						z += nz * settings::epsilon;

						x -= dx;
						y -= dy;
						z -= dz;
						x += p_x;
						y += p_y;
						z += p_z;
						
						out_faces.push_back(face);
						if (bsp_1::get_collide_faces_arbit(bsp_map, x, y, z, radius, dx, dy, dz).size() > 0)
						{
							//test step up

							x -= p_x;
							y -= p_y;
							z -= p_z;
						//	return out_faces;
						}
					}

					
					/*

					x -= dx;
					y -= dy;
					z -= dz;
					return out_faces;

					double nx = face.vertex[0].nx;
					double ny = face.vertex[0].ny;
					double nz = face.vertex[0].nz;
					double wall_theta = atan2(nx, nz);
					double wall_phi = asin(ny / sqrt(nx*nx+nz*nz));

					double value = dx*nz - dz*nx;
					double r90 = 90.0 * PI / 180.0;

					double dx_0 = value * sin(wall_theta + r90);
					double dz_0 = value * cos(wall_theta + r90);
					x_temp = dx_0 / delta_time;
					z_temp = dz_0 / delta_time;

					//check if we are behind the wall
					if (dx*face.vertex[0].nx + dz*face.vertex[0].nz > 0)
					{
						//We are behind it
						//do nothing
					}
					else
					{
						//So this is a real wall. Move along it.
						x -= dx;
						z -= dz;
						x += dx_0;
						z += dz_0;
						out_faces.push_back(face);
						if (bsp_1::get_collide_faces_arbit(bsp_map, x, y, z, radius, dx, dy, dz).size() > 0)
						{
							x -= dx_0;
							z -= dz_0;
						}
					}
					*/
				}
			}
		}
		
		return out_faces;
	}

	bool bullet_collide(double x_, double y_, double z_, double radius_)
	{
		//check bounding box
		if ((x_ + radius_ > x - radius && x_ - radius_ < x + radius) && (z_ + radius_ > z - radius && z_ - radius_ < z + radius) &&
			(y_ + radius_ > y - height && y_ - radius_ < y + height))
		{
			if ((x_ - x)*(x_ - x) + (z_ - z)*(z_ - z) <= (radius_ + radius)*(radius_ + radius))
			{
				return true;
			}
		}
		return false;
	}

	void receive_damage(double damage, entity * inflictor)
	{
		agro_at = inflictor;
		health -= damage;
	}

	/*
	void set_info_packet( bsp_1::node * horz_, bsp_1::node * vert_, std::vector<entity*> * entities_, std::vector<mesh::Mesh_c *> * meshes_, std::vector<Object *> * objects_)
	{
		info->horz = horz_;
		info->vert = vert_;
		info->entities = entities_;
		info->meshes = meshes_;
		info->objects = objects_;
	}*/

	GLfloat x, y, z;
	GLfloat x_vel, y_vel, z_vel;
	GLfloat azimuth, pitch, roll;
	std::vector<std::vector<mesh::Mesh_g *>> display_mesh;
	GLfloat radius, height;		//assumes cylinder collider
	int animation;
	int frame;
	bool has_jump = true;
	double to_move_up = 0.0;
	mesh::face_c last_move_up_face;
	mesh::face_c last_vert_wall;
	double up_y = 0.0;
	bool has_wall_jump = true;
	bool did_wall_jump = false;
	bool is_moving = false;
	double animation_time = 0.0;
	mesh::Mesh_c * mesh_;
	entity * agro_at = NULL;
	std::vector<mesh::Mesh_c *> frames;
	double health;
//	info_packet * info;
	//consider making the meshes be stored as a pointer to a pointer so we can do shit with them.
};

struct player : entity
{
	player() {};
	
	player(GLfloat x_, GLfloat y_, GLfloat z_, GLfloat radius_, GLfloat height_)
	{
		x = x_;
		y = y_;
		z = z_;
		radius = radius_;
		height = height_;
		x_vel = 0.f;
		y_vel = 0.f;
		z_vel = 0.f;
		pitch = 0;
		azimuth = 0;
		roll = 0;
		standing_frame = new mesh::Mesh_c();
		jumping_frame = new mesh::Mesh_c();
	}

	bool door_collide(Door * door)
	{
		if (x + radius > door->lx && x - radius < door->hx &&
			y + height > door->ly && y - height < door->hy &&
			z + radius > door->lz && z - radius < door->hz)
		{
		
			return true;
		}
		return false;
	}

	void key_mouse_move(camera_pos &cam, bsp_1::node *horz_map, bsp_1::node *vert_map, std::vector<Object *> &objects)
	{
		bool u_key = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W);
		bool d_key = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S);
		bool r_key = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D);
		bool l_key = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A);
		bool crouch_key = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl);
		bool jump_key = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space);
		double x_vel_rel = 0.f;
		double z_vel_rel = 0.f;

		double speed = settings::player_speed;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
		{
			if (settings::dev_mode)
				speed = settings::dev_speed_sprint;
			else
				speed = settings::splayer_speed_sprint;
		}

		double goal_roll = 0.0;

		if (r_key == l_key)
		{
			x_vel_rel = 0.f;
		}
		else if (r_key)
		{
			x_vel_rel = speed;
			goal_roll = -settings::strafe_lean;
		}
		else if (l_key)
		{
			x_vel_rel = -speed;
			goal_roll = settings::strafe_lean;
		}


//		roll += (goal_roll - roll)*delta_time;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q) && sf::Keyboard::isKeyPressed(sf::Keyboard::E))
		{

		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
		{
		//	goal_roll = settings::lean_value;
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
		{
		//	goal_roll = -settings::lean_value;
		}



		if (goal_roll - roll > 0)
			roll += 40.0 * delta_time;
		else if (goal_roll - roll < 0)
			roll -= 40.0 * delta_time;
		if (fabs(goal_roll - roll) < 0.01)
			roll = goal_roll;


		if (u_key == d_key)
		{
			z_vel_rel = 0.f;
		}
		else if (u_key)
		{
			z_vel_rel = -speed;
		}
		else if (d_key)
		{
			z_vel_rel = speed;
		}


		if (d_key || u_key || l_key || r_key)
		{
			is_moving = true;
		}
		else
		{
			is_moving = false;
		}


		/*
		bool jumpin = false;
		if (jump_key && has_jump)
		{
			y_vel = settings::jump_speed;
			has_jump = false;
			jumpin = true;
		}


		if (settings::dev_mode)
		{
			x_vel = (x_vel_rel * cos(azimuth * PI / 180.0) + z_vel_rel * sin(azimuth * PI / 180.0) * cos(pitch * PI / 180.0));
			y_vel = (-z_vel_rel * sin(pitch * PI / 180.0));
			z_vel = (-x_vel_rel * sin(azimuth * PI / 180.0) + z_vel_rel * cos(azimuth * PI / 180.0) * cos(pitch * PI / 180.0));
		}
		else
		{


			//	if (has_jump == false && x_vel_rel != 0)
			//	{
			//		z_vel_rel = signum(z_vel_rel) * settings::player_speed_stafe_jump;
			//	}


			x_vel_con = x_vel_rel * cos(azimuth * PI / 180.0) + z_vel_rel * sin(azimuth * PI / 180.0);
			z_vel_con = -x_vel_rel * sin(azimuth * PI / 180.0) + z_vel_rel * cos(azimuth * PI / 180.0);

			//if(x_vel0)
		//	double mag = sqrt(x_vel0*x_vel0 + z_vel0*z_vel0);
	//		std::cout << mag << "\n";
		//	if (mag != 0)
		//	{
			//	double d_x = x_vel + settings::ground_accel * (x_vel0 / mag) * delta_time;
				//double d_z = z_vel + settings::ground_accel * (z_vel0 / mag) * delta_time;

			//	x_vel_con = x_vel0;
		//		z_vel_con = z_vel0;
//
				/*
				if (sqrt(d_x*d_x + d_z*d_z) > settings::player_speed)
				{
					//if you'll be over the speed limit.
					//see if you're slowing down
					if (sqrt(d_x*d_x + d_z*d_z) <= sqrt(x_vel*x_vel + z_vel*z_vel))
					{
						x_vel = d_x;
						z_vel = d_z;
					}
				}
				else
				{
					x_vel = d_x;
					z_vel = d_z;
				}
				* /

				if  (abs(d_x) > settings::player_speed)
				{
					//if you'll be over the speed limit.
					//see if you're slowing down
					if (abs(d_x) <= abs(x_vel))
					{
						x_vel = d_x;
					}
				}
				else
				{
					x_vel = d_x;
				}

				if (abs(d_z) > settings::player_speed)
				{
					//if you'll be over the speed limit.
					//see if you're slowing down
					if (abs(d_z) <= abs(z_vel))
					{
						z_vel = d_z;
					}
				}
				else
				{
					z_vel = d_z;
				}
				* /

				//	}

		}


		if (jumpin)
		{
			if (!has_wall_jump)
			{
				//if you are on a wall...
				//double x_factor = last_vert_wall.vertex[0].nx;
				//double z_factor = last_vert_wall.vertex[0].nz;
				//x and z should be normalized.
				x_vel += settings::wall_jump_speed * last_vert_wall.vertex[0].nx;
				z_vel += settings::wall_jump_speed * last_vert_wall.vertex[0].nz;
				if (abs(x_vel_con) + abs(z_vel_con) > 0)
				{
					double r = sqrt(x_vel_con*x_vel_con + z_vel_con*z_vel_con);
					double vel_w_j = last_vert_wall.vertex[0].nz * settings::wall_jump_speed * x_vel_con / r - last_vert_wall.vertex[0].nx * settings::wall_jump_speed * z_vel_con / r;;

					x_vel += vel_w_j * x_vel_con / r;
					z_vel += vel_w_j * z_vel_con / r;
				}

				//	std::cout << "spidy\n";
			}
		}
		//std::cout << x_vel << "    " << z_vel << "\n";

		/*
		if (!is_moving)
		{
			x_vel_oth = x_vel;
			z_vel_oth = z_vel;
		}
		else
		{
			x_vel = x_vel_con + x_vel_oth;
			z_vel = z_vel_con + z_vel_oth;
		}* /
		if (!is_moving)
		{

		}
		else
		{
			if (abs(x_vel - x_vel_con) <settings::zero_tolerance )
			{
				x_vel = x_vel_con;
			}
			else if (x_vel_con > 0)
			{
				if (x_vel >= x_vel_con)
				{
					//movin faster than you want.
					x_vel -= settings::friction_air_accel*delta_time;
				}
				else
				{
					x_vel += settings::ground_accel*delta_time;
				}
			}
			else
			{
				if (x_vel <= x_vel_con)
				{
					//movin faster than you want.
					x_vel += settings::friction_air_accel*delta_time;
				}
				else
				{
					x_vel -= settings::ground_accel*delta_time;
				}
			}

			if (abs(x_vel) < settings::zero_tolerance)
			{
				x_vel = 0;
			}


			if (abs(z_vel - z_vel_con) <settings::zero_tolerance)
			{
				z_vel = z_vel_con;
			}
			else if (z_vel_con > 0)
			{
				if (z_vel > z_vel_con)
				{
					//movin faster than you want.
					z_vel -= settings::friction_air_accel*delta_time;
				}
				else
				{
					z_vel += settings::ground_accel*delta_time;
				}
			}
			else
			{
				if (z_vel < z_vel_con)
				{
					//movin faster than you want.
					z_vel += settings::friction_air_accel*delta_time;
				}
				else
				{
					z_vel -= settings::ground_accel*delta_time;
				}
			}

			if (abs(z_vel) < settings::zero_tolerance)
			{
				z_vel = 0;
			}
		}
		/*
		if (x_vel > x_vel_con)
		{
			x_vel -= settings::ground_accel*delta_time;
		}
		if (x_vel < x_vel_con)
		{
			x_vel += settings::ground_accel*delta_time;
		}
		if (z_vel > z_vel_con)
		{
			z_vel -= settings::ground_accel*delta_time;
		}
		if (z_vel < z_vel_con)
		{
			z_vel += settings::ground_accel*delta_time;
		}
		* /
		*/



		if (settings::dev_mode)
		{
			x_vel = (x_vel_rel * cos(azimuth * PI / 180.0) + z_vel_rel * sin(azimuth * PI / 180.0) * cos(pitch * PI / 180.0));
			y_vel = (-z_vel_rel * sin(pitch * PI / 180.0));
			z_vel = (-x_vel_rel * sin(azimuth * PI / 180.0) + z_vel_rel * cos(azimuth * PI / 180.0) * cos(pitch * PI / 180.0));
		}
		else
		{
			y_vel -= settings::gravity * delta_time;
			if (jump_key && has_jump && on_ground)
			{
				y_vel = settings::jump_speed;
				has_jump = false;
			}


			x_vel = x_vel_rel * cos(azimuth * PI / 180.0) + z_vel_rel * sin(azimuth * PI / 180.0) + x_vel_con;
			//y_vel = (-z_vel_rel * sin(pitch * PI / 180.0));
			z_vel = -x_vel_rel * sin(azimuth * PI / 180.0) + z_vel_rel * cos(azimuth * PI / 180.0) + z_vel_con;

			if (x_vel_con != 0.0)
			{
				x_vel_con -= signum(x_vel_con) * settings::friction_air_accel * delta_time;
				if (fabs(x_vel_con) < 0.001)
					x_vel_con = 0.0;
			}
			if (z_vel_con != 0.0)
			{
				z_vel_con -= signum(z_vel_con) * settings::friction_air_accel * delta_time;
				if (fabs(z_vel_con) < 0.001)
					z_vel_con = 0.0;
			}

		}

		std::vector<mesh::face_c> col_faces;
		if (!pause)
		{
			if (settings::no_clip)
			{
				x += x_vel * delta_time;
				y += y_vel * delta_time + (jump_key?speed*delta_time:0.0);
				z += z_vel * delta_time;
			}
			else
			{
				col_faces = move3(horz_map, vert_map);
			}
		}


		on_ground = false;
		bool vert_collide = false;
		for (int i = 0; i < col_faces.size(); i++)
		{
			if (col_faces[i].vertex[0].ny == 0)
			{
				vert_collide = true;
			}
			if (col_faces[i].vertex[0].ny > 0)
			{
				on_ground = true;
			}
		}

		if (vert_collide)//&& !on_ground && y_vel != 0.0
		{
			if (y_vel < 0)
			{
				y_vel += settings::gravity * delta_time;
				y_vel -= settings::wall_cling_grav * delta_time;
				if (y_vel < settings::wall_cling_max_down)
					y_vel = -settings::wall_cling_max_down;
			}
			if (jump_key)
			{

				//x_vel = (x_vel_rel * cos(azimuth * PI / 180.0) + z_vel_rel * sin(azimuth * PI / 180.0) * cos(pitch * PI / 180.0));
				//y_vel = (-z_vel_rel * sin(pitch * PI / 180.0));
				//z_vel = (-x_vel_rel * sin(azimuth * PI / 180.0) + z_vel_rel * cos(azimuth * PI / 180.0) * cos(pitch * PI / 180.0));
				
				x_vel_con = -settings::wall_jump_speed * sin(azimuth * PI / 180.0) * cos(pitch * PI / 180.0);
				y_vel = settings::wall_jump_speed * sin(pitch * PI / 180.0);
				z_vel_con = -settings::wall_jump_speed * cos(azimuth * PI / 180.0) * cos(pitch * PI / 180.0);

			}
			else
			{
				x_vel_con = 0;// -x_vel_con;
				z_vel_con = 0;// -z_vel_con;
			}
		}

		if (settings::third_person)
		{
			//for third person..
			//we need to look down on the character
			//so we chose the relative angles, then we add the player angles, then we back up the radius.
			//good enough.
			animation_time += delta_time;
			if (animation_time >= settings::animation_delay)
			{
				animation_time = 0.0;
				animation = (animation + 1) % frames.size();
			}

		//	*mesh_ = *frames[animation];
			bool fall = true;
			for (int i = 0; i < col_faces.size(); i++)
			{
				if (col_faces[i].vertex[0].ny != 0)
				{
					fall = false;
				}
			}
			if (fall)
			{
				*mesh_ = *jumping_frame;
			}
			else
			{
				if (is_moving)
					*mesh_ = *frames[animation];
				else
					*mesh_ = *standing_frame;
			}

			
		

			cam.azimuth = azimuth + settings::third_person_pan;
			cam.pitch = pitch - settings::third_person_declination;
			cam.roll = roll;

			double delta_x = settings::third_person_radius * sin(cam.azimuth * PI / 180.0) * cos(cam.pitch * PI / 180.0);
			double delta_y = -settings::third_person_radius * sin(cam.pitch * PI / 180.0);
			double delta_z = settings::third_person_radius * cos(cam.azimuth * PI / 180.0) * cos(cam.pitch * PI / 180.0);


			if (settings::third_person_camera_collision)
			{
				double d_x_eff = delta_x / 10.0;
				double d_y_eff = delta_y / 10.0;
				double d_z_eff = delta_z / 10.0;
				cam.x = x;
				cam.y = y;
				cam.z = z;
				for (int b = 0; b < 10; b++)
				{
					//we get both wall_collisions..
					std::vector<mesh::face_c> horz_faces = bsp_1::get_collide_faces(horz_map, cam.x, cam.y, cam.z, 0.02, 0.01, bsp_1::TYPE::HORIZONTAL, 0, 0, 0);
					std::vector<mesh::face_c> vert_faces = bsp_1::get_collide_faces(vert_map, cam.x, cam.y, cam.z, 0.02, 0.01, bsp_1::TYPE::VERTICAL, 0, 0, 0);
				
					if (horz_faces.size() > 0 || vert_faces.size() > 0)
					{
						cam.x -= d_x_eff;
						cam.y -= d_y_eff;
						cam.z -= d_z_eff;
						break;
					}
					cam.x += d_x_eff;
					cam.y += d_y_eff;
					cam.z += d_z_eff;
				}
			}
			else
			{
				cam.x = x + delta_x;
				cam.y = y + delta_y;
				cam.z = z + delta_z;
			}

			mesh_->azimuth = fmod(azimuth+180.0,360.0);
			mesh_->pitch = 0;// pitch;
			mesh_->roll = roll;
			mesh_->x = x;
			mesh_->y = y-settings::player_height;
			mesh_->z = z;

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
				settings::third_person_declination += settings::third_person_change_speed*delta_time;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
				settings::third_person_declination -= settings::third_person_change_speed*delta_time;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
				settings::third_person_pan += settings::third_person_change_speed*delta_time;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
				settings::third_person_pan -= settings::third_person_change_speed*delta_time;

		}
		else
		{
			cam.x = x;
			cam.y = y + settings::eye_height_fraction * settings::player_height;
			if (crouch_key)
				cam.y = y - settings::eye_height_fraction * settings::player_height;
			cam.z = z;

			cam.roll = roll;
			cam.azimuth = azimuth;
			cam.pitch = pitch;
		}
		cam.azimuth = fmod(cam.azimuth, 360.0);

		for (int i = 0; i < objects.size(); i++)
		{
			//std::cout << objects[i]->type << "\n";
			if (objects[i]->type == Object::DOOR)
			{
				//std::cout << "door?\n";
				if (!settings::no_clip)
				{
					if (door_collide(static_cast<Door*>(objects[i])))
					{
						x -= x_vel*delta_time;
						z -= z_vel*delta_time;
					}
				}
				if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
				{
					double dx = objects[i]->mesh->x - x;
					double dy = objects[i]->mesh->y - y;
					double dz = objects[i]->mesh->z - z;

					if (  dx*dx+dz*dz <= settings::unlock_radius*settings::unlock_radius && fabs(dy) <= 2.0*height)
					{
						if (holding != NULL)
						{
							if (holding->type == Object::KEY)
							{
								if (static_cast<Door*>(objects[i])->unlock(static_cast<Key*>(holding)))
								{
									drop();
								}
							}
						}
					}
				}
			}
			else
			{
				//Pick up
				//	if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
				{
					//do a rough check to see if close.
					double dx = cam.x - objects[i]->mesh->x;
					double dy = cam.y - objects[i]->mesh->y;
					double dz = cam.z - objects[i]->mesh->z;
					//	std::cout << dx << " " << dy << " " << dz << "\n";
					if (dx*dx + dz*dz <= settings::pick_up_radius*settings::pick_up_radius && fabs(dy) <= 1.3*settings::player_height)
					{
						//check to see if you're pointing right.
						double theta = atan2(dz, dx) * TO_DEG;
						double phi = asin(dy / (sqrt(dx*dx + dz*dz))) * TO_DEG;
						//	std::cout << theta << " : " << cam.azimuth << "     " << phi << " : " << cam.pitch << "\n";
						if (fabs(theta - cam.azimuth < 3) && fabs(phi - cam.pitch) < 3)
						{
							//	std::cout << "Pick thay shit up\n";
						}
						if (objects[i]->type == Object::TYPE::HOLDABLE || objects[i]->type == Object::TYPE::WEAPON || objects[i]->type == Object::TYPE::KEY)
							pick_up(static_cast<holdable*>(objects[i]));
					}
				}
			}
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
		{
			drop();
		}

		if (holding != NULL)
		{
			holding->mesh->azimuth = cam.azimuth;
			holding->mesh->pitch = cam.pitch + settings::gun_pitch_offset_display;

			//this is the pitch angle that is display'd
			double gun_ang = holding->mesh->pitch - settings::gun_pitch_viarance;// //settings::gun_pitch_viarance*((gun->phi_goal - gun->mesh->pitch));// -settings::gun_pitch_offset_display;// +settings::gun_pitch_offset + settings::gun_pitch_viarance*((gun->phi_goal - gun->mesh->pitch));

			if (settings::third_person)
			{
				holding->mesh->x = x + settings::gun_distance * (-sin(holding->mesh->azimuth * PI / 180.0) * cos((gun_ang)* PI / 180.0));
				holding->mesh->y = y + settings::gun_distance * (sin((gun_ang)* PI / 180.0));
				holding->mesh->z = z + settings::gun_distance * (-cos(holding->mesh->azimuth * PI / 180.0) * cos((gun_ang)* PI / 180.0));
			}
			else
			{
				holding->mesh->x = cam.x + settings::gun_distance * (-sin(holding->mesh->azimuth * PI / 180.0) * cos((gun_ang)* PI / 180.0));
				holding->mesh->y = cam.y + settings::gun_distance * (sin((gun_ang)* PI / 180.0));
				holding->mesh->z = cam.z + settings::gun_distance * (-cos(holding->mesh->azimuth * PI / 180.0) * cos((gun_ang)* PI / 180.0));
			}

			holding->action();
		}



		/*		THIS IS UPDATED TO BE "holding" instead of "gun"   To hold keys and shit.
		gun->mesh->azimuth = cam.azimuth;
		gun->mesh->pitch = cam.pitch + settings::gun_pitch_offset_display;
		
		//this is the pitch angle that is display'd
		double gun_ang = gun->mesh->pitch - settings::gun_pitch_viarance;// //settings::gun_pitch_viarance*((gun->phi_goal - gun->mesh->pitch));// -settings::gun_pitch_offset_display;// +settings::gun_pitch_offset + settings::gun_pitch_viarance*((gun->phi_goal - gun->mesh->pitch));

		if (settings::third_person)
		{
			gun->mesh->x = x + settings::gun_distance * (-sin(gun->mesh->azimuth * PI / 180.0) * cos((gun_ang)* PI / 180.0));
			gun->mesh->y = y + settings::gun_distance * (sin((gun_ang)* PI / 180.0));
			gun->mesh->z = z + settings::gun_distance * (-cos(gun->mesh->azimuth * PI / 180.0) * cos((gun_ang)* PI / 180.0));
		}
		else
		{
			gun->mesh->x = cam.x + settings::gun_distance * (-sin(gun->mesh->azimuth * PI / 180.0) * cos((gun_ang)* PI / 180.0));
			gun->mesh->y = cam.y + settings::gun_distance * (sin((gun_ang)* PI / 180.0));
			gun->mesh->z = cam.z + settings::gun_distance * (-cos(gun->mesh->azimuth * PI / 180.0) * cos((gun_ang)* PI / 180.0));
		}
		*/

		/*
		gun->theta_goal = cam.azimuth;
		if (l_key)
		{
			gun->theta_goal += -settings::gun_azimuth_strafe;
		}
		if (r_key)
		{
			gun->theta_goal += settings::gun_azimuth_strafe;
		}
		//this is the pitch angle that is display'd
		gun->phi_goal = cam.pitch + settings::gun_pitch_offset_display;
		if (y_vel > 0.5)
		{
			gun->phi_goal += -settings::gun_pitch_strafe;
		}
		else if (y_vel < -0.5)
		{
			gun->phi_goal += settings::gun_pitch_strafe;
		}

		gun->theta_vel = (gun->theta_goal - gun->mesh->azimuth)*4.0;
		gun->mesh->azimuth += gun->theta_vel * delta_time;

		gun->phi_vel = (gun->phi_goal - gun->mesh->pitch)*4.0;
		gun->mesh->pitch += gun->phi_vel * delta_time;

		if (gun->mesh->azimuth < gun->theta_goal - settings::gun_azimuth_range)
			gun->mesh->azimuth = gun->theta_goal - settings::gun_azimuth_range;
		if (gun->mesh->azimuth > gun->theta_goal + settings::gun_azimuth_range)
			gun->mesh->azimuth = gun->theta_goal + settings::gun_azimuth_range;
	
		if (gun->mesh->pitch < gun->phi_goal - settings::gun_pitch_range)
			gun->mesh->pitch = gun->phi_goal - settings::gun_pitch_range;
		if (gun->mesh->pitch > gun->phi_goal + settings::gun_pitch_range)
			gun->mesh->pitch = gun->phi_goal + settings::gun_pitch_range;
		
		
		//this is the pitch angle that is display'd
		double gun_ang = gun->mesh->pitch - settings::gun_pitch_offset_display + settings::gun_pitch_offset + settings::gun_pitch_viarance*((gun->phi_goal - gun->mesh->pitch));

		if (settings::third_person)
		{
			gun->mesh->x = x + settings::gun_distance * (-sin(gun->mesh->azimuth * PI / 180.0) * cos((gun_ang)* PI / 180.0));
			gun->mesh->y = y + settings::gun_distance * (sin((gun_ang)* PI / 180.0));
			gun->mesh->z = z + settings::gun_distance * (-cos(gun->mesh->azimuth * PI / 180.0) * cos((gun_ang)* PI / 180.0));
		}
		else
		{
			gun->mesh->x = cam.x + settings::gun_distance * (-sin(gun->mesh->azimuth * PI / 180.0) * cos((gun_ang)* PI / 180.0));
			gun->mesh->y = cam.y + settings::gun_distance * (sin((gun_ang)* PI / 180.0));
			gun->mesh->z = cam.z + settings::gun_distance * (-cos(gun->mesh->azimuth * PI / 180.0) * cos((gun_ang)* PI / 180.0));
		}
		*/

		/*  DEPRECIATED
		//Add veocity of gun turn to be laggy sorta. Make it not jitter, lock it to the cam update.
		//gun->mesh->azimuth = cam.azimuth + 90;
		gun->theta_goal = cam.azimuth;
		if (l_key)
		{
			gun->theta_goal += -settings::gun_azimuth_strafe;
		}
		if (r_key)
		{
			gun->theta_goal += settings::gun_azimuth_strafe;
		}
		//this is the pitch angle that is display'd
		gun->phi_goal = cam.pitch + settings::gun_pitch_offset_display;
		if (y_vel > 0.5)
		{
			gun->phi_goal += -settings::gun_pitch_strafe;
		}
		else if (y_vel < -0.5)
		{
			gun->phi_goal += settings::gun_pitch_strafe;
		}
		
		gun->theta_vel = (gun->theta_goal - gun->mesh->azimuth)*4.0;
		gun->mesh->azimuth += gun->theta_vel * delta_time;

		gun->phi_vel = (gun->phi_goal - gun->mesh->pitch)*4.0;
		gun->mesh->pitch += gun->phi_vel * delta_time;

		if (gun->mesh->azimuth < gun->theta_goal - settings::gun_azimuth_range)
			gun->mesh->azimuth = gun->theta_goal - settings::gun_azimuth_range;
		if (gun->mesh->azimuth > gun->theta_goal + settings::gun_azimuth_range)
			gun->mesh->azimuth = gun->theta_goal + settings::gun_azimuth_range;

		if (gun->mesh->pitch < gun->phi_goal - settings::gun_pitch_range)
			gun->mesh->pitch = gun->phi_goal - settings::gun_pitch_range;
		if (gun->mesh->pitch > gun->phi_goal + settings::gun_pitch_range)
			gun->mesh->pitch = gun->phi_goal + settings::gun_pitch_range;


		//gun->mesh->roll = cam.pitch - 10;
		//double gun_ang = cam.pitch - 35;


		//this is the pitch angle that is display'd
		double gun_ang = gun->mesh->pitch - settings::gun_pitch_offset_display + settings::gun_pitch_offset  + settings::gun_pitch_viarance*((gun->phi_goal-gun->mesh->pitch))  ;

		//gun->mesh->x = cam.x + 3 * sin(cam.azimuth * PI / 180.f + PI) * cos(gun_ang * PI / 180.f);
	//	gun->mesh->y = cam.y + 3 * sin(gun_ang * PI / 180.f);
	//	gun->mesh->z = cam.z + 3 * cos(cam.azimuth * PI / 180.f + PI) * cos(gun_ang * PI / 180.f);
	
	//	gun->mesh->x = cam.x + 3 * sin((gun->mesh->azimuth-90) * PI / 180.f + PI) * cos(gun_ang * PI / 180.f);
	//	gun->mesh->y = cam.y + 3 * sin(gun_ang * PI / 180.f);
	//gun->mesh->z = cam.z + 3 * cos((gun->mesh->azimuth - 90) * PI / 180.f + PI) * cos(gun_ang * PI / 180.f);

		gun->mesh->x = cam.x + settings::gun_distance * ( - sin(gun->mesh->azimuth * PI / 180.0) * cos((gun_ang) * PI / 180.0));
		gun->mesh->y = cam.y + settings::gun_distance * (sin((gun_ang) * PI / 180.0));
		gun->mesh->z = cam.z + settings::gun_distance * ( - cos(gun->mesh->azimuth * PI / 180.0) * cos((gun_ang) * PI / 180.0));
		*/
	}

	void key_mouse_move_arbit(camera_pos &cam, bsp_1::node *bsp_map, std::vector<Object *> &objects)
	{
		bool u_key = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W);
		bool d_key = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S);
		bool r_key = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D);
		bool l_key = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A);
		bool crouch_key = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl);
		bool jump_key = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space);
		double x_vel_rel = 0.f;
		double z_vel_rel = 0.f;

		double speed = settings::player_speed;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
		{
			if (settings::dev_mode)
				speed = settings::dev_speed_sprint;
			else
				speed = settings::splayer_speed_sprint;
		}

		double goal_roll = 0.0;

		if (r_key == l_key)
		{
			x_vel_rel = 0.f;
		}
		else if (r_key)
		{
			x_vel_rel = speed;
			goal_roll = -settings::strafe_lean;
		}
		else if (l_key)
		{
			x_vel_rel = -speed;
			goal_roll = settings::strafe_lean;
		}


		//		roll += (goal_roll - roll)*delta_time;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q) && sf::Keyboard::isKeyPressed(sf::Keyboard::E))
		{

		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
		{
			//	goal_roll = settings::lean_value;
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
		{
			//	goal_roll = -settings::lean_value;
		}



		if (goal_roll - roll > 0)
			roll += 40.0 * delta_time;
		else if (goal_roll - roll < 0)
			roll -= 40.0 * delta_time;
		if (fabs(goal_roll - roll) < 0.01)
			roll = goal_roll;


		if (u_key == d_key)
		{
			z_vel_rel = 0.f;
		}
		else if (u_key)
		{
			z_vel_rel = -speed;
		}
		else if (d_key)
		{
			z_vel_rel = speed;
		}


		if (d_key || u_key || l_key || r_key)
		{
			is_moving = true;
		}
		else
		{
			is_moving = false;
		}


		if (settings::dev_mode)
		{
			x_vel = (x_vel_rel * cos(azimuth * PI / 180.0) + z_vel_rel * sin(azimuth * PI / 180.0) * cos(pitch * PI / 180.0));
			y_vel = (-z_vel_rel * sin(pitch * PI / 180.0));
			z_vel = (-x_vel_rel * sin(azimuth * PI / 180.0) + z_vel_rel * cos(azimuth * PI / 180.0) * cos(pitch * PI / 180.0));
		}
		else
		{
			y_vel -= settings::gravity * delta_time;
			if (jump_key && has_jump && on_ground)
			{
				y_vel = settings::jump_speed;
				has_jump = false;
			}


			x_vel = x_vel_rel * cos(azimuth * PI / 180.0) + z_vel_rel * sin(azimuth * PI / 180.0) + x_vel_con;
			//y_vel = (-z_vel_rel * sin(pitch * PI / 180.0));
			z_vel = -x_vel_rel * sin(azimuth * PI / 180.0) + z_vel_rel * cos(azimuth * PI / 180.0) + z_vel_con;

			if (x_vel_con != 0.0)
			{
				x_vel_con -= signum(x_vel_con) * settings::friction_air_accel * delta_time;
				if (fabs(x_vel_con) < 0.001)
					x_vel_con = 0.0;
			}
			if (z_vel_con != 0.0)
			{
				z_vel_con -= signum(z_vel_con) * settings::friction_air_accel * delta_time;
				if (fabs(z_vel_con) < 0.001)
					z_vel_con = 0.0;
			}

		}

		std::vector<mesh::face_c> col_faces;
		if (!pause)
		{
			if (settings::no_clip)
			{
				x += x_vel * delta_time;
				y += y_vel * delta_time + (jump_key ? speed*delta_time : 0.0);
				z += z_vel * delta_time;
			}
			else
			{
				col_faces = move4(bsp_map);
			}
		}


		on_ground = false;
		bool vert_collide = false;
		for (int i = 0; i < col_faces.size(); i++)
		{
			if (col_faces[i].vertex[0].ny == 0)
			{
				vert_collide = true;
			}
			if (col_faces[i].vertex[0].ny > 0)
			{
				on_ground = true;
			}
		}

		if (vert_collide)//&& !on_ground && y_vel != 0.0
		{
			if (y_vel < 0)
			{
				y_vel += settings::gravity * delta_time;
				y_vel -= settings::wall_cling_grav * delta_time;
				if (y_vel < settings::wall_cling_max_down)
					y_vel = -settings::wall_cling_max_down;
			}
			if (jump_key)
			{

				//x_vel = (x_vel_rel * cos(azimuth * PI / 180.0) + z_vel_rel * sin(azimuth * PI / 180.0) * cos(pitch * PI / 180.0));
				//y_vel = (-z_vel_rel * sin(pitch * PI / 180.0));
				//z_vel = (-x_vel_rel * sin(azimuth * PI / 180.0) + z_vel_rel * cos(azimuth * PI / 180.0) * cos(pitch * PI / 180.0));

				x_vel_con = -settings::wall_jump_speed * sin(azimuth * PI / 180.0) * cos(pitch * PI / 180.0);
				y_vel = settings::wall_jump_speed * sin(pitch * PI / 180.0);
				z_vel_con = -settings::wall_jump_speed * cos(azimuth * PI / 180.0) * cos(pitch * PI / 180.0);

			}
			else
			{
				x_vel_con = 0;// -x_vel_con;
				z_vel_con = 0;// -z_vel_con;
			}
		}

		if (settings::third_person)
		{
			//for third person..
			//we need to look down on the character
			//so we chose the relative angles, then we add the player angles, then we back up the radius.
			//good enough.
			animation_time += delta_time;
			if (animation_time >= settings::animation_delay)
			{
				animation_time = 0.0;
				animation = (animation + 1) % frames.size();
			}

			//	*mesh_ = *frames[animation];
			bool fall = true;
			for (int i = 0; i < col_faces.size(); i++)
			{
				if (col_faces[i].vertex[0].ny != 0)
				{
					fall = false;
				}
			}
			if (fall)
			{
				*mesh_ = *jumping_frame;
			}
			else
			{
				if (is_moving)
					*mesh_ = *frames[animation];
				else
					*mesh_ = *standing_frame;
			}




			cam.azimuth = azimuth + settings::third_person_pan;
			cam.pitch = pitch - settings::third_person_declination;
			cam.roll = roll;

			double delta_x = settings::third_person_radius * sin(cam.azimuth * PI / 180.0) * cos(cam.pitch * PI / 180.0);
			double delta_y = -settings::third_person_radius * sin(cam.pitch * PI / 180.0);
			double delta_z = settings::third_person_radius * cos(cam.azimuth * PI / 180.0) * cos(cam.pitch * PI / 180.0);


			if (settings::third_person_camera_collision)
			{
				double d_x_eff = delta_x / 10.0;
				double d_y_eff = delta_y / 10.0;
				double d_z_eff = delta_z / 10.0;
				cam.x = x;
				cam.y = y;
				cam.z = z;
				for (int b = 0; b < 10; b++)
				{
					//we get both wall_collisions..
					std::vector<mesh::face_c> horz_faces = bsp_1::get_collide_faces(bsp_map, cam.x, cam.y, cam.z, 0.02, 0.01, bsp_1::TYPE::SEMI, 0, 0, 0);
					std::vector<mesh::face_c> vert_faces = bsp_1::get_collide_faces(bsp_map, cam.x, cam.y, cam.z, 0.02, 0.01, bsp_1::TYPE::SEMI, 0, 0, 0);

					if (horz_faces.size() > 0 || vert_faces.size() > 0)
					{
						cam.x -= d_x_eff;
						cam.y -= d_y_eff;
						cam.z -= d_z_eff;
						break;
					}
					cam.x += d_x_eff;
					cam.y += d_y_eff;
					cam.z += d_z_eff;
				}
			}
			else
			{
				cam.x = x + delta_x;
				cam.y = y + delta_y;
				cam.z = z + delta_z;
			}

			mesh_->azimuth = fmod(azimuth + 180.0, 360.0);
			mesh_->pitch = 0;// pitch;
			mesh_->roll = roll;
			mesh_->x = x;
			mesh_->y = y - settings::player_height;
			mesh_->z = z;

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
				settings::third_person_declination += settings::third_person_change_speed*delta_time;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
				settings::third_person_declination -= settings::third_person_change_speed*delta_time;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
				settings::third_person_pan += settings::third_person_change_speed*delta_time;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
				settings::third_person_pan -= settings::third_person_change_speed*delta_time;

		}
		else
		{
			cam.x = x;
			cam.y = y + settings::eye_height_fraction * settings::player_height;
			if (crouch_key)
				cam.y = y - settings::eye_height_fraction * settings::player_height;
			cam.z = z;

			cam.roll = roll;
			cam.azimuth = azimuth;
			cam.pitch = pitch;
		}
		cam.azimuth = fmod(cam.azimuth, 360.0);

		for (int i = 0; i < objects.size(); i++)
		{
			//std::cout << objects[i]->type << "\n";
			if (objects[i]->type == Object::DOOR)
			{
				//std::cout << "door?\n";
				if (!settings::no_clip)
				{
					if (door_collide(static_cast<Door*>(objects[i])))
					{
						x -= x_vel*delta_time;
						z -= z_vel*delta_time;
					}
				}
				if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
				{
					double dx = objects[i]->mesh->x - x;
					double dy = objects[i]->mesh->y - y;
					double dz = objects[i]->mesh->z - z;

					if (dx*dx + dz*dz <= settings::unlock_radius*settings::unlock_radius && fabs(dy) <= 2.0*height)
					{
						if (holding != NULL)
						{
							if (holding->type == Object::KEY)
							{
								if (static_cast<Door*>(objects[i])->unlock(static_cast<Key*>(holding)))
								{
									drop();
								}
							}
						}
					}
				}
			}
			else
			{
				//Pick up
				//	if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
				{
					//do a rough check to see if close.
					double dx = cam.x - objects[i]->mesh->x;
					double dy = cam.y - objects[i]->mesh->y;
					double dz = cam.z - objects[i]->mesh->z;
					//	std::cout << dx << " " << dy << " " << dz << "\n";
					if (dx*dx + dz*dz <= settings::pick_up_radius*settings::pick_up_radius && fabs(dy) <= 1.3*settings::player_height)
					{
						//check to see if you're pointing right.
						double theta = atan2(dz, dx) * TO_DEG;
						double phi = asin(dy / (sqrt(dx*dx + dz*dz))) * TO_DEG;
						//	std::cout << theta << " : " << cam.azimuth << "     " << phi << " : " << cam.pitch << "\n";
						if (fabs(theta - cam.azimuth < 3) && fabs(phi - cam.pitch) < 3)
						{
							//	std::cout << "Pick thay shit up\n";
						}
						if (objects[i]->type == Object::TYPE::HOLDABLE || objects[i]->type == Object::TYPE::WEAPON || objects[i]->type == Object::TYPE::KEY)
							pick_up(static_cast<holdable*>(objects[i]));
					}
				}
			}
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
		{
			drop();
		}

		if (holding != NULL)
		{
			holding->mesh->azimuth = cam.azimuth;
			holding->mesh->pitch = cam.pitch + settings::gun_pitch_offset_display;

			//this is the pitch angle that is display'd
			double gun_ang = holding->mesh->pitch - settings::gun_pitch_viarance;// //settings::gun_pitch_viarance*((gun->phi_goal - gun->mesh->pitch));// -settings::gun_pitch_offset_display;// +settings::gun_pitch_offset + settings::gun_pitch_viarance*((gun->phi_goal - gun->mesh->pitch));

			if (settings::third_person)
			{
				holding->mesh->x = x + settings::gun_distance * (-sin(holding->mesh->azimuth * PI / 180.0) * cos((gun_ang)* PI / 180.0));
				holding->mesh->y = y + settings::gun_distance * (sin((gun_ang)* PI / 180.0));
				holding->mesh->z = z + settings::gun_distance * (-cos(holding->mesh->azimuth * PI / 180.0) * cos((gun_ang)* PI / 180.0));
			}
			else
			{
				holding->mesh->x = cam.x + settings::gun_distance * (-sin(holding->mesh->azimuth * PI / 180.0) * cos((gun_ang)* PI / 180.0));
				holding->mesh->y = cam.y + settings::gun_distance * (sin((gun_ang)* PI / 180.0));
				holding->mesh->z = cam.z + settings::gun_distance * (-cos(holding->mesh->azimuth * PI / 180.0) * cos((gun_ang)* PI / 180.0));
			}

			holding->action();
		}
	}

	void pick_up(holdable * pick_)
	{
		drop();
		holding = pick_;
		holding->on_ground = true;
		holding->mesh->hud = true;
		holding->being_held = true;
	}
	
	void drop()
	{
		if (holding != NULL)
		{
			holding->mesh->y = y;
			holding->y_vel = 1.0;
			holding->mesh->pitch = 0.0;
			holding->on_ground = false;
			holding->mesh->hud = false;
			holding->being_held = false;
		}
		holding = NULL;
	}

	//so this is where keyboard shit is to move it.
	
	float speed = 5.f;

	void set_mouse_states(sf::RenderWindow &window)
	{

		if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Tilde))
		{
			if (!pause)
			{
				window.setMouseCursorVisible(false);
				m_x = -sf::Mouse::getPosition(window).x * 200.f / window.getSize().x + 100.f;
				m_y = -sf::Mouse::getPosition(window).y * 200.f / window.getSize().y + 100.f;
				pitch += m_y;
				azimuth += m_x;
				if (pitch > 90)
					pitch = 90;
				if (pitch < -90)
					pitch = -90;
				sf::Mouse::setPosition(sf::Vector2i(window.getSize().x / 2, window.getSize().y / 2), window);
			}
			pause_toggle = false;
		}
		else
		{
			if (!pause_toggle)
			{
				pause_toggle = true;
				pause = !pause;
			}
			edit::input.clear();
			window.setMouseCursorVisible(true);
			sf::Mouse::setPosition(sf::Vector2i(window.getSize().x / 2, window.getSize().y / 2), window);
		}

	}

	double x_vel_con = 0.0;
	double z_vel_con = 0.0;

	double x_vel_oth = 0.0;
	double z_vel_oth = 0.0;
	GLfloat	m_x = 0.f;
	GLfloat m_y = 0.f;
	mesh::Mesh_c * standing_frame;
	mesh::Mesh_c * jumping_frame;
	bool on_ground = false;
	holdable * holding = NULL;
};

struct enemy : entity
{
	enemy() {};

	enemy(GLfloat x_, GLfloat y_, GLfloat z_, GLfloat radius_, GLfloat height_)
	{
		x = x_;
		y = y_;
		z = z_;
		radius = radius_;
		height = height_;
		x_vel = 0.0;
		y_vel = 30.0;
		z_vel = 0.0;
		pitch = 0;
		azimuth = 0;
		roll = 0;
		mesh_ = new mesh::Mesh_c();
		animation = 0;
	}

	void basic_move(bsp_1::node *horz_map, bsp_1::node *vert_map)
	{
		animation_time += delta_time;
		if (animation_time >= settings::animation_delay)
		{
			animation_time = 0.0;
			animation = (animation + 1) % frames.size();
			*mesh_ = *frames[animation];
			//mesh_ = frames[animation];
			mesh_->azimuth = azimuth;
			mesh_->pitch = pitch;
			mesh_->roll = roll;
			mesh_->x = x;
			mesh_->y = y - height;
			mesh_->z = z;
			
		}


		if (sf::Keyboard::isKeyPressed(sf::Keyboard::K))
		{
			if (run_toggle)
			{
				run = !run;
			}
			run_toggle = true;
		}
		else
		{
			run_toggle = false;
		}
		
		if (run)
		{
			y_vel -= settings::gravity * delta_time;
			x_vel = settings::enemy_speed * sin(azimuth * PI / 180.0);
			z_vel = settings::enemy_speed * cos(azimuth * PI / 180.0);
		}
		else
		{
			y_vel = 0.0;
			x_vel = 0.0;
			z_vel = 0.0;
		}



		std::vector<mesh::face_c> faces = move3(horz_map, vert_map);

		for (int i = 0; i < faces.size(); i++)
		{
			if (fabs(faces[i].vertex[0].ny) < 0.000001)
			{
				//hit a fresh wall
				azimuth += random() * 90.0 - 45.0;
			}
		}



		//azimuth += random() * 30.0 - 15.0;
		
	
		mesh_->azimuth = azimuth;
		mesh_->pitch = pitch;
		mesh_->roll = roll;
		mesh_->x = x;
		mesh_->y = y - height;
		mesh_->z = z;
		

		//std::cout << mesh_->x << " " << mesh_->y << " " << mesh_->z << "\n";
	}

	void basic_move_big(bsp_1::node *bsp_map)
	{
		animation_time += delta_time;
		if (animation_time >= settings::animation_delay)
		{
			animation_time = 0.0;
			animation = (animation + 1) % frames.size();
			*mesh_ = *frames[animation];
			//mesh_ = frames[animation];
			mesh_->azimuth = azimuth;
			mesh_->pitch = pitch;
			mesh_->roll = roll;
			mesh_->x = x;
			mesh_->y = y - height;
			mesh_->z = z;

		}


		if (sf::Keyboard::isKeyPressed(sf::Keyboard::K))
		{
			if (run_toggle)
			{
				run = !run;
			}
			run_toggle = true;
		}
		else
		{
			run_toggle = false;
		}

		if (run)
		{
			y_vel -= settings::gravity * delta_time;
			x_vel = settings::enemy_speed * sin(azimuth * PI / 180.0);
			z_vel = settings::enemy_speed * cos(azimuth * PI / 180.0);
		}
		else
		{
			y_vel = 0.0;
			x_vel = 0.0;
			z_vel = 0.0;
		}



		std::vector<mesh::face_c> faces = move4(bsp_map);

		for (int i = 0; i < faces.size(); i++)
		{
			if (fabs(faces[i].vertex[0].ny) < 0.000001)
			{
				//hit a fresh wall
				azimuth += random() * 90.0 - 45.0;
			}
		}



		//azimuth += random() * 30.0 - 15.0;


		mesh_->azimuth = azimuth;
		mesh_->pitch = pitch;
		mesh_->roll = roll;
		mesh_->x = x;
		mesh_->y = y - height;
		mesh_->z = z;


		//std::cout << mesh_->x << " " << mesh_->y << " " << mesh_->z << "\n";
	}

	bool run_toggle = false;
	bool run = false;

	float speed = 5.f;

	GLfloat	m_x = 0.f;
	GLfloat m_y = 0.f;
};

struct enemy_base : entity
{
	enemy_base() {};
	enemy_base(GLfloat x_, GLfloat y_, GLfloat z_, std::vector < std::vector<mesh::Mesh_g * > > display_mesh_, GLfloat radius_, GLfloat height_)
	{
		x = x_;
		y = y_;
		z = z_;
		display_mesh = display_mesh_;
		radius = radius_;
		height = height_;
		x_vel = 0.f;
		y_vel = 0.f;
		z_vel = 0.f;
	}
	
	enum TRANSITION
	{
		
	};
	enum STATE
	{
		MOVE
	};

	void act()
	{
		if(state == STATE::MOVE)
			move();
	}

	virtual void move() = 0;

	STATE state;
};

float edit_speed = 4.f;

void edit_move(mesh::Mesh_c * mesh)
{
	bool f_key = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up);
	bool b_key = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down);
	bool r_key = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right);
	bool l_key = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left);
	bool u_key = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Period);
	bool d_key = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Comma);

	float x_vel_rel = 0.f;
	float y_vel_rel = 0.f;
	float z_vel_rel = 0.f;


	if (r_key == l_key)
	{
		x_vel_rel = 0.f;
	}
	else if (r_key)
	{
		x_vel_rel = edit_speed;
	}
	else if (l_key)
	{
		x_vel_rel = -edit_speed;
	}

	if (u_key == d_key)
	{
		y_vel_rel = 0.f;
	}
	else if (u_key)
	{
		y_vel_rel = edit_speed;
	}
	else if (d_key)
	{
		y_vel_rel = -edit_speed;
	}

	if (f_key == b_key)
	{
		z_vel_rel = 0.f;
	}
	else if (f_key)
	{
		z_vel_rel = edit_speed;
	}
	else if (b_key)
	{
		z_vel_rel = -edit_speed;
	}

	if (!pause)
	{
		mesh->x += x_vel_rel * delta_time;
		mesh->y += y_vel_rel * delta_time;
		mesh->z += z_vel_rel * delta_time;
	}
}

struct info_packet
{
	bsp_1::node * horz;
	bsp_1::node * vert;
	std::vector<entity*> * entities;
	std::vector<mesh::Mesh_c *> * meshes;
	std::vector<Object *> * objects;
};
