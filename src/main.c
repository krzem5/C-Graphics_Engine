#include <g_engine.h>
#include <stdio.h>
#include <stdlib.h>
#include <phong_vertex.h>
#include <phong_pixel.h>
#include <phong_vertex_tex.h>
#include <phong_pixel_tex.h>



struct VertexShaderInput{
	RawMatrix wm;
	RawMatrix cm;
	RawMatrix pm;
	RawVector cp;
	RawVector lp;
};



VS_INPUT_LAYOUT vs_inp[]={
	{
		"POSITION",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		0,
		D3D11_INPUT_PER_VERTEX_DATA,
		0
	},
	{
		"COLOR",
		0,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		0,
		12,
		D3D11_INPUT_PER_VERTEX_DATA,
		0
	}
};



VS_INPUT_LAYOUT vs_inp_tex[]={
	{
		"POSITION",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		0,
		D3D11_INPUT_PER_VERTEX_DATA,
		0
	},
	{
		"NORMAL",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		12,
		D3D11_INPUT_PER_VERTEX_DATA,
		0
	},
	{
		"TEXCOORD",
		0,
		DXGI_FORMAT_R32G32_FLOAT,
		0,
		24,
		D3D11_INPUT_PER_VERTEX_DATA,
		0
	}
};



struct SHADER_DATA sdt[1]={
	{
		SHADER_DATA_TYPE_CONSTANT_BUFFER,
		0,
		0,
		SHADER_DATA_FLAG_VS|SHADER_DATA_FLAG_PS
	}
};



VertexShader vs;
VertexShader vs_tex;
PixelShader ps;
PixelShader ps_tex;
ConstantBuffer cb;
Matrix wm;
Matrix pm;
Camera c;
ObjectBuffer ob;
Model m;
Animation ia;
Animation ftra;
Animator an;



void init(void){
	vs=GEngine_load_vertex_shader(g_vertex_shader,sizeof(g_vertex_shader),vs_inp,sizeof(vs_inp)/sizeof(VS_INPUT_LAYOUT));
	vs_tex=GEngine_load_vertex_shader(g_vertex_shader_tex,sizeof(g_vertex_shader_tex),vs_inp_tex,sizeof(vs_inp_tex)/sizeof(VS_INPUT_LAYOUT));
	ps=GEngine_load_pixel_shader(g_pixel_shader,sizeof(g_pixel_shader));
	ps_tex=GEngine_load_pixel_shader(g_pixel_shader_tex,sizeof(g_pixel_shader_tex));
	cb=GEngine_create_constant_buffer(sizeof(struct VertexShaderInput));
	sdt->id=cb;
	c=GEngine_create_camera(50,5,0,5,0,-90,0,0);
	c->lock=true;
	c->enabled=true;
	GEngine_set_shader_data(sdt);
	wm=GEngine_identity_matrix();
	pm=GEngine_perspective_fov_matrix(GENGINE_PIDIV2,GEngine_aspect_ratio(),0.01f,1000);
	ob=GEngine_box_object_buffer();
	m=GEngine_load_model("rsrc\\ybot.mdl",1);
	ia=GEngine_load_animation("rsrc\\idle.anm",m);
	ftra=GEngine_load_animation("rsrc\\falling to roll.anm",m);
	an=GEngine_create_animator(m);
}



void render(double dt){
	static double t=0;
	t+=dt*1e-6;
	Matrix cm=GEngine_update_camera(c,(float)(dt*1e-6));
	GEngine_set_color(0,0,0,255);
	if (cm==NULL){
		return;
	}
	static bool ap=false;
	GEngine_update_animator(an,(float)(dt*1e-6));
	if (GEngine_animation_finished(an)==true){
		ap=false;
		GEngine_set_animation(an,ia);
	}
	if (GEngine_is_pressed(0x26)==true&&ap==false){
		GEngine_set_animation(an,ftra);
		ap=true;
	}
	struct VertexShaderInput cb1={
		GEngine_as_raw_matrix(wm),
		GEngine_as_raw_matrix(cm),
		GEngine_as_raw_matrix(pm),
		GEngine_pos_from_camera(c),
		GEngine_raw_vector(-1000,-1000,-1000,1)
	};
	GEngine_update_constant_buffer(cb,&cb1);
	GEngine_use_vertex_shader(vs);
	GEngine_use_pixel_shader(ps);
	GEngine_draw_object_buffer(ob);
	GEngine_draw_model_bones(m,0);
	GEngine_use_vertex_shader(vs_tex);
	GEngine_use_pixel_shader(ps_tex);
	GEngine_draw_model(m,0);
	GEngine_draw_model(m,1);
	if (GEngine_is_pressed(0x1b)==true){
		GEngine_close();
	}
	GEngine_free_matrix(cm);
}



int main(int argc,const char** argv){
	GEngine_set_init_func(init);
	GEngine_set_render_func(render);
	atexit(GEngine_close);
	GEngine_create(L"HEY!");
	return 0;
}
