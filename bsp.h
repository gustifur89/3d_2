#pragma once
#include "headers.h"
#include "mesh.h"

//---------------------------------------------
//
// 1)	start with aribtrary face.
// 2)	make plane of of the face with normal
// 3)	go through all faces and split into {left, right, coincinent, span}
// 4)	recursively go through until all faces added.
// 5)	I think
//
//---------------------------------------------

//TODO: switch from mesh_g to mesh_c


//running this should change a mesh to a node tree
/*
int signum(float a)
{
	if (a == 0.f)
		return 0;
	if (a > 0.f)
		return 1;
	else
		return -1;
}
*/


namespace bsp_1
{
	int count = 0;
	double side_dist(mesh::face_c *face, float x_, float y_, float z_)
	{
	//	double c = -(face->normal[0] * face->position[0] + face->normal[1] * face->position[1] + face->normal[2] * face->position[2]);
	//	double d = face->normal[0] * x_ + face->normal[1] * y_ + face->normal[2] * z_ + c;
		double c = -(face->vertex[0].nx * face->position[0] + face->vertex[0].ny * face->position[1] + face->vertex[0].nz * face->position[2]);
		double d = face->vertex[0].nx * x_ + face->vertex[0].ny * y_ + face->vertex[0].nz * z_ + c;
		return d;
	}
	double side_dist(mesh::face_c face, float x_, float y_, float z_)
	{
		//double c = -(face.normal[0] * face.position[0] + face.normal[1] * face.position[1] + face.normal[2] * face.position[2]);
		//double d = face.normal[0] * x_ + face.normal[1] * y_ + face.normal[2] * z_ + c;
		double c = -(face.vertex[0].nx * face.position[0] + face.vertex[0].ny * face.position[1] + face.vertex[0].nz * face.position[2]);
		double d = face.vertex[0].nx * x_ + face.vertex[0].ny * y_ + face.vertex[0].nz * z_ + c;
		return d;
	}
	enum TYPE
	{
		HORIZONTAL,
		VERTICAL,
		SEMI
	};
	TYPE face_type(mesh::face_c * face)
	{
		//solve the angle between the plane of the face and the flat floor.
		//magnitudes are 1. And the plane's normal is <0,1,0>
		double angle = 180.0 * acos(abs(face->normal[1])) / PI;
		if (angle < 0.01) return TYPE::HORIZONTAL;
		if (angle < 89) return TYPE::SEMI;

		return TYPE::VERTICAL;
	}

	bool same_sign(double a, double b)
	{
		if (a == 0 || b == 0) return true;
		return signum(a) == signum(b);
	}

	/**
	* Gets the case given the side of the plane the three verts are on.
	* Returns based on which side is non-zero unique.
	* Return	0 - a is unique side
	*			1 - b is unique side
	*			2 - c is unique side
	*/
	int get_case_num(double a, double b, double c)
	{
		bool is_zero[3];
		a == 0 ? is_zero[0] = true : is_zero[0] = false;
		b == 0 ? is_zero[1] = true : is_zero[1] = false;
		c == 0 ? is_zero[2] = true : is_zero[2] = false;
		if (is_zero[0] || is_zero[1] || is_zero[2])
		{
			//so if one or more is zero
			//if two are zero, this won't be called anyway, so no need to decide on that.
			//But what do we do if one is zero...
			//we would split it down the middle, so... oh god... 
			//idk what do you think we should do?
			//it works kinda okay without any code in here. maybe just leave it be for a while. 
			//No I should make it right...
			if (is_zero[0]) return 3;
			if (is_zero[1]) return 4;
			if (is_zero[2]) return 5;
		}
		else
		{
			//like regular
			if (signum(b) == signum(c)) return 0;
			if (signum(a) == signum(c)) return 1;
			if (signum(a) == signum(b)) return 2;
		}
		return 0;
	}

	void cull_faces(std::vector<mesh::face_c*> &faces_in, std::vector<mesh::face_c*> &faces_out, TYPE type)
	{
		for (int i = 0; i < faces_in.size(); i++)
		{
			if (face_type(faces_in[i]) == type || face_type(faces_in[i]) == TYPE::SEMI)
				faces_out.push_back(faces_in[i]);
		}
	}

	double dot(mesh::face_c face1, mesh::face_c face2)
	{
		return face1.vertex[0].nx * face2.vertex[0].nx + face1.vertex[0].ny * face2.vertex[0].ny + face1.vertex[0].nz * face2.vertex[0].nz;
	}

	bool vert_equals(mesh::Vertex_c vert1, mesh::Vertex_c vert2)
	{
		return vert1.x == vert2.x && vert1.y == vert2.y && vert1.z == vert2.z;
	}

	void remove_duplicates(std::vector<mesh::face_c*> &faces)
	{
		int count = 0;
		std::vector<mesh::face_c*> out;
		for (int i = 0; i < faces.size(); i++) 
		{
			bool unique = true;
			for (int j = 0; j < out.size(); j++)
			{
				int u_count = 0;
				if (vert_equals(faces[i]->vertex[0], out[j]->vertex[0])) u_count++;
				if (vert_equals(faces[i]->vertex[1], out[j]->vertex[1])) u_count++;
				if (vert_equals(faces[i]->vertex[2], out[j]->vertex[2])) u_count++;
				if (u_count >= 2)
				{
					double dot_ = dot(*faces[i], *out[j]);
					if (dot_ <= -0.9)
					{
						unique = false;
						count++;
					}
				}
			}
			if (unique)
			{
				out.push_back(faces[i]);
			}
		}
		std::cout << count << " removed dupes\n";
		faces = out;
	}

	bool is_good_value(double a)
	{
		return isfinite(a) && fabs(a) <= 1;
	}

	double round(double a, double b)
	{
		return ((int)(a/b)) * b;
	}
	
	double side_length(mesh::Vertex_c vert1, mesh::Vertex_c vert2)
	{
		return sqrt( (vert1.x - vert2.x)*(vert1.x - vert2.x) + (vert1.y - vert2.y)*(vert1.y - vert2.y) + (vert1.z - vert2.z)*(vert1.z - vert2.z) );
	}

	bool degenerate(mesh::face_c * face)
	{
		double a = side_length(face->vertex[0], face->vertex[1]);
		double b = side_length(face->vertex[1], face->vertex[2]);
		double c = side_length(face->vertex[2], face->vertex[0]);
		if (a < 0.001 || b < 0.001 || c < 0.001)
			return true;
		return false;
	}

	struct node
	{
		//constructor
		node()
		{
			//sets null pointers
			current = NULL;
			parent = NULL;
			front = NULL;
			back = NULL;
		};

		//construct is a recursive function. It takes a list of lines and sorts them based on wheteher they are in front of the current node or behind it.
		//	then it creates new nodes (front and back). Add calls construct with the sorted lists (front takes the list of front lines and back takes the
		//	sorted list of back lines)


