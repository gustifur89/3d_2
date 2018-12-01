#pragma once
#include "headers.h"
#include "mesh.h"



mesh::Mesh_c prim_cube;
mesh::Mesh_c prim_cyl;
mesh::Mesh_c prim_ramp;
mesh::Mesh_c prim_sphere;
mesh::Mesh_c prim_cone;
mesh::Mesh_c prim_dome;

//namespace
namespace hold
{

	struct weapon
	{
		//add some stuff like type, &c.
		weapon() {}

		float theta_goal;
		float phi_goal;
		float theta_vel;
		float phi_vel;
		float max_theta_vel = 30;
		float max_phi_vel;
		double max_delta_relative_theta = 2;
		double max_delta_relative_phi = 2;
		double prev_theta = 0;
		double prev_phi = 0;

	};

}