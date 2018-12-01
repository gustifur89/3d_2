#pragma once
#include "mesh.h"
#include "headers.h"
#include "settings.h"
#include "editing.h"
//std::string vert_shader = "#version 330 core\nin vec4 s_vPosition;\nvoid main () {\ngl_Position = s_vPosition;\n} ";
//std::string frag_shader = "#version 330 core\nout vec4 s_vColor;\nvoid main () {\nfColor = vec4 (1.0,0.0,0.0,1.0);\n} ";

namespace shaders
{
	struct shader
	{
		shader() {};
		shader(const std::string vert_shader_, const std::string frag_shader_)
		{
			vsh = vert_shader_;
			fsh = frag_shader_;
		}
		std::string vsh;
		std::string fsh;
	};

	const std::string vert_shader_red_burn =
		"varying vec3 normal;\n"
		"varying vec3 vertex_to_light_vector;\n"
		"uniform vec4 camera_position;\n"
		"varying float eye_z;\n"
		"void main()\n"
		"{\n"
		"gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
		"normal = gl_NormalMatrix * gl_Normal;\n"
		"vec4 vertex_in_modelview_space = gl_ModelViewMatrix * gl_Vertex;\n"
		"eye_z = length(vertex_in_modelview_space) / 20.f;\n"
		"gl_FrontColor = gl_Color;\n"
		"gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;\n"
		"vertex_to_light_vector = vec3(vertex_in_modelview_space);\n"
		"}\n";
	const std::string frag_shader_red_burn =
		"uniform sampler2D texture;\n"
		"varying vec3 normal;\n"
		"varying vec3 vertex_to_light_vector;\n"
		"varying float eye_z;\n"
		"void main()\n"
		"{\n"
		"const vec4 AmbientColor = vec4(0.1, 0.0, 0.0, 1.0);\n"
		"vec3 normalized_normal = normalize(normal);\n"
		"vec3 normalized_vertex_to_light_vector = normalize(vertex_to_light_vector);\n"
		"float DiffuseTerm = clamp(dot(normalized_normal, normalized_vertex_to_light_vector), 0.0, 1.0);\n"
		"vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);\n"
		"float depth = 1-eye_z;\n//((gl_FragCoord.z));\n"
		"gl_FragColor = gl_Color * pixel * depth + AmbientColor*(1.f/depth);\n"
		"}\n";

	const std::string vert_shader_r2 =
		"varying vec3 normal;\n"
		"varying vec3 vertex_to_light_vector;\n"
		"uniform vec4 camera_position;\n"
		"varying float eye_z;\n"
		"void main()\n"
		"{\n"
		"gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
		"normal = gl_NormalMatrix * gl_Normal;\n"
		"vec4 vertex_in_modelview_space = gl_ModelViewMatrix * gl_Vertex;\n"
		"eye_z = length(vertex_in_modelview_space) / 40.f;\n//(gl_ModelViewMatrix * gl_Vertex).z/20.f;\n"
		"gl_FrontColor = gl_Color;\n"
		"gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;\n"
		"vertex_to_light_vector = vec3(vertex_in_modelview_space);\n"
		"}\n";
	const std::string frag_shader_r2 =
		"uniform sampler2D texture;\n"
		"varying vec3 normal;\n"
		"varying vec3 vertex_to_light_vector;\n"
		"varying float eye_z;\n"
		"void main()\n"
		"{\n"
		"const vec4 AmbientColor = vec4(0.045, 0.0, 0.03, 1.0);\n"
		"vec3 normalized_normal = normalize(normal);\n"
		"vec3 normalized_vertex_to_light_vector = normalize(vertex_to_light_vector);\n"
		"float DiffuseTerm = clamp(dot(normalized_normal, normalized_vertex_to_light_vector), 0.0, 1.0);\n"
		"vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);\n"
		"float depth = 1-eye_z;\n//((gl_FragCoord.z));\n"
		"float r2 = 0.01f / ( eye_z * eye_z);\n"
		"r2 = clamp(r2, 0.0,1.0);\n"
		"gl_FragColor = gl_Color * pixel * r2 + 0.0*AmbientColor*(1.f/(depth));\n"
		"}\n";
	
	const std::string vert_shader_norm =
		"varying vec3 col;"
		"void main()\n"
		"{\n"
		"gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
		"gl_FrontColor = gl_Color;\n"
		"col = normalize(gl_Normal);\n"
		"}\n";
	const std::string frag_shader_norm_fun =
		"uniform sampler2D texture;\n"
		"varying vec3 col;\n"
		"void main()\n"
		"{\n"
		"vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);\n"
		"gl_FragColor = vec4(col,1.0);\n"
		"}\n";

	const std::string vert_shader_norm_fun =
		"varying vec3 col;"
		"void main()\n"
		"{\n"
		"gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
		"gl_FrontColor = gl_Color;\n"
		"col = normalize(gl_NormalMatrix * gl_Normal);\n"
		"}\n";
	const std::string frag_shader_norm =
		"uniform sampler2D texture;\n"
		"varying vec3 col;\n"
		"void main()\n"
		"{\n"
		"vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);\n"
		"gl_FragColor = vec4(col,1.0);\n"
		"}\n";

	const std::string vert_shader_directional =
		"varying float diff;\n"
		"void main()\n"
		"{\n"
		"gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
		"gl_FrontColor = gl_Color;\n"
		"gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;\n"
		"diff = max(0.0, dot( normalize(gl_NormalMatrix * gl_Normal), normalize(-vec3(gl_ModelViewMatrix * gl_Vertex) ) ) );\n"
		"}\n";
	const std::string frag_shader_directional =
		"uniform sampler2D texture;\n"
		"varying float diff;\n"
		"void main()\n"
		"{\n"
		"vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);\n"
		"gl_FragColor = diff * gl_Color * pixel;\n"
		"}\n";

