#pragma once
#include "headers.h"
#include "settings.h"
namespace vec
{
	struct Vec3
	{
		Vec3()
		{
			x = 0.f;
			y = 0.f;
			z = 0.f;
		}
		Vec3(float x_, float y_, float z_)
		{
			x = x_;
			y = y_;
			z = z_;
		}
		bool operator==(const Vec3&other) const
		{
			return (this->x == other.x && this->y == other.y && this->z == other.z);
		}
		bool operator!=(const Vec3&other) const
		{
			return !(this->x == other.x && this->y == other.y && this->z == other.z);
		}
		Vec3 operator+(const Vec3&other) const
		{
			return Vec3(this->x + other.x, this->y + other.y, this->z + other.z);
		}
		Vec3 operator-(const Vec3&other) const
		{
			return Vec3(this->x - other.x, this->y - other.y, this->z - other.z);
		}
		Vec3 operator*(const float a) const
		{
			return Vec3(a*this->x, a*this->y, a*this->z);
		}
		Vec3 operator/(const float a) const
		{
			return Vec3((1 / a)*this->x, (1 / a)*this->y, (1 / a)*this->z);
		}

		float x, y, z;
	};

	struct Vec2
	{
		Vec2()
		{
			x = 0.f;
			y = 0.f;
		}
		Vec2(float x_, float y_)
		{
			x = x_;
			y = y_;
		}
		bool operator==(const Vec2&other) const
		{
			return (this->x == other.x && this->y == other.y);
		}
		bool operator!=(const Vec2&other) const
		{
			return !(this->x == other.x && this->y == other.y);
		}
		Vec2 operator+(const Vec2&other) const
		{
			return Vec2(this->x + other.x, this->y + other.y);
		}
		Vec2 operator-(const Vec2&other) const
		{
			return Vec2(this->x - other.x, this->y - other.y);
		}
		Vec2 operator*(const float a) const
		{
			return Vec2(a*this->x, a*this->y);
		}
		Vec2 operator/(const float a) const
		{
			return Vec2((1 / a)*this->x, (1 / a)*this->y);
		}
		float x, y;
	};

	struct Vertex
	{
		Vec3 position;
		Vec3 normal;
		Vec3 texture_coordiante;
	};

	struct Mesh
	{
		Mesh()
		{

		}

		Mesh(std::vector<Vertex> &vertices_, std::vector<unsigned int>& indices_)
		{
			vertices = vertices_;
			indices = indices_;

		}

		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
	};

	namespace math
	{
		Vec3 cross_vec3(const Vec3 a, const Vec3 b)
		{
			return Vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
		}

		float magnitude(const Vec3 v_)
		{
			return (sqrtf(powf(v_.x, 2) + powf(v_.y, 2) + powf(v_.z, 2)));
		}

		float magnitude(const Vec2 v_)
		{
			return (sqrtf(powf(v_.x, 2) + powf(v_.y, 2)));
		}

		float dot(const Vec3 a, const Vec3 b)
		{
			return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
		}

		float dot(const Vec2 a, const Vec2 b)
		{
			return (a.x * b.x) + (a.y * b.y);
		}

		float angle_between_vec3(const Vec3 a, const Vec3 b)
		{
			float angle = dot(a, b);
			angle /= (magnitude(a) * magnitude(b));
			return angle = acosf(angle);
		}
	}
}
#define TO_RAD 0.01745329251

namespace mesh
{
	//int scale = 1;
	float scale = 3;

	std::vector<sf::Texture> textures;

	struct Vertex
	{
		Vertex()
		{
			x, y, z, uvx, uvy, nx, ny, nz, nd = 0.f;
		}
		Vertex(GLfloat x_, GLfloat y_, GLfloat z_, GLfloat uvx_, GLfloat uvy_, GLfloat nx_, GLfloat ny_, GLfloat nz_)
		{
			x = x_;
			y = y_;
			z = z_;
			uvx = uvx_;
			uvy = uvy_;
			nx = nx_;
			ny = ny_;
			nz = nz_;
		}
		GLfloat x, y, z, uvx, uvy, nx, ny, nz, nd;
	};

	struct Mesh
	{
		Mesh()
		{

		}

		Mesh(std::vector<std::vector<GLfloat>> vertices_)
		{
			vertices = vertices_;
		}

		std::vector<std::vector<GLfloat>> vertices;

		std::vector<std::vector<GLfloat>> normals;

		float r, g, b;
		float x, y, z;
	};

	std::vector<GLfloat> calculate_normal(std::vector<GLfloat> a, std::vector<GLfloat> b, std::vector<GLfloat> c)
	{
		GLfloat N_x = ((b[1] - a[1])*(c[2] - a[2])) - ((b[2] - a[2])*(c[1] - a[1]));
		GLfloat N_y = ((b[2] - a[2])*(c[0] - a[0])) - ((b[0] - a[0])*(c[2] - a[2]));
		GLfloat N_z = ((b[0] - a[0])*(c[1] - a[1])) - ((b[1] - a[1])*(c[0] - a[0]));
		GLfloat mag = vec::math::magnitude(vec::Vec3(N_x, N_y, N_z));
		return { N_x / mag,N_y / mag,N_z / mag };
	}

	std::vector<GLfloat> calculate_centroid(std::vector<GLfloat> a, std::vector<GLfloat> b, std::vector<GLfloat> c)
	{
		GLfloat x = (a[1] + b[1] + c[1]) / 3.f;
		GLfloat y = (a[2] + b[2] + c[2]) / 3.f;
		GLfloat z = (a[3] + b[3] + c[3]) / 3.f;
		return { x,y,z };
	}

	struct face
	{
		face()
		{

		}
		face(std::vector<GLfloat> vert1_, std::vector<GLfloat> vert2_, std::vector<GLfloat> vert3_, std::vector<GLfloat> color_)
		{
			vertex.push_back(vert1_);
			vertex.push_back(vert2_);
			vertex.push_back(vert3_);
			//normal = calculate_normal(vert1_, vert2_, vert3_);
			position = calculate_centroid(vert1_, vert2_, vert3_);
			color = color_;
		}
		std::vector<GLfloat> position;
		std::vector<GLfloat> color;
		std::vector<std::vector<GLfloat>> vertex;
		std::vector<GLfloat> normal;
	};

	std::vector<GLfloat> calculate_centroid(face face_)
	{
		std::vector<GLfloat> a = face_.vertex[0];
		std::vector<GLfloat> b = face_.vertex[1];
		std::vector<GLfloat> c = face_.vertex[2];
		GLfloat x = (a[1] + b[1] + c[1]) / 3.f;
		GLfloat y = (a[2] + b[2] + c[2]) / 3.f;
		GLfloat z = (a[3] + b[3] + c[3]) / 3.f;
		return { x,y,z };
	}

	std::vector<GLfloat> calculate_normal(face face_)
	{
		std::vector<GLfloat> a = face_.vertex[0];
		std::vector<GLfloat> b = face_.vertex[1];
		std::vector<GLfloat> c = face_.vertex[2];
		GLfloat N_x = ((b[2] - a[2])*(c[3] - a[3])) - ((b[3] - a[3])*(c[2] - a[2]));
		GLfloat N_y = ((b[3] - a[3])*(c[1] - a[1])) - ((b[1] - a[1])*(c[3] - a[3]));
		GLfloat N_z = ((b[1] - a[1])*(c[2] - a[2])) - ((b[2] - a[2])*(c[1] - a[1]));
		GLfloat mag = vec::math::magnitude(vec::Vec3(N_x, N_y, N_z));
		return { 1.0 , N_x / mag,N_y / mag,N_z / mag };
	}

	struct Mesh_t
	{
		Mesh_t()
		{

		}

		Mesh_t(std::vector<face> faces_)
		{
			faces = faces_;
		}

		std::vector<face> faces;

		float _x, _y, _z;
		float r, g, b;
		float x, y, z;
	};

	struct Mesh_r
	{
		Mesh_r()
		{

		}

		Mesh_r(std::vector<face> faces_)
		{
			faces = faces_;
		}

		std::vector<face> faces;

		float azimuth, pitch, roll;
		float _x, _y, _z;
		float r, g, b;
		float x, y, z;
		float x_speed, y_speed, z_speed;
	};

	std::vector<GLfloat> calculate_centroid(Vertex vert1_, Vertex vert2_, Vertex vert3_)
	{
		GLfloat x = (vert1_.x + vert2_.x + vert3_.x) / 3.f;
		GLfloat y = (vert1_.y + vert2_.y + vert3_.y) / 3.f;
		GLfloat z = (vert1_.z + vert2_.z + vert3_.z) / 3.f;
		return { x,y,z };
	}

