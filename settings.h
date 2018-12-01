#pragma once
namespace settings
{
	int window_width = 1000;
	int window_height = 800;
	float view_port_width = 1080;//800;
	float view_port_height = 800;// 600;
	int view_port_x = 0;
	int view_port_y = 0;
	float view_depth = 200.f;
	float clip_plane = 0.2;
	float fov_x = 106.f;
	float fov_y = 90.f;
	int bg_r = 190;
	int bg_g = 190;
	int bg_b = 210;



	double player_height = 0.8;
	double player_radius = 0.6;

	double pick_up_radius = 2.0 * player_radius;
	double unlock_radius = 6 * player_radius;

	double enemy_speed = 2.0;

	double gun_pitch_offset = -40;
	double gun_distance = 0.5;
	double gun_scale = 0.4;
	double gun_pitch_offset_display = -16;
	double gun_pitch_viarance = 26;
	double gun_pitch_range = 10;
	double gun_azimuth_range = 14;
	double gun_azimuth_strafe = 5;
	double gun_pitch_strafe = 5;

	double gravity = 20.0;
	double max_y_vel = 400.0;
	double jump_speed = 10.0;// 10.0;
	double player_speed = 5;
	double splayer_speed_sprint = 8;
	double dev_speed_sprint = 40;
	double player_speed_stafe_jump = 5.8;
	
	double step_up = 0.5;
	
	double step_speed = 6;
	double max_walking_angle = 45.0;
	int hit_resolution = 3;
	double collide_coeff = 0.8;
	bool use_console = false;

	double strafe_lean = 6.0;
	double lean_value = 30.0;

	double wall_cling_grav = 1.0;
	double wall_cling_max_down = 1.0;
	double wall_jump_speed = 30;// 16;
	double friction_air_accel = 16;

	double friction_ground_accel = 10;
	double ground_accel = 80;
	double zero_tolerance = 0.01;

	double grid_size = 0.01;

	double eye_height_fraction = 0.6;
	double third_person_radius = 4.0;
	double third_person_declination = 30.0;
	double third_person_pan = 10.0;
	double third_person_change_speed = 120.0;
	bool third_person_camera_collision = false;

	double animation_fps = 8;
	double animation_delay = 1.0 / animation_fps;

	bool random_enabled = true;
	int random_seed = 150;

	bool fullscreen = false;
	bool dev_mode = false;
	bool no_clip = false;
	bool ex_render = false;
	bool random_ex_colors = true;
	bool third_person = false;
	bool ex_norms = false;
	bool fov_stuff = false;
	bool static_shader = true;
	bool show_hud = true;
	bool object_collide = true;
	bool degerneracy_culling = true;

	double max_incline = 60;

	double object_range = 2.0;
	bool snap_verts_to_grid = false;
	double snap_grid = 0.01;
	bool color_test = false;
	bool show_nodes = false;
	bool build_control = false;
	double epsilon = 0.01;
	double start_time_delay = 0.8;


	//level generation settings
	//this removes caps, if there are two caps touching each other.
	bool remove_hall_blockage = true;
	//This is the percent of spawns that try to be a hall. Higher means more spread out. UNUSED
	double hall_fraction = 0.50;
	//this is the max rooms it tries to add.
	int max_rooms = 30;//10
	//how many times it tries to add a new room before giving up.
	int max_tries = 8;
	int max_key_length = 6;
	double door_chance = 0.1;

	int min_key_placement = 2;
	int max_key_placement = 10;
	int key_min_buffer = 3;
	bool reset_key_placement = true;
	int min_door_placement = 4;
	int max_door_placement = 7; //40?
	double door_fraction = 0.8; // number of doors is ceil(door_fraction * map_size);

	bool map_show_content = false;
}