	const std::string vert_shader_light_0 =
		"varying float diff;\n"
		"varying float dist;\n"
		"void main()\n"
		"{\n"
		"gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
		"gl_FrontColor = gl_Color;\n"
		"gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;\n"
		"diff = max(0.0, dot( normalize(gl_NormalMatrix * gl_Normal), normalize(-vec3(gl_ModelViewMatrix * gl_Vertex) ) ) );\n"
		"dist = clamp(1.f - length(vec3(gl_ModelViewMatrix * gl_Vertex))/40.f, 0.0, 1.0);\n"
		"}\n";
	const std::string frag_shader_light_0 =
		"uniform sampler2D texture;\n"
		"varying float diff;\n"
		"varying float dist;\n"
		"void main()\n"
		"{\n"
		"vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);\n"
		"gl_FragColor = diff * dist * gl_Color * pixel;\n"
		"}\n";

	const std::string vert_shader_light_atten =
		"uniform float atten;\n"
		"varying float diff;\n"
		"varying float dist;\n"
		"void main()\n"
		"{\n"
		"gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
		"gl_FrontColor = gl_Color;\n"
		//"gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;\n"
		"diff = max(0.0, dot( normalize(gl_NormalMatrix * gl_Normal), normalize(-vec3(gl_ModelViewMatrix * gl_Vertex) ) ) );\n"
		"dist = clamp(1.f - length(vec3(gl_ModelViewMatrix * gl_Vertex) )/ 40.f, 0.0, 1.0);\n"
		"}\n";
	const std::string frag_shader_light_atten =
		//"uniform sampler2D texture;\n"
		"varying float diff;\n"
		"varying float dist;\n"
		"void main()\n"
		"{\n"
		//"vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);\n"
		"gl_FragColor = diff * dist * gl_Color;\n// * pixel;\n"
		"}\n";




	const std::string vert_shader_directional_c =
		"varying vec4 pos;\n"
		"varying vec3 norm;\n"
		"void main()\n"
		"{\n"
		"gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
		"gl_FrontColor = gl_Color;\n"
		"pos = gl_Vertex;\n"
		"norm = gl_Normal;\n"
		"}\n";
	const std::string frag_shader_directional_c =
		"varying vec4 pos;\n"
		"varying vec3 norm;\n"
		"void main()\n"
		"{\n"
		"float diff = max(0.0, dot(normalize(gl_NormalMatrix * norm), normalize(-vec3(gl_ModelViewMatrix * pos)))); \n"
		"gl_FragColor = (0.4*diff+0.6) * gl_Color;\n"
		"}\n";

	const std::string vert_shader_light_atten_c =
		"varying vec4 pos;\n"
		"varying vec3 norm;\n"
		"void main()\n"
		"{\n"
		"gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
		"gl_FrontColor = gl_Color;\n"
		"pos = gl_Vertex;\n"
		"norm = gl_Normal;\n"
		"}\n";
	const std::string frag_shader_light_atten_c =
		"varying vec4 pos;\n"
		"varying vec3 norm;\n"
		"void main()\n"
		"{\n"
		"float dist = clamp(1.f - length(vec3(gl_ModelViewMatrix * pos) )/ 100.f, 0.0, 1.0);\n"		// /80.f
		"float diff = max(0.0, dot(normalize(gl_NormalMatrix * norm), normalize(-vec3(gl_ModelViewMatrix * pos)))); \n"
		//"gl_FragColor = clamp( 0.4*dist + 0.4*diff - 0.2, 0.0, 1.0) * gl_Color;\n"
		// THIS IS THE PROPER ONE "gl_FragColor = clamp( 0.4*dist + 0.4*diff +0.4, 0.0, 1.0) * gl_Color;\n"
		"gl_FragColor = (dist) * (0.3*diff + 0.4) * gl_Color + (1.f - dist)*vec4(0.0, 0.05, 0.01, 1.0);"
		"}\n";


	const std::string vert_static =
		"varying vec4 pos;\n"
		"varying vec3 norm;\n"
		"void main()\n"
		"{\n"
		"gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
		"gl_FrontColor = gl_Color;\n"
		"pos = gl_Vertex;\n"
		"norm = gl_Normal;\n"
		"}\n";
	const std::string frag_static =
		"varying vec4 pos;\n"
		"varying vec3 norm;\n"
		"uniform float seed;\n"
		"void main()\n"
		"{\n"
		"float dist = 1.0;\n"// -(gl_FragCoord.z / gl_FragCoord.w) / 40.0; \n"// / gl_FragCoord.w // 80.0
	//	"float dist = clamp(1.f - length(vec3(gl_ModelViewMatrix * pos) )/ 300.f, 0.0, 1.0);\n"		// /80.f
	//	"dist = originalZ;\n"
		"float diff = max(0.0, dot(normalize(gl_NormalMatrix * norm), normalize(-vec3(gl_ModelViewMatrix * pos)))); \n"
		"float rand = fract(sin(dot(vec2(dist*seed, diff), vec2(12.9898, diff*78.233))) * 43758.5453);\n"
	//	"float rand_g = fract(sin(dot(vec2(dist*rand_r, diff), vec2(12.9898, diff*78.233))) * 43758.5453);\n"
	//	"float rand_b = fract(sin(dot(vec2(dist*rand_g, diff), vec2(12.9898, diff*78.233))) * 43758.5453);\n"
		//"gl_FragColor = clamp( 0.4*dist + 0.4*diff - 0.2, 0.0, 1.0) * gl_Color;\n"
		// THIS IS THE PROPER ONE "gl_FragColor = clamp( 0.4*dist + 0.4*diff +0.4, 0.0, 1.0) * gl_Color;\n"
		"gl_FragColor = (dist) * (0.3*diff + 0.4) * gl_Color + (1.f - dist)*vec4(0.0, 0.0, 0.0, 1.0) + (dist) * 0.0 * rand * vec4(0.1, 0.1, 0.1, 1.0);\n"//(dist) * rand * vec4(rand_r, rand_g, rand_b, 1.0); // (0.25 - dist*dist) * 0.6 * vec4(rand_r, 3.0*rand_g, 3.0*rand_b, 1.0);\n
		"}\n";

