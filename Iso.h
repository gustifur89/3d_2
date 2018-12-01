#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <string>
#include <vector>

namespace iso
{
	#define TO_RAD 0.01745329251
	#define PI 3.14159265359
	#define Set std::vector<pointdef *>
	using namespace std;

	bool run = true;

	double deltaTime;
	double oldTime;

	double deltaMouseX;
	double deltaMouseY;
	double oldMouseX;
	double oldMouseY;

	void setDeltaTime(sf::Clock * time) {
		deltaTime = time->getElapsedTime().asSeconds() - oldTime;
		oldTime += deltaTime;
	}

	void setDeltaMouse() {
		deltaMouseX = sf::Mouse::getPosition().x - oldMouseX;
		oldMouseX = sf::Mouse::getPosition().x;

		deltaMouseY = sf::Mouse::getPosition().y - oldMouseY;
		oldMouseY = sf::Mouse::getPosition().y;
	}

	struct mat3
	{
		mat3()
		{
			//sets up Identity
			a0 = 1; a1 = 0;	a2 = 0;
			b0 = 0; b1 = 1; b2 = 0;
			c0 = 0; c1 = 0; c2 = 1;
			//array points to same data;
			arr[0] = &a0;
			arr[1] = &a1;
			arr[2] = &a2;
			arr[3] = &b0;
			arr[4] = &b1;
			arr[5] = &b2;
			arr[6] = &c0;
			arr[7] = &c1;
			arr[8] = &c2;
		}

		double * arr[9];

		double a0, a1, a2;
		double b0, b1, b2;
		double c0, c1, c2;

		//Transpose, &c.

		mat3 operator*(const mat3& other) const
		{
			mat3 out;

			out.a0 = this->a0 * other.a0 + this->a1 * other.b0 + this->a2 * other.c0;
			out.a1 = this->a0 * other.a1 + this->a1 * other.b1 + this->a2 * other.c1;
			out.a2 = this->a0 * other.a2 + this->a1 * other.b2 + this->a2 * other.c2;

			out.b0 = this->b0 * other.a0 + this->b1 * other.b0 + this->b2 * other.c0;
			out.b1 = this->b0 * other.a1 + this->b1 * other.b1 + this->b2 * other.c1;
			out.b2 = this->b0 * other.a2 + this->b1 * other.b2 + this->b2 * other.c2;

			out.c0 = this->c0 * other.a0 + this->c1 * other.b0 + this->c2 * other.c0;
			out.c1 = this->c0 * other.a1 + this->c1 * other.b1 + this->c2 * other.c1;
			out.c2 = this->c0 * other.a2 + this->c1 * other.b2 + this->c2 * other.c2;

			return out;
		}

	};

	struct mat4
	{
		mat4()
		{
			//sets up Identity
			a0 = 1; a1 = 0;	a2 = 0; a3 = 0;
			b0 = 0; b1 = 1; b2 = 0; b2 = 0;
			c0 = 0; c1 = 0; c2 = 1; c3 = 0;
			d0 = 0; d1 = 0; d2 = 0; d3 = 1;
			//array points to same data;
			arr[0] = &a0;
			arr[1] = &a1;
			arr[2] = &a2;
			arr[3] = &a3;
			arr[4] = &b0;
			arr[5] = &b1;
			arr[6] = &b2;
			arr[7] = &b3;
			arr[8] = &c0;
			arr[9] = &c1;
			arr[10] = &c2;
			arr[11] = &c3;
			arr[12] = &d0;
			arr[13] = &d1;
			arr[14] = &d2;
			arr[15] = &d3;
		}

		mat4(mat3 mat)
		{
			mat4();
			a0 = mat.a0; a1 = mat.a1; a2 = mat.a2;
			b0 = mat.b0; b1 = mat.b1; b2 = mat.b2;
			c0 = mat.c0; c1 = mat.c1; c2 = mat.c2;
		}

		double * arr[9];

		double a0, a1, a2, a3;
		double b0, b1, b2, b3;
		double c0, c1, c2, c3;
		double d0, d1, d2, d3;

		//Transpose, &c.