		void construct(std::vector<mesh::face_c*> *faces)
		{
			//Because you may not be familiar enough with pointer to understand all the random *'s and &'s (I wasn't when I started this project)
			//	I'll just refer to things by what they ultimately point to, not them as pointers.
			count++;
			//sets the nodes value to be the first line in the list
	//		std::cout << count << "\n";
			int chosen = 0;// (int)(faces->size() / 2);// (int)(faces->size() - 1);// rand() % faces->size();// (int)(faces->size() / 2);// 0;// (int)(faces->size()-1);
			this->current = (*faces)[chosen];
			this->type = face_type(this->current);

			/*
			if (this->type == TYPE::HORIZONTAL)
			{
				this->current->vertex[0].r = 0;
				this->current->vertex[0].g = 255;
				this->current->vertex[0].b = 0;
				this->current->vertex[1].r = 0;
				this->current->vertex[1].g = 255;
				this->current->vertex[1].b = 0;
				this->current->vertex[2].r = 0;
				this->current->vertex[2].g = 255;
				this->current->vertex[2].b = 0;
			}
			if (this->type == TYPE::SEMI)
			{
				this->current->vertex[0].r = 255;
				this->current->vertex[0].g = 0;
				this->current->vertex[0].b = 0;
				this->current->vertex[1].r = 255;
				this->current->vertex[1].g = 0;
				this->current->vertex[1].b = 0;
				this->current->vertex[2].r = 255;
				this->current->vertex[2].g = 0;
				this->current->vertex[2].b = 0;
			}
			if (this->type == TYPE::VERTICAL)
			{
				this->current->vertex[0].r = 0;
				this->current->vertex[0].g = 0;
				this->current->vertex[0].b = 255;
				this->current->vertex[1].r = 0;
				this->current->vertex[1].g = 0;
				this->current->vertex[1].b = 255;
				this->current->vertex[2].r = 0;
				this->current->vertex[2].g = 0;
				this->current->vertex[2].b = 255;
			}
			*/
			int r = rand() % 115 + 140;
			int g = rand() % 115 + 140;
			int k = rand() % 115 + 140;
			if (settings::random_ex_colors)
			{
				this->current->vertex[0].r = r;
				this->current->vertex[0].g = g;
				this->current->vertex[0].b = k;
				this->current->vertex[1].r = r;
				this->current->vertex[1].g = g;
				this->current->vertex[1].b = k;
				this->current->vertex[2].r = r;
				this->current->vertex[2].g = g;
				this->current->vertex[2].b = k;
			}
			//	*/
			//front and back sub lists. They are placed in the function because after the function is called, they are not needed. They have the scope of this 
			//	function, then are deleted to return memory. [Using some scope]
			std::vector<mesh::face_c*> *front_ = new std::vector<mesh::face_c*>;
			std::vector<mesh::face_c*> *back_ = new std::vector<mesh::face_c*>;
			//as the first line is already in this node, we start at one. If there is only one line in the current *lines, then nothing happens
			for (int i = 0; i < faces->size(); i++)
			{
				if (i == chosen) continue;
				//if (face_type((*faces)[i]) != type) continue;
				//get the signed distances (yes because we are in linear algebra land now)
				//	from the two end points of the line we are checking to the current line.
				// (int)
				double as = side_dist((*faces)[chosen], (*faces)[i]->vertex[0].x, (*faces)[i]->vertex[0].y, (*faces)[i]->vertex[0].z);// , 0.00000000001);	//cast to int because of weird issues. No problems thus far with the cast.
				double bs = side_dist((*faces)[chosen], (*faces)[i]->vertex[1].x, (*faces)[i]->vertex[1].y, (*faces)[i]->vertex[1].z);// , 0.00000000001);
				double cs = side_dist((*faces)[chosen], (*faces)[i]->vertex[2].x, (*faces)[i]->vertex[2].y, (*faces)[i]->vertex[2].z);// , 0.00000000001);
				if (fabs(as) < 0.1) as = 0.0;
				if (fabs(bs) < 0.1) bs = 0.0;
				if (fabs(cs) < 0.1) cs = 0.0;

				//	std::cout << as << "\n";
//	std::cout << (*faces)[i]->vertex[0].x << " " << (*faces)[i]->vertex[1].x << " " << (*faces)[i]->vertex[2].x << std::endl;
				if (as == 0 && bs == 0 && cs == 0)
					//	if(fabs(as) < 0.0000001 && fabs(bs) < 0.0000001 && fabs(cs) < 0.0000001)
				{
					//fully incident
					//Look at normals... 
					if (dot(*(*faces)[chosen], *(*faces)[i]) >= 0)
					{
						front_->push_back((*faces)[i]);
					}
					else
					{
						back_->push_back((*faces)[i]);
					}
					//OMG! It actually works. I think I fixed it. I don't slide through walls anymore. Yay!
				}
				else if (as >= 0 && bs >= 0 && cs >= 0)
				{
					//std::cout << as << " " << bs << " " << cs << "\n";
					front_->push_back((*faces)[i]);
				}
				else if (as <= 0 && bs <= 0 && cs <= 0)
				{
					back_->push_back((*faces)[i]);
					//	std::cout << "whoo!\n";
				}
				else
				{
					double a = (*faces)[chosen]->vertex[0].nx;
					double b = (*faces)[chosen]->vertex[0].ny;
					double c = (*faces)[chosen]->vertex[0].nz;
					double d = -(a*(*faces)[chosen]->vertex[0].x + b*(*faces)[chosen]->vertex[0].y + c*(*faces)[chosen]->vertex[0].z);

					double x0 = (*faces)[i]->vertex[0].x;
					double y0 = (*faces)[i]->vertex[0].y;
					double z0 = (*faces)[i]->vertex[0].z;

					double x1 = (*faces)[i]->vertex[1].x;
					double y1 = (*faces)[i]->vertex[1].y;
					double z1 = (*faces)[i]->vertex[1].z;

					double x2 = (*faces)[i]->vertex[2].x;
					double y2 = (*faces)[i]->vertex[2].y;
					double z2 = (*faces)[i]->vertex[2].z;
					/*
					(*faces)[i]->vertex[0].r = r;
					(*faces)[i]->vertex[0].g = g;
					(*faces)[i]->vertex[0].b = k;
					(*faces)[i]->vertex[1].r = r;
					(*faces)[i]->vertex[1].g = g;
					(*faces)[i]->vertex[1].b = k;
					(*faces)[i]->vertex[2].r = r;
					(*faces)[i]->vertex[2].g = g;
					(*faces)[i]->vertex[2].b = k;
					*/
					double nx = (*faces)[i]->vertex[0].nx;
					double ny = (*faces)[i]->vertex[0].ny;
					double nz = (*faces)[i]->vertex[0].nz;

					//	std::cout << x0 << " " << x1 << " " << x2 << " --------" << std::endl;
						//get what case we are in
					int case_num = get_case_num(as, bs, cs);
					/*
					switch (case_num)
					{
					case 0:
					{
						//from a
						double t01 = -(a * x0 + b * y0 + c * z0 + d) / (a * (x1 - x0) + b * (y1 - y0) + c * (z1 - z0));
						double t02 = -(a * x0 + b * y0 + c * z0 + d) / (a * (x2 - x0) + b * (y2 - y0) + c * (z2 - z0));
						std::cout << t01 << " " << t02 << std::endl;
						double temp01_x = x0 + t01*(x1 - x0);
						double temp01_y = y0 + t01*(y1 - y0);
						double temp01_z = z0 + t01*(z1 - z0);

						double temp02_x = x0 + t02*(x2 - x0);
						double temp02_y = y0 + t02*(y2 - y0);
						double temp02_z = z0 + t02*(z2 - z0);
						//make the three triangles

						mesh::Vertex_c vert01(temp01_x, temp01_y, temp01_z, nx, ny, nz, 255, 0, 0);
						mesh::Vertex_c vert02(temp02_x, temp02_y, temp02_z, nx, ny, nz, 255, 0, 0);

						mesh::face_c * temp_f1 = new mesh::face_c((*faces)[i]->vertex[0], vert01, vert02);
						mesh::face_c * temp_f2 = new mesh::face_c((*faces)[i]->vertex[2], vert02, vert01);
						mesh::face_c * temp_f3 = new mesh::face_c((*faces)[i]->vertex[1], vert01, (*faces)[i]->vertex[2]);

						if (as >= 0)
						{
							front_->push_back(temp_f1);
							back_->push_back(temp_f2);
							back_->push_back(temp_f3);
						}
						else
						{
							back_->push_back(temp_f1);
							front_->push_back(temp_f2);
							front_->push_back(temp_f3);
						}
					}
						break;
					case 1:
					{
						//from b
						double t10 = -(a * x1 + b * y1 + c * z1 + d) / (a * (x0 - x1) + b * (y0 - y1) + c * (z0 - z1));
						double t12 = -(a * x1 + b * y1 + c * z1 + d) / (a * (x2 - x1) + b * (y2 - y1) + c * (z2 - z1));
						std::cout << t10 << " " << t12 << std::endl;
						double temp10_x = x1 + t10*(x0 - x1);
						double temp10_y = y1 + t10*(y0 - y1);
						double temp10_z = z1 + t10*(z0 - z1);

						double temp12_x = x1 + t12*(x2 - x1);
						double temp12_y = y1 + t12*(y2 - y1);
						double temp12_z = z1 + t12*(z2 - z1);
						//make the three triangles

						mesh::Vertex_c vert10(temp10_x, temp10_y, temp10_z, nx, ny, nz, 255, 0, 0);
						mesh::Vertex_c vert12(temp12_x, temp12_y, temp12_z, nx, ny, nz, 255, 0, 0);


						mesh::face_c * temp_f1 = new mesh::face_c((*faces)[i]->vertex[1], vert12, vert10);
						mesh::face_c * temp_f2 = new mesh::face_c((*faces)[i]->vertex[2], vert10, vert12);
						mesh::face_c * temp_f3 = new mesh::face_c((*faces)[i]->vertex[0], vert10, (*faces)[i]->vertex[2]);

						if (bs >= 0)
						{
							front_->push_back(temp_f1);
							back_->push_back(temp_f2);
							back_->push_back(temp_f3);
						}
						else
						{
							back_->push_back(temp_f1);
							front_->push_back(temp_f2);
							front_->push_back(temp_f3);
						}
					}
						break;
					case 2:
					{
						//from c
						double t20 = -(a * x2 + b * y2 + c * z2 + d) / (a * (x0 - x2) + b * (y0 - y2) + c * (z0 - z2));
						double t21 = -(a * x2 + b * y2 + c * z2 + d) / (a * (x1 - x2) + b * (y1 - y2) + c * (z1 - z2));
						std::cout << t20 << " " << t21 << std::endl;
						double temp20_x = x2 + t20*(x0 - x2);
						double temp20_y = y2 + t20*(y0 - y2);
						double temp20_z = z2 + t20*(z0 - z2);

						double temp21_x = x2 + t21*(x1 - x2);
						double temp21_y = y2 + t21*(y1 - y2);
						double temp21_z = z2 + t21*(z1 - z2);
						//make the three triangles

						mesh::Vertex_c vert20(temp20_x, temp20_y, temp20_z, nx, ny, nz, 255, 0, 0);
						mesh::Vertex_c vert21(temp21_x, temp21_y, temp21_z, nx, ny, nz, 255, 0, 0);


						mesh::face_c * temp_f1 = new mesh::face_c((*faces)[i]->vertex[2], vert20, vert21);
						mesh::face_c * temp_f2 = new mesh::face_c((*faces)[i]->vertex[1], vert21, vert20);
						mesh::face_c * temp_f3 = new mesh::face_c((*faces)[i]->vertex[0], vert20, (*faces)[i]->vertex[1]);

						if (cs >= 0)
						{
							front_->push_back(temp_f1);
							back_->push_back(temp_f2);
							back_->push_back(temp_f3);
						}
						else
						{
							back_->push_back(temp_f1);
							front_->push_back(temp_f2);
							front_->push_back(temp_f3);
						}
					}
						break;
					}


					*/
					//	std::cout << as << " " << bs << " " << cs << "\n";
					if (case_num == 0) //same_sign(bs,cs))//signum(bs) == signum(cs))
					{
						//	/*
							//from a
						double t01 = -(a * x0 + b * y0 + c * z0 + d) / (a * (x1 - x0) + b * (y1 - y0) + c * (z1 - z0));
						double t02 = -(a * x0 + b * y0 + c * z0 + d) / (a * (x2 - x0) + b * (y2 - y0) + c * (z2 - z0));

						if (!is_good_value(t01) || !is_good_value(t02))
						{
							continue;
						}

						//std::cout << t01 << " " << t02 << std::endl;
						double temp01_x = x0 + t01*(x1 - x0);
						double temp01_y = y0 + t01*(y1 - y0);
						double temp01_z = z0 + t01*(z1 - z0);

						double temp02_x = x0 + t02*(x2 - x0);
						double temp02_y = y0 + t02*(y2 - y0);
						double temp02_z = z0 + t02*(z2 - z0);
						//make the three triangles
						mesh::Vertex_c vert01(temp01_x, temp01_y, temp01_z, nx, ny, nz, 255, 0, 0);
						mesh::Vertex_c vert02(temp02_x, temp02_y, temp02_z, nx, ny, nz, 255, 0, 0);

						mesh::face_c * temp_f1 = new mesh::face_c((*faces)[i]->vertex[0], vert01, vert02);
						mesh::face_c * temp_f2 = new mesh::face_c((*faces)[i]->vertex[2], vert02, vert01);
						mesh::face_c * temp_f3 = new mesh::face_c((*faces)[i]->vertex[1], vert01, (*faces)[i]->vertex[2]);

						if (as >= 0)
						{
							front_->push_back(temp_f1);
							back_->push_back(temp_f2);
							back_->push_back(temp_f3);
						}
						else
						{
							back_->push_back(temp_f1);
							front_->push_back(temp_f2);
							front_->push_back(temp_f3);
						}
						//*/
					}
					else if (case_num == 1)//same_sign(as,cs))//signum(as) == signum(cs))
					{
						//	/*
							//from b
						double t10 = -(a * x1 + b * y1 + c * z1 + d) / (a * (x0 - x1) + b * (y0 - y1) + c * (z0 - z1));
						double t12 = -(a * x1 + b * y1 + c * z1 + d) / (a * (x2 - x1) + b * (y2 - y1) + c * (z2 - z1));

						if (!is_good_value(t10) || !is_good_value(t12))
						{
							continue;
						}
						//	std::cout << t10 << " " << t12 << std::endl;
						double temp10_x = x1 + t10*(x0 - x1);
						double temp10_y = y1 + t10*(y0 - y1);
						double temp10_z = z1 + t10*(z0 - z1);

						double temp12_x = x1 + t12*(x2 - x1);
						double temp12_y = y1 + t12*(y2 - y1);
						double temp12_z = z1 + t12*(z2 - z1);
						//make the three triangles

						mesh::Vertex_c vert10(temp10_x, temp10_y, temp10_z, nx, ny, nz, 255, 0, 0);
						mesh::Vertex_c vert12(temp12_x, temp12_y, temp12_z, nx, ny, nz, 255, 0, 0);


						mesh::face_c * temp_f1 = new mesh::face_c((*faces)[i]->vertex[1], vert12, vert10);
						mesh::face_c * temp_f2 = new mesh::face_c((*faces)[i]->vertex[2], vert10, vert12);
						mesh::face_c * temp_f3 = new mesh::face_c((*faces)[i]->vertex[0], vert10, (*faces)[i]->vertex[2]);

						if (bs >= 0)
						{
							front_->push_back(temp_f1);
							back_->push_back(temp_f2);
							back_->push_back(temp_f3);
						}
						else
						{
							back_->push_back(temp_f1);
							front_->push_back(temp_f2);
							front_->push_back(temp_f3);
						}
						//	*/
					}
					else if (case_num == 2)//same_sign(as,bs))//signum(as) == signum(bs))
					{
						//	/*

							//from c
						double t20 = -(a * x2 + b * y2 + c * z2 + d) / (a * (x0 - x2) + b * (y0 - y2) + c * (z0 - z2));
						double t21 = -(a * x2 + b * y2 + c * z2 + d) / (a * (x1 - x2) + b * (y1 - y2) + c * (z1 - z2));
						//	std::cout << t20 << " " << t21 << std::endl;
						if (!is_good_value(t20) || !is_good_value(t21))
						{
							continue;
						}
						double temp20_x = x2 + t20*(x0 - x2);
						double temp20_y = y2 + t20*(y0 - y2);
						double temp20_z = z2 + t20*(z0 - z2);

						double temp21_x = x2 + t21*(x1 - x2);
						double temp21_y = y2 + t21*(y1 - y2);
						double temp21_z = z2 + t21*(z1 - z2);
						//make the three triangles

						mesh::Vertex_c vert20(temp20_x, temp20_y, temp20_z, nx, ny, nz, 255, 0, 0);
						mesh::Vertex_c vert21(temp21_x, temp21_y, temp21_z, nx, ny, nz, 255, 0, 0);


						mesh::face_c * temp_f1 = new mesh::face_c((*faces)[i]->vertex[2], vert20, vert21);
						mesh::face_c * temp_f2 = new mesh::face_c((*faces)[i]->vertex[1], vert21, vert20);
						mesh::face_c * temp_f3 = new mesh::face_c((*faces)[i]->vertex[0], vert20, (*faces)[i]->vertex[1]);

						if (cs >= 0)
						{
							front_->push_back(temp_f1);
							back_->push_back(temp_f2);
							back_->push_back(temp_f3);
						}
						else
						{
							back_->push_back(temp_f1);
							front_->push_back(temp_f2);
							front_->push_back(temp_f3);
						}
						//*/
					}
					else if (case_num == 3)
					{
						//a is zero...
						double t12 = -(a * x1 + b * y1 + c * z1 + d) / (a * (x2 - x1) + b * (y2 - y1) + c * (z2 - z1));
						if (!is_good_value(t12))
						{
							continue;
						}
						double temp12_x = x1 + t12*(x2 - x1);
						double temp12_y = y1 + t12*(y2 - y1);
						double temp12_z = z1 + t12*(z2 - z1);

						//make the two triangles

						mesh::Vertex_c vert12(temp12_x, temp12_y, temp12_z, nx, ny, nz, 255, 0, 0);


						mesh::face_c * temp_f1 = new mesh::face_c((*faces)[i]->vertex[0], (*faces)[i]->vertex[1], vert12);
						mesh::face_c * temp_f2 = new mesh::face_c((*faces)[i]->vertex[0], (*faces)[i]->vertex[2], vert12);

						if (bs >= 0)
						{
							front_->push_back(temp_f1);
							back_->push_back(temp_f2);
						}
						else
						{
							back_->push_back(temp_f1);
							front_->push_back(temp_f2);
						}
					}
					else if (case_num == 4)
					{
						//b is zero...
						double t02 = -(a * x0 + b * y0 + c * z0 + d) / (a * (x2 - x0) + b * (y2 - y0) + c * (z2 - z0));
						if (!is_good_value(t02))
						{
							continue;
						}
						double temp02_x = x0 + t02*(x2 - x0);
						double temp02_y = y0 + t02*(y2 - y0);
						double temp02_z = z0 + t02*(z2 - z0);

						//make the two triangles

						mesh::Vertex_c vert02(temp02_x, temp02_y, temp02_z, nx, ny, nz, 255, 0, 0);


						mesh::face_c * temp_f1 = new mesh::face_c((*faces)[i]->vertex[1], (*faces)[i]->vertex[0], vert02);
						mesh::face_c * temp_f2 = new mesh::face_c((*faces)[i]->vertex[1], (*faces)[i]->vertex[2], vert02);

						if (as >= 0)
						{
							front_->push_back(temp_f1);
							back_->push_back(temp_f2);
						}
						else
						{
							back_->push_back(temp_f1);
							front_->push_back(temp_f2);
						}
					}
					else if (case_num == 5)
					{
						//c is zero...
						double t10 = -(a * x1 + b * y1 + c * z1 + d) / (a * (x0 - x1) + b * (y0 - y1) + c * (z0 - z1));
						if (!is_good_value(t10))
						{
							continue;
						}
						double temp10_x = x1 + t10*(x0 - x1);
						double temp10_y = y1 + t10*(y0 - y1);
						double temp10_z = z1 + t10*(z0 - z1);

						//make the two triangles

						mesh::Vertex_c vert10(temp10_x, temp10_y, temp10_z, nx, ny, nz, 255, 0, 0);


						mesh::face_c * temp_f1 = new mesh::face_c((*faces)[i]->vertex[2], (*faces)[i]->vertex[0], vert10);
						mesh::face_c * temp_f2 = new mesh::face_c((*faces)[i]->vertex[2], (*faces)[i]->vertex[1], vert10);

						if (as >= 0)
						{
							front_->push_back(temp_f1);
							back_->push_back(temp_f2);
						}
						else
						{
							back_->push_back(temp_f1);
							front_->push_back(temp_f2);
						}
					}

					/*
					//for line 1 (one to be split)
					float x1 = (*lines)[i]->a.x;		//putting the values into a little bit nicer format. (x1 is just a tad bit easier
					float y1 = (*lines)[i]->a.y;		//	to work with than (*lines)[i]->a.x )
					float x2 = (*lines)[i]->b.x;
					float y2 = (*lines)[i]->b.y;

					//line 2 is current line
					float x3 = (*lines)[0]->a.x;
					float y3 = (*lines)[0]->a.y;
					float x4 = (*lines)[0]->b.x;
					float y4 = (*lines)[0]->b.y;

					//point of interesction
					float temp_x = ((x1*y2 - y1*x2)*(x3 - x4) - (x1 - x2)*(x3*y4 - y3*x4)) / ((x1 - x2)*(y3 - y4) - (y1 - y2)*(x3 - x4));		//found with algebra and such
					float temp_y = ((x1*y2 - y1*x2)*(y3 - y4) - (y1 - y2)*(x3*y4 - y3*x4)) / ((x1 - x2)*(y3 - y4) - (y1 - y2)*(x3 - x4));
					pointdef pt(temp_x, temp_y);


					if (as > 0)
					{
						//if the the end point A is in front of the line

						//We order it so that it goes pt (on line) to A (in front of line) on one side...
						linedef *la = new linedef(pt, (*lines)[i]->a, sf::Color::Yellow);
						//and B to pt on the other side.
						linedef *lb = new linedef((*lines)[i]->b, pt, sf::Color::Blue);

						//We explicitly set the 'normals' to be that of the original line so that it conserves expected behavior
						//	(these lines still have the same 'inside' as the parents)
						la->an = (*lines)[i]->an;
						la->bn = (*lines)[i]->bn;
						lb->an = (*lines)[i]->an;
						lb->bn = (*lines)[i]->bn;
						front_->push_back(la);		//so if A is in front, add it to front.
						back_->push_back(lb);		//Add B to back
					}
					else
					{
						//B is in front of the line
						//line_seg goes from A to pt (because A is behind this time)
						linedef *la = new linedef((*lines)[i]->a, pt, sf::Color::Blue);
						//other goes from pt to B
						linedef *lb = new linedef(pt, (*lines)[i]->b, sf::Color::Yellow);
						la->an = (*lines)[i]->an;
						la->bn = (*lines)[i]->bn;
						lb->an = (*lines)[i]->an;
						lb->bn = (*lines)[i]->bn;
						front_->push_back(lb);		//so if B is in front, add it to front.
						back_->push_back(la);		//Add A to back
					}
					*/
				}
			}

			if(settings::degerneracy_culling)
			{
				//purge the faces. Cull any 'bad' faces
				std::vector<mesh::face_c*> *front_temp = new std::vector<mesh::face_c*>;
				std::vector<mesh::face_c*> *back_temp = new std::vector<mesh::face_c*>;
				for (int i = 0; i < front_->size(); i++)
				{
					//check if face is degerate. i.e. has coplanar points.
					if (!degenerate((*front_)[i]))
					{
						front_temp->push_back((*front_)[i]);
					}
				}
				front_ = front_temp;
				for (int i = 0; i < back_->size(); i++)
				{
					//check if face is degerate. i.e. has coplanar points.
					if (!degenerate((*back_)[i]))
					{
						back_temp->push_back((*back_)[i]);
					}
				}
				back_ = back_temp;
			}
			//Checks first to see if it has any elements
			if (front_->size() > 0)
			{
				//create a new node
				node *nd = new node();
				//make this current node point to the new node as the "front".
				front = nd;
				//make the new node point to this node as "parent"
				nd->parent = this;
				//pass the recursive construct on the new node. Pass all the lines in front of this node.
				nd->construct(front_);
			}
			if (back_->size() > 0)
			{
				//create a new node
				node *nd = new node();
				//make this current node point to the new node as the "back".
				back = nd;
				//make the new node point to this node as "parent"
				nd->parent = this;
				//pass the recursive construct on the new node. Pass all the lines behind this node.
				nd->construct(back_);
			}
			delete front_;
			delete back_;
			//"just an observation: it is "behind" but "in front of". Is there a similar structure way to say these? sorry. just tangential...
		}