	const std::string vert_static_a =
		"varying vec4 pos;\n"
		"varying vec3 norm;\n"
		"varying float diff;\n"
		//"uniform vec3 dir;\n"
		"void main()\n"
		"{\n"
		"gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
		"gl_FrontColor = gl_Color;\n"
		"pos = gl_Vertex;\n"
		//"diff = dot(normalize(gl_Normal),normalize(dir));\n"// max(0.0, dot(normalize(gl_NormalMatrix * gl_Normal), normalize(dir)));\n"//-vec3(gl_ModelViewMatrix * pos
		"norm = gl_Normal;\n"
		"}\n";
	const std::string frag_static_a =
		"varying vec4 pos;\n"
		"varying vec3 norm;\n"
		"uniform float seed;\n"
		"void main()\n"
		"{\n"
		"float dist = 1.0 - (gl_FragCoord.z / gl_FragCoord.w) / 400.0;\n"// -(gl_FragCoord.z * gl_FragCoord.w) / 40.0; \n"// / gl_FragCoord.w // 80.0
							 //	"float dist = clamp(1.f - length(vec3(gl_ModelViewMatrix * pos) )/ 300.f, 0.0, 1.0);\n"		// /80.f
							 //	"dist = originalZ;\n"//40.0
		"float diff = max(0.0, dot(normalize(gl_NormalMatrix * norm), normalize(-vec3(gl_ModelViewMatrix * pos)))); \n"
		"float rand = fract(sin(dot(vec2(dist*seed, diff), vec2(12.9898, diff*78.233))) * 43758.5453);\n"
		//	"float rand_g = fract(sin(dot(vec2(dist*rand_r, diff), vec2(12.9898, diff*78.233))) * 43758.5453);\n"
		//	"float rand_b = fract(sin(dot(vec2(dist*rand_g, diff), vec2(12.9898, diff*78.233))) * 43758.5453);\n"
		//"gl_FragColor = clamp( 0.4*dist + 0.4*diff - 0.2, 0.0, 1.0) * gl_Color;\n"
		// THIS IS THE PROPER ONE "gl_FragColor = clamp( 0.4*dist + 0.4*diff +0.4, 0.0, 1.0) * gl_Color;\n"
		"gl_FragColor = (dist) * (0.2*diff + 0.6) * gl_Color + (1.f - dist)*vec4(0.0, 0.0, 0.0, 1.0) + (dist) * rand * vec4(0.06, 0.06, 0.06, 1.0);\n"//(dist) * rand * vec4(rand_r, rand_g, rand_b, 1.0); // (0.25 - dist*dist) * 0.6 * vec4(rand_r, 3.0*rand_g, 3.0*rand_b, 1.0);\n
		"}\n";

	/*
	float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}*/

	const std::string vert_shader_normal =
	//	"varying vec4 pos;\n"
		"varying vec3 norm;\n"
		"void main()\n"
		"{\n"
		"gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
		"gl_FrontColor = gl_Color;\n"
	//	"pos = gl_Vertex;\n"
		"norm = gl_Normal;\n"
		"}\n";
	const std::string frag_shader_normal =
		//"varying vec4 pos;\n"
		"varying vec3 norm;\n"
		"void main()\n"
		"{\n"
		"gl_FragColor = vec4((norm.x+1)*0.5,(norm.y+1)*0.5,(norm.z+1)*0.5,1.0);"//"gl_FragColor = vec4(norm,1.0);"
		"}\n";
}

namespace render
{
	GLuint vertID = 0;
	GLuint fragID = 0;
	GLuint progID = 0;
	sf::Shader f_shad;
	GLfloat light_attenuation = 60.f;	//40.f is good
	float fps = 0.f;
	sf::Font font;

	void set_gl_states(sf::RenderWindow * window)
	{
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glClearDepth(1.f);
	//	glClearColor(28/255.f, 38/255.f, 50/255.f, 255);
		glClearColor(settings::bg_r / 255.f, settings::bg_r / 255.f, settings::bg_r / 255.f, 255);
		glEnable(GL_CULL_FACE);
		glDisable(GL_LIGHTING);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		//GLfloat ratio = static_cast<float>(settings::view_port_width) / settings::view_port_height;
		if (settings::fov_stuff)
		{
			GLfloat ratio_x = tan(settings::fov_x * (PI / 360.f)) * settings::clip_plane;
			GLfloat ratio_y = tan(settings::fov_y * (PI / 360.f)) * settings::clip_plane;
			glFrustum(-ratio_x, ratio_x, -ratio_y, ratio_y, settings::clip_plane, settings::view_depth);
		}
		else
		{
			GLfloat ratio_x = tan(settings::fov_x * (PI / 360.f)) * settings::clip_plane;
			GLfloat ratio_y = ratio_x * (settings::view_port_height / settings::view_port_width);
			glFrustum(-ratio_x, ratio_x, -ratio_y, ratio_y, settings::clip_plane, settings::view_depth);
		}
		glEnable(GL_TEXTURE_2D);
	}
	
