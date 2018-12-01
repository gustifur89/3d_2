#pragma once
#include "headers.h"
#include "mesh.h"
#include "reader.h"



mesh::Mesh_c prim_cube_c;



mesh::Mesh_c *editing;

void load_primitives()
{
	reader::read_ply("src/prim_cube.ply", prim_cube);
	reader::read_ply("src/prim_cyl.ply", prim_cyl);
	reader::read_ply("src/prim_ramp.ply", prim_ramp);
	reader::read_ply("src/prim_sphere.ply", prim_sphere);
	reader::read_ply("src/prim_dome.ply", prim_dome);
	reader::read_ply("src/prim_cone.ply", prim_cone);
}

/*
void spawn_primitive(std::vector<mesh::Mesh_c *> &meshes, int type, float x_, float y_, float z_, float x_size, float y_size, float z_size, float azimuth_, float pitch_, float roll_, int text_num)
{
	mesh::Mesh_c * new_m = new mesh::Mesh_c();
	if(type == PRIM_CUBE)
		*new_m = prim_cube;
	else if (type == PRIM_CYL)
		*new_m = prim_cyl;
	else if (type == PRIM_RAMP)
		*new_m = prim_ramp;
	else if (type == PRIM_SPHERE)
		*new_m = prim_sphere;
	else if (type == PRIM_CONE)
		*new_m = prim_cone;
	else if (type == PRIM_DOME)
		*new_m = prim_dome;
	new_m->type = type;
	new_m->x = x_;
	new_m->y = y_;
	new_m->z = z_;
	new_m->roll = roll_;
	new_m->pitch = pitch_;
	new_m->azimuth = azimuth_;
	new_m->re_color = true;
	new_m->r = new_m->faces[0].vertex[0].r;
	new_m->g = new_m->faces[0].vertex[0].g;
	new_m->b = new_m->faces[0].vertex[0].b;
	mesh::resize_mesh(new_m, x_size, y_size, z_size);
	meshes.push_back(new_m);
	editing = new_m;
}*/

void spawn_primitive(std::vector<mesh::Mesh_c * > &meshes, int type, float x_, float y_, float z_, float x_size, float y_size, float z_size, float azimuth_, float pitch_, float roll_, int text_num)
{
	mesh::Mesh_c * new_m = new mesh::Mesh_c();
	if (type == PRIM_CUBE)
		*new_m = prim_cube;
	else if (type == PRIM_CYL)
		*new_m = prim_cyl;
	else if (type == PRIM_RAMP)
		*new_m = prim_ramp;
	else if (type == PRIM_SPHERE)
		*new_m = prim_sphere;
	else if (type == PRIM_CONE)
		*new_m = prim_cone;
	else if (type == PRIM_DOME)
		*new_m = prim_dome;
	new_m->type = type;
	new_m->x = x_;
	new_m->y = y_;
	new_m->z = z_;
	new_m->roll = roll_;
	new_m->pitch = pitch_;
	new_m->azimuth = azimuth_;
	new_m->re_color = true;
	new_m->r = 0;// new_m->faces[0]->vertex[0].r;
	new_m->g = 0;// new_m->faces[0]->vertex[0].g;
	new_m->b = 0;// new_m->faces[0]->vertex[0].b;
	mesh::resize_mesh(new_m, x_size, y_size, z_size);
	meshes.push_back(new_m);
	editing = new_m;
}