		//each node has four pointers to work with:
		mesh::face_c *current;	//current points to the line of the node. This is what gives the node any useability. The line is how the tree traversing works
		node *parent;		//parent is a pointer to the parent node. So the node from which this node was created. NULL if node is root
		node *front;		//front points to the node that is in front of this node. See the construct function for further details
		node *back;			//back points to the node that is behind this node. See the construct function for further details
		TYPE type;
	};

	bool face_collide(mesh::face_c * face, float x_, float y_, float z_, float radius, float height)
	{
		//float d = fabs(side_dist(face, x_, y_, z_));
		if (/*d <= radius*/ true)
		{
			
			float max_x = fmax(face->vertex[0].x, fmax(face->vertex[1].x, face->vertex[2].x));
			float min_x = fmin(face->vertex[0].x, fmin(face->vertex[1].x, face->vertex[2].x));
			
			float max_y = fmax(face->vertex[0].y, fmax(face->vertex[1].y, face->vertex[2].y));
			float min_y = fmin(face->vertex[0].y, fmin(face->vertex[1].y, face->vertex[2].y));

			float max_z = fmax(face->vertex[0].z, fmax(face->vertex[1].z, face->vertex[2].z));
			float min_z = fmin(face->vertex[0].z, fmin(face->vertex[1].z, face->vertex[2].z));
			
		//TODO: ====================================================================   add proper hitting

			if (x_ - radius <= max_x && x_ + radius >= min_x &&  y_ - height <= max_y && y_ + height >= min_y &&  z_ - radius <= max_z && z_ + radius >= min_z)
			{
				return 1;
			}
		}
		return 0;
	}
	
	double horiz_side_dist(double x0, double z0, double x1, double z1, double x_, double z_)
	{
		double an = z0 - z1;	//Linear algebra. The determinant of the line.
		double bn = x1 - x0;
		float n = 1.f / sqrt(an * an + bn * bn);		//normalize the length
		an = an * n;
		bn = bn * n;
		float c = -(an * x0 + bn * z0);
		float d = an*x_ + bn*z_ + c;
		return d;
	}

	bool new_collide(node *node_, float x_, float y_, float z_, float radius, float height)
	{
		//This is a simple "box collider" that checks the cylinder' box volume against the triangle's box volume.
		//when false, we know that it is false, but true is not always true.
		if (!face_collide(node_->current, x_, y_, z_, radius, height)) return false;
		
		if (node_->type == TYPE::HORIZONTAL)
		{
			//check if inside the triangle
			double xa = node_->current->vertex[0].x;
			double za = node_->current->vertex[0].z;

			double xb = node_->current->vertex[1].x;
			double zb = node_->current->vertex[1].z;

			double xc = node_->current->vertex[2].x;
			double zc = node_->current->vertex[2].z;

			//Let's do a quick size test. If the radius is very much bigger, we can assume it is hitting. Also, It's not working right, so I have to try something.
	//		double cent_x = (xa + xb + xc) / 3.0;
	//		double cent_z = (za + zb + zc) / 3.0;
	//		double pseudo_radius = (sqrt((cent_x - xa)*(cent_x - xa) + (cent_z - za)*(cent_z - za)) + sqrt((cent_x - xb)*(cent_x - xb) + (cent_z - zb)*(cent_z - zb)) + sqrt((cent_x - xc)*(cent_x - xc) + (cent_z - zc)*(cent_z - zc)))/3.0;
	/////		if (radius > pseudo_radius)
//			{
			//	return true;
	//		}

			double sd1 = horiz_side_dist(xa, za, xb, zb, x_, z_);
			double sd2 = horiz_side_dist(xb, zb, xc, zc, x_, z_);
			double sd3 = horiz_side_dist(xc, zc, xa, za, x_, z_);

			//if (signum(sd1) == signum(sd2) == signum(sd3))
			if (signum(sd1) == signum(sd2) && signum(sd2) == signum(sd3) && signum(sd3) == signum(sd1))
			{
				//if it is on the same side of all the lines, it has to be inside, so it is a hit.
				return true;
			}

			if ( (fabs(sd1) <= radius && sd1 > 0.0) || (fabs(sd2) <= radius && sd2 > 0.0) || (fabs(sd3) <= radius && sd3 > 0.0) )
			{
			//	std::cout << sd1 << " " << sd2 << " " << sd3 << " \n";
				return true;
			}

			/*
			if (fabs(sd1) <= radius)
			{
				double a1 = xb*xb + zb*zb;
				double b1 = 2.0 * (xb*(xa - x_) + zb*(za - z_));
				double c1 = (xa - x_)*(xa - x_) + (za - z_)*(za - z_) - radius*radius;
				double d1 = b1*b1 - 4.0*a1*c1;
				//test if there are any solutions
				if (d1 >= 0)
				{
					//so there are solutions...   now we need to see if the solutions are within the bounds.
					double t0 = (-b1 + sqrt(d1)) / (2.0 * a1);
					double t1 = (-b1 - sqrt(d1)) / (2.0 * a1);
					if ((t0 <= 1.0 && t0 >= 0.0) || (t1 <= 1.0 && t1 >= 0.0))
					{
						//hitting it
						return true;
					}
				}
			}


			if (fabs(sd2) <= radius)
			{
				double a2 = xc*xc + zc*zc;
				double b2 = 2.0 * (xc*(xb - x_) + zc*(zb - z_));
				double c2 = (xb - x_)*(xb - x_) + (zb - z_)*(zb - z_) - radius*radius;
				double d2 = b2*b2 - 4.0*a2*c2;
				//test if there are any solutions
				if (d2 >= 0)
				{
					//so there are solutions...   now we need to see if the solutions are within the bounds.
					double t0 = (-b2 + sqrt(d2)) / (2.0 * a2);
					double t1 = (-b2 - sqrt(d2)) / (2.0 * a2);
					if ((t0 <= 1.0 && t0 >= 0.0) || (t1 <= 1.0 && t1 >= 0.0))
					{
						//hitting it
						return true;
					}
				}
			}

			if (fabs(sd3) <= radius)
			{
				double a3 = xa*xa + za*za;
				double b3 = 2.0 * (xa*(xc - x_) + za*(zc - z_));
				double c3 = (xc - x_)*(xc - x_) + (zc - z_)*(zc - z_) - radius*radius;
				double d3 = b3*b3 - 4.0*a3*c3;
				//test if there are any solutions
				if (d3 >= 0)
				{
					//so there are solutions...   now we need to see if the solutions are within the bounds.
					double t0 = (-b3 + sqrt(d3)) / (2.0 * a3);
					double t1 = (-b3 - sqrt(d3)) / (2.0 * a3);
					if ((t0 <= 1.0 && t0 >= 0.0) || (t1 <= 1.0 && t1 >= 0.0))
					{
						//hitting it
						return true;
					}
				}
			}
			*/

			/*
			if (fabs(sd1) <= radius || fabs(sd2) <= radius || fabs(sd3) <= radius)
			{
				std::cout << sd1 << " " << sd2 << " " << sd3 << " \n";
				return true;
			}
			*/

			//if inconclusive
			//1 is a-b ; 2 is b-c ; 3 is c-a

			/*
			//FOR a b
			double a1 = xb*xb + zb*zb;
			double b1 = 2.0 * (xb*(xa - x_) + zb*(za - z_));
			double c1 = (xa - x_)*(xa - x_) + (za - z_)*(za - z_) - radius*radius;
			double d1 = b1*b1 - 4.0*a1*c1;
			//test if there are any solutions
			if (d1 >= 0)
			{
				//so there are solutions...   now we need to see if the solutions are within the bounds.
				double t0 = (-b1 + sqrt(d1)) / (2.0 * a1);
				double t1 = (-b1 - sqrt(d1)) / (2.0 * a1);
				if ((t0 <= 1.0 && t0 >= 0.0) || (t1 <= 1.0 && t1 >= 0.0))
				{
					//hitting it
					return true;
				}
			}

			//FOR b c
			double a2 = xc*xc + zc*zc;
			double b2 = 2.0 * (xc*(xb - x_) + zc*(zb - z_));
			double c2 = (xb - x_)*(xb - x_) + (zb - z_)*(zb - z_) - radius*radius;
			double d2 = b2*b2 - 4.0*a2*c2;
			//test if there are any solutions
			if (d2 >= 0)
			{
				//so there are solutions...   now we need to see if the solutions are within the bounds.
				double t0 = (-b2 + sqrt(d2)) / (2.0 * a2);
				double t1 = (-b2 - sqrt(d2)) / (2.0 * a2);
				if ((t0 <= 1.0 && t0 >= 0.0) || (t1 <= 1.0 && t1 >= 0.0))
				{
					//hitting it
					return true;
				}
			}

			//FOR c a
			double a3 = xa*xa + za*za;
			double b3 = 2.0 * (xa*(xc - x_) + za*(zc - z_));
			double c3 = (xc - x_)*(xc - x_) + (zc - z_)*(zc - z_) - radius*radius;
			double d3 = b3*b3 - 4.0*a3*c3;
			//test if there are any solutions
			if (d3 >= 0)
			{
				//so there are solutions...   now we need to see if the solutions are within the bounds.
				double t0 = (-b3 + sqrt(d3)) / (2.0 * a3);
				double t1 = (-b3 - sqrt(d3)) / (2.0 * a3);
				if ((t0 <= 1.0 && t0 >= 0.0) || (t1 <= 1.0 && t1 >= 0.0))
				{
					//hitting it
					return true;
				}
			}
			//*/
			//Then I guess we aren't hitting it.
			return false;
		}
		else if (node_->type == TYPE::SEMI)
		{
			//Needs full work.
		
			return true;
		}
		else
		{
			float d = fabs(side_dist(node_->current, x_, y_, z_));
			if (d < radius)
				return true;
		}
		return false;
	}