	void set_shaders()
	{
		/*
		const std::string frag_shader =
		//	"out vec4 color;\n"
			"uniform sampler2D texture;\n"
			"varying vec3 lightVector;\n"
			"varying vec3 vertNormal;\n"
			"varying vec3 col;\n"
			"varying float diff;\n"
			"varying float dist;\n"
			"void main()\n"
			"{\n"
			"vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);\n"
			"float dot_product = max(dot(vertNormal, lightVector), 0.0);\n"
		//	"gl_FragColor = dot_product *gl_Color * pixel;\n // vec4(1.0, 1.0, 1.0, 1.0);\n"
		//	"gl_FragColor = vec4(col,1.0) *  gl_Color * pixel;\n"
			"gl_FragColor = diff * gl_Color * pixel * dist;\n"
			"}\n";
		*/	
		/*
			"uniform sampler2D texture;\n"
			"varying vec3 normal;\n"
			"varying vec3 vertex_to_light_vector;\n"
			"varying float eye_z;\n"
			"varying float diff;\n"
			"void main()\n"
			"{\n"

			"const vec4 AmbientColor = vec4(0.1, 0.0, 0.03, 1.0);\n"
			//	"const vec4 DiffuseColor = vec4(1.0, 0.0, 0.0, 1.0);\n"

			"vec3 normalized_normal = normalize(normal);\n"
			"vec3 normalized_vertex_to_light_vector = normalize(vertex_to_light_vector);\n"

			"float DiffuseTerm = clamp( dot( normal,vertex_to_light_vector ), 0.0,1.0 );\n"

			//color = LightColor * cosTheta;//dot(normalized_normal,normalized_vertex_to_light_vector);\n //clamp(dot(normalized_normal, normalized_vertex_to_light_vector), 0.0, 1.0);\n"

			"vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);\n"
			"float depth = 1-eye_z;\n//((gl_FragCoord.z));\n"// * gl_FragCoord.w)/gl_FragCoord.w);\n"
		//	"float r2 = (eye_z * eye_z)/10.f;\n"
		//	"r2 = clamp(r2, 0.0,1.0);\n"
															 //"gl_FragColor = gl_Color * pixel;\n"
			"gl_FragColor = gl_Color * pixel * depth * diff;\n;// + AmbientColor* pixel *r2;\n;// + DiffuseTerm;\n;//+ AmbientColor;\n"// ( AmbientColor)\n;"// + pixel * gl_Color * DiffuseTerm);\n"
			"}\n";
		*/
		//pixel * gl_Color;\n"// *
		/*	"uniform sampler2D texture;\n"
			"varying vec3 N;\n"
			"varying vec3 v;\n"
			"varying vec4 D;\n"
			"void main()\n"
			"{"
			"vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);\n"
			//"vec4 L = normalize(D - v);\n"
			//"vec4 Idiff = max(dot(N, L), 0.0);\n"
			//"Idiff = clamp(Idiff, 0.0, 1.0);\n"
			"gl_FragColor = gl_Color * pixel;\n"
			"}\n";

		/* vec3 L = normalize(gl_LightSource[0].position.xyz - v);   
   vec4 Idiff = gl_FrontLightProduct[0].diffuse * max(dot(N,L), 0.0);  
   Idiff = clamp(Idiff, 0.0, 1.0); 

   gl_FragColor = Idiff;*/
		/*
		const std::string vert_shader =
			"uniform vec3 player;\n"

			"varying vec3 col;"
			"varying vec3 vertNormal;\n"
			"varying vec3 lightVector;\n"
			"varying float diff;\n"
			"varying float dist;\n"
			"void main()\n"
			"{\n"
			"gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
			"gl_FrontColor = gl_Color;\n"
			"gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;\n"
			//"mat4 pm = projectionMatrix * viewMatrix;\n"
			"vertNormal = normalize(gl_Normal);\n"//(gl_ModelViewMatrix * vec4(gl_Normal, 0)).xyz;\n"
			"lightVector = normalize((gl_Vertex-vec4(0.0,0.0,0.0,1.0)).xyz);\n;//gl_ModelViewMatrix * gl_Vertex;\n// -(viewMatrix * vec4(vertPos3D.xyz, 1.0)).xyz;\n"
			"col = gl_Normal.xyz;\n"

			//we need the vector from me to them... how though?

			"vec3 camera_to_vertex = normalize((gl_Vertex).xyz-player);\n//vec3(0.414,0.414,0.0);\n"
			//gl_NormalMatrix *      //gl_ModelViewMatrix * 
			"diff = max(0.0, dot( gl_Normal, -camera_to_vertex ) );\n"//max(0.0,dot(normalize(gl_NormalMatrix * gl_Normal),camera_to_vertex));\n"
			"dist = clamp(1.f - length(player - gl_Vertex.xyz)/40.f, 0.0, 1.0);\n"
			
			"}\n";
		*/
		/*
			"varying vec3 normal;\n"
			"varying vec3 vertex_to_light_vector;\n"
			"uniform vec4 camera_position;\n"
			"varying float eye_z;\n"
			"varying float diff;\n"
			"void main()\n"
			"{\n"
			"gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
			"normal = gl_NormalMatrix * gl_Normal;\n"
			"vec4 vertex_in_modelview_space = gl_ModelViewMatrix * gl_Vertex;\n"
			"eye_z = length(vertex_in_modelview_space) / 80.f;\n"
			"diff = dot(normalize(normal),normalize(-vertex_in_modelview_space.xyz));\n "
			"gl_FrontColor = gl_Color;\n"
			"gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;\n"
			"vertex_to_light_vector = vec3(vertex_in_modelview_space);\n"
			"}\n";
			*/			/*		"varying vec3 N;\n"
			"varying vec3 v;\n"
			"varying vec4 D;\n"
			"uniform vec4 camera_position;"
			"void main()\n"
			"{\n"
//			"gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
			"gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;\n"
			"gl_FrontColor = gl_Color;\n"
			"D = camera_position - gl_Vertex;\n"
			"v = vec3(gl_ModelViewMatrix * gl_Vertex);\n"
			"N = normalize(gl_NormalMatrix * gl_Normal);\n"
			"gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
			"}\n";
			*/
		/*varying vec3 N;
varying vec3 v;

void main(void)
{

   v = vec3(gl_ModelViewMatrix * gl_Vertex);       
   N = normalize(gl_NormalMatrix * gl_Normal);
   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}*/
		//render::f_shad.loadFromFile("src/vert_shader.vsh", "src/frag_shader.fsh");
		//f_shad.loadFromMemory(vert_shader,frag_shader);
	
		if (settings::static_shader)
		{	
			f_shad.loadFromMemory(shaders::vert_static_a, shaders::frag_static_a);
		}
		else
		{
			f_shad.loadFromMemory(shaders::vert_shader_light_atten_c, shaders::frag_shader_light_atten_c);
		}

//		f_shad.loadFromMemory(shaders::vert_shader_normal, shaders::frag_shader_normal);

	
	//	f_shad.loadFromMemory(shaders::vert_shader_light_0, shaders::frag_shader_light_0);
	//	f_shad.loadFromMemory(shaders::vert_shader_directional, shaders::frag_shader_directional);
	//	f_shad.loadFromMemory(shaders::vert_shader_directional_c, shaders::frag_shader_directional_c);
	//	f_shad.loadFromMemory(shaders::vert_shader_norm, shaders::frag_shader_norm)
		/*
		vertID = glCreateShader(GL_VERTEX_SHADER);
		const char * vSh = vert_shader.c_str();
		glShaderSource(vertID, 1, (const char**) &vSh , NULL);
		glCompileShader(vertID);

		fragID = glCreateShader(GL_FRAGMENT_SHADER);
		const char * fSh = frag_shader.c_str();
		glShaderSource(fragID, 1, (const char**)&fSh, NULL);
		glCompileShader(fragID);

		progID = glCreateProgram();
		glAttachShader(progID, vertID);
		glAttachShader(progID, fragID);
		glLinkProgram(progID);
		glValidateProgram(progID);
		glUseProgram(progID);

		glDeleteShader(vertID);
		glDeleteShader(fragID);
		*/
	}