		//TODO: fix
		mat4 operator*(const mat4& other) const
		{
			mat4 out;

			out.a0 = this->a0 * other.a0 + this->a1 * other.b0 + this->a2 * other.c0 + this->a3 * other.d0;
			out.a1 = this->a0 * other.a1 + this->a1 * other.b1 + this->a2 * other.c1 + this->a3 * other.d1;
			out.a2 = this->a0 * other.a2 + this->a1 * other.b2 + this->a2 * other.c2 + this->a3 * other.d2;
			out.a3 = this->a0 * other.a3 + this->a1 * other.b3 + this->a2 * other.c3 + this->a3 * other.d3;

			out.b0 = this->b0 * other.a0 + this->b1 * other.b0 + this->b2 * other.c0 + this->b3 * other.d0;
			out.b1 = this->b0 * other.a1 + this->b1 * other.b1 + this->b2 * other.c1 + this->b3 * other.d1;
			out.b2 = this->b0 * other.a2 + this->b1 * other.b2 + this->b2 * other.c2 + this->b3 * other.d2;
			out.b3 = this->b0 * other.a3 + this->b1 * other.b3 + this->b2 * other.c3 + this->b3 * other.d3;

			out.c0 = this->c0 * other.a0 + this->c1 * other.b0 + this->c2 * other.c0 + this->c3 * other.d0;
			out.c1 = this->c0 * other.a1 + this->c1 * other.b1 + this->c2 * other.c1 + this->c3 * other.d1;
			out.c2 = this->c0 * other.a2 + this->c1 * other.b2 + this->c2 * other.c2 + this->c3 * other.d2;
			out.c3 = this->c0 * other.a3 + this->c1 * other.b3 + this->c2 * other.c3 + this->c3 * other.d3;

			out.d0 = this->d0 * other.a0 + this->d1 * other.b0 + this->d2 * other.c0 + this->d3 * other.d0;
			out.d1 = this->d0 * other.a1 + this->d1 * other.b1 + this->d2 * other.c1 + this->d3 * other.d1;
			out.d2 = this->d0 * other.a2 + this->d1 * other.b2 + this->d2 * other.c2 + this->d3 * other.d2;
			out.d3 = this->d0 * other.a3 + this->d1 * other.b3 + this->d2 * other.c3 + this->d3 * other.d3;

			return out;
		}

	};

	ostream& operator<< (std::ostream& stream, mat3 & matrix) {
		stream << "[" << matrix.a0 << ", " << matrix.a1 << ", " << matrix.a2 << "; "
			<< matrix.b0 << ", " << matrix.b1 << ", " << matrix.b2 << "; "
			<< matrix.c0 << ", " << matrix.c1 << ", " << matrix.c2 << "]";
		return stream;
	}

	//MATRIX FUNCTIONS
	mat3 rotation(double theta)
	{
		mat3 mat;
		mat.a0 = cos(theta);
		mat.a1 = -sin(theta);
		mat.b0 = sin(theta);
		mat.b1 = cos(theta);
		return mat;
	}

	mat3 rotationX(double theta)
	{
		mat3 mat;
		mat.b1 = cos(theta);
		mat.b2 = -sin(theta);
		mat.c1 = sin(theta);
		mat.c2 = cos(theta);
		return mat;
	}

	mat3 rotationY(double theta)
	{
		mat3 mat;
		mat.a0 = cos(theta);
		mat.a2 = sin(theta);
		mat.c0 = -sin(theta);
		mat.c2 = cos(theta);
		return mat;
	}

	mat3 rotationZ(double theta)
	{
		mat3 mat;
		mat.a0 = cos(theta);
		mat.a1 = -sin(theta);
		mat.b0 = sin(theta);
		mat.b1 = cos(theta);
		return mat;
	}

	mat3 shear(double lamdaX, double lamdaY)
	{
		mat3 mat;
		mat.a1 = lamdaX;
		mat.b0 = lamdaY;
		return mat;
	}

	mat3 scale(double scaleX, double scaleY, double scaleZ = 1.0)
	{
		mat3 mat;
		mat.a0 = scaleX;
		mat.b1 = scaleY;
		mat.c2 = scaleZ;
		return mat;
	}

	mat3 translate(double tX, double tY, double tZ = 0.0)
	{
		mat3 mat;
		mat.a2 = tX;
		mat.b2 = tY;
		mat.c2 = tZ;
		return mat;
	}

	mat3 flatProjection()
	{
		mat3 mat;
		mat.c2 = 0;
		return mat;
	}

	//GEOMETRY
	struct pointdef
	{
		pointdef(double x_, double y_, double z_ = 0.0)
		{
			x = x_;
			y = y_;
			z = z_;
		}
		pointdef()
		{
			pointdef(0.0, 0.0, 0.0);
		}

		void translate(double tX, double tY, double tZ)
		{
			x += tX;
			y += tY;
			z += tZ;
		}

		double x, y, z;
		double projX, projY, projZ;
	};

	struct linedef
	{
		linedef(pointdef * A_, pointdef * B_, sf::Color color_ = sf::Color(0, 255, 0, 255))
		{
			A = A_;
			B = B_;
			color = color_;
		}
		pointdef * A;
		pointdef * B;
		sf::Color color;
	};

