#pragma once
#include "headers.h"
namespace geo
{
	struct pointdef
	{
		pointdef() {};
		pointdef(float x_, float y_, float z_)
		{
			x = x_;
			y = y_;
			z = z_;
		}
		bool operator==(const pointdef& other) const
		{
			return ((this->x == other.x) && (this->y == other.y) && (this->z == other.z));
		}
		bool operator!=(const pointdef& other) const
		{
			return !((this->x == other.x) && (this->y == other.y) && (this->z == other.z));
		}
		pointdef operator*(const float &other) const
		{
			return pointdef(other*this->x, other*this->y, other*this->z);
		}
		float x, y, z;
	};

	struct linedef
	{
		linedef() {};
		linedef(pointdef a_, pointdef b_)
		{
			a = a_;
			b = b_;
		}
		pointdef a, b;
	};

	struct vertex
	{
		vertex() {};
		vertex(float x_, float y_, float z_, float uvx_, float uvy_)
		{
			pt = pointdef(x_, y_, z_);
			uvx = uvx_;
			uvy = uvy_;
		}
		vertex(pointdef pt_, float uvx_, float uvy_)
		{
			pt = pt_;
			uvx = uvx_;
			uvy = uvy_;
		}
		pointdef pt;
		float uvx, uvy;
	};
	
	struct planedef
	{
		planedef() {};
		planedef(pointdef a_, pointdef b_, pointdef c_) 
		{
			a = ((b_.y - a_.y)*(c_.z - a_.z)) - ((b_.z - a_.z)*(c_.y - a_.y));
			b = ((b_.z - a_.z)*(c_.x - a_.x)) - ((b_.x - a_.x)*(c_.z - a_.z));
			c = ((b_.x - a_.x)*(c_.y - a_.y)) - ((b_.y - a_.y)*(c_.x - a_.x));
			float n = sqrtf(a*a + b*b + c*c);
			a = a / n;
			b = b / n;
			c = c / n;
			d = -(a * a_.x + b * a_.y + c * a_.z);
		};

		float a, b, c, d;
	};
}