	bool new_collide(mesh::face_c * face, float x_, float y_, float z_, float radius, float height)
	{
		if (!face_collide(face, x_, y_, z_, radius, height)) return false;

		if (face->vertex[0].ny != 0.0)
		{
			double xa = face->vertex[0].x;
			double za = face->vertex[0].z;

			double xb = face->vertex[1].x;
			double zb = face->vertex[1].z;

			double xc = face->vertex[2].x;
			double zc = face->vertex[2].z;

			double sd1 = horiz_side_dist(xa, za, xb, zb, x_, z_);
			double sd2 = horiz_side_dist(xb, zb, xc, zc, x_, z_);
			double sd3 = horiz_side_dist(xc, zc, xa, za, x_, z_);

			if (signum(sd1) == signum(sd2) && signum(sd2) == signum(sd3) && signum(sd3) == signum(sd1))
			{
				return true;
			}

			if ((fabs(sd1) <= radius && sd1 > 0.0) || (fabs(sd2) <= radius && sd2 > 0.0) || (fabs(sd3) <= radius && sd3 > 0.0))
			{
				return true;
			}
			return false;
		}
		else
		{
			float d = fabs(side_dist(face, x_, y_, z_));
			if (d < radius)
				return true;
		}
		return false;
	}

	double * projection(double x, double y, double z, double cosT, double sinT, double cosP, double sinP)
	{
		double zp = -sinT*y + cosT * z;

		double xpp = cosP * x - sinP * zp;
		double zpp = x * sinP + zp * cosP;

		double out[3];
		out[0] = xpp;
		out[1] = 0;
		out[2] = zpp;
		return &out[0];
	}

	bool arbit_collide(node *node_, float x_, float y_, float z_, float radius)
	{
		//This is a simple "box collider" that checks the cylinder' box volume against the triangle's box volume.
		//when false, we know that it is false, but true is not always true.
		if (!face_collide(node_->current, x_, y_, z_, radius, radius)) return false;


		//We fudge the vertices so it looks horizontal
		//We project it down into Horizontal then check

		//check if inside the triangle


		//Distance to plane
		float d = fabs(side_dist(node_->current, x_, y_, z_));
		if (d > radius)
			return false;

		//get the normals
		double nx = node_->current->vertex[0].nx;
		double ny = node_->current->vertex[0].ny;
		double nz = node_->current->vertex[0].nz;

		//We calculate the radius of the projected circle
		double proj_radius = sqrt(radius * radius - d*d);

		//We find the center of the circle on the plane
		//	To do this we subtract the vector normal to the plane with distance d from the center of the circle
		double cx = x_ - nx*d;
		double cy = y_ - ny*d;
		double cz = z_ - nz*d;

		//Now we have to bring everything into the right coordinate frame
		//First we bring everything into the entity's frame
		//(x_,y_,z_) is new origin. translate everything by <-x.-y,-z>

		//extract vertecies
		cx -= x_;
		cy -= y_;
		cz -= z_;

		double xa = node_->current->vertex[0].x - x_;
		double ya = node_->current->vertex[0].y - y_;
		double za = node_->current->vertex[0].z - z_;

		double xb = node_->current->vertex[1].x - x_;
		double yb = node_->current->vertex[1].y - y_;
		double zb = node_->current->vertex[1].z - z_;

		double xc = node_->current->vertex[2].x - x_;
		double yc = node_->current->vertex[2].y - y_;
		double zc = node_->current->vertex[2].z - z_;

		//then we rotate around to have all the points on the ground.

		double theta = atan2(nz, nx);
		double phi = asin(ny / sqrt(nx*nx + nz*nz));

		//Then apply rotation
		double cosT = cos(theta);
		double sinT = sin(theta);
		double cosP = cos(phi);
		double sinP = sin(phi);

		double * cnMM = projection(cx, cy, cz, cosT, sinT, cosP, sinP);
		cx = cnMM[0];
		cy = cnMM[1];
		cz = cnMM[2];

		double * aMM = projection(xa, ya, za, cosT, sinT, cosP, sinP);
		xa = aMM[0];
		ya = aMM[1];
		za = aMM[2];

		double * bMM = projection(xb, yb, zb, cosT, sinT, cosP, sinP);
		xb = bMM[0];
		yb = bMM[1];
		zb = bMM[2];

		double * cMM = projection(xc, yc, zc, cosT, sinT, cosP, sinP);
		xc = cMM[0];
		yc = cMM[1];
		zc = cMM[2];

		double sd1 = horiz_side_dist(xa, za, xb, zb, cx, cz);
		double sd2 = horiz_side_dist(xb, zb, xc, zc, cx, cz);
		double sd3 = horiz_side_dist(xc, zc, xa, za, cx, cz);

		//if (signum(sd1) == signum(sd2) == signum(sd3))
		if (signum(sd1) == signum(sd2) && signum(sd2) == signum(sd3) && signum(sd3) == signum(sd1))
		{
			//if it is on the same side of all the lines, it has to be inside, so it is a hit.
			return true;
		}

		if ((fabs(sd1) <= radius && sd1 > 0.0) || (fabs(sd2) <= radius && sd2 > 0.0) || (fabs(sd3) <= radius && sd3 > 0.0))
		{
			//	std::cout << sd1 << " " << sd2 << " " << sd3 << " \n";
			return true;
		}
		//Then I guess we aren't hitting it.
		return false;
		
	}

	bool arbit_collide_q(node *node_, float x_, float y_, float z_, float radius)
	{
		//This is a simple "box collider" that checks the cylinder' box volume against the triangle's box volume.
		//when false, we know that it is false, but true is not always true.
		if (!face_collide(node_->current, x_, y_, z_, radius, radius)) return false;


		//Distance to plane
		float d = fabs(side_dist(node_->current, x_, y_, z_));
		if (d > radius)
			return false;

		double xa = node_->current->vertex[0].x;
		double ya = node_->current->vertex[0].y;
		double za = node_->current->vertex[0].z;

		double xb = node_->current->vertex[1].x;
		double yb = node_->current->vertex[1].y;
		double zb = node_->current->vertex[1].z;

		double xc = node_->current->vertex[2].x;
		double yc = node_->current->vertex[2].y;
		double zc = node_->current->vertex[2].z;

		//we project the scene into three orthogonal projections. Then we see if all of them intersect

		int num_ortho_collides = 0;

		//double horiz_side_dist(double x0, double z0, double x1, double z1, double x_, double z_)

		//Project onto XZ (y=0);
		double sd1 = horiz_side_dist(xa, za, xb, zb, x_, z_);
		double sd2 = horiz_side_dist(xb, zb, xc, zc, x_, z_);
		double sd3 = horiz_side_dist(xc, zc, xa, za, x_, z_);

		if (signum(sd1) == signum(sd2) && signum(sd2) == signum(sd3) && signum(sd3) == signum(sd1))
		{
			num_ortho_collides++;
		} 
		else if ((fabs(sd1) <= radius && sd1 > 0.0) || (fabs(sd2) <= radius && sd2 > 0.0) || (fabs(sd3) <= radius && sd3 > 0.0))
		{
			num_ortho_collides++;
		}

		//project onto XY (z=0)
		sd1 = horiz_side_dist(xa, ya, xb, yb, x_, y_);
		sd2 = horiz_side_dist(xb, yb, xc, yc, x_, y_);
		sd3 = horiz_side_dist(xc, yc, xa, ya, x_, y_);

		if (signum(sd1) == signum(sd2) && signum(sd2) == signum(sd3) && signum(sd3) == signum(sd1))
		{
			num_ortho_collides++;
		}
		else if ((fabs(sd1) <= radius && sd1 > 0.0) || (fabs(sd2) <= radius && sd2 > 0.0) || (fabs(sd3) <= radius && sd3 > 0.0))
		{
			num_ortho_collides++;
		}

		//project onto YZ (x=0)
		sd1 = horiz_side_dist(ya, za, yb, zb, y_, z_);
		sd2 = horiz_side_dist(yb, zb, yc, zc, y_, z_);
		sd3 = horiz_side_dist(yc, zc, ya, za, y_, z_);

		if (signum(sd1) == signum(sd2) && signum(sd2) == signum(sd3) && signum(sd3) == signum(sd1))
		{
			num_ortho_collides++;
		}
		else if ((fabs(sd1) <= radius && sd1 > 0.0) || (fabs(sd2) <= radius && sd2 > 0.0) || (fabs(sd3) <= radius && sd3 > 0.0))
		{
			num_ortho_collides++;
		}

		if (num_ortho_collides == 3)
			return true;
		return false;

		/*
		double sd1 = horiz_side_dist(xa, za, xb, zb, cx, cz);
		double sd2 = horiz_side_dist(xb, zb, xc, zc, cx, cz);
		double sd3 = horiz_side_dist(xc, zc, xa, za, cx, cz);

		if (signum(sd1) == signum(sd2) && signum(sd2) == signum(sd3) && signum(sd3) == signum(sd1))
		{
			return true;
		} 
		else if ((fabs(sd1) <= radius && sd1 > 0.0) || (fabs(sd2) <= radius && sd2 > 0.0) || (fabs(sd3) <= radius && sd3 > 0.0))
		{
			return true;
		}
		*/
		//Then I guess we aren't hitting it.		
	}

	bool collide(node *node_i, float x_, float y_, float z_, float radius, float height, float check_y, TYPE type)
	{
		if (node_i->current == nullptr)
			return false;
		node *node_ = &(*node_i);			//we want to copy the address, but not chnage the pointer

											//goes through to find the last node in the tree.
		while (1)	//infinite loop becuase it is manually broken. It is because the tree is recursive, but this method is loopy
		{
			//style: paint what lines you go through as Cyan to be able to visually trace the algorithm
#ifdef SHOW_NORMALS
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glBegin(GL_TRIANGLES);
				glVertex3f(node_->current->position[0] , node_->current->position[1]  , node_->current->position[2] );
				glVertex3f(node_->current->position[0]  + 3*node_->current->normal[0] , node_->current->position[1]  + 3 * node_->current->normal[1] , node_->current->position[2]  + 3 * node_->current->normal[2] );
				glVertex3f(node_->current->position[0] + 0.1, node_->current->position[1]   + 0.1, node_->current->position[2]   + 0.1);
			glEnd();
			glColor3f(0, 0, 0);
#endif
			//if (face_collide(node_->current, x_, y_, z_, radius))
			if (new_collide(node_, x_, y_, z_, radius, height)){
				if(node_->type == type || node_->type == TYPE::SEMI)
					return true;
			}
			if (side_dist(node_->current, x_, check_y, z_) >= 0)
			{
				if (!node_->front)			
				{
					break;					
				}
				node_ = node_->front;
			}
			else
			{
				//if the point is behind
				if (!node_->back)			//if the node doesn't contain a back it is a null pointer, so null check.
				{
					break;					//break if it doesn't (the last node checked is the last valid node)
				}
				//but if it does contain a back, then set the current node to that back node. 
				node_ = node_->back;
			}
			//loops back to the top and runs the code for the new node_ (either front or back node)
		}

		//For a description of regions, see my emails. Or send me a new email at 181095@whrhs-stu.org 
		//	Working under the impression you understand the logic

		/*
		node *region = &(*node_);				//put the memory address of node_'s node in region, but don't alias
												//We copy the current node so that we can go back up the tree and still know where the last node is.


												//go back to find the region's top node.
		while (0)//1
		{
			//style: paint the line magenta so that you can visualy trace the algorithm
			
			//checks conditions that would end the region
			if (!region->parent)
			{
				//if no parent, then the region is root and break.
				break;
			}
			if (region->parent->back)
			{
				//more of a null check, but it first determines that its parent has a back first...
				if (region == region->parent->back)
				{
					//but if the current region (node) is the same node as the parents back, then stop. 
					//	Again, this is discussed in the logic of the region.
					break;
				}
			}
			//but if no conditions exist that make it the top node in the region, make the region the parent
			region = region->parent;
			//then loop back up and go through again with the parent.
		}

		//although the function is called collision, there is no collision check... It requires too much geometry and linear algebra for a tuesday night.
		//	But the preceding code does get the top of the region (region) and bottom of the region (node_). 


		//For the actual collisions
		//first checks the last node in the region node_
		if (line_collide(node_->current, x_, y_, radius))
		{
			return true;
		}


		while (region != node_)
		{
			//check stuff
			if (line_collide(region->current, x_, y_, radius))
			{
				return true;
			}
			region = region->front;
		}



		// The following is all depreciated trial code. I kept it because I'm a code horder, but it is garbage and I will probably delete some time:
		//	Just don't bother going through it

		/*	while (1)
		{
		node_->current->color = sf::Color::Cyan;
		if (side(node_->current,x_,y_))
		{
		//front
		if (node_->front)
		{
		node_ = node_->front;
		}
		else
		{
		break;
		}
		}
		else
		{
		//back
		if (node_->back)
		{
		node_ = node_->back;
		}
		else
		{
		break;
		}
		}
		}


		while (1)
		{
		break;
		/*
		if (!node_)
		break;

		if (node_->front)
		{
		node_->current->color = sf::Color::Yellow;
		node_ = node_->front;
		}
		else
		{
		break;
		}
		*/

		/*
		if (!side(node_->current, x_, y_))
		{
		//front
		//	std::cout << "prpe";
		//std::cout << node_->front.current.a.x << "\n";
		if (!node_->front)
		{
		break;
		}
		//std::cout << "front\n";
		node_->current->color = sf::Color::Blue;
		node_ = node_->front;
		}
		else
		{
		//back
		//	std::cout << "prpe";
		if (!node_->back)
		{
		break;
		}
		//std::cout << "back\n";
		node_->current->color = sf::Color::Yellow;
		node_ = node_->back;
		}
		//*9/
		}*/


		return false;	//return false to make the function happy.
	}

