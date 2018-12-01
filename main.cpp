#include "headers.h"
#include "mesh.h"
#include "render.h"
#include "bsp.h"
#include "entity.h"
#include "primitives.h"
#include "editing.h"
#include "settings.h"
#include "level_builder.h"
#include "segment.h"
#include "dynamic_object.h"
#define _CRT_SECURE_NO_DEPRECATE
#ifndef GL_SRGB8_ALPHA8
#define GL_SRGB8_ALPHA8 0x8C43
#endif

/*
{
if (GetAsyncKeyState(68) == GetAsyncKeyState(65))
{
x_speed = 0.f;
}
else if (GetAsyncKeyState(68))
{
x_speed = speed;
}
else if (GetAsyncKeyState(65))
{
x_speed = -speed;
}

if (GetAsyncKeyState(83) == GetAsyncKeyState(87))
{
z_speed = 0.f;
}
else if (GetAsyncKeyState(83))
{
z_speed = -speed;
}
else if (GetAsyncKeyState(87))
{
z_speed = speed;
}
}



//	std::cout << z_speed << "\n";



if (!GetAsyncKeyState(VK_SPACE))
{
if (!pause)
{
window.setMouseCursorVisible(false);

m_x = sf::Mouse::getPosition(window).x * 200.f / window.getSize().x - 100.f;
m_y = -sf::Mouse::getPosition(window).y * 200.f / window.getSize().y + 100.f;
t += m_x;
p += m_y;
if (p > 90)
p = 90;
if (p < -90)
p = -90;
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
window.setMouseCursorVisible(true);
}


//p += p_speed * delta_time;
//t += t_speed * delta_time;



if (GetAsyncKeyState(VK_NUMPAD9))
{
	mini_map_scan_size = 10;
	pixels = new uint8_t[mini_map_scan_size *mini_map_scan_size * 4];
}

if (GetAsyncKeyState(VK_NUMPAD8))
{
	mini_map_scan_size = 20;
	pixels = new uint8_t[mini_map_scan_size *mini_map_scan_size * 4];
}





if (MOVE_MODE == 1)
{
	x += (z_speed * sin(t * PI / 180.0) * cos(p * PI / 180.0)) * delta_time;
	y += (z_speed * sin(p * PI / 180.0)) * delta_time;
	z += (-z_speed * cos(t * PI / 180.0) * cos(p * PI / 180.0)) * delta_time;
}
else if (MOVE_MODE == 0)
{
	float x_spd = (x_speed * cos(t * PI / 180.0) + z_speed * sin(t * PI / 180.0)) * delta_time;
	float z_spd = (x_speed * sin(t * PI / 180.0) - z_speed * cos(t * PI / 180.0)) * delta_time;
	y = 0;
	collision(map, x_spd, z_spd, p_width);

	//	x += (x_speed * cos(t * PI / 180.0) + z_speed * sin(t * PI / 180.0)) * delta_time;
	//	y = 0;
	//	z += (x_speed * sin(t * PI / 180.0) - z_speed * cos(t * PI / 180.0)) * delta_time;
}

*/




mesh_p::Mesh_c mer;
mesh::Mesh_g obj;
mesh::Mesh_g sab;

mesh::Mesh_c moon;
mesh::Mesh_c man;

camera_pos cam;