	void set_font()
	{
		font.loadFromFile("src/verdana.ttf");
	}

	std::vector<GLfloat> vertexes;
	std::vector<GLfloat> q_vertexes;
	std::vector<GLfloat> t_vertexes;
	GLint vertex_number;

	void draw_mesh (mesh::Mesh_g mesh, camera_pos cam)
	{
		GLfloat* obj = &mesh.vertexes[0];
	//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//	glColor3f(1, 1, 1);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		//glEnableClientState(GL_COLOR_ARRAY);		//DONT UNCOMMENT; EVIL THINGS HAPPEN
		glVertexPointer(3, GL_FLOAT, 8 * sizeof(GLfloat), obj);
		glTexCoordPointer(2, GL_FLOAT, 8 * sizeof(GLfloat), obj + 3);
		glNormalPointer(GL_FLOAT, 8 * sizeof(GLfloat), obj + 5);
		
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();	
		//rotate(-camera_theta)
		//translate(-camer)
		//rotate (object)
		//translate (object)
		

		glRotatef(-cam.pitch, 1.f, 0.f, 0.f);
		glRotatef(-cam.azimuth, 0.f, 1.f, 0.f);
		glRotatef(-cam.roll, 0.f, 0.f, 1.f);
		
		glTranslatef(-cam.x, -cam.y, -cam.z);

		//glRotatef()
		glTranslatef(mesh.x, mesh.y, mesh.z);
		//glRotatef();
	//	glRotatef(0.0, 1.f, 0.f, 0.f);
	//	glRotatef(30.0, 0.f, 1.f, 0.f);
	//	glRotatef(0.0, 0.f, 0.f, 1.f);
		glRotatef(mesh.pitch, 1.f, 0.f, 0.f);
		glRotatef(mesh.azimuth, 0.f, 1.f, 0.f);
		glRotatef(mesh.roll, 0.f, 0.f, 1.f);
		
		//	glRotatef(0, 1.f, 0.f, 0.f);

		//glEnable(GL_TEXTURE_2D)

	//	sf::Texture::bind(&mesh::textures[mesh.texture_number]);
		
		//	glRotatef(clock.getElapsedTime().asSeconds() * 90.f, 0.f, 0.f, 1.f);
		if(mesh.type == R_QUADS)
			glDrawArrays(GL_QUADS, 0, mesh.vertex_number);
		else if(mesh.type == R_TRIANGLES)
			glDrawArrays(GL_TRIANGLES, 0, mesh.vertex_number);
	}

	void draw_mesh (mesh::Mesh_c mesh, camera_pos cam)
	{
		GLfloat * obj = &mesh.vertexes[0];
		//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//	glColor3f(1, 1, 1);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);		//DONT UNCOMMENT; EVIL THINGS HAPPEN
		glVertexPointer(3, GL_FLOAT, 9 * sizeof(GLfloat), obj);
		glNormalPointer(GL_FLOAT, 9 * sizeof(GLfloat), obj + 3);
		glColorPointer(3, GL_FLOAT, 9 * sizeof(GLfloat), obj + 6);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//rotate(-camera_theta)
		//translate(-camer)
		//rotate (object)
		//translate (object)

		/*
		 Matrix.setIdentityM(mViewMatrix, 0);
                ///THIS ONE WORKS!!!
                Matrix.rotateM(mViewMatrix, 0, (float) -cameraView.getRol(), 0, 0, 1.f);

                Matrix.rotateM(mViewMatrix, 0, (float) -cameraView.getPit(), 1.f, 0, 0);

                Matrix.rotateM(mViewMatrix, 0, (float) -cameraView.getAz(), 0, 1.f, 0);

                Matrix.translateM(mViewMatrix, 0, (float) -cameraView.getX(), (float) -cameraView.getY(), (float) -cameraView.getZ());

                Matrix.translateM(mViewMatrix, 0, (float) actors.get(i).pos.x, (float) actors.get(i).pos.y, (float) actors.get(i).pos.z);

                Matrix.rotateM(mViewMatrix, 0, (float) actors.get(i).pos.rol, 0, 0, 1.f);

                Matrix.rotateM(mViewMatrix, 0, (float) actors.get(i).pos.az, 0, 1.f, 0);

                Matrix.rotateM(mViewMatrix, 0, (float) actors.get(i).pos.pit, 1.f, 0, 0);*/
		glRotatef(-cam.roll, 0.f, 0.f, 1.f);
		glRotatef(-cam.pitch, 1.f, 0.f, 0.f);
		glRotatef(-cam.azimuth, 0.f, 1.f, 0.f);
		

		glTranslatef(-cam.x, -cam.y, -cam.z);

		//glRotatef()
		glTranslatef(mesh.x, mesh.y, mesh.z);

		//glRotatef();
		//	glRotatef(0.0, 1.f, 0.f, 0.f);
		//	glRotatef(30.0, 0.f, 1.f, 0.f);
		//	glRotatef(0.0, 0.f, 0.f, 1.f);
		
		glRotatef(mesh.azimuth, 0.f, 1.f, 0.f);
		glRotatef(mesh.pitch, 1.f, 0.f, 0.f);
		glRotatef(mesh.roll, 0.f, 0.f, 1.f);
		
		/** /
		glRotatef(-cam.pitch, 1.f, 0.f, 0.f);
		glRotatef(-cam.azimuth, 0.f, 1.f, 0.f);
		glRotatef(-cam.roll, 0.f, 0.f, 1.f);
		
		glTranslatef(-cam.x, -cam.y, -cam.z);

		//glRotatef()
		glTranslatef(mesh.x, mesh.y, mesh.z);

		//glRotatef();
		//	glRotatef(0.0, 1.f, 0.f, 0.f);
		//	glRotatef(30.0, 0.f, 1.f, 0.f);
		//	glRotatef(0.0, 0.f, 0.f, 1.f);
		
		glRotatef(mesh.azimuth, 0.f, 1.f, 0.f);
		glRotatef(mesh.roll, 0.f, 0.f, 1.f);
		glRotatef(mesh.pitch, -1.f, 0.f, 0.f);
		*/
		//	glRotatef(0, 1.f, 0.f, 0.f);

		//glEnable(GL_TEXTURE_2D)
		glDrawArrays(GL_TRIANGLES, 0, mesh.vertex_number);
	}