	bool collide_r(node *node_i, float x_, float y_, float z_, float radius, float height, TYPE type, mesh::face_c & face)
	{
		if (node_i->current == nullptr)
			return false;
		node *node_ = &(*node_i);			//we want to copy the address, but not chnage the pointer

											//goes through to find the last node in the tree.
		while (1)	//infinite loop becuase it is manually broken. It is because the tree is recursive, but this method is loopy
		{
			//style: paint what lines you go through as Cyan to be able to visually trace the algorithm
#ifdef SHOW_NORMALS
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glBegin(GL_TRIANGLES);
			glVertex3f(node_->current->position[0], node_->current->position[1], node_->current->position[2]);
			glVertex3f(node_->current->position[0] + 3 * node_->current->normal[0], node_->current->position[1] + 3 * node_->current->normal[1], node_->current->position[2] + 3 * node_->current->normal[2]);
			glVertex3f(node_->current->position[0] + 0.1, node_->current->position[1] + 0.1, node_->current->position[2] + 0.1);
			glEnd();
			glColor3f(0, 0, 0);
#endif
			//if (face_collide(node_->current, x_, y_, z_, radius))
			if (new_collide(node_, x_, y_, z_, radius, height)) {
				if (node_->type == type || node_->type == TYPE::SEMI)
				{
					face = *node_->current;
					return true;
				}
			}
			if (side_dist(node_->current, x_, y_, z_) >= 0)
			{
				if (!node_->front)
				{
					break;
				}
				node_ = node_->front;
			}
			else
			{
				//if the point is behind
				if (!node_->back)			//if the node doesn't contain a back it is a null pointer, so null check.
				{
					break;					//break if it doesn't (the last node checked is the last valid node)
				}
				//but if it does contain a back, then set the current node to that back node. 
				node_ = node_->back;
			}
			//loops back to the top and runs the code for the new node_ (either front or back node)
		}

		return false;	//return false to make the function happy.
	}

	bool collide_r(node *node_i, float x_, float y_, float z_, float x_check, float y_check, float z_check, float radius, float height, TYPE type, mesh::face_c & face)
	{
		if (node_i->current == nullptr)
			return false;
		node *node_ = &(*node_i);			//we want to copy the address, but not chnage the pointer

											//goes through to find the last node in the tree.
		while (1)	//infinite loop becuase it is manually broken. It is because the tree is recursive, but this method is loopy
		{
			//style: paint what lines you go through as Cyan to be able to visually trace the algorithm
#ifdef SHOW_NORMALS
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glBegin(GL_TRIANGLES);
			glVertex3f(node_->current->position[0], node_->current->position[1], node_->current->position[2]);
			glVertex3f(node_->current->position[0] + 3 * node_->current->normal[0], node_->current->position[1] + 3 * node_->current->normal[1], node_->current->position[2] + 3 * node_->current->normal[2]);
			glVertex3f(node_->current->position[0] + 0.1, node_->current->position[1] + 0.1, node_->current->position[2] + 0.1);
			glEnd();
			glColor3f(0, 0, 0);
#endif
			//if (face_collide(node_->current, x_, y_, z_, radius))
			if (new_collide(node_, x_, y_, z_, radius, height)) {
				if (node_->type == type || node_->type == TYPE::SEMI)
				{
					face = *node_->current;
					return true;
				}
			}
			if (side_dist(node_->current, x_check, y_check, z_check) >= 0)
			{
				if (!node_->front)
				{
					break;
				}
				node_ = node_->front;
			}
			else
			{
				//if the point is behind
				if (!node_->back)			//if the node doesn't contain a back it is a null pointer, so null check.
				{
					break;					//break if it doesn't (the last node checked is the last valid node)
				}
				//but if it does contain a back, then set the current node to that back node. 
				node_ = node_->back;
			}
			//loops back to the top and runs the code for the new node_ (either front or back node)
		}

		return false;	//return false to make the function happy.
	}

	bool collide_k(node *node_i, float x_, float y_, float z_, float x_check, float y_check, float z_check, float radius, float height, TYPE type, mesh::face_c & face)
	{
		//This is a wavy new one that may solve all our problems. maybe. 
		//In this one, we go allllllll the way down the tree, then sluuuurp back up it to find where we hitting. idk. maybe better. 
		
		if (node_i->current == nullptr)
			return false;
		node *node_ = &(*node_i);			//we want to copy the address, but not chnage the pointer

											//goes through to find the last node in the tree.
		while (1)	//infinite loop becuase it is manually broken. It is because the tree is recursive, but this method is loopy
		{
			//This loop is only to move into the deepest node.
			//Just slide down and break when we at the end.
			if (side_dist(node_->current, x_check, y_check, z_check) >= 0)
			{
				if (!node_->front)
				{
					break;
				}
				node_ = node_->front;
			}
			else
			{
				//if the point is behind
				if (!node_->back)			//if the node doesn't contain a back it is a null pointer, so null check.
				{
					break;					//break if it doesn't (the last node checked is the last valid node)
				}
				//but if it does contain a back, then set the current node to that back node. 
				node_ = node_->back;
			}
			//loops back to the top and runs the code for the new node_ (either front or back node)
		}


		//slide back up this thing.
		while (1)	
		{
#ifdef SHOW_NORMALS
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glBegin(GL_TRIANGLES);
			glVertex3f(node_->current->position[0], node_->current->position[1], node_->current->position[2]);
			glVertex3f(node_->current->position[0] + 3 * node_->current->vertex[0].nx, node_->current->position[1] + 3 * node_->current->vertex[0].ny, node_->current->position[2] + 3 * node_->current->vertex[0].nz);
			glVertex3f(node_->current->position[0] + 0.1, node_->current->position[1] + 0.1, node_->current->position[2] + 0.1);
			glEnd();
			glColor3f(0, 0, 0);
#endif
			//if (arbit_collide(node_, x_, y_, z_, radius)) {
			if (new_collide(node_, x_, y_, z_, radius, height)) {
			//if (arbit_collide(node_, x_, y_, z_, radius)) {
				if (node_->type == type || node_->type == TYPE::SEMI)
				{
					face = *node_->current;
					return true;
				}
			}

			//when we reach that kid without a dad. The prius node
			if(!node_->parent)//|| node_->parent->back == node_)
			{
				break;
			}
			node_ = node_->parent;
		}

		return false;	//return false to make the function happy.
	}
	
	bool collide_q(node *node_i, float x_, float y_, float z_, float x_check, float y_check, float z_check, float radius, mesh::face_c & face)
	{
		//This is a wavy new one that may solve all our problems. maybe. 
		//In this one, we go allllllll the way down the tree, then sluuuurp back up it to find where we hitting. idk. maybe better. 

		if (node_i->current == nullptr)
			return false;
		node *node_ = &(*node_i);			//we want to copy the address, but not chnage the pointer

											//goes through to find the last node in the tree.
		while (1)	//infinite loop becuase it is manually broken. It is because the tree is recursive, but this method is loopy
		{
			//This loop is only to move into the deepest node.
			//Just slide down and break when we at the end.
			if (side_dist(node_->current, x_check, y_check, z_check) >= 0)
			{
				if (!node_->front)
				{
					break;
				}
				node_ = node_->front;
			}
			else
			{
				//if the point is behind
				if (!node_->back)			//if the node doesn't contain a back it is a null pointer, so null check.
				{
					break;					//break if it doesn't (the last node checked is the last valid node)
				}
				//but if it does contain a back, then set the current node to that back node. 
				node_ = node_->back;
			}
			//loops back to the top and runs the code for the new node_ (either front or back node)
		}


		//slide back up this thing.
		while (1)
		{
#ifdef SHOW_NORMALS
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glBegin(GL_TRIANGLES);
			glVertex3f(node_->current->position[0], node_->current->position[1], node_->current->position[2]);
			glVertex3f(node_->current->position[0] + 3 * node_->current->vertex[0].nx, node_->current->position[1] + 3 * node_->current->vertex[0].ny, node_->current->position[2] + 3 * node_->current->vertex[0].nz);
			glVertex3f(node_->current->position[0] + 0.1, node_->current->position[1] + 0.1, node_->current->position[2] + 0.1);
			glEnd();
			glColor3f(0, 0, 0);
#endif
			//if (arbit_collide(node_, x_, y_, z_, radius)) {
			if (arbit_collide_q(node_, x_, y_, z_, radius)) {
				face = *node_->current;
				return true;
			}

			//when we reach that kid without a dad. The prius node
			if (!node_->parent)//|| node_->parent->back == node_)
			{
				break;
			}
			node_ = node_->parent;
		}

		return false;	//return false to make the function happy.
	}

	bool collide_e(node *node_i, float x_, float y_, float z_, float x_check, float y_check, float z_check, float radius, float height, TYPE type, mesh::face_c & face, mesh::face_c excep)
	{
		if (node_i->current == nullptr)
			return false;
		node *node_ = &(*node_i);			//we want to copy the address, but not chnage the pointer

											//goes through to find the last node in the tree.
		while (1)	//infinite loop becuase it is manually broken. It is because the tree is recursive, but this method is loopy
		{
			//style: paint what lines you go through as Cyan to be able to visually trace the algorithm
#ifdef SHOW_NORMALS
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glBegin(GL_TRIANGLES);
			glVertex3f(node_->current->position[0], node_->current->position[1], node_->current->position[2]);
			glVertex3f(node_->current->position[0] + 3 * node_->current->normal[0], node_->current->position[1] + 3 * node_->current->normal[1], node_->current->position[2] + 3 * node_->current->normal[2]);
			glVertex3f(node_->current->position[0] + 0.1, node_->current->position[1] + 0.1, node_->current->position[2] + 0.1);
			glEnd();
			glColor3f(255, 130, 0);
#endif
			//if (new_collide(node_, x_, y_, z_, radius, height)) {
			if (arbit_collide(node_, x_, y_, z_, radius)) {
				if (node_->type == type)
				{
					if (*node_->current != excep)
					{
						face = *node_->current;
						return true;
					}
				}
			}
			if (side_dist(node_->current, x_check, y_check, z_check) >= 0)
			{
				if (!node_->front)
				{
					break;
				}
				node_ = node_->front;
			}
			else
			{
				//if the point is behind
				if (!node_->back)			//if the node doesn't contain a back it is a null pointer, so null check.
				{
					break;					//break if it doesn't (the last node checked is the last valid node)
				}
				//but if it does contain a back, then set the current node to that back node. 
				node_ = node_->back;
			}
			//loops back to the top and runs the code for the new node_ (either front or back node)
		}

		return false;	//return false to make the function happy.
	}