	std::vector<GLfloat> calculate_normal(Vertex vert1_, Vertex vert2_, Vertex vert3_)
	{
		GLfloat nx = ((vert2_.y - vert1_.y)*(vert3_.z - vert1_.z)) - ((vert2_.z - vert1_.z)*(vert3_.y - vert1_.y));
		GLfloat ny = ((vert2_.z - vert1_.z)*(vert3_.x - vert1_.x)) - ((vert2_.x - vert1_.x)*(vert3_.z - vert1_.z));
		GLfloat nz = ((vert2_.x - vert1_.x)*(vert3_.y - vert1_.y)) - ((vert2_.y - vert1_.y)*(vert3_.x - vert1_.x));
		GLfloat n = sqrtf(nx*nx + ny*ny + nz*nz);
		return { nx / n,ny / n,nz / n };
	}

	std::vector<GLfloat> calculate_centroid(Vertex vert1_, Vertex vert2_, Vertex vert3_, Vertex vert4_)
	{
		GLfloat x = (vert1_.x + vert2_.x + vert3_.x + vert4_.x) / 4.f;
		GLfloat y = (vert1_.y + vert2_.y + vert3_.y + vert4_.x) / 4.f;
		GLfloat z = (vert1_.z + vert2_.z + vert3_.z + vert4_.x) / 4.f;
		return { x,y,z };
	}

	std::vector<GLfloat> normalize(GLfloat in_0, GLfloat in_1, GLfloat in_2)
	{
		float n = sqrt(in_0 * in_0 + in_1 * in_1 + in_2 * in_2);
		return { in_0 / n, in_1 / n, in_2 / n};
	}

	struct face_g
	{
		face_g()
		{

		}
		face_g(Vertex vert1_, Vertex vert2_, Vertex vert3_)
		{
			vertex.push_back(vert1_);
			vertex.push_back(vert2_);
			vertex.push_back(vert3_);
			normal = calculate_normal(vert1_, vert2_, vert3_);
			position = calculate_centroid(vert1_, vert2_, vert3_);
			type = 0b0001;
		}
		face_g(Vertex vert1_, Vertex vert2_, Vertex vert3_, Vertex vert4_)
		{
			vertex.push_back(vert1_);
			vertex.push_back(vert2_);
			vertex.push_back(vert3_);
			vertex.push_back(vert4_);
			//normal = calculate_normal(vert1_, vert2_, vert3_);
			position = calculate_centroid(vert1_, vert2_, vert3_, vert4_);
			type = 0b0010;
		}
		byte type;
		std::vector<GLfloat> position;
		std::vector<GLfloat> color;
		std::vector<Vertex> vertex;
		std::vector<GLfloat> normal;
	};

	struct Mesh_g
	{
		Mesh_g()
		{

		}

		Mesh_g(std::vector<face_g> faces_)
		{
			faces = faces_;
		}
		/*
		Mesh_g operator=(const Mesh_g& other) const
		{
			this->vertexes = other.vertexes;
			this->azimuth = other.azimuth;
			this->faces = other.faces;
			this->pitch = other.pitch;
			this->roll = other.roll;
		}
			/*
			bool operator==(const pointdef& other) const
		{
			return ((this->x == other.x) && (this->y == other.y) && (this->z == other.z));
		}*/
		void commit_mesh()
		{
			vertexes.clear();
			vertex_number = 0;
			if (type == R_TRIANGLES)
			{
				for (int i = 0; i < faces.size(); i++)
				{
					std::vector<GLfloat> norms = calculate_normal(faces[i].vertex[0], faces[i].vertex[1], faces[i].vertex[2]);

					for (int j = 0; j < 3; j++)
					{
						vertexes.push_back(faces[i].vertex[j].x);
						vertexes.push_back(faces[i].vertex[j].y);
						vertexes.push_back(faces[i].vertex[j].z);
						vertexes.push_back(faces[i].vertex[j].uvx);
						vertexes.push_back(1.f - faces[i].vertex[j].uvy);
						
						std::vector<GLfloat> norms_ = normalize(faces[i].vertex[j].nx, faces[i].vertex[j].ny, faces[i].vertex[j].nz);

				//		vertexes.push_back(norms_[0]);
				//		vertexes.push_back(norms_[1]);
				//		vertexes.push_back(norms_[2]);


					//	vertexes.push_back(faces[i].vertex[j].nx);
					//	vertexes.push_back(faces[i].vertex[j].ny);
					//	vertexes.push_back(faces[i].vertex[j].nz);
					//	
						//TODO: why this ^ static? look into the reader perhaps.


						vertexes.push_back(norms[0]);
						vertexes.push_back(norms[1]);
						vertexes.push_back(norms[2]);
					}
					vertex_number += 3;
				}
			}
			else if (type == R_QUADS)
			{
				for (int i = 0; i < faces.size(); i++)
				{
					for (int j = 0; j < 4; j++)
					{
						vertexes.push_back(faces[i].vertex[j].x);
						vertexes.push_back(faces[i].vertex[j].y);
						vertexes.push_back(faces[i].vertex[j].z);
						vertexes.push_back(faces[i].vertex[j].uvx);
						vertexes.push_back(1.f - faces[i].vertex[j].uvy);
						vertexes.push_back(faces[i].vertex[j].nx);
						vertexes.push_back(faces[i].vertex[j].ny);
						vertexes.push_back(faces[i].vertex[j].nz);
					}
					vertex_number += 4;
				}
			}
			vertex_array = &vertexes[0];
			//std::cout << vertex_array << "\n";
		}
		std::vector<GLfloat> vertexes;
		std::vector<face_g> faces;
	//	std::vector<face_g*> face_ptr;
		uint8_t texture_number;
		int vertex_number;
		int type;
		float _x, _y, _z;
		float r, g, b;
		float x, y, z = 0.f;
		float azimuth, pitch, roll = 0.f;
		GLfloat* vertex_array;
		float width = 1.f, height = 1.f, depth = 1.f;
	};

	void resize_mesh(Mesh_g *mesh_, float x_size, float y_size, float z_size)
	{
		if (mesh_->width != x_size || mesh_->height != y_size || mesh_->depth != z_size)
		{
			float x_scale = x_size / mesh_->width;
			float y_scale = y_size / mesh_->height;
			float z_scale = z_size / mesh_->depth;
			for (int i = 0; i < mesh_->faces.size(); i++)
			{
				for (int j = 0; j < 3; j++)
				{
					mesh_->faces[i].vertex[j].x *= x_scale;
					mesh_->faces[i].vertex[j].y *= y_scale;
					mesh_->faces[i].vertex[j].z *= z_scale;
				}
				mesh_->faces[i].position = calculate_centroid(mesh_->faces[i].vertex[0], mesh_->faces[i].vertex[1], mesh_->faces[i].vertex[2]);
			}
			mesh_->commit_mesh();
			mesh_->width = x_size;
			mesh_->height = y_size;
			mesh_->depth = z_size;
		}
	}

