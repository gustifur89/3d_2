#pragma once
#include "headers.h"
#include "mesh.h"
#include "reader.h"
#include "bsp.h"


namespace object_values
{
	bsp_1::node * bsp_tree = NULL;
}


struct Object
{
	enum TYPE
	{
		OTHER,
		DOOR,
		WEAPON,
		HOLDABLE,
		KEY
	};
	Object() 
	{ 
		x = 0.0; 
		y = 0.0;
		z = 0.0;
		mesh = new mesh::Mesh_c();
	}
	Object(std::string file_name)
	{
		x = 0.0;
		y = 0.0;
		z = 0.0;
		mesh = new mesh::Mesh_c();
		reader::read_ply(file_name, *mesh);
	}
	Object(double x_, double y_, double z_)
	{
		x = x_;
		y = y_;
		z = z_;
		mesh = new mesh::Mesh_c();
	}
	
	virtual void action() {};
	
	double x;
	double y;
	double z;
	bool being_held=false;
	mesh::Mesh_c * mesh;
	TYPE type;
};

struct holdable : Object
{
	holdable(std::string file_name) : Object(file_name)
	{
		type = TYPE::HOLDABLE;
	}

	holdable() : Object()
	{
		type = TYPE::HOLDABLE;
	}


	void action()
	{
		if (!on_ground)
		{
			y_vel -= settings::gravity*delta_time;
			mesh->y += y_vel*delta_time;
			std::vector<mesh::face_c> horz_faces = bsp_1::get_collide_faces(object_values::bsp_tree, mesh->x, mesh->y, mesh->z, 0.2, 0.2, bsp_1::TYPE::HORIZONTAL, 0, 0, 0);
			if (horz_faces.size() > 0)
			{
				on_ground = true;
				//	mesh->y = horz_faces[0].vertex[0].y;
			}
		}
	}

	double y_vel = 0.0;
	bool on_ground;
};

struct Key : holdable
{
	enum STATE
	{
		ACTIVE,
		INACTIVE
	};
	Key() {};
	Key(std::string file_name) : holdable(file_name)
	{
		lock = rand() % 1000000;
		type = TYPE::KEY;
	}
	
	int lock;
	STATE state;
};


struct Door : Object
{
	enum STATE
	{
		TO,
		BACK,
		STOP
	};

	Door() { mesh = new mesh::Mesh_c(); };
	Door(double x_, double y_, double z_, double dest_x_, double dest_y_, double dest_z_, double speed_, Key *key_) : Object(x_,y_,z_)
	{
		x = x_;
		y = y_;
		z = z_;
		dest_x = dest_x_;
		dest_y = dest_y_;
		dest_z = dest_z_;
		org_x = x_;
		org_y = y_;
		org_z = z_;
		key = key_;
		key_lock = key_->lock;
		mesh = new mesh::Mesh_c();
		mesh->x = x_;
		mesh->y = y_;
		mesh->z = z_;
		double t_dx = dest_x - x;
		double t_dy = dest_y - y;
		double t_dz = dest_z - z;
		double n = sqrt(t_dx*t_dx + t_dy*t_dy + t_dz*t_dz);
		dir_x = t_dx / n;
		dir_y = t_dy / n;
		dir_z = t_dz / n;
		type = TYPE::DOOR;
		speed = speed_;
		state = STATE::STOP;
	}