	bool collide_n(node *node_i, float x_, float y_, float z_, float radius, float height, TYPE type, mesh::face_c ignore, mesh::face_c & out)
	{
		if (node_i->current == nullptr)
			return false;
		node *node_ = &(*node_i);			//we want to copy the address, but not chnage the pointer

											//goes through to find the last node in the tree.
		while (1)	//infinite loop becuase it is manually broken. It is because the tree is recursive, but this method is loopy
		{
			//style: paint what lines you go through as Cyan to be able to visually trace the algorithm
#ifdef SHOW_NORMALS
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glBegin(GL_TRIANGLES);
			glVertex3f(node_->current->position[0], node_->current->position[1], node_->current->position[2]);
			glVertex3f(node_->current->position[0] + 3 * node_->current->normal[0], node_->current->position[1] + 3 * node_->current->normal[1], node_->current->position[2] + 3 * node_->current->normal[2]);
			glVertex3f(node_->current->position[0] + 0.1, node_->current->position[1] + 0.1, node_->current->position[2] + 0.1);
			glEnd();
			glColor3f(0, 0, 0);
#endif
			//if (face_collide(node_->current, x_, y_, z_, radius))
			if (new_collide(node_, x_, y_, z_, radius, height)) {
				if (node_->type == type || node_->type == TYPE::SEMI)
				{
					if (*node_->current != ignore)
					{
						out = *node_->current;
						return true;
					}
				}
			}
			if (side_dist(node_->current, x_, y_, z_) >= 0)
			{
				if (!node_->front)
				{
					break;
				}
				node_ = node_->front;
			}
			else
			{
				//if the point is behind
				if (!node_->back)			//if the node doesn't contain a back it is a null pointer, so null check.
				{
					break;					//break if it doesn't (the last node checked is the last valid node)
				}
				//but if it does contain a back, then set the current node to that back node. 
				node_ = node_->back;
			}
			//loops back to the top and runs the code for the new node_ (either front or back node)
		}

		return false;	//return false to make the function happy.
	}

	bool collide_n(node *node_i, float x_, float y_, float z_, float x_check, float y_check, float z_check, float radius, float height, TYPE type, mesh::face_c ignore, mesh::face_c & out)
	{
		if (node_i->current == nullptr)
			return false;
		node *node_ = &(*node_i);			//we want to copy the address, but not chnage the pointer

											//goes through to find the last node in the tree.
		while (1)	//infinite loop becuase it is manually broken. It is because the tree is recursive, but this method is loopy
		{
			//style: paint what lines you go through as Cyan to be able to visually trace the algorithm
#ifdef SHOW_NORMALS
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glBegin(GL_TRIANGLES);
			glVertex3f(node_->current->position[0], node_->current->position[1], node_->current->position[2]);
			glVertex3f(node_->current->position[0] + 3 * node_->current->normal[0], node_->current->position[1] + 3 * node_->current->normal[1], node_->current->position[2] + 3 * node_->current->normal[2]);
			glVertex3f(node_->current->position[0] + 0.1, node_->current->position[1] + 0.1, node_->current->position[2] + 0.1);
			glEnd();
			glColor3f(0, 0, 0);
#endif
			//if (face_collide(node_->current, x_, y_, z_, radius))
			if (new_collide(node_, x_, y_, z_, radius, height)) {
				if (node_->type == type || node_->type == TYPE::SEMI)
				{
					if (*node_->current != ignore)
					{
						out = *node_->current;
						return true;
					}
				}
			}
			if (side_dist(node_->current, x_check, y_check, z_check) >= 0)
			{
				if (!node_->front)
				{
					break;
				}
				node_ = node_->front;
			}
			else
			{
				//if the point is behind
				if (!node_->back)			//if the node doesn't contain a back it is a null pointer, so null check.
				{
					break;					//break if it doesn't (the last node checked is the last valid node)
				}
				//but if it does contain a back, then set the current node to that back node. 
				node_ = node_->back;
			}
			//loops back to the top and runs the code for the new node_ (either front or back node)
		}

		return false;	//return false to make the function happy.
	}

	int get_face(std::vector<mesh::face_c> faces, mesh::face_c face)
	{
		for (int i = 0; i < faces.size(); i++)
		{
			if (faces[i] == face)
				return i;
		}
		return -1;
	}

	double c_round(double x)
	{
		double gs = settings::grid_size;
		return gs * ((int)((x+gs*0.5) / gs));
	}

	std::vector<mesh::face_c> get_collide_faces(node *node_i, float x_, float y_, float z_, float radius, float height, TYPE type, double dx, double dy, double dz)
	{
		std::vector<mesh::face_c> faces;
		mesh::face_c blank;
		std::vector<mesh::face_c> face_trials;
		/*
		//fill up a blank array
		for (int i = 0; i < 15; i++)
		{
			mesh::face_c face_;
			face_trials.push_back(face_);
		}
		//test all the sides.
		
		//center
		collide_r(node_i, x_, y_, z_, x_, y_, z_, radius, height, type, face_trials[0]);
		
		//+x
		collide_r(node_i, x_, y_, z_, x_ + radius* settings::collide_coeff, y_ , z_, radius, height, type, face_trials[11]);

		//-x
		collide_r(node_i, x_, y_, z_, x_ - radius* settings::collide_coeff, y_ , z_, radius, height, type, face_trials[12]);

		//+z
		collide_r(node_i, x_, y_, z_, x_, y_, z_ + radius, radius, height, type, face_trials[13]);

		//-z
		collide_r(node_i, x_, y_, z_, x_, y_ , z_ - radius, radius, height, type, face_trials[14]);

		//+y +x
		collide_r(node_i, x_, y_, z_,	x_ + radius* settings::collide_coeff, y_ - 0.5 * height* settings::collide_coeff, z_,		radius, height, type, face_trials[1]);
		
		//+y -x
		collide_r(node_i, x_, y_, z_,	x_ - radius* settings::collide_coeff, y_ - 0.5 * height* settings::collide_coeff, z_,		radius, height, type, face_trials[2]);
		
		//+y +z
		collide_r(node_i, x_, y_, z_,	x_ , y_ - 0.5 * height* settings::collide_coeff, z_ + radius,		radius, height, type, face_trials[3]);
		
		//+y -z
		collide_r(node_i, x_, y_, z_,	x_ , y_ - 0.5 * height* settings::collide_coeff, z_ - radius,		radius, height, type, face_trials[4]);
		
		//-y +x	
		collide_r(node_i, x_, y_, z_,	x_ + radius* settings::collide_coeff, y_ - height* settings::collide_coeff, z_,		radius, height, type, face_trials[5]);
		
		//-y -x
		collide_r(node_i, x_, y_, z_,	x_ - radius* settings::collide_coeff, y_ - height* settings::collide_coeff, z_,		radius, height, type, face_trials[6]);
		
		//-y +z
		collide_r(node_i, x_, y_, z_,	x_, y_ - height* settings::collide_coeff, z_ + radius* settings::collide_coeff,		radius, height, type, face_trials[7]);
		
		//-y -z
		collide_r(node_i, x_, y_, z_,	x_, y_ - height* settings::collide_coeff, z_ - radius* settings::collide_coeff,		radius, height, type, face_trials[8]);
		
		//+y 0 0
		collide_r(node_i, x_, y_, z_, x_, y_ + height* settings::collide_coeff, z_, radius, height, type, face_trials[9]);

		//-y 0 0 
		collide_r(node_i, x_, y_, z_, x_, y_ - height* settings::collide_coeff, z_, radius, height, type, face_trials[10]);

		*/

		//fill up a blank array
		for (int i = 0; i < 3; i++)
		{
			mesh::face_c face_;
			face_trials.push_back(face_);
		}
		//test all the sides.
		//x_ = c_round(x_);
		//y_ = c_round(y_);
		//z_ = c_round(z_);

		//if you are running
		if (dx != 0 || dz != 0)
		{
			for (int i = 0; i < 4; i++)
			{
				mesh::face_c face_;
				face_trials.push_back(face_);
			}
			double x_radius = (dx / sqrt(dx*dx + dz*dz)) * radius;
			double z_radius = (dz / sqrt(dx*dx + dz*dz)) * radius;
			collide_k(node_i, x_, y_, z_, x_ + x_radius, y_, z_ + z_radius, radius, height, type, face_trials[4]);
			collide_k(node_i, x_, y_, z_, x_ - x_radius, y_, z_ - z_radius, radius, height, type, face_trials[3]);

			collide_k(node_i, x_, y_, z_, x_ - z_radius, y_, z_ + x_radius, radius, height, type, face_trials[5]);
			collide_k(node_i, x_, y_, z_, x_ + z_radius, y_, z_ - x_radius, radius, height, type, face_trials[6]);
		}

		collide_k(node_i, x_, y_, z_, x_, y_ - height*settings::collide_coeff, z_, radius, height, type, face_trials[0]);
		collide_k(node_i, x_, y_, z_, x_, y_ + height*settings::collide_coeff, z_, radius, height, type, face_trials[1]);
		collide_k(node_i, x_, y_, z_, x_, y_, z_, radius, height, type, face_trials[2]);

		
		
		
		/*	double k = sqrt(dx*dx + dz*dz);
		if (k == 0)
		{
			//not moving...
			collide_k(node_i, x_, y_+ dy, z_, x_, y_, z_, radius, height, type, face_trials[0]);
			collide_k(node_i, x_, y_+ dy, z_, x_, y_ - height, z_, radius, height, type, face_trials[2]);
		}
		else
		{
			//moving
			double xp = x_ + radius * dx / k;
			double zp = z_ + radius * dz / k;
			collide_k(node_i, x_, y_, z_, x_, y_, z_, radius, height, type, face_trials[0]);
			collide_k(node_i, x_, y_, z_, xp, y_, zp, radius, height, type, face_trials[1]);
			collide_k(node_i, x_, y_, z_, x_, y_-height, z_, radius, height, type, face_trials[2]);
			collide_k(node_i, x_, y_, z_, xp, y_-height, zp, radius, height, type, face_trials[3]);
			//and angles

			collide_k(node_i, x_, y_, z_, -zp, y_, xp, radius, height, type, face_trials[4]);
			collide_k(node_i, x_, y_, z_, -zp, y_ - height, xp, radius, height, type, face_trials[5]);

			collide_k(node_i, x_, y_, z_, zp, y_, -xp, radius, height, type, face_trials[6]);
			collide_k(node_i, x_, y_, z_, zp, y_ - height, -xp, radius, height, type, face_trials[7]);
		}
		//Put all unique/real faces in the return'd array
		for (int i = 0; i < 8; i++)
		{
			if (get_face(faces, face_trials[i]) == -1 && face_trials[i] != blank)
			{
				faces.push_back(face_trials[i]);
			}
		}*/


		for (int i = 0; i < face_trials.size(); i++)
		{
			if (get_face(faces, face_trials[i]) == -1 && face_trials[i] != blank)
			{
				faces.push_back(face_trials[i]);
			}
		}
		return faces;
	}

	std::vector<mesh::face_c> get_collide_faces_arbit(node *node_i, float x_, float y_, float z_, float radius, double dx, double dy, double dz)
	{
		std::vector<mesh::face_c> faces;
		mesh::face_c blank;
		std::vector<mesh::face_c> face_trials;
	
		//fill up a blank array
		for (int i = 0; i < 3; i++)
		{
			mesh::face_c face_;
			face_trials.push_back(face_);
		}

		//if you are running
		if (dx != 0 || dz != 0)
		{
			for (int i = 0; i < 4; i++)
			{
				mesh::face_c face_;
				face_trials.push_back(face_);
			}
			double x_radius = (dx / sqrt(dx*dx + dz*dz)) * radius;
			double z_radius = (dz / sqrt(dx*dx + dz*dz)) * radius;
			collide_q(node_i, x_, y_, z_, x_ + x_radius, y_, z_ + z_radius, radius, face_trials[4]);
			collide_q(node_i, x_, y_, z_, x_ - x_radius, y_, z_ - z_radius, radius, face_trials[3]);

			collide_q(node_i, x_, y_, z_, x_ - z_radius, y_, z_ + x_radius, radius, face_trials[5]);
			collide_q(node_i, x_, y_, z_, x_ + z_radius, y_, z_ - x_radius, radius, face_trials[6]);
		}

		collide_q(node_i, x_, y_, z_, x_, y_ - radius*settings::collide_coeff, z_, radius, face_trials[0]);
		collide_q(node_i, x_, y_, z_, x_, y_ + radius*settings::collide_coeff, z_, radius, face_trials[1]);
		collide_q(node_i, x_, y_, z_, x_, y_, z_, radius, face_trials[2]);




		for (int i = 0; i < face_trials.size(); i++)
		{
			if (get_face(faces, face_trials[i]) == -1 && face_trials[i] != blank)
			{
				faces.push_back(face_trials[i]);
			}
		}
		return faces;
	}

	std::vector<mesh::face_c> get_collide_faces(node *node_i, float x_, float y_, float z_, float radius, float height, TYPE type, mesh::face_c excep)
	{
		std::vector<mesh::face_c> faces;
		mesh::face_c blank;
		std::vector<mesh::face_c> face_trials;
		//fill up a blank array
		for (int i = 0; i < 11; i++)
		{
			mesh::face_c face_;
			face_trials.push_back(face_);
		}
		//test all the sides.

		//center
		collide_e(node_i, x_, y_, z_, x_, y_, z_, radius, height, type, face_trials[0], excep);

		//+y +x
		collide_e(node_i, x_, y_, z_, x_ + radius, y_ + height, z_, radius, height, type, face_trials[1], excep);

		//+y -x
		collide_e(node_i, x_, y_, z_, x_ - radius, y_ + height, z_, radius, height, type, face_trials[2], excep);

		//+y +z
		collide_e(node_i, x_, y_, z_, x_, y_ + height, z_ + radius, radius, height, type, face_trials[3], excep);

		//+y -z
		collide_e(node_i, x_, y_, z_, x_, y_ + height, z_ - radius, radius, height, type, face_trials[4], excep);

		//-y +x	
		collide_e(node_i, x_, y_, z_, x_ + radius, y_ - height, z_, radius, height, type, face_trials[5], excep);

		//-y -x
		collide_e(node_i, x_, y_, z_, x_ - radius, y_ - height, z_, radius, height, type, face_trials[6], excep);

		//-y +z
		collide_e(node_i, x_, y_, z_, x_, y_ - height, z_ + radius, radius, height, type, face_trials[7], excep);

		//-y -z
		collide_e(node_i, x_, y_, z_, x_, y_ - height, z_ - radius, radius, height, type, face_trials[8], excep);

		//+y 0 0
		collide_e(node_i, x_, y_, z_, x_, y_ + height, z_, radius, height, type, face_trials[9], excep);

		//-y 0 0 
		collide_e(node_i, x_, y_, z_, x_, y_ - height, z_, radius, height, type, face_trials[10], excep);


		//Put all unique/real faces in the return'd array
		for (int i = 0; i < 10; i++)
		{
			if (get_face(faces, face_trials[i]) == -1 && face_trials[i] != blank)
			{
				faces.push_back(face_trials[i]);
			}
		}

		return faces;
	}