	void applyMatrix(mat3 matrix, pointdef * point)
	{
		point->projX = matrix.a0 * point->x + matrix.a1 * point->y + matrix.a2 * point->z;
		point->projY = matrix.b0 * point->x + matrix.b1 * point->y + matrix.b2 * point->z;
		point->projZ = matrix.c0 * point->x + matrix.c1 * point->y + matrix.c2 * point->z;
	}

	void applyMatrix(mat3 matrix, linedef * line)
	{
		applyMatrix(matrix, line->A);
		applyMatrix(matrix, line->B);
	}

	void applyMatrixTranslation(mat3 matrix, pointdef translate, pointdef * point)
	{
		double x = point->x;// +translate.x;
		double y = point->y;// +translate.y;
		double z = point->z;// +translate.z;
		point->projX = matrix.a0 * x + matrix.a1 * y + matrix.a2 * z + translate.x;
		point->projY = matrix.b0 * x + matrix.b1 * y + matrix.b2 * z + translate.y;
	}

	void applyMatrixTranslation(mat3 matrix, pointdef translate, linedef *line)
	{
		applyMatrixTranslation(matrix, translate, line->A);
		applyMatrixTranslation(matrix, translate, line->B);
	}

	//COLLECTIONS
	struct mapdef
	{
		mapdef()
		{

		}

		void addLine(linedef * line)
		{
			lines.push_back(line);
		}

		mapdef operator+(const mapdef& other) const
		{
			mapdef out;

			out.lines = this->lines;
			for (int i = 0; i < other.lines.size(); i++)
			{
				out.lines.push_back(other.lines[i]);
			}
			return out;
		}

		vector<linedef *> lines;
		Set set;
	};

	void applyMatrix(mat3 matrix, mapdef * map)
	{
		for (int i = 0; i < map->lines.size(); i++)
		{
			applyMatrix(matrix, map->lines[i]);
		}
	}

	void applyMatrixTranslation(mat3 matrix, pointdef translate, mapdef * map)
	{
		for (int i = 0; i < map->lines.size(); i++)
		{
			applyMatrixTranslation(matrix, translate, map->lines[i]);
		}
	}

	//VISUAL FUNCTIONS
	void draw(sf::RenderWindow * window, linedef * line)
	{
		sf::VertexArray lines(sf::LinesStrip, 2);
		lines[0].position = sf::Vector2f(line->A->projX, line->A->projY);
		lines[0].color = line->color;
		lines[1].position = sf::Vector2f(line->B->projX, line->B->projY);
		lines[1].color = line->color;
		window->draw(lines);
	}

	void draw(sf::RenderWindow * window, mapdef * map)
	{
		for (int i = 0; i < map->lines.size(); i++)
		{
			draw(window, map->lines[i]);
		}
	}

	//AUXILLARY FUNCTIONS
	double x = 0.0;
	double y = 0.0;
	double theta = 30.0;
	double phi = 45.0;
	double scaleF = 1.0;

	sf::RenderWindow * setup_window()
	{
		int width = 500;// sf::VideoMode::getDesktopMode().width;
		int height = 500;// sf::VideoMode::getDesktopMode().height;
		x = width / 2;
		y = height / 2;
		sf::RenderWindow * renderWindow = new sf::RenderWindow(sf::VideoMode(width, height), "ISO test", sf::Style::Default /*| sf::Style::Fullscreen*/);
		//sf::RenderWindow * renderWindow = new sf::RenderWindow(sf::VideoMode(500, 500), "ISO test");
		return renderWindow;
	}

	void render(sf::RenderWindow * renderWindow, mapdef * map)
	{
		sf::Color clearColor(0, 40, 12, 255);
		renderWindow->clear(clearColor);
		draw(renderWindow, map);
		renderWindow->display();
	}

	void processEvents(sf::RenderWindow * window)
	{
		sf::Event event;
		while (window->pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window->close();
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
			{
				window->close();
			}
			if (event.type == sf::Event::MouseWheelScrolled)
				scaleF *= event.mouseWheelScroll.delta<0 ? 0.9 : 1.1;
		}
	}

	void function(pointdef * pt)
	{
		pt->z = pt->x + pt->y;
	}

	void function(mapdef * map)
	{
		for (int i = 0; i < map->set.size(); i++)
		{
			function(map->set[i]);
		}
	}

	void act(mapdef * map)
	{
		/*
		NOTES:
		SCALE(p):
		X(p) = 1 / cos(p)
		Y(p) = cos(p)
		ROTATE:
		R(p) = regular
		*/

		mat3 projection;

		//T - translate
		//R - rotationn
		//S - scale

		/*

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		x += 100.0 * deltaTime;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		x -= 100.0 * deltaTime;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		y += 100.0 * deltaTime;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		y -= 100.0 * deltaTime;


		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		phi -= 100.0 * deltaTime;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		phi += 100.0 * deltaTime;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		theta -= 100.0 * deltaTime;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		theta += 100.0 * deltaTime;
		*/

		if (sf::Mouse::isButtonPressed(sf::Mouse::Middle))
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
			{
				y += deltaMouseY;
				x += deltaMouseX;
			}
			else
			{
				phi += deltaMouseY;
				theta -= deltaMouseX;
			}
		}