/*
// Request a 24-bits depth buffer when creating the window
sf::ContextSettings contextSettings;
contextSettings.depthBits = 24;

// Create the main window
//sf::Window mini_map(sf::VideoMode(400, 400), "mini_map", sf::Style::None);
sf::Window window(sf::VideoMode(890,500), "3d_0", sf::Style::Default, contextSettings);

// Make it the active window for OpenGL calls
window.setActive();

//sf::Window mini_map(sf::VideoMode(200, 200), "map", sf::Style::Default, contextSettings);

// Make it the active window for OpenGL calls
//mini_map.setActive();


// Set the color and depth clear values
glClearDepth(1.f);



//lighting
//glEnable(GL_LIGHTING);

sf::Texture map_border;
map_border.loadFromFile("src/map_border.png");


sf::Texture hud;
hud.create(mini_map_scan_size, mini_map_scan_size);
//std::string image2 = "src/spook.png";
//hud.loadFromFile(image2);



glEnable(GL_TEXTURE_2D);//tell OpenGL to use textures when drawing

sf::Texture::bind(&hud);


glClearColor(sky_color_r, sky_color_g, sky_color_b, 1.f);

// Enable Z-buffer read and write
glEnable(GL_DEPTH_TEST);
glDepthMask(GL_TRUE);

// Disable lighting and texturing
//glDisable(GL_LIGHTING);

glEnable(GL_CULL_FACE);

glDisable(GL_TEXTURE_2D);

// Configure the viewport (the same size as the window)


// Setup a perspective projection
glMatrixMode(GL_PROJECTION);
glLoadIdentity();
GLfloat ratio = static_cast<float>(window.getSize().x) / window.getSize().y;
glFrustum(-ratio, ratio, -1.f, 1.f, 1.f, view_distance);


//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); */

void set_delta_time(sf::Clock &clock)
{
	sf::Time time = clock.getElapsedTime();
	delta_time = time.asSeconds();
	clock.restart();
}

float x, y, z = 0.f;
float x_speed, y_speed, z_speed = 0.f;
float ta, pa, qa = 0.f;
float speed = 5.f;
float t = 0.0;
float p = 0.0;

int m_x, m_y = 0;


//toggles
bool spawn_toggle = 0;

bool mode_toggle = 0;
bool no_clip_toggle = 0;



//Save files
/*
void save_file(const char* file_name, std::vector<mesh::Mesh_g> meshes)
{
	std::ofstream file;
	std::string file_name_str;
	std::cout << "File name:\n";
	std::cin >> file_name_str;
	file.open(file_name_str);
	file << file_name_str << "\n";
	//get all unique vertexes
	std::vector<mesh::Vertex> vertexes;
	for (int i = 0; i < map.size(); i++)
	{
		if (find(points, map[i].a, 0) == -1)
		{
			points.push_back(map[i].a);
		}
		if (find(points, map[i].b, 0) == -1)
		{
			points.push_back(map[i].b);
		}
		if (find(colors, map[i].color, 0) == -1)
		{
			colors.push_back(map[i].color);
		}
	}
	//yes I could do it in one loop, but eh. Gives me something to optimize later
	//adds points to the file
	for (int i = 0; i < points.size(); i++)
	{
		file << "pt " << points[i].x << " " << points[i].y << "\n";
	}
	//adds colors to the file
	for (int i = 0; i < colors.size(); i++)
	{
		file << "c " << std::hex << colors[i].toInteger() << "\n";
	}
	//goes through map and adds the line points and color to the file
	for (int i = 0; i < map.size(); i++)
	{
		file << "ld " << std::dec << find(points, map[i].a, 0) + 1 << " " << find(points, map[i].b, 0) + 1 << " " << find(colors, map[i].color, 0) + 1 << "\n";
	}
	file.close();
	if (choose_map)
		std::cout << "saved " << file_name_str << "\n";
	else
		std::cout << "saved " << file_name << "\n";
}



/*
void save_file(const char* file_name, std::vector<linedef> map, bool ask_name)
{
std::ofstream file;
std::string file_name_str;
if (choose_map)
{
std::cout << "File name:\n";
std::cin >> file_name_str;
file.open(file_name_str);
file << file_name_str << "\n";
}
else
{
file.open(file_name);
file << file_name << "\n";
}
//get all unique vertexes
std::vector<pointdef> points;
std::vector<sf::Color> colors;
for (int i = 0; i < map.size(); i++)
{
if (find(points, map[i].a, 0) == -1)
{
points.push_back(map[i].a);
}
if (find(points, map[i].b, 0) == -1)
{
points.push_back(map[i].b);
}
if (find(colors, map[i].color, 0) == -1)
{
colors.push_back(map[i].color);
}
}
//yes I could do it in one loop, but eh. Gives me something to optimize later
//adds points to the file
for (int i = 0; i < points.size(); i++)
{
file << "pt " << points[i].x << " " << points[i].y << "\n";
}
//adds colors to the file
for (int i = 0; i < colors.size(); i++)
{
file << "c " <<std::hex<< colors[i].toInteger() << "\n";
}
//goes through map and adds the line points and color to the file
for (int i = 0; i < map.size(); i++)
{
file << "ld " <<std::dec<< find(points, map[i].a, 0)+1 << " " << find(points, map[i].b, 0)+1 << " " << find(colors, map[i].color, 0)+1 << "\n";
}
file.close();
if (choose_map)
std::cout << "saved " << file_name_str << "\n";
else
std::cout << "saved "<< file_name << "\n";
}



bool read_file(const char* file_name, std::vector<linedef> *map)
{
std::vector<pointdef> temp_points;
std::vector<sf::Color> temp_colors;
std::ifstream file;
if (choose_map)
{
std::string file_name_str;
std::cout << "File name:\n";
std::cin >> file_name_str;
file = std::ifstream(file_name_str);
}
else
{
file = std::ifstream(file_name);
}
std::string type;

//while (file >> type)
//	std::cout << type << "\n";

while (file >> type)
{
if (type == "pt")
{
float x, y;
file >> x >> y;
temp_points.push_back(pointdef(x,y));
}
else if (type == "c")
{
int color;
//file >> color;
//temp_colors.push_back(sf::Color(color));
}
else if (type == "ld")
{
unsigned int ptIndex[2], colorIndex;

file >> ptIndex[0] >> ptIndex[1] >> colorIndex;
linedef temp(temp_points[ptIndex[0] - 1], temp_points[ptIndex[1] - 1], sf::Color::Red);
//	temp.a = temp_points[ptIndex[0] - 1];
//	temp.b = temp_points[ptIndex[1] - 1];
//	temp.color = sf::Color::Red;
map->push_back(temp);
}
}
file.close();
}*/