	void generate_pointer_map(std::vector<mesh_p::face *> &faces, std::vector<mesh_p::face*> &out)
	{
		for (int i = 0; i < faces.size(); i++)
		{
			out.push_back(faces[i]);
		}
	}

	void generate_pointer_map(std::vector<mesh::Mesh_c*> meshes, std::vector<mesh::face_c*> &out)
	{
		for (int i = 0; i < meshes.size(); i++)
		{
			if (meshes[i]->s_type == mesh::Mesh_c::TYPE::STATIC)
			{
			//	std::cout << i << "\n";
				mesh::Mesh_c * n_mesh = new mesh::Mesh_c();
				mesh::Mesh_c n_m = *meshes[i];
				*n_mesh = n_m;
				mesh::translate_mesh(n_mesh, meshes[i]->x, meshes[i]->y, meshes[i]->z);
				for (int j = 0; j < meshes[i]->faces.size(); j++)
				{
					out.push_back(new mesh::face_c(meshes[i]->faces[j]));
				}
				delete n_mesh;
			}
		}
	}

	void generate_pointer_map(mesh::Mesh_c * mesh, std::vector<mesh::face_c*> &out)
	{
		mesh::Mesh_c * n_mesh = new mesh::Mesh_c();
		mesh::Mesh_c n_m = *mesh;
		*n_mesh = n_m;
		mesh::translate_mesh(n_mesh, mesh->x, mesh->y, mesh->z);
		for (int j = 0; j < mesh->faces.size(); j++)
		{
			out.push_back(new mesh::face_c(mesh->faces[j]));
		}
		delete n_mesh;
	}

	void generate_pointer_map(std::vector<mesh::face_c *> &faces, std::vector<mesh::face_c*> &out)
	{
		for (int i = 0; i < faces.size(); i++)
		{
			out.push_back(faces[i]);
		}
	}

	void draw_bsp(node *bsp, float x_, float y_, float z_)
	{
		if (bsp->front)
		{
			draw_bsp(bsp->front, x_, y_, z_);
		}
		if (bsp->back)
		{
			draw_bsp(bsp->back, x_, y_, z_);
		}
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glBegin(GL_TRIANGLES);
		if (bsp->parent)
		{
			if (bsp == bsp->parent->front)
			{
				glColor3f(0, 0, 0.7);
			}
			else
			{
				glColor3f(0.7, 0, 0);
			}
		}
			glVertex3f(bsp->current->position[0]*2 -x_, bsp->current->position[1]*2 - y_, bsp->current->position[2]*2 - z_);
			glVertex3f(bsp->current->position[0]*2 + bsp->current->vertex[0].nx - x_, bsp->current->position[1]*2 + bsp->current->vertex[0].ny - y_, bsp->current->position[2]*2 + bsp->current->vertex[0].nz - z_);
			glVertex3f(bsp->current->position[0]*2 - x_+0.1, bsp->current->position[1]*2 - y_+0.1, bsp->current->position[2]*2 - z_+0.1);
		glEnd();
		glColor3f(0, 0, 0);
		//std::cout<<bsp->current->vertex[0].ny << "\n";
	}

	void render_triangle(mesh::face_c * face, sf::RenderWindow *window, camera_pos * cam, bool front)
	{
		std::vector<GLfloat> verts;
		for (int j = 0; j < 3; j++)
		{
			verts.push_back(face->vertex[j].x);
			verts.push_back(face->vertex[j].y);
			verts.push_back(face->vertex[j].z);
			verts.push_back(face->vertex[j].nx);
			verts.push_back(face->vertex[j].ny);
			verts.push_back(face->vertex[j].nz);
		/*	if (front)
			{
				verts.push_back(0.5);
				verts.push_back(0.2);
				verts.push_back(0.21);
			} 
			else
			{
				verts.push_back(0.21);
				verts.push_back(0.2);
				verts.push_back(0.5);
			}*/
			verts.push_back(face->vertex[j].r / 255.f);
			verts.push_back(face->vertex[j].g / 255.f);
			verts.push_back(face->vertex[j].b / 255.f);
		}
		GLfloat * obj = &verts[0];
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glVertexPointer(3, GL_FLOAT, 9 * sizeof(GLfloat), obj);
		glNormalPointer(GL_FLOAT, 9 * sizeof(GLfloat), obj + 3);
		glColorPointer(3, GL_FLOAT, 9 * sizeof(GLfloat), obj + 6);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRotatef(-cam->pitch, 1.f, 0.f, 0.f);
		glRotatef(-cam->azimuth, 0.f, 1.f, 0.f);
		glRotatef(-cam->roll, 0.f, 0.f, 1.f);
		glTranslatef(-cam->x, -cam->y, -cam->z);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		if (settings::ex_norms)
		{
			glBegin(GL_TRIANGLES);
			glVertex3f(face->position[0] * 2 - cam->x, face->position[1] * 2 - cam->y, face->position[2] * 2 - cam->z);
			glVertex3f(face->position[0] * 2 + face->vertex[0].nx - cam->x, face->position[1] * 2 + face->vertex[0].ny - cam->y, face->position[2] * 2 + face->vertex[0].nz - cam->z);
			glVertex3f(face->position[0] * 2 - cam->x + 0.1, face->position[1] * 2 - cam->y + 0.1, face->position[2] * 2 - cam->z + 0.1);
			glEnd();
			glColor3f(0, 0, 0);
		}
	}

	void render_node(node* node_, sf::RenderWindow *window, camera_pos * cam, bool front)
	{
		if (node_->current)
		{
			render_triangle(node_->current, window, cam, true);
		}
		if (node_->back)
		{
			render_node(node_->back, window, cam, false);
		}
		if (node_->front)
		{
			render_node(node_->front, window, cam, true);
		}
	}
}

namespace  bsp {

	std::vector<mesh::face_g> tree_faces;


	struct node {
		node()
		{

		}

		node(int index_)
		{
			index = index_;
		}

		

		void construct(std::vector<int> indexes)
		{
#ifdef USE_BSP
			//put the lists here so that they get deallocated after it runs through, when they aren't needed.
			std::vector<int> back_list;
			std::vector<int> front_list;

			for (int i = 1; i < indexes.size(); i++)				//start at 1, because the faces list contains this.face at 0, so don't want to count it.
			{

				GLfloat a = tree_faces[index].vertex[0].nx;
				GLfloat b = tree_faces[index].vertex[0].ny;
				GLfloat c = tree_faces[index].vertex[0].nz;
				GLfloat d = -(a*tree_faces[index].position[0] + b*tree_faces[index].position[1] + c*tree_faces[index].position[2]);
				tree_faces[index].vertex[0].nd = d;
				//assume + is forwards; - is backwards
				//need to check all vertexes

				GLfloat v1 = a*tree_faces[indexes[i]].vertex[0].x + b*tree_faces[indexes[i]].vertex[0].y + c*tree_faces[indexes[i]].vertex[0].z + d;
				GLfloat v2 = a*tree_faces[indexes[i]].vertex[1].x + b*tree_faces[indexes[i]].vertex[1].y + c*tree_faces[indexes[i]].vertex[1].z + d;
				GLfloat v3 = a*tree_faces[indexes[i]].vertex[2].x + b*tree_faces[indexes[i]].vertex[2].y + c*tree_faces[indexes[i]].vertex[2].z + d;
			
				//removing quad support
				//GLfloat v4 = a*faces[i].position[0] + b*faces[i].position[1] + c*faces[i].position[2] + d;
				if (v1 == 0 && v2 == 0 && v3 == 0)
				{
					front_list.push_back(indexes[i]);
				}
				else if (v1 >= 0 && v2 >= 0 && v3 >= 0)			//in front   (include or-equal-to incase a point is incident);
				{
					front_list.push_back(indexes[i]);
				}
				else if (v1 <= 0 && v2 <= 0 && v3 <= 0)		//in back
				{
					back_list.push_back(indexes[i]);
				}
				else
				{
#ifdef BSP_EXPERIMENTAL
					mesh::Vertex vert1, vert2, vert3;
					int which;
					if (signum(v2) == signum(v3))		//v1 must be on other side
					{
						which = 1;
						vert1 = tree_faces[indexes[i]].vertex[0];
						vert2 = tree_faces[indexes[i]].vertex[1];
						vert3 = tree_faces[indexes[i]].vertex[2];
					}
					else if (signum(v1) == signum(v3))	//v2 must be on other side
					{
						which = 2;
						vert1 = tree_faces[indexes[i]].vertex[1];
						vert2 = tree_faces[indexes[i]].vertex[0];
						vert3 = tree_faces[indexes[i]].vertex[2];
					}
					else
					{									//if not, v3 must be
						which = 3;
						vert1 = tree_faces[indexes[i]].vertex[2];
						vert2 = tree_faces[indexes[i]].vertex[0];
						vert3 = tree_faces[indexes[i]].vertex[1];
					}




					//make vert1, vert2, vert3// vert1 is unique


					float ua = -(a*vert1.x + b*vert1.y + c*vert1.z + d) / (vert2.x - vert1.x + vert2.y - vert1.y + vert2.z - vert1.z);
					float ub = -(a*vert1.x + b*vert1.y + c*vert1.z + d) / (vert3.x - vert1.x + vert3.y - vert1.y + vert3.z - vert1.z);

					mesh::Vertex A, B;
					A.x = vert1.x + ua*(vert2.x - vert1.x);
					A.y = vert1.y + ua*(vert2.y - vert1.y);
					A.z = vert1.z + ua*(vert2.z - vert1.z);

					B.x = vert1.x + ua*(vert3.x - vert1.x);
					B.y = vert1.y + ua*(vert3.y - vert1.y);
					B.z = vert1.z + ua*(vert3.z - vert1.z);

					//make the triangles (1,A,B) (2,A,3), (2,B,A)
					//	mesh::face_g(vert1, A, B);
					//	mesh::face_g(vert2, A, vert3);
					//	mesh::face_g(vert3, B, A);
					int index1 = tree_faces.size();
					int index2 = tree_faces.size()+1;
					int index3 = tree_faces.size()+2;

					tree_faces.push_back(mesh::face_g(vert1, A, B));
					tree_faces.push_back(mesh::face_g(vert2, A, vert3));
					tree_faces.push_back(mesh::face_g(vert3, B, A));





					//add 2 + 3 to one side, 1 to the other.


					//no add the stuff based on the sign of vert1..

					//possbible error. if v1 is inceident... TODO: check incidenece.
					if (which == 1)
					{
						if (v1 > 0)
						{
							front_list.push_back(index1);
							back_list.push_back(index2);
							back_list.push_back(index3);
							//add v1 to front, other to back
						}
						else
						{
							back_list.push_back(index1);
							front_list.push_back(index2);
							front_list.push_back(index3);
						}
					}
					else if (which == 2)
					{
						if (v2 > 0)
						{
							front_list.push_back(index2);
							back_list.push_back(index1);
							back_list.push_back(index3);
						}
						else
						{
							back_list.push_back(index2);
							front_list.push_back(index1);
							front_list.push_back(index3);
						}
					}
					else
					{
						if (v3 > 0)
						{
							front_list.push_back(index3);
							back_list.push_back(index2);
							back_list.push_back(index1);
						}
						else
						{
							back_list.push_back(index3);
							front_list.push_back(index2);
							front_list.push_back(index1);
						}
					}

#endif				
					front_list.push_back(indexes[i]);
					//dsajdhk

					//incident to plane
					//split it
					//	back_list.push_back(faces[i]);
					}
				}

			if (front_list.size() > 0)		//front contains at least 1 element
			{
				front = new node();
				front->index = front_list[0];
				front->parent = this;
				//	front->is_back = false;
				front->construct(front_list);
			}
			if (back_list.size() > 0)
			{
				back = new node();
				back->index = back_list[0];
				back->parent = this;
				//				front->is_back = true;
				back->construct(back_list);
			}
#endif
		}





		
		//has to use triangle faces.
		//gives a stack overflow if I work with real data... So I got to use pointers.
	/*
		void construct(std::vector<mesh::face_g> *faces)
		{
			//put the lists here so that they get deallocated after it runs through, when they aren't needed.
			std::vector<mesh::face_g*> back_list;
			std::vector<mesh::face_g*> front_list;

			for (int i = 1; i < faces.size(); i++)				//start at 1, because the faces list contains this.face at 0, so don't want to count it.
			{

				GLfloat a = face->vertex[0].nx;
				GLfloat b = face->vertex[0].ny;
				GLfloat c = face->vertex[0].nz;
				GLfloat d = -(a*face->position[0] + b*face->position[1] + c*face->position[2]);
				face->vertex[0].nd = d;
				//assume + is forwards; - is backwards
				//need to check all vertexes
			
				GLfloat v1 = a*faces[i]->vertex[0].x + b*faces[i]->vertex[0].y + c*faces[i]->vertex[0].z + d;
				GLfloat v2 = a*faces[i]->vertex[1].x + b*faces[i]->vertex[1].y + c*faces[i]->vertex[1].z + d;
				GLfloat v3 = a*faces[i]->vertex[2].x + b*faces[i]->vertex[2].y + c*faces[i]->vertex[2].z + d;
				
				//removing quad support
				//GLfloat v4 = a*faces[i].position[0] + b*faces[i].position[1] + c*faces[i].position[2] + d;

				if (v1 >= 0 && v2 >= 0 && v3 >= 0)			//in front   (include or-equal-to incase a point is incident);
				{
					front_list.push_back(faces[i]);
				}
				else if (v1 <= 0 && v2 <= 0 && v3 <= 0)		//in back
				{
					back_list.push_back(faces[i]);
				}
				else
				{
				/*
					mesh::Vertex vert1, vert2, vert3;
					int which;
					if (signum(v2) == signum(v3))		//v1 must be on other side
					{
						which = 1;
						vert1 = faces[i].vertex[0];
						vert2 = faces[i].vertex[1];
						vert3 = faces[i].vertex[2];
					}
					else if (signum(v1) == signum(v3))	//v2 must be on other side
					{
						which = 2;
						vert1 = faces[i].vertex[1];
						vert2 = faces[i].vertex[0];
						vert3 = faces[i].vertex[2];
					}
					else
					{									//if not, v3 must be
						which = 3;
						vert1 = faces[i].vertex[2];
						vert2 = faces[i].vertex[0];
						vert3 = faces[i].vertex[1];
					}
					

				

					//make vert1, vert2, vert3// vert1 is unique
				

					float ua = -(a*vert1.x + b*vert1.y + c*vert1.z + d) / (vert2.x - vert1.x + vert2.y - vert1.y + vert2.z - vert1.z);
					float ub = -(a*vert1.x + b*vert1.y + c*vert1.z + d) / (vert3.x - vert1.x + vert3.y - vert1.y + vert3.z - vert1.z);

					mesh::Vertex A, B;
					A.x = vert1.x + ua*(vert2.x - vert1.x);
					A.y = vert1.y + ua*(vert2.y - vert1.y);
					A.z = vert1.z + ua*(vert2.z - vert1.z);

					B.x = vert1.x + ua*(vert3.x - vert1.x);
					B.y = vert1.y + ua*(vert3.y - vert1.y);
					B.z = vert1.z + ua*(vert3.z - vert1.z);
					
					//make the triangles (1,A,B) (2,A,3), (2,B,A)
				//	mesh::face_g(vert1, A, B);
				//	mesh::face_g(vert2, A, vert3);
				//	mesh::face_g(vert3, B, A);

					
					
					//add 2 + 3 to one side, 1 to the other.


					//no add the stuff based on the sign of vert1.. 
					
					//possbible error. if v1 is inceident... TODO: check incidenece.
					if (which == 1)
					{
						if (v1 > 0)
						{
							front_list.push_back(mesh::face_g(vert1, A, B));
							back_list.push_back(mesh::face_g(vert2, A, vert3));
							back_list.push_back(mesh::face_g(vert3, B, A));
							//add v1 to front, other to back
						}
						else
						{
							back_list.push_back(mesh::face_g(vert1, A, B));
							front_list.push_back(mesh::face_g(vert2, A, vert3));
							front_list.push_back(mesh::face_g(vert3, B, A));
						}
					}
					else if (which == 2)
					{
						if (v2 > 0)
						{
							front_list.push_back(mesh::face_g(vert2, A, vert3));
							back_list.push_back(mesh::face_g(vert1, A, B));
							back_list.push_back(mesh::face_g(vert3, B, A));
						}
						else
						{
							back_list.push_back(mesh::face_g(vert2, A, vert3));
							front_list.push_back(mesh::face_g(vert1, A, B));
							front_list.push_back(mesh::face_g(vert3, B, A));
						}
					}
					else
					{
						if (v3 > 0)
						{
							front_list.push_back(mesh::face_g(vert3, B, A));
							back_list.push_back(mesh::face_g(vert2, A, vert3));
							back_list.push_back(mesh::face_g(vert1, A, B));
						}
						else
						{
							back_list.push_back(mesh::face_g(vert3, B, A));
							front_list.push_back(mesh::face_g(vert2, A, vert3));
							front_list.push_back(mesh::face_g(vert1, A, B));
						}
					}
					
				///	**0-/

					//dsajdhk
				
					//incident to plane
					//split it
				//	back_list.push_back(faces[i]);
				}
			}

			if (front_list.size() > 0)		//front contains at least 1 element
			{
				front = new node();
				front->face = front_list[0];
				front->parent = this;
				//	front->is_back = false;
				front->construct(front_list);
			}
			if (back_list.size() > 0)
			{
				back = new node();
				back->face = back_list[0];
				back->parent = this;
				//				front->is_back = true;
				back->construct(back_list);
			}
		}

		*/
		/*     for later when checking for regions.
		if (this->parent == NULL)
		{
			//then is root
		}
		*/