		double theta_use = round(theta / 5.0)*5.0 * TO_RAD;
		double phi_use = round(phi / 5.0)*5.0 * TO_RAD;

		mat3 rMat = rotation(theta_use);

		mat3 rpMat = rotation(-PI / 4.0);

		double factor = cos(phi_use);

		mat3 sMat = shear(0.5, 0.5);

		mat3 scMat = scale(scaleF, scaleF);

		mat3 tMat = translate(x, y);

		mat3 proj;


		//proj = proj * tMat;

		//proj = proj * rpMat;

		//proj = proj * sMat;

		//proj = proj * rMat;

		//function(map);

		mat3 rXMat = rotationX(phi_use);
		mat3 rZMat = rotationZ(theta_use);

		proj = ((proj * scMat) * rXMat) * rZMat;

		pointdef trans(x, y, 0.0);

		//applyMatrix(proj, map);

		applyMatrixTranslation(proj, trans, map);


		//applyMatrix(projection, map);
	}

	mapdef * makeGrid_defunct(int xNum, int yNum, double scale, double tX, double tY, sf::Color color = sf::Color(0, 255, 0, 255))
	{
		mapdef * map = new mapdef();
		xNum /= 2;
		yNum /= 2;

		vector<vector<pointdef *>> set;


		for (int i = -yNum; i <= yNum; i++)
		{
			//color.b += 20;//7
			pointdef * A = new pointdef(-xNum*scale + tX, i*scale + tY);
			pointdef * B = new pointdef(xNum*scale + tX, i*scale + tY);
			map->addLine(new linedef(A, B, color));
		}
		for (int i = -xNum; i <= xNum; i++)
		{
			//color.g += 25; //5
			pointdef * A = new pointdef(i*scale + tX, -yNum*scale + tY);
			pointdef * B = new pointdef(i*scale + tX, yNum*scale + tY);
			map->addLine(new linedef(A, B, color));
		}
		return map;
	}

	double distFunc(double x_, double y_, double scale)
	{
		double x = x_ / scale;
		double y = y_ / scale;
		return 100.0 / exp((x*x + y*y) / 30.0);
	}

	mapdef * makeGrid(int xNum, int yNum, double scale, double tX, double tY, sf::Color color = sf::Color(0, 255, 0, 255))
	{
		mapdef * map = new mapdef();
		xNum /= 2;
		yNum /= 2;
		pointdef * null = 0x00000000;

		//set up grid
		vector<vector<pointdef *>> set;
		for (int i = 0; i <= xNum; i++)
		{
			vector<pointdef *> n;
			set.push_back(n);
			for (int j = 0; j <= xNum; j++)
			{
				set[i].push_back(null);
			}
		}

		//add points
		for (int i = 0; i <= xNum; i++)
		{
			for (int j = 0; j <= yNum; j++)
			{
				//Add new point.
				double x = (i - xNum / 2) * scale + tX;
				double y = (j - yNum / 2) * scale + tY;
				set[i][j] = new pointdef(x, y, distFunc(x, y, scale));
				map->set.push_back(set[i][j]);
			}
		}

		//make lines
		//map->addLine(new linedef(A, B, color));

		for (int i = 0; i <= xNum; i++)
		{
			for (int j = 0; j <= yNum; j++)
			{
				pointdef * A = set[i][j];
				if (i < xNum)
				{
					pointdef * B = set[i + 1][j];
					map->addLine(new linedef(A, B, color));
				}
				if (j < yNum)
				{
					pointdef * C = set[i][j + 1];
					map->addLine(new linedef(A, C, color));
				}
			}
		}


		return map;
	}

	mapdef * makeBox(int xNum, int yNum, double scale, double tX, double tY)
	{
		mapdef * map = new mapdef();
		xNum /= 2;
		yNum /= 2;
		for (int z = 0; z < scale* 40.0; z += scale*4.0)
		{
			for (int i = -yNum; i <= yNum; i++)
			{
				pointdef * A = new pointdef(-xNum*scale + tX, i*scale + tY, z);
				pointdef * B = new pointdef(xNum*scale + tX, i*scale + tY, z);
				map->addLine(new linedef(A, B));
			}
			for (int i = -xNum; i <= xNum; i++)
			{
				pointdef * A = new pointdef(i*scale + tX, -yNum*scale + tY, z);
				pointdef * B = new pointdef(i*scale + tX, yNum*scale + tY, z);
				map->addLine(new linedef(A, B));
			}
		}


		return map;
	}
}