	Door(double x_, double y_, double z_, double dest_x_, double dest_y_, double dest_z_, double speed_, Key *key_, std::string file_name)
	{
		x = x_;
		y = y_;
		z = z_;
		dest_x = dest_x_;
		dest_y = dest_y_;
		dest_z = dest_z_;
		org_x = x_;
		org_y = y_;
		org_z = z_;
		key = key_;
		key_lock = key_->lock;
		//mesh = new mesh::Mesh_c();
		mesh->x = x_;
		mesh->y = y_;
		mesh->z = z_;
		double t_dx = dest_x - x;
		double t_dy = dest_y - y;
		double t_dz = dest_z - z;
		double n = sqrt(t_dx*t_dx + t_dy*t_dy + t_dz*t_dz);
		dir_x = t_dx / n;
		dir_y = t_dy / n;
		dir_z = t_dz / n;
		type = TYPE::DOOR;
		speed = speed_;
		state = STATE::STOP;
	//	mesh = new mesh::Mesh_c();
		reader::read_ply(file_name, *mesh);
		mesh->x = x_;
		mesh->y = y_;
		mesh->z = z_;
		mesh::calculate_bounds(mesh, lx, ly, lz, hx, hy, hz);
		type = TYPE::DOOR;
	}

	bool unlock(Key * key_)
	{
	//	std::cout << key_ << " " << key << "\n";
		if (key_== key)
		{
			state = STATE::TO;
			return true;
		}
		return false;
	}

	void reconfigure()
	{
		mesh->reconfigure();
		mesh::calculate_bounds(mesh, lx, ly, lz, hx, hy, hz);
	}

	void action()
	{
		/*
		if (key->state == Key::ACTIVE)
		{
			state = STATE::TO;
		}*/
		double dx = 0.0;
		double dy = 0.0;
		double dz = 0.0;
		switch (state)
		{
		case STATE::STOP:
			break;
		case STATE::TO:
			dx = dir_x * speed * delta_time;
			dy = dir_y * speed * delta_time;
			dz = dir_z * speed * delta_time;

			mesh->x += dx;
			mesh->y += dy;
			mesh->z += dz;

			lx += dx;
			hx += dx;
			ly += dy;
			hy += dy;
			lz += dz;
			hz += dz;
			if (fabs(mesh->x - dest_x) <= 0.01 && fabs(mesh->y - dest_y) <= 0.01 && fabs(mesh->z - dest_z) <= 0.01)
			{
				state = STATE::STOP;
			}
			break;
		case STATE::BACK:
			dx = -dir_x * speed * delta_time;
			dy = -dir_y * speed * delta_time;
			dz = -dir_z * speed * delta_time;

			mesh->x += dx;
			mesh->y += dy;
			mesh->z += dz;

			lx += dx;
			hx += dx;
			ly += dy;
			hy += dy;
			lz += dz;
			hz += dz;
			if (fabs(mesh->x - org_x) <= 0.001 && fabs(mesh->y - org_y) <= 0.001 && fabs(mesh->z - org_z) <= 0.001)
			{
				state = STATE::STOP;
			}
			break;
		}
		//std::cout << x << " " << y << " " << z << " \n";
		//reconfigure();
	}
	double dest_x;
	double dest_y;
	double dest_z;
	double org_x;
	double org_y;
	double org_z;
	double speed;
	double dir_x;
	double dir_y;
	double dir_z;
	double lx = 0.0, hx=0.0, ly=0.0, hy=0.0, lz=0.0, hz=0.0;
	Key *key=NULL;
	int key_lock=0;
	size_t d_node = NULL;
	STATE state;
};
/** /
void bullet_event(double theta, double phi, double x_, double y_, double z_, double damage, bsp_1::node * horz, bsp_1::node * vert,
	std::vector<entity*> entities, entity * agent, std::vector<mesh::Mesh_c *> meshes);
*/
struct weapon : holdable
{
	weapon(std::string file_name, std::string snd_file) : holdable(file_name)
	{
		buffer.loadFromFile(snd_file);
		sound.setBuffer(buffer);
		sound.setLoop(false);
		type = TYPE::WEAPON;
	}

	void fire()
	{
		sound.setPosition(mesh->x, mesh->y, mesh->z);
		sound.play();
		/*
		bsp_1::node * horz, bsp_1::node * vert,
			std::vector<entity*> entities, entity * agent, std::vector<mesh::Mesh_c *> meshes*/
	//	holder->info;
	//	bullet_event(mesh->azimuth*TO_RAD, mesh->pitch*TO_RAD, x, y, z, damage,)
	}