		GLfloat distance(GLfloat x_, GLfloat y_, GLfloat z_)
		{
			GLfloat d = -(tree_faces[index].vertex[0].nx*tree_faces[index].position[0] + tree_faces[index].vertex[0].ny*tree_faces[index].position[1] + tree_faces[index].vertex[0].nz*tree_faces[index].position[2]);
		//	std::cout << (face.vertex[0].nx*x_ + face.vertex[0].ny*y_ + face.vertex[0].nz*z_ + d) / sqrt(face.vertex[0].nx*face.vertex[0].nx + face.vertex[0].ny*face.vertex[0].ny + face.vertex[0].nz*face.vertex[0].nz) << "\n";
			return (tree_faces[index].vertex[0].nx*x_ + tree_faces[index].vertex[0].ny*y_ + tree_faces[index].vertex[0].nz*z_ + d) / sqrt(tree_faces[index].vertex[0].nx*tree_faces[index].vertex[0].nx + tree_faces[index].vertex[0].ny*tree_faces[index].vertex[0].ny + tree_faces[index].vertex[0].nz*tree_faces[index].vertex[0].nz);
		}

		bool collide(float x_, float y_, float z_, float radius)
		{
			if (abs(this->distance(x_, y_, z_)) <= radius)
				return true;
			return false;
		}

		//node only contains face (data), pointers to its parent and children, and whether or not it is a back
	//	mesh::face_g *face;// = NULL;			//old
		int index;
	//	bool is_back = NULL;
		node *parent = NULL;
		node *back = NULL;
		node *front = NULL;
	};

	bool collide(node *node_, float x_, float y_, float z_, float radius)
	{
#ifdef USE_BSP
		//find what node you're in
		while (1)
		{
			if (node_->distance(x_, y_, z_) > 0)
			{
				if (!node_->front)
				{
				//	std::cout << "megga oopf?\n";
					break;
				}
				/*
				glBegin(GL_LINES);
				glColor3f(1, 0, 0);
				glVertex3f(0, 0, 0);
				glVertex3f(1, 0, 0);

				glColor3f(0, 1, 0);
				glVertex3f(0, 0, 0);
				glVertex3f(0, 1, 0);

				glColor3f(0, 0, 1);
				glVertex3f(0, 0, 0);
				glVertex3f(0, 0, 1);
				glEnd();
				std::cout << "shouyld be\n";
				
				glBegin(GL_TRIANGLES);
					glVertex3f(tree_faces[node_->index].position[0]-x_, tree_faces[node_->index].position[1] - y_, tree_faces[node_->index].position[2]-z_);
					glVertex3f(tree_faces[node_->index].position[0] - x_ + tree_faces[node_->index].vertex[0].nx * 3.0, tree_faces[node_->index].position[1] - y_ + tree_faces[node_->index].vertex[0].ny * 3.0, tree_faces[node_->index].position[2] - z_ + tree_faces[node_->index].vertex[0].nz * 3.0);
					glVertex3f(tree_faces[node_->index].position[0] - x_ + 0.1, tree_faces[node_->index].position[1] - y_, tree_faces[node_->index].position[2] - z_);
				glEnd();
				*/
				node_ = node_->front;
			}
			else
			{
				if (!node_->back)
				{
				//	std::cout << "out?\n";
					return false;					//if you're behind and there is no behind, then you're off the map and not hitting anything.
					break;
				}
				/*
				glBegin(GL_TRIANGLES);
					glVertex3f(tree_faces[node_->index].position[0] - x_, tree_faces[node_->index].position[1] - y_, tree_faces[node_->index].position[2] - z_);
					glVertex3f(tree_faces[node_->index].position[0] - x_ + tree_faces[node_->index].vertex[0].nx * 3.0, tree_faces[node_->index].position[1] - y_ + tree_faces[node_->index].vertex[0].ny * 3.0, tree_faces[node_->index].position[2] - z_ + tree_faces[node_->index].vertex[0].nz * 3.0);
					glVertex3f(tree_faces[node_->index].position[0] - x_ + 0.1, tree_faces[node_->index].position[1] - y_, tree_faces[node_->index].position[2] - z_);
				glEnd();
				*/
				node_ = node_->back;
			}

		}

		//node_ is now the extreme node you're in
		if (!node_)
		{
			std::cout << "nullpointer?";
			return false;
		}
		
		node *region = node_;

		//find what the first node in your region is
		//need to use infinite loop with manual break for nullpointer checks.
		while (1)
		{
			if (!region->parent)					//is null pointer (region is root)
			{
				break;
			}
			if (region == region->parent->back)		//region is it's parent's back node so exit because that ends the region.
			{
				break;
			}
			region = region->parent;
		}
		//region is now the first node in the region
		
		//test collisions against all nodes in the region from region to node_
		while (region != node_)
		{
			glBegin(GL_TRIANGLES);
			glVertex3f(tree_faces[region->index].position[0] - x_, tree_faces[region->index].position[1] - y_, tree_faces[region->index].position[2] - z_);
			glVertex3f(tree_faces[region->index].position[0] - x_ + tree_faces[region->index].vertex[0].nx , tree_faces[region->index].position[1] - y_ + tree_faces[region->index].vertex[0].ny, tree_faces[region->index].position[2] - z_ + tree_faces[region->index].vertex[0].nz);
			glVertex3f(tree_faces[region->index].position[0] - x_ + 0.1, tree_faces[region->index].position[1] - y_, tree_faces[region->index].position[2] - z_);
			glEnd();
			
			if (region->collide(x_, y_, z_, radius))
			{
				return true;
			}
			region = region->front;
		}
		//will exit when region is node_, but we need to still test node_
		if (node_->collide(x_, y_, z_, radius))
		{
			glBegin(GL_TRIANGLES);
				glVertex3f(tree_faces[node_->index].position[0] - x_, tree_faces[node_->index].position[1] - y_, tree_faces[node_->index].position[2] - z_);
				glVertex3f(tree_faces[node_->index].position[0] - x_ + tree_faces[node_->index].vertex[0].nx * 3.0, tree_faces[node_->index].position[1] - y_ + tree_faces[node_->index].vertex[0].ny * 3.0, tree_faces[node_->index].position[2] - z_ + tree_faces[node_->index].vertex[0].nz * 3.0);
				glVertex3f(tree_faces[node_->index].position[0] - x_ + 0.1, tree_faces[node_->index].position[1] - y_, tree_faces[node_->index].position[2] - z_);
			glEnd();
			node_ = node_->front;
			return true;
		}
#endif
		return false;
		
	}



	void print_bsp(node *node_)
	{
		while (node_->front != NULL)
		{
			std::cout <<"check\n";
			node_ = node_->front;
		}
	}


}




//depreciated 
/*
struct bsp_node {


	void consider_face(mesh::face_g &check_)
	{
		GLfloat a = current.vertex[0].nx;
		GLfloat b = current.vertex[0].ny;
		GLfloat c = current.vertex[0].nz;
		GLfloat d = -(a*current.position[0] + b*current.position[1] + c*current.position[2]);
		//assume + is forwards; - is backwards
		//need to check all vertexes
		GLfloat v1 = a*check_.position[0] + b*check_.position[1] + c*check_.position[2] + d;
		GLfloat v2 = a*check_.position[0] + b*check_.position[1] + c*check_.position[2] + d;
		GLfloat v3 = a*check_.position[0] + b*check_.position[1] + c*check_.position[2] + d;
		GLfloat v4 = a*check_.position[0] + b*check_.position[1] + c*check_.position[2] + d;

		if (v1 > 0 && v2 > 0 && v3 > 0 && v4 > 0)
		{
			//in front
			bsp_node bsp_new;
			bsp_new.current = check_;
			bsp_new.parent = this;
			front.push_back(bsp_new);
		}
		else if (v1 < 0 && v2 < 0 && v3 < 0 && v4 < 0)
		{
			bsp_node bsp_new;
			bsp_new.current = check_;
			bsp_new.parent = this;
			back.push_back(bsp_new);
		}
		else
		{
			//split it or some shit
		}
	}

	//void iterate

	bsp_node *parent;
	mesh::face_g current;
	std::vector<GLfloat> normal;
	std::vector<bsp_node> front;
	std::vector<bsp_node> back;
};
*/







