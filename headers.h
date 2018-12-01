#pragma once
#include <SFML/Graphics.hpp>
//#include <SFML/OpenGL.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>
#include <ctime>
#include <Windows.h>
#include "states.h"
#include <GL\glew.h>
//#include <gl\glew.h>
//#include <GL\glew.h>
#include <random>
#include <gl\GL.h>
#include <gl\GLU.h>
#include <thread>
#include <fstream>
#include "items.h"

#define PRIM_CUBE	0
#define PRIM_CYL	1
#define PRIM_RAMP	2
#define PRIM_SPHERE	4
#define PRIM_CONE	8
#define PRIM_DOME	16
#define TO_RAD 0.01745329251
#define TO_DEG 57.2957795131

float delta_time = 0.f;
bool pause = false;
bool pause_toggle = false;
bool line_mode_toggle = false;
bool run_game = true;

struct position
{
	position()
	{
		x = 0;
		y = 0;
		z = 0;
	}
	float x, y, z;
};

struct camera_pos
{
	camera_pos()
	{

	}
	float x, y, z = 0.f;
	float radius = 0.f;
	float azimuth, pitch, roll = 0.f;
};

float signum(float a)
{
	if (a >= 0)
		return 1.0;
	return -1.0;
}

double random()
{
	return (rand() % 10000) / 10000.0;
}

double dmod(double a, double b)
{
	return a - b * floor(a / b);
}

sf::Color HSV_RGB(double hue, double sat, double val)
{
	hue = dmod(hue, 360.0);
	double h = hue / 60.0;
	double c = sat * val;
	double x = c * (1.0 - fabs(dmod(h,2.0) - 1.0));

	double r = 0.0;
	double g = 0.0;
	double b = 0.0;
	
	if (h >= 0 && h < 1)
	{
		r = c;
		g = x;
		b = 0.0;
	}
	else if (h < 2)
	{
		r = x;
		g = c;
		b = 0.0;
	}
	else if (h < 3)
	{
		r = 0.0;
		g = c;
		b = x;
	}
	else if (h < 4)
	{
		r = 0.0;
		g = x;
		b = c;
	}
	else if (h < 5)
	{
		r = x;
		g = 0.0;
		b = c;
	}
	else if (h < 6)
	{
		r = c;
		g = 0.0;
		b = x;
	}
	else
	{
		r = 0.0;
		g = 0.0;
		b = 0.0;
	}

	double m = val - c;

	return sf::Color((r + m)*255.0, (g + m)*255.0, (b + m)*255.0, 255);

	/*
	int h = (int)hue / 60;
	double f = hue / 60.0 - h;
	double p = val*(1.0 - sat);
	double q = val*(1.0 - sat*f);
	double t = val*(1.0 - sat*(1.0 - f));

	val *= 255.0;
	t *= 255.0;
	p *= 255.0;
	q *= 255.0;


	switch (h)
	{
	case 0:
	case 6: return sf::Color(val, t, p);
	case 1: return sf::Color(q, val, p);
	case 2: return sf::Color(p, val, p);
	case 3: return sf::Color(p, q, val);
	case 4: return sf::Color(t, p, val);
	case 5:  return sf::Color(val, p, q);
	default: return sf::Color(0, 0, 0);
	}
	*/
}

std::vector<mesh::Mesh_c*> remove_mesh(std::vector<mesh::Mesh_c*> & meshes, mesh::Mesh_c * remove)
{
	std::vector<mesh::Mesh_c*> out;
	for (int i = 0; i < meshes.size(); i++)
	{
		if (meshes[i] != remove)
			out.push_back(meshes[i]);
	}
	return out;
}