int main()
{
	if (settings::random_enabled)
	{
		int seed = int(time(NULL));
		srand(seed);
		std::cout << "seed: " << seed << "\n";
	}
	else
	{
		srand(settings::random_seed);
	}

	//LOAD TEXTURES
	/*
	sf::Texture texture_0;
	texture_0.loadFromFile("src/hat.png");
	mesh::textures.push_back(texture_0);

	sf::Texture texture_1;
	texture_1.loadFromFile("src/marine.png");
	mesh::textures.push_back(texture_1);

	sf::Texture texture_2;
	texture_2.loadFromFile("src/test.png");
	mesh::textures.push_back(texture_2);

	sf::Texture texture_3;
	texture_3.loadFromFile("src/sabre.png");
	mesh::textures.push_back(texture_3);
	
	//LOAD MESHES
	//reader::read_ply("src/plane.ply", mer);


	mesh::read_obj("src/marine.obj", obj, 0, true, R_TRIANGLES,1,1.0);
	mesh::read_obj("src/sabre.obj", sab, 0, true, R_TRIANGLES, 3, 1.0);
	*/
	//reader::read_ply("src/crab.ply", moon);
	//reader::read_ply("prius.ply", moon);
	//reader::read_ply("src/chub.ply", man);
	//////
	//stuff to add at the begininng
	//std::cout << "here\n";
	
	load_primitives();
	//std::cout << "here1/2\n";
	obj.y = 0;

	sab.x = -5;
	sab.y = 7;
	sab.z = 10;

	/**/

	std::vector<mesh::Mesh_g *> world_meshes;

	
	std::vector<mesh_p::Mesh *> world_meshes_p;
	



	std::vector<mesh::Mesh_c *> world_meshes_c;
	std::vector<mesh::Mesh_c *> hud_meshes_c;
	std::vector<mesh::Mesh_c *> object_list;
	std::vector<mesh::Mesh_c *> wall_object_list;
	std::vector<mesh::Mesh_c *> roof_object_list;
	std::vector<Object *> objects;
	std::vector<seg::node *> spawn_nodes;

	
	
	//	mesh::Mesh_c sofa;
//	reader::read_ply("src/meshes/sofa.ply", sofa);
//	world_meshes_c.push_back(&sofa);
//	world_meshes_c.push_back(&mer);
//	mesh::resize_mesh(&gun, 0.25, 0.25, 0.25);
	//world_meshes_c.push_back(&moon);
	//world_meshes_c.push_back(&man);
//	world_meshes.push_back(&mer);
//	world_meshes.push_back(&obj);
//	world_meshes.push_back(&sab);
	//edit::cmd("load_edit city.mp", world_meshes_c);
//	std::cout << "here1\n";


		
	//LOAD STATICS FROM FILE
	
	//builder::build_level("src/level_0_cmd.txt", world_meshes_c);

	builder::build_level("src/level_test_cmd.txt", world_meshes_c);
	std::vector<seg::segment> segments;
	
	

	builder::load_segments("src/better_segs", segments);
	
	mesh::Mesh_c * bsp_mesh = new mesh::Mesh_c();

	builder::load_objects("src/objects", object_list);
	builder::load_objects("src/wall_objects", wall_object_list);
	builder::load_objects("src/roof_objects", roof_object_list);
	

	reader::read_ply("src/meshes/bullet_pop.ply", bullets::pop);

	
	//world_meshes_c.push_back(&area);

	//mesh::translate_mesh(&area, 0, -10, 0);

	


	/*
	Key key;

	Door dor(0, 0, 0, 10, 10, 10, 2.0, &key, "src/meshes/Door.ply");

	world_meshes_c.push_back(dor.mesh);
	*/
	//make_map_adv2
	seg::node * node_map = seg::make_map_adv_3(segments, object_list, wall_object_list, roof_object_list, bsp_mesh, world_meshes_c, spawn_nodes, objects);
	//world_meshes_c.resize(1);
	//std::cout << big_mesh.faces.size() <<"  yeet"<< "\n";
//	world_meshes_c.push_back(&big_mesh);
	//big_mesh.s_type = mesh::Mesh_c::STATIC;
	//	for (int i = 0; i < big_mesh.vertexes.size(); i++)
//	{
		//std::cout << " " << big_mesh.vertexes[i] << "\n";
//	}


	//MINI MAP
//	seg::displayNodeMap(node_map);
	
	mesh::Mesh_c area;
	reader::read_ply("src/meshes/MOON BASE disp.ply", area);

	mesh::Mesh_c areaDisplay;
	reader::read_ply("src/meshes/MOON BASE disp.ply", areaDisplay);
	world_meshes_c.push_back(&areaDisplay);

	//seg::make_map(segments, big_mesh);

	mesh::Mesh_c * thisMesh = mesh::combine_meshes(bsp_mesh, &area);

	bsp_mesh = &area;//thisMesh;

	//BUILD BSP-tree
	bsp_1::node bsp_tree_1;
	bsp_1::node bsp_tree_horz;
	bsp_1::node bsp_tree_vert;
//	std::cout << "here2\n";
	
//	mesh::Mesh_c sofa;
//	reader::read_ply("src/meshes/sofa.ply", sofa);
//	world_meshes_c.push_back(&sofa);
	

	bsp_tree_1.current = NULL;// mer.faces[0];start_time_delay
	std::vector<mesh::face_c*> pMap;
//	bsp_1::generate_pointer_map(world_meshes_c,pMap);
	bsp_1::generate_pointer_map(bsp_mesh, pMap);
	std::cout << "MESH Triangle: " << pMap.size() << "\n";
	//	bsp_1::remove_duplicates(pMap);
	std::vector<mesh::face_c*> pMap_horz;
	std::vector<mesh::face_c*> pMap_vert;
	bsp_1::cull_faces(pMap, pMap_horz, bsp_1::TYPE::HORIZONTAL);
	bsp_1::cull_faces(pMap, pMap_vert, bsp_1::TYPE::VERTICAL);
	bsp_tree_horz.construct(&pMap_horz);
	bsp_tree_vert.construct(&pMap_vert);
	std::cout << "BSP Triangle: " << bsp_1::count << "\n";
	bsp_tree_1.construct(&pMap);
	object_values::bsp_tree = &bsp_tree_horz;
	

	//std::cout << "traignes before: " << mer.faces.size() << "\n";
	//std::cout << "triangles after: " << bsp_1::count << "\n";
	
	
	//ADD DYNAMICS
	/*
	mesh::Mesh_c gun_mesh;
	reader::read_ply("src/meshes/shotgun.ply", gun_mesh);
	mesh::resize_mesh(&gun_mesh, settings::gun_scale, settings::gun_scale, settings::gun_scale);
	gun_mesh.azimuth = 0;
	gun_mesh.pitch = 0;
	gun_mesh.roll = 0;
	hold::weapon gun;
	gun.mesh = &gun_mesh;
	world_meshes_c.push_back(&gun_mesh);
*/

	//big_mesh.commit_mesh();
	//std::cout << "ower " << big_mesh.vertexes.size() << "\n";


//	world_meshes_c.push_back(&big_mesh);

//	std::cout << "here3\n";
	man.x = 5;
	man.y = 0;
	man.z = 0;

	moon.z = 0;

	//try the bsp to see if it works.
	/*
	bsp::tree_faces = mer.faces;

	std::vector<int> indexes;
	for (int i = 0; i < mer.faces.size(); i++)
	{
		indexes.push_back(i);
	}

	bsp::node bsp_tree(0);
	bsp_tree.construct(indexes);
	*/
	sf::Clock clock_g;
	clock_g.restart();

	

	//std::cout << sizeof(mesh::Mesh_c) << "\n";

	bool sRgb = false;
	// Request a 24-bits depth buffer when creating the window
	sf::ContextSettings contextSettings;
	contextSettings.depthBits = 24;
	contextSettings.sRgbCapable = sRgb;
	if (settings::fullscreen)
	{
		settings::window_width = sf::VideoMode::getDesktopMode().width;
		settings::window_height = sf::VideoMode::getDesktopMode().height;
		settings::view_port_width = sf::VideoMode::getDesktopMode().width;
		settings::view_port_height = sf::VideoMode::getDesktopMode().height;
	}
	// Create the main window
	sf::RenderWindow window(sf::VideoMode(settings::window_width, settings::window_height), "-_-", sf::Style::Close | sf::Style::Titlebar | (settings::fullscreen?sf::Style::Fullscreen:0), contextSettings);
//	sf::RenderWindow window(sf::VideoMode(800, 800), "-_-", sf::Style::Fullscreen, contextSettings);

	window.setVerticalSyncEnabled(true);
	sf::ContextSettings settings = window.getSettings();
	std::cout << "OpenGL version:" << settings.majorVersion << "." << settings.minorVersion << "\n";
	window.setActive(false);

	render::set_font();
	//----------------------------

	// Create a sprite for the background
//	sf::Texture backgroundTexture;
//	backgroundTexture.setSrgb(sRgb);
	//if (!backgroundTexture.loadFromFile("src\back.png"))
	///	return EXIT_	FAILURE;
	
	// Create some text to draw on top of our OpenGL object
	//sf::Font font;
	//if (!font.loadFromFile("resources/sansation.ttf"))
	//	return EXIT_FAILURE;
	//sf::Text text("SFML / OpenGL demo", font);
	//sf::Text sRgbInstructions("Press space to toggle sRGB conversion", font);
	//sf::Text mipmapInstructions("Press return to toggle mipmapping", font);
	//text.setFillColor(sf::Color(255, 255, 255, 170));
	//sRgbInstructions.setFillColor(sf::Color(255, 255, 255, 170));
	//mipmapInstructions.setFillColor(sf::Color(255, 255, 255, 170));
	//text.setPosition(250.f, 450.f);
	//sRgbInstructions.setPosition(150.f, 500.f);
	//mipmapInstructions.setPosition(180.f, 550.f);

	// Load a texture to apply to our 3D cube
	
	//if (!texture.loadFromFile("src\cube_face.png"))
	//	return EXIT_FAILURE;

//	std::cout << texture.getSize().x;
	// Attempt to generate a mipmap for our cube texture
	// We don't check the return value here since
	// mipmapping is purely optional in this example
	//texture.generateMipmap();

	// Configure the viewport (the same size as the window)
	glViewport(0, 0, settings::view_port_width, settings::view_port_height);

	GLfloat ratio = static_cast<float>(settings::view_port_width) / settings::view_port_height;

	// Enable position and texture coordinates vertex components
	//glEnableClientState(GL_VERTEX_ARRAY);
	//glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	//glVertexPointer(3, GL_FLOAT, 5 * sizeof(GLfloat), cube);
	//glTexCoordPointer(2, GL_FLOAT, 5 * sizeof(GLfloat), cube + 3);
	// Disable normal and color vertex components
//	glDisableClientState(GL_NORMAL_ARRAY);
//	glDisableClientState(GL_COLOR_ARRAY);

	// Create a clock for measuring the time elapsed
	sf::Clock clock;

	// Flag to track whether mipmapping is currently enabled
	bool mipmapEnabled = true;

	//music stuff
	sf::SoundBuffer buffer;
	buffer.loadFromFile("src/EBS.ogg");
	
	sf::Sound sound;
	sound.setBuffer(buffer);
	//sound.play();			//TODO: sound
	sound.setLoop(true);
	sound.setPosition(0, 0, 0);

	sf::Listener ply_ear;
	ply_ear.setPosition(0, 0, 0);
	
	sound.setAttenuation(4);
	//sf::Music music;
	//music.openFromFile("src/videogame retro.mp3");
	//music.play();
	//music.setLoop(true);
	
	player ply(0.f, 1.f, 0.f, settings::player_radius, settings::player_height);
	if (spawn_nodes.size() > 0)
	{
		seg::node * spawn_pt = spawn_nodes[0];
		ply.x = spawn_pt->x;
		ply.y = spawn_pt->y;
		ply.z = spawn_pt->z;
		ply.azimuth = spawn_pt->direction;
	}

	blaster_test blasty;
	//mesh::resize_mesh(&gun_mesh, settings::gun_scale, settings::gun_scale, settings::gun_scale);
	world_meshes_c.push_back(blasty.mesh);
	objects.push_back(&blasty);
	if (spawn_nodes.size() > 0)
	{
		seg::node * spawn_pt = spawn_nodes[rand() % spawn_nodes.size()];
		blasty.mesh->x = spawn_pt->x;
		blasty.mesh->y = spawn_pt->y;
		blasty.mesh->z = spawn_pt->z;
		blasty.mesh->azimuth = spawn_pt->direction;
	}


	A18 a18;
	//mesh::resize_mesh(&gun_mesh, settings::gun_scale, settings::gun_scale, settings::gun_scale);
	world_meshes_c.push_back(a18.mesh);
	objects.push_back(&a18);
	if (spawn_nodes.size() > 0)
	{
		seg::node * spawn_pt = spawn_nodes[rand() % spawn_nodes.size()];
		a18.mesh->x = spawn_pt->x;
		a18.mesh->y = spawn_pt->y;
		a18.mesh->z = spawn_pt->z;
		a18.mesh->azimuth = spawn_pt->direction;
	}


	


	//ply.pick_up(&blasty);

	enemy en(0.0, 0.0, 0.0, settings::player_radius, settings::player_height);
//	reader::read_ply("src/animated_objects_meshes/r_0.ply", *en.mesh_);
	reader::read_animation("src/animated_objects/Amalek.an", en.frames);
	world_meshes_c.push_back(en.mesh_);
	if (spawn_nodes.size() > 0)
	{
		seg::node * spawn_pt = spawn_nodes[rand() % spawn_nodes.size()];
		en.x = spawn_pt->x;
		en.y = spawn_pt->y;
		en.z = spawn_pt->z;
		en.azimuth = spawn_pt->direction;
	}


	mesh::Mesh_c player_mesh;
	reader::read_ply("src/animated_objects_meshes/r_0.ply", player_mesh);
	ply.mesh_ = &player_mesh;
	if (settings::third_person)
	{
		//world_meshes_c.push_back(&player_mesh);
		world_meshes_c.push_back(ply.mesh_);
	}

	reader::read_ply("src/animated_objects_meshes/Amalek_stand.ply", *ply.standing_frame);
	reader::read_ply("src/animated_objects_meshes/Amalek_jump.ply", *ply.jumping_frame);
	reader::read_animation("src/animated_objects/Amalek.an", ply.frames);


	render::set_shaders();

	//threading
	if (settings::ex_render)
	{
		window.setActive(true);
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glClearDepth(1.f);
		//	glClearColor(28/255.f, 38/255.f, 50/255.f, 255);
		glClearColor(255 / 255.f, 210 / 255.f, 210 / 255.f, 255);
	//	glEnable(GL_CULL_FACE);
		glDisable(GL_LIGHTING);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		//GLfloat ratio = static_cast<float>(settings::view_port_width) / settings::view_port_height;
		GLfloat ratio_x = tan(settings::fov_x * (PI / 360.f)) * settings::clip_plane;
		GLfloat ratio_y = tan(settings::fov_y * (PI / 360.f)) * settings::clip_plane;
		glFrustum(-ratio_x, ratio_x, -ratio_y, ratio_y, settings::clip_plane, settings::view_depth);
		glEnable(GL_TEXTURE_2D);
		glViewport(settings::view_port_x, settings::view_port_y - settings::view_port_height + settings::window_height, settings::view_port_width, settings::view_port_height);
	}
	else
	{
		std::thread rendering(render::render_loop, &window, &cam, &world_meshes_c, &hud_meshes_c);
		rendering.detach();
	}

	run_game = true;
	

	//-------------------
	//std::cout << "here4\n";
//	std::thread edit_thread(edit::edit_input);
//	edit_thread.detach();
//	ply.azimuth = 0;
//	ply.pitch = 0;
//	ply.y = 1;
	// Start game loop

	double c_time = 0.0;
	while (c_time < settings::start_time_delay)
	{
		set_delta_time(clock_g);
		c_time += delta_time;
	}

	bool isFocus = true;

	while (run_game/*window.isOpen()*/)
	{	
		// Process events
		sf::Event event;
		while (window.pollEvent(event))
		{
			// Close window: exit
		//	if (event.type == sf::Event::Closed)
		//	{
				//exit = true;
		//		window.close();
		//	}

			// Escape key: exit
			if (((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape)) || event.type == sf::Event::Closed)
			{
				//exit = true;
				run_game = false;
			}

			// Space key: toggle sRGB conversion
			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Space))
			{
				sRgb = !sRgb;
			//	window.close();
			}

			// Adjust the viewport when the window is resized
			if (event.type == sf::Event::Resized)
			{
				glViewport(0, 0, event.size.width, event.size.height);
				ratio = static_cast<float>(event.size.width) / event.size.height;
				glFrustum(-ratio, ratio, -1.f, 1.f, 1.f, 100.f);
			}
			
			if (event.type == sf::Event::TextEntered)
			{
				edit::text_edit(event, world_meshes_c);
			}
			if (event.type == sf::Event::MouseWheelScrolled) {
				if (settings::third_person)
				{
					settings::third_person_radius *= event.mouseWheelScroll.delta>0 ? 0.9 : 1.1;
				}
			}
			if (event.type == event.GainedFocus)
				isFocus = true;
			if (event.type == event.LostFocus)
				isFocus = false;
		}
		
		//edit::dynamic_edit(world_meshes_c, window, cam);


		// Draw the background
		//window.pushGLStates();
		//window.draw(background);
		//window.popGLStates();

		set_delta_time(clock_g);
	
		
		//pause
		//BSP
		if (settings::ex_render)
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
		ply.set_mouse_states(window);
		//	ply.key_mouse_move(cam, &bsp_tree_horz,&bsp_tree_vert, objects);
		ply.key_mouse_move_arbit(cam, &bsp_tree_1, objects);
		
		en.basic_move_big(&bsp_tree_1);

		for (int j = 0; j < objects.size(); j++)
		{
			if(!objects[j]->being_held)
				objects[j]->action();
		}

		if (editing)
			edit_move(editing);
		if (settings::ex_render)
		{
		//	bsp_1::render_node(&bsp_tree_horz, &window, &cam, true);
		//	bsp_1::render_node(&bsp_tree_vert, &window, &cam, true);
			bsp_1::render_node(&bsp_tree_1, &window, &cam, true);
			sf::Shader::bind(&render::f_shad);
			window.display();
		}

	
		//obj.y -= delta_time * 3;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::V))
		{
			if (!no_clip_toggle)
			{
				settings::dev_mode = !settings::dev_mode;
				settings::no_clip = !settings::no_clip;
			}
			no_clip_toggle = 1;
		}
		else
		{
			no_clip_toggle = 0;
		}



		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::F2))
		{
			if (!mode_toggle)
			{
				settings::use_console = !settings::use_console;
			}
			mode_toggle = 1;
		}
		else
		{
			mode_toggle = 0;
		}
		//std::cout << cam.x << " " << cam.y << " " << cam.z << "\n";