	void action()
	{
		holdable::action();
		if (being_held)
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::F))
			{
				//hold weapon side-ways CS:GO style.
				mesh->x += 0.4 * (-sin(mesh->azimuth * PI / 180.0) * cos((mesh->pitch)* PI / 180.0));
				mesh->y += 0.4 * (sin((mesh->pitch)* PI / 180.0));
				mesh->z += 0.4 * (-cos(mesh->azimuth * PI / 180.0) * cos((mesh->pitch)* PI / 180.0));
				mesh->azimuth += 90.0;
				mesh->pitch /= 2.0;
			}
			else if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				if (first_shot)
				{
					first_shot = false;
					fire();
				}

				c_time += delta_time;// fmod(c_time + delta_time, reload_time);
				if (c_time > reload_time)
				{
					c_time = 0.0;
					fire();
					
				}
				
				if (c_time <= kick_time)
				{
					double t_fraction = c_time / kick_time;  // (c_time < reload_time / 2.0) ? ((2.0*c_time / reload_time)) : ((1.0 - (2.0*c_time / reload_time)));
					mesh->pitch += t_fraction*kick_back;
					mesh->azimuth += t_fraction*(random()*2.0 - 1.0)*kick_back;
				}
			}
			else
			{
				first_shot = true;
			}
		}
	}

	bool first_shot = true;
	double c_time = 0.0;
	double reload_time;
	double kick_time;
	double kick_back;
	sf::SoundBuffer buffer;
	sf::Sound sound;
//	entity * holder;
	double damage = 10.0;
};

struct blaster_test : weapon
{
	blaster_test() : weapon("src/meshes/blaster_0.ply", "src/snds/sndGun_1.wav")
	{
		reload_time = 0.1;
		kick_time = 0.09;
		kick_back = 4.0;
	}

	void action()
	{
		weapon::action();
		//std::cout << type << "\n";
	}
};

struct A18 : weapon
{
	A18() : weapon("src/meshes/a18.ply", "src/snds/sndGun_0.wav")
	{
		reload_time = 0.2;
		kick_time = 0.1;
		kick_back = 10.0;
	}

	void action()
	{
		weapon::action();
		//std::cout << type << "\n";
	}
};


namespace bullets
{
	mesh::Mesh_c pop;
}


double max_bullet_range = 80.0;
double bullet_radius = 0.2;
double bullet_step_size = 0.1;
/*
void bullet_event(double theta, double phi, double x_, double y_, double z_, double damage, bsp_1::node * horz, bsp_1::node * vert,
	std::vector<entity*> entities, entity * agent, std::vector<mesh::Mesh_c *> meshes)
{
	double dist = 0.0;

	double x = x_;
	double y = y_;
	double z = z_;

	double zv = bullet_step_size * cos(theta)*cos(phi);
	double xv = bullet_step_size * sin(theta)*cos(phi);
	double yv = bullet_step_size * sin(phi);

	while ((x - x_)*(x - x_) + (y - y_)*(y - y_) + (z - z_)*(z - z_) <= max_bullet_range*max_bullet_range)
	{
		//check collisions against entities
		for (int i = 0; i < entities.size(); i++)
		{
			//Make sure you don't shoot yourself
			if (entities[i] != agent)
			{
				if (entities[i]->bullet_collide(x, y, z, bullet_radius))
				{
					entities[i]->receive_damage(damage, agent);
					break;
				}
			}
		}
		//check collisions against walls



		//move
		x += xv;
		z += zv;
		y += yv;
	}

	//add a pop.
	if ((x - x_)*(x - x_) + (y - y_)*(y - y_) + (z - z_)*(z - z_) <= max_bullet_range*max_bullet_range)
	{
		//pop
		mesh::Mesh_c * popop = new mesh::Mesh_c(bullets::pop);
		popop->x = x;
		popop->y = y;
		popop->z = z;

		meshes.push_back(popop);

	}
	else
	{
		//no pop
	}


}*/