	//the only good one!
	bool read_obj(const char* file_path, mesh::Mesh_g &mesh_, int color, bool use_uv, int type, uint8_t texture_number, float scale_)
	{
		//std::cout << "you okay?" << "\n";
		//std::ifstream file("src/untiltled1.obj");
		//std::cout << "hey" << "\n";

		FILE * file = fopen(file_path, "r");
		if (file == NULL) {
			printf("Impossible to open the file !\n");
			return false;
		}

		std::vector<sf::Vector3f> temp_vertices;
		std::vector<sf::Vector2f> temp_uvs;
		std::vector<sf::Vector3f> temp_normals;

		std::vector<unsigned int> vertexIndices;
		std::vector<unsigned int> uvIndices;
		std::vector<unsigned int> normalIndices;

		while (1 == 1) {

			char lineHeader[128];
			// read the first word of the line
			int res = fscanf(file, "%s", lineHeader);
			if (res == EOF)
				break;
			if (strcmp(lineHeader, "v") == 0) {
				sf::Vector3f vertex;
				fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
				temp_vertices.push_back(vertex);
			}
			else if (strcmp(lineHeader, "vt") == 0) {
				sf::Vector2f uv;
				fscanf(file, "%f %f\n", &uv.x, &uv.y);
				temp_uvs.push_back(uv);
			}

			else if (strcmp(lineHeader, "vn") == 0) {
				sf::Vector3f normal;
				fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
				temp_normals.push_back(normal);
			}
			else if (strcmp(lineHeader, "f") == 0) {
			//	std::string vertex1, vertex2, vertex3;
				
				if (type == R_TRIANGLES)
				{
					unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
					int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);

					if (matches != 9) {
						printf("File can't be read by our simple parser : ( Try exporting with other options\n");
						return false;
					}
					vertexIndices.push_back(vertexIndex[0]);
					vertexIndices.push_back(vertexIndex[1]);
					vertexIndices.push_back(vertexIndex[2]);
					uvIndices.push_back(uvIndex[0]);
					uvIndices.push_back(uvIndex[1]);
					uvIndices.push_back(uvIndex[2]);
					normalIndices.push_back(normalIndex[0]);
					normalIndices.push_back(normalIndex[1]);
					normalIndices.push_back(normalIndex[2]);

		

					Vertex vert_1(temp_vertices[vertexIndex[0] - 1].x*scale_, temp_vertices[vertexIndex[0] - 1].y*scale_, temp_vertices[vertexIndex[0] - 1].z*scale_, temp_uvs[uvIndex[0] - 1].x, temp_uvs[uvIndex[0] - 1].y, temp_normals[normalIndices[0] - 1].x, temp_normals[normalIndices[0] - 1].y, temp_normals[normalIndices[0] - 1].z);
					Vertex vert_2(temp_vertices[vertexIndex[1] - 1].x*scale_, temp_vertices[vertexIndex[1] - 1].y*scale_, temp_vertices[vertexIndex[1] - 1].z*scale_, temp_uvs[uvIndex[1] - 1].x, temp_uvs[uvIndex[1] - 1].y, temp_normals[normalIndices[1] - 1].x, temp_normals[normalIndices[1] - 1].y, temp_normals[normalIndices[1] - 1].z);
					Vertex vert_3(temp_vertices[vertexIndex[2] - 1].x*scale_, temp_vertices[vertexIndex[2] - 1].y*scale_, temp_vertices[vertexIndex[2] - 1].z*scale_, temp_uvs[uvIndex[2] - 1].x, temp_uvs[uvIndex[2] - 1].y, temp_normals[normalIndices[2] - 1].x, temp_normals[normalIndices[2] - 1].y, temp_normals[normalIndices[2] - 1].z);

					mesh_.faces.push_back(face_g(vert_1, vert_2, vert_3));

				}
				else if (type == R_QUADS)
				{
					unsigned int vertexIndex[4], uvIndex[4], normalIndex[4];
					int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2], &vertexIndex[3], &uvIndex[3], &normalIndex[3]);

					if (matches != 12) {
						printf("File can't be read by our simple parser : ( Try exporting with other options\n");
						return false;
					}
					vertexIndices.push_back(vertexIndex[0]);
					vertexIndices.push_back(vertexIndex[1]);
					vertexIndices.push_back(vertexIndex[2]);
					vertexIndices.push_back(vertexIndex[3]);
					uvIndices.push_back(uvIndex[0]);
					uvIndices.push_back(uvIndex[1]);
					uvIndices.push_back(uvIndex[2]);
					uvIndices.push_back(uvIndex[3]);
					normalIndices.push_back(normalIndex[0]);
					normalIndices.push_back(normalIndex[1]);
					normalIndices.push_back(normalIndex[2]);
					normalIndices.push_back(normalIndex[3]);



					Vertex vert_1(temp_vertices[vertexIndex[0] - 1].x, temp_vertices[vertexIndex[0] - 1].y, temp_vertices[vertexIndex[0] - 1].z, temp_uvs[uvIndex[0] - 1].x, temp_uvs[uvIndex[0] - 1].y, temp_normals[normalIndices[0] - 1].x, temp_normals[normalIndices[0] - 1].y, temp_normals[normalIndices[0] - 1].z);
					Vertex vert_2(temp_vertices[vertexIndex[1] - 1].x, temp_vertices[vertexIndex[1] - 1].y, temp_vertices[vertexIndex[1] - 1].z, temp_uvs[uvIndex[1] - 1].x, temp_uvs[uvIndex[1] - 1].y, temp_normals[normalIndices[1] - 1].x, temp_normals[normalIndices[1] - 1].y, temp_normals[normalIndices[1] - 1].z);
					Vertex vert_3(temp_vertices[vertexIndex[2] - 1].x, temp_vertices[vertexIndex[2] - 1].y, temp_vertices[vertexIndex[2] - 1].z, temp_uvs[uvIndex[2] - 1].x, temp_uvs[uvIndex[2] - 1].y, temp_normals[normalIndices[2] - 1].x, temp_normals[normalIndices[2] - 1].y, temp_normals[normalIndices[2] - 1].z);
					Vertex vert_4(temp_vertices[vertexIndex[3] - 1].x, temp_vertices[vertexIndex[3] - 1].y, temp_vertices[vertexIndex[3] - 1].z, temp_uvs[uvIndex[3] - 1].x, temp_uvs[uvIndex[3] - 1].y, temp_normals[normalIndices[3] - 1].x, temp_normals[normalIndices[3] - 1].y, temp_normals[normalIndices[3] - 1].z);

					mesh_.faces.push_back(face_g(vert_1, vert_2, vert_3, vert_4));
				//	mesh_.face_ptr.push_back(new face_g(vert_1, vert_2, vert_3, vert_4));
				}	
			}
		}
		//std::cout << verts.size() << "\n";

		int r_ = (color / 1000000) * 1000000;
		int g_ = ((color) / 1000) * 1000 - r_;
		int b_ = color - r_ - g_;

		//std::cout << r_ << " : " << g_ << " : " << b_ << "\n";

		GLfloat r = (GLfloat)(r_ / 1000000) / 255.f;
		GLfloat g = (GLfloat)(g_ / 1000) / 255.f;
		GLfloat b = (GLfloat)(b_) / 255.f;

		mesh_.r = r;
		mesh_.g = g;
		mesh_.b = b;

		mesh_.type = type;
		//mesh_.vertices.push_back({ 1, r, g, b});
		/*
		for (unsigned int i = 0; i < vertexIndices.size(); i += 3) {
			//mesh_.vertices.push_back({ 0, temp_vertices[vertexIndices[i] - 1].x * scale, temp_vertices[vertexIndices[i] - 1].y * scale, temp_vertices[vertexIndices[i] - 1].z * scale });
			mesh_.faces.push_back(mesh::face({ 0, temp_vertices[vertexIndices[i] - 1].x * scale, temp_vertices[vertexIndices[i] - 1].y * scale, temp_vertices[vertexIndices[i] - 1].z * scale },
			{ 0, temp_vertices[vertexIndices[i + 1] - 1].x * scale, temp_vertices[vertexIndices[i + 1] - 1].y * scale, temp_vertices[vertexIndices[i + 1] - 1].z * scale },
			{ 0, temp_vertices[vertexIndices[i + 2] - 1].x * scale, temp_vertices[vertexIndices[i + 2] - 1].y * scale, temp_vertices[vertexIndices[i + 2] - 1].z * scale },
			{ r,g,b }));
			mesh_.faces[mesh_.faces.size() - 1].normal = { 0.f,temp_normals[normalIndices[i] - 1].x,temp_normals[normalIndices[i] - 1].y,temp_normals[normalIndices[i] - 1].z };
		}*/
		
		mesh_.commit_mesh();
		mesh_.texture_number = texture_number;

		for (unsigned int i = 0; i < vertexIndices.size(); i += 3) {
			//mesh_.vertices.push_back({ 0, temp_vertices[vertexIndices[i] - 1].x * scale, temp_vertices[vertexIndices[i] - 1].y * scale, temp_vertices[vertexIndices[i] - 1].z * scale });
//			Vertex vert_1(temp_vertices[vertexIndices[i] - 1].x, temp_vertices[vertexIndices[i] - 1].y, temp_vertices[vertexIndices[i] - 1].z, temp_uvs[uvIndices[i] - 1].x, temp_uvs[uvIndices[i] - 1].y);
//			Vertex vert_2(temp_vertices[vertexIndices[i+1] - 1].x, temp_vertices[vertexIndices[i+1] - 1].y, temp_vertices[vertexIndices[i+1] - 1].z, temp_uvs[uvIndices[i+1] - 1].x, temp_uvs[uvIndices[i+1] - 1].y);
//			Vertex vert_3(temp_vertices[vertexIndices[i+2] - 1].x, temp_vertices[vertexIndices[i+2] - 1].y, temp_vertices[vertexIndices[i+2] - 1].z, temp_uvs[uvIndices[i+2] - 1].x, temp_uvs[uvIndices[i+2] - 1].y);

//			mesh_.faces.push_back(mesh::face_g(vert_1,vert_2,vert_3));
			
			/*
			mesh_.faces.push_back(mesh::face({ 0, temp_vertices[vertexIndices[i] - 1].x * scale, temp_vertices[vertexIndices[i] - 1].y * scale, temp_vertices[vertexIndices[i] - 1].z * scale },
			{ 0, temp_vertices[vertexIndices[i + 1] - 1].x * scale, temp_vertices[vertexIndices[i + 1] - 1].y * scale, temp_vertices[vertexIndices[i + 1] - 1].z * scale },
			{ 0, temp_vertices[vertexIndices[i + 2] - 1].x * scale, temp_vertices[vertexIndices[i + 2] - 1].y * scale, temp_vertices[vertexIndices[i + 2] - 1].z * scale },
			{ r,g,b }));
			mesh_.faces[mesh_.faces.size() - 1].normal = { 0.f,temp_normals[normalIndices[i] - 1].x,temp_normals[normalIndices[i] - 1].y,temp_normals[normalIndices[i] - 1].z };
			*/
		}

	}


	//c series
	struct Vertex_c
	{
		Vertex_c() { x = 0, y = 0, z = 0, nx = 0, ny = 0, nz = 0; };
		Vertex_c(GLfloat x_, GLfloat y_, GLfloat z_, GLfloat nx_, GLfloat ny_, GLfloat nz_, int r_, int g_, int b_)
		{
			x = x_;
			y = y_;
			z = z_;
			nx = nx_;
			ny = ny_;
			nz = nz_;
			r = r_;
			g = g_;
			b = b_;
		}
		bool operator==(const Vertex_c&other) const
		{
			return (this->x == other.x && this->y == other.y && this->z == other.z && this->nx == other.nx && this->nz == other.nz && this->nz == other.nz);// && this->r == other.r && this->g == other.g && this->b == other.b);
		}
		Vertex_c operator+(const Vertex_c&other) const
		{
			return Vertex_c(this->x + other.x, this->y + other.y, this->z + other.z, this->nx, this->ny, this->nz, this->r, this->g, this->b);
		}
		GLfloat x, y, z, nx, ny, nz, nd;
		int r, g, b;
	};

	std::vector<GLfloat> calculate_centroid(Vertex_c vert1_, Vertex_c vert2_, Vertex_c vert3_)
	{
		GLfloat x = (vert1_.x + vert2_.x + vert3_.x) / 3.f;
		GLfloat y = (vert1_.y + vert2_.y + vert3_.y) / 3.f;
		GLfloat z = (vert1_.z + vert2_.z + vert3_.z) / 3.f;
		return { x, y, z };
	}

	std::vector<GLfloat> calculate_normal(Vertex_c vert1_, Vertex_c vert2_, Vertex_c vert3_)
	{
		GLfloat nx = ((vert2_.y - vert1_.y)*(vert3_.z - vert1_.z)) - ((vert2_.z - vert1_.z)*(vert3_.y - vert1_.y));
		GLfloat ny = ((vert2_.z - vert1_.z)*(vert3_.x - vert1_.x)) - ((vert2_.x - vert1_.x)*(vert3_.z - vert1_.z));
		GLfloat nz = ((vert2_.x - vert1_.x)*(vert3_.y - vert1_.y)) - ((vert2_.y - vert1_.y)*(vert3_.x - vert1_.x));
		GLfloat n = sqrtf(nx*nx + ny*ny + nz*nz);
		return { nx / n,ny / n,nz / n };
	}

	struct face_c
	{
		face_c() { vertex.push_back(Vertex_c()); vertex.push_back(Vertex_c());
		vertex.push_back(Vertex_c());
		};

		face_c(Vertex_c vert1_, Vertex_c vert2_, Vertex_c vert3_)
		{
			vertex.push_back(vert1_);
			vertex.push_back(vert2_);
			vertex.push_back(vert3_);
			position = calculate_centroid(vert1_, vert2_, vert3_);
			normal = { vert1_.nx, vert1_.ny, vert1_.nz };
			color = { vert1_.r/255.f, vert1_.g/255.f, vert1_.b/255.f};
		}
		std::vector<GLfloat> position;
		std::vector<GLfloat> color;
		std::vector<Vertex_c> vertex;
		std::vector<GLfloat> normal;

		bool operator!=(const face_c&other) const
		{
			if (this == nullptr)
				return false;
			if (&other == nullptr)
				return false;
			return !(this->vertex[0] == other.vertex[0] && this->vertex[1] == other.vertex[1] && this->vertex[2] == other.vertex[2]);
		}
		bool operator==(const face_c&other) const
		{
			return (this->vertex[0] == other.vertex[0] && this->vertex[1] == other.vertex[1] && this->vertex[2] == other.vertex[2]);
		}
	};
	
	struct Mesh_c
	{
		enum TYPE
		{
			STATIC,
			DYNAMIC
		};

		Mesh_c() 
		{
			x = 0.0; 
			y = 0.0; 
			z = 0.0;
			azimuth = 0.0;
			pitch = 0.0;
			roll = 0.0;
		};

		Mesh_c(std::vector<face_c> faces_)
		{
			faces = faces_;
		}

		void reconfigure()
		{
			for (int i = 0; i < faces.size(); i++)
			{
				faces[i].position = calculate_centroid(faces[i].vertex[0], faces[i].vertex[1], faces[i].vertex[2]);
			}
		}
		
		void commit_mesh()
		{
			vertexes.clear();
			vertex_number = 0;
			
			for (int i = 0; i < faces.size(); i++)
			{
				//std::vector<GLfloat> norms = calculate_normal(faces[i].vertex[0], faces[i].vertex[1], faces[i].vertex[2]);
				for (int j = 0; j < 3; j++)
				{
					vertexes.push_back(faces[i].vertex[j].x);
					vertexes.push_back(faces[i].vertex[j].y);
					vertexes.push_back(faces[i].vertex[j].z);
//					std::vector<GLfloat> norms_ = normalize(faces[i].vertex[j].nx, faces[i].vertex[j].ny, faces[i].vertex[j].nz);

					
//		vertexes.push_back(norms_[0]);
					//		vertexes.push_back(norms_[1]);
					//		vertexes.push_back(norms_[2]);
				//	vertexes.push_back(norms[0]);
				//	vertexes.push_back(norms[1]);
				//	vertexes.push_back(norms[2]);

					vertexes.push_back(faces[i].vertex[j].nx);
					vertexes.push_back(faces[i].vertex[j].ny);
					vertexes.push_back(faces[i].vertex[j].nz);

				//	vertexes.push_back(r_);
				//	vertexes.push_back(g_);
				//	vertexes.push_back(b_);
					vertexes.push_back(faces[i].vertex[j].r / 255.f);
					vertexes.push_back(faces[i].vertex[j].g / 255.f);
					vertexes.push_back(faces[i].vertex[j].b / 255.f);

					//TODO: why this ^ static? look into the reader perhaps;
				}
				vertex_number += 3;
			}
			vertex_array = &vertexes[0];
			//std::cout << vertex_array << "\n";
		}
		
		std::vector<GLfloat> vertexes;
		std::vector<face_c> faces;
		int vertex_number;
		float x, y, z;
		float azimuth, pitch, roll;
		GLfloat* vertex_array;
		float width = 1.f, height = 1.f, depth = 1.f;
		int type;
		bool re_color = false;
		int r, g, b;
		std::string file;
		TYPE s_type = DYNAMIC;
		bool hud = false;
	};

	Vertex_c transform_vertex(Vertex_c vert, float x, float y, float z, float azi, float pit, float rol)
	{
		float a = pit, b = azi, c = rol;

		float ca = cos(a * (PI / 180.f)), sa = sin(a * (PI / 180.f)), cb = cos(b * (PI / 180.f)), sb = sin(b * (PI / 180.f)), cc = cos(c * (PI / 180.f)), sc = sin(c * (PI / 180.f));
		
		float x_ = cb*cc * vert.x + -cb*sc * vert.y + sb * vert.z + x;
		float y_ = (sa*sb*cc + ca*sc)*vert.x + (-sa*sb*sc + ca*cc)*vert.y + -sa*cb*vert.z + y;
		float z_ = (-ca*sb*cc + sa*sc)*vert.x + (ca*sb*sc + sa*cc)*vert.y + ca*cb*vert.z + z;
		
		float nx_ = cb*cc * vert.nx + -cb*sc * vert.ny + sb * vert.nz;
		float ny_ = (sa*sb*cc + ca*sc)*vert.nx + (-sa*sb*sc + ca*cc)*vert.ny + -sa*cb*vert.nz;
		float nz_ = (-ca*sb*cc + sa*sc)*vert.nx + (ca*sb*sc + sa*cc)*vert.ny + ca*cb*vert.nz;

		return Vertex_c(x_, y_, z_, nx_, ny_, nz_, vert.r, vert.g, vert.b);
	}

	void resize_mesh(Mesh_c *mesh_, float x_size, float y_size, float z_size)
	{
		if (mesh_->width != x_size || mesh_->height != y_size || mesh_->depth != z_size)
		{
			float x_scale = x_size / mesh_->width;
			float y_scale = y_size / mesh_->height;
			float z_scale = z_size / mesh_->depth;
			for (int i = 0; i < mesh_->faces.size(); i++)
			{
				for (int j = 0; j < 3; j++)
				{
					mesh_->faces[i].vertex[j].x *= x_scale;
					mesh_->faces[i].vertex[j].y *= y_scale;
					mesh_->faces[i].vertex[j].z *= z_scale;
					if (x_size < 0)
						mesh_->faces[i].vertex[j].nx *= -1.0;
					if (y_size < 0)
						mesh_->faces[i].vertex[j].ny *= -1.0;
					if (z_size < 0)
						mesh_->faces[i].vertex[j].nz *= -1.0;
				}
				mesh_->faces[i].position = calculate_centroid(mesh_->faces[i].vertex[0], mesh_->faces[i].vertex[1], mesh_->faces[i].vertex[2]);
			}
			mesh_->commit_mesh();
			mesh_->width = x_size;
			mesh_->height = y_size;
			mesh_->depth = z_size;
		}
	}

	void translate_mesh(Mesh_c *mesh_, float x_, float y_, float z_)
	{
		for (int i = 0; i < mesh_->faces.size(); i++)
		{
			for (int j = 0; j < 3; j++)
			{
				mesh_->faces[i].vertex[j].x += x_;
				mesh_->faces[i].vertex[j].y += y_;
				mesh_->faces[i].vertex[j].z += z_;
			}
			mesh_->faces[i].position = calculate_centroid(mesh_->faces[i].vertex[0], mesh_->faces[i].vertex[1], mesh_->faces[i].vertex[2]);
		}
		mesh_->commit_mesh();
	}

	//rotate about y, so not a nasty amount of linear algebra.
	//	assume the rotation is about origin.
	void rotate_mesh(Mesh_c *mesh_, double angle, double px, double pz)
	{
		double s = sin(angle*TO_RAD);
		double c = cos(angle*TO_RAD);
		if (fabs(s) < 0.0001) s = 0;
		if (fabs(c) < 0.0001) c = 0;
		if (s < -0.99) s = -1;
		if (s > 0.99) s = 1;
		if (c < -0.99) c = -1;
		if (c > 0.99) c = 1;
		for (int i = 0; i < mesh_->faces.size(); i++)
		{
			for (int j = 0; j < 3; j++)
			{
				//x: x * c - z * s;
				//z: x * s + z * c;
				double x = mesh_->faces[i].vertex[j].x - px;
				double z = mesh_->faces[i].vertex[j].z - pz;
				double nx = mesh_->faces[i].vertex[j].nx;
				double nz = mesh_->faces[i].vertex[j].nz;
				mesh_->faces[i].vertex[j].x = x * c - z * s + px;
				mesh_->faces[i].vertex[j].z = x * s + z * c + pz;
				mesh_->faces[i].vertex[j].nx = nx * c - nz * s;
				mesh_->faces[i].vertex[j].nz = nx * s + nz * c;
				//*/
			}
			mesh_->faces[i].position = calculate_centroid(mesh_->faces[i].vertex[0], mesh_->faces[i].vertex[1], mesh_->faces[i].vertex[2]);
		}
		mesh_->commit_mesh();
	}

	void recolor_mesh(Mesh_c *mesh_, int r, int g, int b)
	{
		for (int i = 0; i < mesh_->faces.size(); i++)
		{
			for (int j = 0; j < 3; j++)
			{
				mesh_->faces[i].vertex[j].r = r;
				mesh_->faces[i].vertex[j].g = g;
				mesh_->faces[i].vertex[j].b = b;
			}
		}
		mesh_->commit_mesh();
		mesh_->r = r;
		mesh_->g = g;
		mesh_->b = b;
	}

	void calculate_bounds(Mesh_c * mesh_, double &lx, double &ly, double &lz, double &hx, double &hy, double &hz)
	{
		bool first = true;

		for (int i = 0; i < mesh_->faces.size(); i++)
		{
			for (int j = 0; j < 3; j++)
			{
				double x = mesh_->faces[i].vertex[j].x;
				double y = mesh_->faces[i].vertex[j].y;
				double z = mesh_->faces[i].vertex[j].z;
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
		if (fabs(lx) <= 0.0001) lx = 0.0;
		if (fabs(hx) <= 0.0001) hx = 0.0;
		if (fabs(ly) <= 0.0001) ly = 0.0;
		if (fabs(hy) <= 0.0001) hy = 0.0;
		if (fabs(lz) <= 0.0001) lz = 0.0;
		if (fabs(hz) <= 0.0001) hz = 0.0;
		lx += mesh_->x;
		hx += mesh_->x;
		ly += mesh_->y;
		hy += mesh_->y;
		lz += mesh_->z;
		hz += mesh_->z;
	}

	void set_bounds(Mesh_c * mesh_, double lx, double ly, double lz, double hx, double hy, double hz)
	{
	//	std::cout << lx << " : " << hx << " : " << ly << " : " << hy << " : " << lz << " : " << hz << "\n";
		for (int i = 0; i < mesh_->faces.size(); i++)
		{
			for (int j = 0; j < 3; j++)
			{
				if (fabs(mesh_->faces[i].vertex[j].x - lx) <= fabs(mesh_->faces[i].vertex[j].x - hx))
					mesh_->faces[i].vertex[j].x = lx;
				else
					mesh_->faces[i].vertex[j].x = hx;
				
				if (fabs(mesh_->faces[i].vertex[j].y - ly) <= fabs(mesh_->faces[i].vertex[j].y - hy))
					mesh_->faces[i].vertex[j].y = ly;
				else
					mesh_->faces[i].vertex[j].y = hy;

				if (fabs(mesh_->faces[i].vertex[j].z - lz) <= fabs(mesh_->faces[i].vertex[j].z - hz))
					mesh_->faces[i].vertex[j].z = lz;
				else
					mesh_->faces[i].vertex[j].z = hz;

				//mesh_->faces[i].vertex[j].x += x_;
				//mesh_->faces[i].vertex[j].y += y_;
				//mesh_->faces[i].vertex[j].z += z_;
			}
			mesh_->faces[i].position = calculate_centroid(mesh_->faces[i].vertex[0], mesh_->faces[i].vertex[1], mesh_->faces[i].vertex[2]);
		}
		mesh_->commit_mesh();
	}

	void snap_vert(Vertex_c & vert)
	{
		vert.x = round(vert.x/settings::snap_grid) * settings::snap_grid;
		vert.y = round(vert.y/settings::snap_grid) * settings::snap_grid;
		vert.z = round(vert.z/settings::snap_grid) * settings::snap_grid;
	}

	void snap_face(face_c & face)
	{
		snap_vert(face.vertex[0]);
		snap_vert(face.vertex[1]);
		snap_vert(face.vertex[2]);
	}

	Mesh_c get_object_collider()
	{
		Vertex_c vert1(1.0, 2.0, 2.0, 0.00000, - 0.0, 1.0, 141, 92, 207);
		Vertex_c vert2(-1.0, 2.0, 2.0, 0.0, - 0.0, 1.0, 141, 92, 207);
		Vertex_c vert3(-1.0, 0.0, 2.0, 0.0, - 0.0, 1.0, 141, 92, 207);
		Vertex_c vert4(1.0, 0.0, 2.0, 0.0, - 0.0, 1.0, 141, 92, 207);
		Vertex_c vert5(1.0, 2.0, 0.0, 1.0, - 0.0, 0.0, 141, 92, 207);
		Vertex_c vert6(1.0, 2.0, 2.0, 1.0, - 0.0, 0.0, 141, 92, 207);
		Vertex_c vert7(1.0, 0.0, 2.0, 1.0, - 0.0, 0.0, 141 ,92 ,207);
		Vertex_c vert8(1.0, 0.0, 0.0, 1.0, - 0.0, 0.0, 141, 92 ,207);
		Vertex_c vert9(- 1.0, 0.0, 0.0, - 1.0, 0.0, - 0.0, 141, 92, 207);
		Vertex_c vert10(- 1.0, 0.0, 2.0, - 1.0, 0.0, - 0.0, 141 ,92, 207);
		Vertex_c vert11(- 1.0, 2.0, 2.0, - 1.0, 0.0, - 0.0, 141, 92, 207);
		Vertex_c vert12(- 1.0, 2.0, 0.0, - 1.0, 0.0, - 0.0, 141, 92 ,207);
		Vertex_c vert13(1.0, 2.0, 2.0, 0.0, 1.0, 0.0, 141, 92 ,207);
		Vertex_c vert14(1.0, 2.0, 0.0, 0.0, 1.0, 0.0, 141, 92, 207);
		Vertex_c vert15(- 1.0, 2.0, 0.0, 0.0, 1.0, 0.0, 141, 92 ,207);
		Vertex_c vert16(- 1.0, 2.0, 2.0, 0.0, 1.0, 0.0, 141, 92, 207);
	}

	Mesh_c get_wall_object_collider()
	{
		Vertex_c vert1(1.0, 2.0, 2.0, 0.00000, -0.0, 1.0, 141, 92, 207);
		Vertex_c vert2(-1.0, 2.0, 2.0, 0.0, -0.0, 1.0, 141, 92, 207);
		Vertex_c vert3(-1.0, 0.0, 2.0, 0.0, -0.0, 1.0, 141, 92, 207);
		Vertex_c vert4(1.0, 0.0, 2.0, 0.0, -0.0, 1.0, 141, 92, 207);
		Vertex_c vert5(1.0, 2.0, 0.0, 1.0, -0.0, 0.0, 141, 92, 207);
		Vertex_c vert6(1.0, 2.0, 2.0, 1.0, -0.0, 0.0, 141, 92, 207);
		Vertex_c vert7(1.0, 0.0, 2.0, 1.0, -0.0, 0.0, 141, 92, 207);
		Vertex_c vert8(1.0, 0.0, 0.0, 1.0, -0.0, 0.0, 141, 92, 207);
		Vertex_c vert9(-1.0, 0.0, 0.0, -1.0, 0.0, -0.0, 141, 92, 207);
		Vertex_c vert10(-1.0, 0.0, 2.0, -1.0, 0.0, -0.0, 141, 92, 207);
		Vertex_c vert11(-1.0, 2.0, 2.0, -1.0, 0.0, -0.0, 141, 92, 207);
		Vertex_c vert12(-1.0, 2.0, 0.0, -1.0, 0.0, -0.0, 141, 92, 207);
		Vertex_c vert13(1.0, 2.0, 2.0, 0.0, 1.0, 0.0, 141, 92, 207);
		Vertex_c vert14(1.0, 2.0, 0.0, 0.0, 1.0, 0.0, 141, 92, 207);
		Vertex_c vert15(-1.0, 2.0, 0.0, 0.0, 1.0, 0.0, 141, 92, 207);
		Vertex_c vert16(-1.0, 2.0, 2.0, 0.0, 1.0, 0.0, 141, 92, 207);
	}

	Mesh_c * combine_meshes(Mesh_c * obj1, Mesh_c * obj2)
	{
		Mesh_c * out = new Mesh_c();
		for (int i = 0; i < obj1->faces.size(); i++)
		{
			out->faces.push_back(obj1->faces[i]);
		}
		for (int i = 0; i < obj2->faces.size(); i++)
		{
			out->faces.push_back(obj2->faces[i]);
		}
		out->commit_mesh();
		return out;
	}

	/*
	bool read_obj(const char* file_path, mesh::Mesh_t &mesh_, int color, bool use_uv)
	{
	//std::cout << "you okay?" << "\n";
	//std::ifstream file("src/untiltled1.obj");
	//std::cout << "hey" << "\n";

	FILE * file = fopen(file_path, "r");
	if (file == NULL) {
	printf("Impossible to open the file !\n");
	return false;
	}

	std::vector<sf::Vector3f> temp_vertices;
	std::vector<sf::Vector2f> temp_uvs;
	std::vector<sf::Vector3f> temp_normals;

	std::vector<unsigned int> vertexIndices;
	std::vector<unsigned int> uvIndices;
	std::vector<unsigned int> normalIndices;

	while (1 == 1) {

	char lineHeader[128];
	// read the first word of the line
	int res = fscanf(file, "%s", lineHeader);
	if (res == EOF)
	break;
	if (strcmp(lineHeader, "v") == 0) {
	sf::Vector3f vertex;
	fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
	temp_vertices.push_back(vertex);
	}
	else if (strcmp(lineHeader, "vt") == 0) {
	sf::Vector2f uv;
	fscanf(file, "%f %f\n", &uv.x, &uv.y);
	temp_uvs.push_back(uv);
	}

	else if (strcmp(lineHeader, "vn") == 0) {
	sf::Vector3f normal;
	fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
	temp_normals.push_back(normal);
	}
	else if (strcmp(lineHeader, "f") == 0) {
	std::string vertex1, vertex2, vertex3;
	unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];

	if (use_uv)
	{
	int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);

	if (matches != 9) {
	printf("File can't be read by our simple parser : ( Try exporting with other options\n");
	return false;
	}
	vertexIndices.push_back(vertexIndex[0]);
	vertexIndices.push_back(vertexIndex[1]);
	vertexIndices.push_back(vertexIndex[2]);
	uvIndices.push_back(uvIndex[0]);
	uvIndices.push_back(uvIndex[1]);
	uvIndices.push_back(uvIndex[2]);
	normalIndices.push_back(normalIndex[0]);
	normalIndices.push_back(normalIndex[1]);
	normalIndices.push_back(normalIndex[2]);
	}
	else
	{
	int matches = fscanf(file, "%d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2]);

	if (matches != 6) {
	printf("File can't be read by our simple parser : ( Try exporting with other options\n");
	return false;
	}
	vertexIndices.push_back(vertexIndex[0]);
	vertexIndices.push_back(vertexIndex[1]);
	vertexIndices.push_back(vertexIndex[2]);
	//uvIndices.push_back(uvIndex[0]);
	//uvIndices.push_back(uvIndex[1]);
	//uvIndices.push_back(uvIndex[2]);
	normalIndices.push_back(normalIndex[0]);
	normalIndices.push_back(normalIndex[1]);
	normalIndices.push_back(normalIndex[2]);
	}
	}

	}
	//std::cout << verts.size() << "\n";

	int r_ = (color / 1000000) * 1000000;
	int g_ = ((color) / 1000) * 1000 - r_;
	int b_ = color - r_ - g_;

	//std::cout << r_ << " : " << g_ << " : " << b_ << "\n";

	GLfloat r = (GLfloat)(r_ / 1000000) / 255.f;
	GLfloat g = (GLfloat)(g_ / 1000) / 255.f;
	GLfloat b = (GLfloat)(b_) / 255.f;

	mesh_.r = r;
	mesh_.g = g;
	mesh_.b = b;

	//mesh_.vertices.push_back({ 1, r, g, b});
	for (unsigned int i = 0; i < vertexIndices.size(); i += 3) {
	//mesh_.vertices.push_back({ 0, temp_vertices[vertexIndices[i] - 1].x * scale, temp_vertices[vertexIndices[i] - 1].y * scale, temp_vertices[vertexIndices[i] - 1].z * scale });
	mesh_.faces.push_back(mesh::face({ 0, temp_vertices[vertexIndices[i] - 1].x * scale, temp_vertices[vertexIndices[i] - 1].y * scale, temp_vertices[vertexIndices[i] - 1].z * scale },
	{ 0, temp_vertices[vertexIndices[i + 1] - 1].x * scale, temp_vertices[vertexIndices[i + 1] - 1].y * scale, temp_vertices[vertexIndices[i + 1] - 1].z * scale },
	{ 0, temp_vertices[vertexIndices[i + 2] - 1].x * scale, temp_vertices[vertexIndices[i + 2] - 1].y * scale, temp_vertices[vertexIndices[i + 2] - 1].z * scale },
	{ r,g,b }));
	mesh_.faces[mesh_.faces.size() - 1].normal = { 0.f,temp_normals[normalIndices[i] - 1].x,temp_normals[normalIndices[i] - 1].y,temp_normals[normalIndices[i] - 1].z };
	}
	/*
	for (unsigned int i = 0; i < normalIndices.size(); i++)
	{
	mesh_.normals.push_back({ 0, temp_normals[vertexIndices[i] - 1].x, temp_normals[vertexIndices[i] - 1].y, temp_normals[vertexIndices[i] - 1].z });
	}
	
	//std::cout << mesh_.normals.size() << " : " << mesh_.vertices.size() << "\n";
	/*
	for (unsigned int i = 0; i < vertexIndices.size(); i += 3)
	{
	std::vector<GLfloat> norm = calculate_normal({ mesh_.vertices[i][1], mesh_.vertices[i][2], mesh_.vertices[i][2] }, { mesh_.vertices[i + 1][1],mesh_.vertices[i + 1][2],mesh_.vertices[i + 1][3] }, { mesh_.vertices[i + 2][1],mesh_.vertices[i + 2][2],mesh_.vertices[i + 2][3] });
	mesh_.normals.push_back({ 0,norm[0],norm[1],norm[2] });
	mesh_.normals.push_back({ 0,norm[0],norm[1],norm[2] });
	mesh_.normals.push_back({ 0,norm[0],norm[1],norm[2] });
	}
	
	//	std::cout << verts.size() << "\n";


	/*

	for (unsigned int i = 0; i < vertexIndices.size(); i++)
	{
	unsigned int vertexIndex = vertexIndices[i];
	sf::Vector3f vertex = temp_vertices[vertexIndex - 1];
	vert_out.push_back(vertex);
	}
	for (unsigned int i = 0; i < vertexIndices.size(); i++)
	{
	unsigned int vertexIndex = vertexIndices[i];
	sf::Vector2f uv = temp_uvs[vertexIndex - 1];
	uvs_out.push_back(uv);
	}
	for (unsigned int i = 0; i < vertexIndices.size(); i++)
	{
	unsigned int vertexIndex = vertexIndices[i];
	sf::Vector3f normal = temp_normals[vertexIndex - 1];
	normal_out.push_back(ormal);
	}
	aaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
	

	}
	bool read_obj(const char* file_path, mesh::Mesh_t &mesh_, int color, bool use_uv, float width_)
	{
		FILE * file = fopen(file_path, "r");
		if (file == NULL) {
			printf("Impossible to open the file !\n");
			return false;
		}

		std::vector<sf::Vector3f> temp_vertices;
		std::vector<sf::Vector2f> temp_uvs;
		std::vector<sf::Vector3f> temp_normals;

		std::vector<unsigned int> vertexIndices;
		std::vector<unsigned int> uvIndices;
		std::vector<unsigned int> normalIndices;

		while (1 == 1) {

			char lineHeader[128];
			// read the first word of the line
			int res = fscanf(file, "%s", lineHeader);
			if (res == EOF)
				break;
			if (strcmp(lineHeader, "v") == 0) {
				sf::Vector3f vertex;
				fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
				temp_vertices.push_back(vertex);
			}
			else if (strcmp(lineHeader, "vt") == 0) {
				sf::Vector2f uv;
				fscanf(file, "%f %f\n", &uv.x, &uv.y);
				temp_uvs.push_back(uv);
			}

			else if (strcmp(lineHeader, "vn") == 0) {
				sf::Vector3f normal;
				fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
				temp_normals.push_back(normal);
			}
			else if (strcmp(lineHeader, "f") == 0) {
				std::string vertex1, vertex2, vertex3;
				unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];

				if (use_uv)
				{
					int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);

					if (matches != 9) {
						printf("File can't be read by our simple parser : ( Try exporting with other options\n");
						return false;
					}
					vertexIndices.push_back(vertexIndex[0]);
					vertexIndices.push_back(vertexIndex[1]);
					vertexIndices.push_back(vertexIndex[2]);
					uvIndices.push_back(uvIndex[0]);
					uvIndices.push_back(uvIndex[1]);
					uvIndices.push_back(uvIndex[2]);
					normalIndices.push_back(normalIndex[0]);
					normalIndices.push_back(normalIndex[1]);
					normalIndices.push_back(normalIndex[2]);
				}
				else
				{
					int matches = fscanf(file, "%d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2]);

					if (matches != 6) {
						printf("File can't be read by our simple parser : ( Try exporting with other options\n");
						return false;
					}
					vertexIndices.push_back(vertexIndex[0]);
					vertexIndices.push_back(vertexIndex[1]);
					vertexIndices.push_back(vertexIndex[2]);
					normalIndices.push_back(normalIndex[0]);
					normalIndices.push_back(normalIndex[1]);
					normalIndices.push_back(normalIndex[2]);
				}
			}

		}

		int r_ = (color / 1000000) * 1000000;
		int g_ = ((color) / 1000) * 1000 - r_;
		int b_ = color - r_ - g_;

		GLfloat r = (GLfloat)(r_ / 1000000) / 255.f;
		GLfloat g = (GLfloat)(g_ / 1000) / 255.f;
		GLfloat b = (GLfloat)(b_) / 255.f;

		mesh_.r = r;
		mesh_.g = g;
		mesh_.b = b;
		width_ = width_ / 2.0;
		for (unsigned int i = 0; i < vertexIndices.size(); i += 3) {
			mesh_.faces.push_back(mesh::face({ 0, temp_vertices[vertexIndices[i] - 1].x * width_, temp_vertices[vertexIndices[i] - 1].y * width_, temp_vertices[vertexIndices[i] - 1].z * width_ },
			{ 0, temp_vertices[vertexIndices[i + 1] - 1].x * width_, temp_vertices[vertexIndices[i + 1] - 1].y * width_, temp_vertices[vertexIndices[i + 1] - 1].z * width_ },
			{ 0, temp_vertices[vertexIndices[i + 2] - 1].x * width_, temp_vertices[vertexIndices[i + 2] - 1].y * width_, temp_vertices[vertexIndices[i + 2] - 1].z * width_ },
			{ r,g,b }));
			mesh_.faces[mesh_.faces.size() - 1].normal = { 0.f,temp_normals[normalIndices[i] - 1].x,temp_normals[normalIndices[i] - 1].y,temp_normals[normalIndices[i] - 1].z };
		}
	}

	bool read_obj(const char* file_path, mesh::Mesh_r &mesh_, int color, bool use_uv, float width_)
	{
		FILE * file = fopen(file_path, "r");
		if (file == NULL) {
			printf("Impossible to open the file !\n");
			return false;
		}

		std::vector<sf::Vector3f> temp_vertices;
		std::vector<sf::Vector2f> temp_uvs;
		std::vector<sf::Vector3f> temp_normals;

		std::vector<unsigned int> vertexIndices;
		std::vector<unsigned int> uvIndices;
		std::vector<unsigned int> normalIndices;

		while (1 == 1) {

			char lineHeader[128];
			// read the first word of the line
			int res = fscanf(file, "%s", lineHeader);
			if (res == EOF)
				break;
			if (strcmp(lineHeader, "v") == 0) {
				sf::Vector3f vertex;
				fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
				temp_vertices.push_back(vertex);
			}
			else if (strcmp(lineHeader, "vt") == 0) {
				sf::Vector2f uv;
				fscanf(file, "%f %f\n", &uv.x, &uv.y);
				temp_uvs.push_back(uv);
			}

			else if (strcmp(lineHeader, "vn") == 0) {
				sf::Vector3f normal;
				fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
				temp_normals.push_back(normal);
			}
			else if (strcmp(lineHeader, "f") == 0) {
				std::string vertex1, vertex2, vertex3;
				unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];

				if (use_uv)
				{
					int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);

					if (matches != 9) {
						printf("File can't be read by our simple parser : ( Try exporting with other options\n");
						return false;
					}
					vertexIndices.push_back(vertexIndex[0]);
					vertexIndices.push_back(vertexIndex[1]);
					vertexIndices.push_back(vertexIndex[2]);
					uvIndices.push_back(uvIndex[0]);
					uvIndices.push_back(uvIndex[1]);
					uvIndices.push_back(uvIndex[2]);
					normalIndices.push_back(normalIndex[0]);
					normalIndices.push_back(normalIndex[1]);
					normalIndices.push_back(normalIndex[2]);
				}
				else
				{
					int matches = fscanf(file, "%d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2]);

					if (matches != 6) {
						printf("File can't be read by our simple parser : ( Try exporting with other options\n");
						return false;
					}
					vertexIndices.push_back(vertexIndex[0]);
					vertexIndices.push_back(vertexIndex[1]);
					vertexIndices.push_back(vertexIndex[2]);
					normalIndices.push_back(normalIndex[0]);
					normalIndices.push_back(normalIndex[1]);
					normalIndices.push_back(normalIndex[2]);
				}
			}

		}

		int r_ = (color / 1000000) * 1000000;
		int g_ = ((color) / 1000) * 1000 - r_;
		int b_ = color - r_ - g_;

		GLfloat r = (GLfloat)(r_ / 1000000) / 255.f;
		GLfloat g = (GLfloat)(g_ / 1000) / 255.f;
		GLfloat b = (GLfloat)(b_) / 255.f;

		mesh_.r = r;
		mesh_.g = g;
		mesh_.b = b;
		width_ = width_ / 2.0;
		for (unsigned int i = 0; i < vertexIndices.size(); i += 3) {
			mesh_.faces.push_back(mesh::face({ 0, temp_vertices[vertexIndices[i] - 1].x * width_, temp_vertices[vertexIndices[i] - 1].y * width_, temp_vertices[vertexIndices[i] - 1].z * width_ },
			{ 0, temp_vertices[vertexIndices[i + 1] - 1].x * width_, temp_vertices[vertexIndices[i + 1] - 1].y * width_, temp_vertices[vertexIndices[i + 1] - 1].z * width_ },
			{ 0, temp_vertices[vertexIndices[i + 2] - 1].x * width_, temp_vertices[vertexIndices[i + 2] - 1].y * width_, temp_vertices[vertexIndices[i + 2] - 1].z * width_ },
			{ r,g,b }));
			mesh_.faces[mesh_.faces.size() - 1].normal = { 0.f,temp_normals[normalIndices[i] - 1].x,temp_normals[normalIndices[i] - 1].y,temp_normals[normalIndices[i] - 1].z };
		}
		mesh_.azimuth = 0;
		mesh_.pitch = 0;
		mesh_.roll = 0;
		
	}
	*/
}

namespace mesh_p
{
	struct Vertex
	{
		Vertex()
		{
			x, y, z, uvx, uvy, nx, ny, nz, nd = 0.f;
		}
		Vertex(GLfloat x_, GLfloat y_, GLfloat z_, GLfloat nx_, GLfloat ny_, GLfloat nz_)
		{
			x = x_;
			y = y_;
			z = z_;
			nx = nx_;
			ny = ny_;
			nz = nz_;
		}
		bool operator==(const Vertex&other) const
		{
			return (this->x == other.x && this->y == other.y && this->z == other.z && this->nx == other.nx && this->nz == other.nz && this->nz == other.nz);
		}
		Vertex operator+(const Vertex&other) const
		{
			return Vertex(this->x + other.x, this->y + other.y, this->z + other.z, this->nx, this->ny, this->nz);
		}
		GLfloat x, y, z, uvx, uvy, nx, ny, nz, nd;
	};

	struct Vertex_c : Vertex
	{
		int r, g, b = 0;
		Vertex_c() {};
		Vertex_c(GLfloat x_, GLfloat y_, GLfloat z_, GLfloat nx_, GLfloat ny_, GLfloat nz_, int r_, int g_, int b_) : Vertex(x_, y_, z_, nx_, ny_, nz_)
		{
			r = r_;
			g = g_;
			b = b_;
		}
		bool operator==(const Vertex_c&other) const
		{
			return (this->x == other.x && this->y == other.y && this->z == other.z && this->nx == other.nx && this->nz == other.nz && this->nz == other.nz && this->r == other.r && this->g == other.g && this->b == other.b);
		}
		Vertex_c operator+(const Vertex_c&other) const
		{
			return Vertex_c(this->x + other.x, this->y + other.y, this->z + other.z, this->nx, this->ny, this->nz, this->r, this->g, this->b);
		}
	};

	std::vector<GLfloat> calculate_centroid(Vertex vert1_, Vertex vert2_, Vertex vert3_)
	{
		GLfloat x = (vert1_.x + vert2_.x + vert3_.x) / 3.f;
		GLfloat y = (vert1_.y + vert2_.y + vert3_.y) / 3.f;
		GLfloat z = (vert1_.z + vert2_.z + vert3_.z) / 3.f;
		return { x, y, z };
	}

	std::vector<GLfloat> calculate_normal(Vertex vert1_, Vertex vert2_, Vertex vert3_)
	{
		GLfloat nx = ((vert2_.y - vert1_.y)*(vert3_.z - vert1_.z)) - ((vert2_.z - vert1_.z)*(vert3_.y - vert1_.y));
		GLfloat ny = ((vert2_.z - vert1_.z)*(vert3_.x - vert1_.x)) - ((vert2_.x - vert1_.x)*(vert3_.z - vert1_.z));
		GLfloat nz = ((vert2_.x - vert1_.x)*(vert3_.y - vert1_.y)) - ((vert2_.y - vert1_.y)*(vert3_.x - vert1_.x));
		GLfloat n = sqrtf(nx*nx + ny*ny + nz*nz);
		return { nx / n,ny / n,nz / n };
	}

	struct face
	{
		face() {};

		face(Vertex vert1_, Vertex vert2_, Vertex vert3_)
		{
			vertex.push_back(&vert1_);
			vertex.push_back(&vert2_);
			vertex.push_back(&vert3_);
			position = calculate_centroid(vert1_, vert2_, vert3_);
			normal = { vert1_.nx, vert1_.ny, vert1_.nz };
		//	color = { vert1_.r / 255.f, vert1_.g / 255.f, vert1_.b / 255.f };
		}
		
		
		std::vector<GLfloat> position;
	//	std::vector<GLfloat> color;
		std::vector<Vertex*> vertex;
		std::vector<GLfloat> normal;
	};

	struct face_c : face
	{
		std::vector<GLfloat> color;
		std::vector<Vertex_c *> vertex;
		face_c(Vertex_c vert1_, Vertex_c vert2_, Vertex_c vert3_) : face(vert1_,vert2_,vert3_)
		{
			vertex.push_back(&vert1_);
			vertex.push_back(&vert2_);
			vertex.push_back(&vert3_);
			color = { vert1_.r / 255.f, vert1_.g / 255.f, vert1_.b / 255.f };
		}
	};

	struct Mesh
	{
		enum TYPE
		{
			NO_COLOR,
			COLOR,
			TEXTURE
		};

		Mesh() {};

		Mesh(std::vector<face> faces_)
		{
			faces = faces_;
			type = TYPE::NO_COLOR;
		}

		void commit_mesh()
		{
			vertexes.clear();
			vertex_number = 0;

			for (int i = 0; i < faces.size(); i++)
			{
				//std::vector<GLfloat> norms = calculate_normal(faces[i].vertex[0], faces[i].vertex[1], faces[i].vertex[2]);
				for (int j = 0; j < 3; j++)
				{
					vertexes.push_back(faces[i].vertex[j]->x);
					vertexes.push_back(faces[i].vertex[j]->y);
					vertexes.push_back(faces[i].vertex[j]->z);
					//					std::vector<GLfloat> norms_ = normalize(faces[i].vertex[j].nx, faces[i].vertex[j].ny, faces[i].vertex[j].nz);


					//		vertexes.push_back(norms_[0]);
					//		vertexes.push_back(norms_[1]);
					//		vertexes.push_back(norms_[2]);
					//	vertexes.push_back(norms[0]);
					//	vertexes.push_back(norms[1]);
					//	vertexes.push_back(norms[2]);

					vertexes.push_back(faces[i].vertex[j]->nx);
					vertexes.push_back(faces[i].vertex[j]->ny);
					vertexes.push_back(faces[i].vertex[j]->nz);

					//	vertexes.push_back(r_);
					//	vertexes.push_back(g_);
					//	vertexes.push_back(b_);
				//	vertexes.push_back(faces[i].vertex[j].r / 255.f);
				//	vertexes.push_back(faces[i].vertex[j].g / 255.f);
				//	vertexes.push_back(faces[i].vertex[j].b / 255.f);

					//TODO: why this ^ static? look into the reader perhaps;
				}
				vertex_number += 3;
			}
			vertex_array = &vertexes[0];
			//std::cout << vertex_array << "\n";
		}

		std::vector<GLfloat> vertexes;
		std::vector<face> faces;
		int vertex_number;
		float x, y, z;
		float azimuth, pitch, roll;
		GLfloat* vertex_array;
		float width = 1.f, height = 1.f, depth = 1.f;
		int type;
		std::string file;
	};

	struct Mesh_c : Mesh
	{
		std::vector<face_c> faces;
		bool re_color = false;
		int r, g, b;

		Mesh_c() : Mesh() 
		{
			type = TYPE::COLOR;
		}

		Mesh_c (std::vector<face_c> faces_ ) : Mesh()	
		{
			faces = faces_;
			type = TYPE::NO_COLOR;
		}
	
		void commit_mesh()
		{
		//	std::cout << "we out here?\n";
			vertexes.clear();
			vertex_number = 0;
			for (int i = 0; i < faces.size(); i++)
			{
				for (int j = 0; j < 3; j++)
				{
				//	std::cout << "das vectors??\n";
				//	std::cout << &faces[i].vertex[0]<< "\n";
					this->vertexes.push_back(faces[i].vertex[j]->x);
					this->vertexes.push_back(faces[i].vertex[j]->y);
					this->vertexes.push_back(faces[i].vertex[j]->z);
				//	std::cout << "der norms??\n";
					vertexes.push_back(faces[i].vertex[j]->nx);
					vertexes.push_back(faces[i].vertex[j]->ny);
					vertexes.push_back(faces[i].vertex[j]->nz);
				//	std::cout << "col boys?\n";
					vertexes.push_back(faces[i].vertex[j]->r / 255.f);
					vertexes.push_back(faces[i].vertex[j]->g / 255.f);
					vertexes.push_back(faces[i].vertex[j]->b / 255.f);
				}
				vertex_number += 3;
			}
			vertex_array = &vertexes[0];
		//	std::cout << "whete?\n";
		}
	};

	void resize_mesh(Mesh *mesh_, float x_size, float y_size, float z_size)
	{
		if (mesh_->width != x_size || mesh_->height != y_size || mesh_->depth != z_size)
		{
			float x_scale = x_size / mesh_->width;
			float y_scale = y_size / mesh_->height;
			float z_scale = z_size / mesh_->depth;
			for (int i = 0; i < mesh_->faces.size(); i++)
			{
				for (int j = 0; j < 3; j++)
				{
					mesh_->faces[i].vertex[j]->x *= x_scale;
					mesh_->faces[i].vertex[j]->y *= y_scale;
					mesh_->faces[i].vertex[j]->z *= z_scale;
				}
				mesh_->faces[i].position = calculate_centroid(*mesh_->faces[i].vertex[0], *mesh_->faces[i].vertex[1], *mesh_->faces[i].vertex[2]);
			}
			mesh_->commit_mesh();
			mesh_->width = x_size;
			mesh_->height = y_size;
			mesh_->depth = z_size;
		}
	}

	void recolor_mesh(Mesh_c *mesh_, int r, int g, int b)
	{
		for (int i = 0; i < mesh_->faces.size(); i++)
		{
			for (int j = 0; j < 3; j++)
			{
				mesh_->faces[i].vertex[j]->r = r;
				mesh_->faces[i].vertex[j]->g = g;
				mesh_->faces[i].vertex[j]->b = b;
			}
		}
		mesh_->commit_mesh();
		mesh_->r = r;
		mesh_->g = g;
		mesh_->b = b;
	}

	void recolor_mesh(Mesh *mesh_, int r, int g, int b)
	{
		if (mesh_->type == mesh_p::Mesh::TYPE::COLOR)
		{
			recolor_mesh((Mesh_c *)mesh_, r, g, b);
		}
	}
}