//		render::draw_mesh(mer, cam);
	//	bsp_1::draw_bsp(&bsp_tree_1, x, y, z);

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::BackSlash))
		{
			ply.azimuth = 0;
			ply.pitch = 0;
			//reader::export_map_mesh(world_meshes_c);
			reader::export_map_edit(world_meshes_c);
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::F1))
		{
			ply.azimuth = 0;
			ply.pitch = 0;
			ply.y = 5;
		}
		//
		/*
		if (bsp_1::collide(&bsp_tree_1, obj.x, obj.y, obj.z, 2.0f))
		{
			obj.x = v_x;
			//obj.y = v_y;
			obj.y += delta_time * 3;
			obj.z = v_z;
		}
		obj.z = 0;
		*/
		
		// Draw some text on top of our OpenGL object
	//	window.pushGLStates();
	//	window.draw(text);
	//	window.draw(sRgbInstructions);
	//	window.draw(mipmapInstructions);
	//	window.popGLStates();
		// Finally, display the rendered frame on screen

	//	std::cout << delta_time << "\n";
		ply_ear.setPosition(cam.x, cam.y, cam.z);
	//	ply_ear.setDirection(sin(cam.azimuth*(PI/180.f))*cos(cam.pitch*(PI/180.f)), sin(cam.pitch*(PI/180.f)), cos(cam.azimuth*(PI/180.f))*cos(cam.pitch*(PI/180.f)));
		ply_ear.setDirection(sin(cam.azimuth*(PI/180.f)), 0, cos(cam.azimuth*(PI / 180.f)));
		ply_ear.setUpVector(0, 1, 0);
	}	
	Sleep(100);
//	if(window.isOpen())
//		window.close();
	
	return EXIT_SUCCESS;
}		