	void draw_mesh(mesh::Mesh_c * mesh, camera_pos cam)
	{
		GLfloat * obj = &mesh->vertexes[0];
		//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glVertexPointer(3, GL_FLOAT, 9 * sizeof(GLfloat), obj);
		glNormalPointer(GL_FLOAT, 9 * sizeof(GLfloat), obj + 3);
		glColorPointer(3, GL_FLOAT, 9 * sizeof(GLfloat), obj + 6);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glRotatef(-cam.roll, 0.f, 0.f, 1.f);
		glRotatef(-cam.pitch, 1.f, 0.f, 0.f);
		glRotatef(-cam.azimuth, 0.f, 1.f, 0.f);

		glTranslatef(-cam.x, -cam.y, -cam.z);

		glTranslatef(mesh->x, mesh->y, mesh->z);

		glRotatef(mesh->azimuth, 0.f, 1.f, 0.f);
		glRotatef(mesh->pitch, 1.f, 0.f, 0.f);
		glRotatef(mesh->roll, 0.f, 0.f, 1.f);

		
		//glEnable(GL_TEXTURE_2D)
		glDrawArrays(GL_TRIANGLES, 0, mesh->vertex_number);
	}

	void draw_mesh (mesh_p::Mesh mesh, camera_pos cam)
	{
		GLfloat * obj = &mesh.vertexes[0];
		std::cout << "n=" << mesh.vertex_number << "\n";
		std::cout << "x=" << mesh.vertexes[0]<<"\n";
		
		//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//	glColor3f(1, 1, 1);
		if (mesh.type == mesh_p::Mesh::TYPE::COLOR)
		{
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_NORMAL_ARRAY);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			glEnableClientState(GL_COLOR_ARRAY);		//DONT UNCOMMENT; EVIL THINGS HAPPEN
			glVertexPointer(3, GL_FLOAT, 9 * sizeof(GLfloat), obj);
			glNormalPointer(GL_FLOAT, 9 * sizeof(GLfloat), obj + 3);
			glColorPointer(3, GL_FLOAT, 9 * sizeof(GLfloat), obj + 6);
		}
		else if (mesh.type == mesh_p::Mesh::TYPE::TEXTURE)
		{
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_NORMAL_ARRAY);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glDisableClientState(GL_COLOR_ARRAY);		//DONT UNCOMMENT; EVIL THINGS HAPPEN
			glVertexPointer(3, GL_FLOAT, 8 * sizeof(GLfloat), obj);
			glNormalPointer(GL_FLOAT, 8 * sizeof(GLfloat), obj + 3);
			glTexCoordPointer(2, GL_FLOAT, 8 * sizeof(GLfloat), obj + 6);
		}


		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRotatef(-cam.pitch, 1.f, 0.f, 0.f);
		glRotatef(-cam.azimuth, 0.f, 1.f, 0.f);
		glRotatef(-cam.roll, 0.f, 0.f, 1.f);
		glTranslatef(-cam.x, -cam.y, -cam.z);
		glTranslatef(mesh.x, mesh.y, mesh.z);
		glRotatef(mesh.azimuth, 0.f, 1.f, 0.f);
		glRotatef(mesh.pitch, 1.f, 0.f, 0.f);
		glRotatef(mesh.roll, 0.f, 0.f, 1.f);

		glDrawArrays(GL_TRIANGLES, 0, mesh.vertex_number);
	}


	void get_fps(sf::Clock &clock)
	{
		fps = 1.f / clock.getElapsedTime().asSeconds();
		clock.restart();
	}

	void render_fields(sf::RenderWindow &window, std::vector<edit::input_field*> &fields, sf::Font font)
	{
		bool did_select = false;
		for (int i = 0; i < fields.size(); i++)
		{
			fields[i]->draw_field(window, font, did_select);
		}
		if (!did_select)
		{
			edit::focus = nullptr;
		}
	}

	void setup_fields(std::vector<edit::input_field*> window_fields)
	{


		edit::input_field * t = new edit::input_field;
		t->x = 810;
		t->size = 15;
		t->color = sf::Color(100, 100, 100, 255);
		t->dynamic = false;
		t->content = "POSITION:";
		window_fields.push_back(t);

		t = new edit::input_field;
		t->x = 810;
		t->y = 20;
		t->size = 15;
		t->color = sf::Color(100, 100, 100, 255);
		t->dynamic = false;
		t->content = "X=";
		window_fields.push_back(t);

		edit::pos_x.x = 834;
		edit::pos_x.y = 20;
		edit::pos_x.size = 15;
		edit::pos_x.color = sf::Color(100, 100, 100, 255);
		edit::pos_x.dynamic = true;
		edit::pos_x.content = "";
		window_fields.push_back(&edit::pos_x);

		t = new edit::input_field;
		t->x = 810;
		t->y = 40;
		t->size = 15;
		t->color = sf::Color(100, 100, 100, 255);
		t->dynamic = false;
		t->content = "Y=";
		window_fields.push_back(t);

		edit::pos_y.x = 834;
		edit::pos_y.y = 40;
		edit::pos_y.size = 15;
		edit::pos_y.color = sf::Color(100, 100, 100, 255);
		edit::pos_y.dynamic = true;
		edit::pos_y.content = "";
		window_fields.push_back(&edit::pos_y);

		t = new edit::input_field;;
		t->x = 810;
		t->y = 60;
		t->size = 15;
		t->color = sf::Color(100, 100, 100, 255);
		t->dynamic = false;
		t->content = "Z=";
		window_fields.push_back(t);

		edit::pos_z.x = 834;
		edit::pos_z.y = 60;
		edit::pos_z.size = 15;
		edit::pos_z.color = sf::Color(100, 100, 100, 255);
		edit::pos_z.dynamic = true;
		edit::pos_z.content = "";
		window_fields.push_back(&edit::pos_z);




		t = new edit::input_field;
		t->x = 810;
		t->y = 80;
		t->size = 15;
		t->color = sf::Color(100, 100, 100, 255);
		t->dynamic = false;
		t->content = "SIZE:";
		window_fields.push_back(t);

		t = new edit::input_field;
		t->x = 810;
		t->y = 100;
		t->size = 15;
		t->color = sf::Color(100, 100, 100, 255);
		t->dynamic = false;
		t->content = "X=";
		window_fields.push_back(t);

		edit::siz_x.x = 834;
		edit::siz_x.y = 100;
		edit::siz_x.size = 15;
		edit::siz_x.color = sf::Color(100, 100, 100, 255);
		edit::siz_x.dynamic = true;
		edit::siz_x.content = "";
		window_fields.push_back(&edit::siz_x);

		t = new edit::input_field;
		t->x = 810;
		t->y = 120;
		t->size = 15;
		t->color = sf::Color(100, 100, 100, 255);
		t->dynamic = false;
		t->content = "Y=";
		window_fields.push_back(t);

		edit::siz_y.x = 834;
		edit::siz_y.y = 120;
		edit::siz_y.size = 15;
		edit::siz_y.color = sf::Color(100, 100, 100, 255);
		edit::siz_y.dynamic = true;
		edit::siz_y.content = "";
		window_fields.push_back(&edit::siz_y);

		t = new edit::input_field;
		t->x = 810;
		t->y = 140;
		t->size = 15;
		t->color = sf::Color(100, 100, 100, 255);
		t->dynamic = false;
		t->content = "Z=";
		window_fields.push_back(t);

		edit::siz_z.x = 834;
		edit::siz_z.y = 140;
		edit::siz_z.size = 15;
		edit::siz_z.color = sf::Color(100, 100, 100, 255);
		edit::siz_z.dynamic = true;
		edit::siz_z.content = "";
		window_fields.push_back(&edit::siz_z);




		t = new edit::input_field;
		t->x = 810;
		t->y = 160;
		t->size = 15;
		t->color = sf::Color(100, 100, 100, 255);
		t->dynamic = false;
		t->content = "ROTATION:";
		window_fields.push_back(t);

		t = new edit::input_field;
		t->x = 810;
		t->y = 180;
		t->size = 15;
		t->color = sf::Color(100, 100, 100, 255);
		t->dynamic = false;
		t->content = "X=";
		window_fields.push_back(t);

		edit::rot_x.x = 834;
		edit::rot_x.y = 180;
		edit::rot_x.size = 15;
		edit::rot_x.color = sf::Color(100, 100, 100, 255);
		edit::rot_x.dynamic = true;
		edit::rot_x.content = "";
		window_fields.push_back(&edit::rot_x);

		t = new edit::input_field;
		t->x = 810;
		t->y = 200;
		t->size = 15;
		t->color = sf::Color(100, 100, 100, 255);
		t->dynamic = false;
		t->content = "Y=";
		window_fields.push_back(t);

		edit::rot_y.x = 834;
		edit::rot_y.y = 200;
		edit::rot_y.size = 15;
		edit::rot_y.color = sf::Color(100, 100, 100, 255);
		edit::rot_y.dynamic = true;
		edit::rot_y.content = "";
		window_fields.push_back(&edit::rot_y);

		t = new edit::input_field;
		t->x = 810;
		t->y = 220;
		t->size = 15;
		t->color = sf::Color(100, 100, 100, 255);
		t->dynamic = false;
		t->content = "Z=";
		window_fields.push_back(t);

		edit::rot_z.x = 834;
		edit::rot_z.y = 220;
		edit::rot_z.size = 15;
		edit::rot_z.color = sf::Color(100, 100, 100, 255);
		edit::rot_z.dynamic = true;
		edit::rot_z.content = "";
		window_fields.push_back(&edit::rot_z);

		//grid size
		t = new edit::input_field;
		t->x = 810;
		t->y = 240;
		t->size = 15;
		t->color = sf::Color(100, 100, 100, 255);
		t->dynamic = false;
		t->content = "GRID=";
		window_fields.push_back(t);

		edit::on_grd.x = 864;
		edit::on_grd.y = 240;
		edit::on_grd.size = 15;
		edit::on_grd.color = sf::Color(100, 100, 100, 255);
		edit::on_grd.dynamic = true;
		edit::on_grd.is_button = true;
		window_fields.push_back(&edit::on_grd);

		t = new edit::input_field;
		t->x = 810;
		t->y = 260;
		t->size = 15;
		t->color = sf::Color(100, 100, 100, 255);
		t->dynamic = false;
		t->content = "GRID SIZE=";
		window_fields.push_back(t);

		edit::grd_size.x = 904;
		edit::grd_size.y = 260;
		edit::grd_size.size = 15;
		edit::grd_size.color = sf::Color(100, 100, 100, 255);
		edit::grd_size.dynamic = true;
		edit::grd_size.content = "";

		//COLORS

		t = new edit::input_field;
		t->x = 810;
		t->y = 280;
		t->size = 15;
		t->color = sf::Color(100, 100, 100, 255);
		t->dynamic = false;
		t->content = "COLOR:";
		window_fields.push_back(t);

		t = new edit::input_field;
		t->x = 810;
		t->y = 300;
		t->size = 15;
		t->color = sf::Color(100, 100, 100, 255);
		t->dynamic = false;
		t->content = "R=";
		window_fields.push_back(t);

		edit::col_r.x = 834;
		edit::col_r.y = 300;
		edit::col_r.size = 15;
		edit::col_r.color = sf::Color(100, 100, 100, 255);
		edit::col_r.dynamic = true;
		edit::col_r.content = "";
		window_fields.push_back(&edit::col_r);

		t = new edit::input_field;
		t->x = 810;
		t->y = 320;
		t->size = 15;
		t->color = sf::Color(100, 100, 100, 255);
		t->dynamic = false;
		t->content = "G=";
		window_fields.push_back(t);

		edit::col_g.x = 834;
		edit::col_g.y = 320;
		edit::col_g.size = 15;
		edit::col_g.color = sf::Color(100, 100, 100, 255);
		edit::col_g.dynamic = true;
		edit::col_g.content = "";
		window_fields.push_back(&edit::col_g);

		t = new edit::input_field;
		t->x = 810;
		t->y = 340;
		t->size = 15;
		t->color = sf::Color(100, 100, 100, 255);
		t->dynamic = false;
		t->content = "B=";
		window_fields.push_back(t);

		edit::col_b.x = 834;
		edit::col_b.y = 340;
		edit::col_b.size = 15;
		edit::col_b.color = sf::Color(100, 100, 100, 255);
		edit::col_b.dynamic = true;
		edit::col_b.content = "";
		window_fields.push_back(&edit::col_b);

		window_fields.push_back(&edit::grd_size);
		/*t = new edit_field() t->x = 820;
		t->y = 20;
		t->size = 15;
		t->color = sf::Color(200, 200, 200, 255);
		t->dynamic = true;
		t->content = "";
		window_fields.push_back(t);*/
		/*t->x = 820;
		t->y = 40;
		t->size = 15;
		t->color = sf::Color(100, 100, 100, 255);
		t->dynamic = false;
		t->content = "SIZE:";
		window_fields.push_back(t);

		t->x = 820;
		t->y = 60;
		t->size = 15;
		t->color = sf::Color(200, 200, 200, 255);
		t->dynamic = true;
		t->content = "";
		window_fields.push_back(t);

		t->x = 820;
		t->y = 80;
		t->size = 15;
		t->color = sf::Color(100, 100, 100, 255);
		t->dynamic = false;
		t->content = "ROTATION:";
		window_fields.push_back(t);

		t->x = 820;
		t->y = 100;
		t->size = 15;
		t->color = sf::Color(200, 200, 200, 255);
		t->dynamic = true;
		t->content = "";
		window_fields.push_back(t);*/
	}

	void render_loop (sf::RenderWindow * window, camera_pos * cam, std::vector<mesh::Mesh_c *> * meshes, std::vector<mesh::Mesh_c *> * hud_meshes)
	{
		sf::Clock fps_clock;
		fps_clock.restart();
		window->setActive(true);
		set_gl_states(window);
		glViewport(settings::view_port_x, settings::view_port_y - settings::view_port_height + settings::window_height, settings::view_port_width, settings::view_port_height);
		window->setFramerateLimit(30);
		
		std::vector<edit::input_field*> window_fields;
		setup_fields(window_fields);

		while (run_game)
		{
			get_fps(fps_clock);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			camera_pos * renderCam = new camera_pos();
			renderCam->x = cam->x;
			renderCam->y = cam->y;
			renderCam->z = cam->z;
			renderCam->azimuth = cam->azimuth;
			renderCam->pitch = cam->pitch;
			renderCam->roll = cam->roll;
			std::vector<mesh::Mesh_c*> meshes_;
		
			std::vector<mesh::Mesh_c*> hud_meshes_draw;
			/*if (settings::show_hud)
			{
				for (int i = 0; i < hud_meshes->size(); i++)
				{
					hud_meshes_draw.push_back(*(*hud_meshes)[i]);
				}
			}*/
			for (int i = 0; i < meshes->size(); i++)
			{
				if((*meshes)[i]->hud)		
					hud_meshes_draw.push_back((*meshes)[i]);
				//else
				//	meshes_.push_back((*meshes)[i]);
			}


			//for (int i = 0; i < meshes->size(); i++)
			//{
			//	meshes_.push_back(*(*meshes)[i]);
			//}
			//3D STUFF
			//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			for (int i = 0; i <(*meshes).size(); i++)
			{
				//draw_mesh(*(*meshes)[i], *renderCam); //GOOD
				draw_mesh((*meshes)[i], *renderCam); // GOODER
				//draw_mesh(meshes_[i], *renderCam);
				//draw_mesh(meshes_[i], *renderCam);
			}

			
			if (settings::show_hud)
			{
				glClear(GL_DEPTH_BUFFER_BIT);
				for (int i = 0; i < hud_meshes_draw.size(); i++)
				{
					draw_mesh(hud_meshes_draw[i], *renderCam);
				}
			}
			//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		//	render::draw_mesh(mer, cam);

		//	render::draw_mesh(obj, cam);

			if(settings::build_control)
			{
				window->pushGLStates();
				sf::RectangleShape shape(sf::Vector2f(200, 600));
				shape.setFillColor(sf::Color::Color(255, 255, 255, 255));
				shape.setPosition(800, 0);
				window->draw(shape);
				sf::RectangleShape shape2(sf::Vector2f(1000, 200));
				shape2.setFillColor(sf::Color::Color(255, 255, 255, 255));
				shape2.setPosition(0, 600);
				window->draw(shape2);

				sf::Text text(edit::input, font);
				text.setFillColor(sf::Color(255, 255, 255, 255));
				text.setPosition(0, 0);
				text.setCharacterSize(15);

				sf::Text text2(edit::hist, font);
				text2.setFillColor(sf::Color(200, 200, 200, 255));
				text2.setPosition(0, 0);
				text2.setCharacterSize(15);

				render_fields(*window, window_fields, font);

				window->draw(text);
				window->draw(text2);

				window->popGLStates();
			}



		//	sf::Glsl::Vec3 vec(cam->x, cam->y, cam->z);
		//	render::f_shad.setUniform("player", vec);
		//	render::f_shad.setUniform("atten", render::light_attenuation);
			if (settings::static_shader)
			{
				f_shad.setUniform("seed", (float)random());

				double x_ = sin(cam->azimuth * PI / 180.0) * cos(cam->pitch * PI / 180.0);
				double y_ = sin(cam->pitch * PI / 180.0);
				double z_ = cos(cam->azimuth * PI / 180.0) * cos(cam->pitch * PI / 180.0);

				/*
				x_vel = sin(cam->azimuth * PI / 180.0) * cos(cam->pitch * PI / 180.0));
				y_vel = sin(cam->pitch * PI / 180.0));
				z_vel = cos(cam->azimuth * PI / 180.0) * cos(cam->pitch * PI / 180.0));
				*/


				//f_shad.setUniform("dir", sf::Vector3f(x_,y_,z_));
			}
			sf::Shader::bind(&render::f_shad);
			window->display();

		//	std::cout << 1.0/fps << "  \t:  \t"<< fps << "\n";
		}
	}
}
