#include <g_engine.h>
#include <stdio.h>
#include <stdlib.h>



struct VertexShaderInput{
	RawMatrix wm;
	RawMatrix cm;
	RawMatrix pm;
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



struct SHADER_DATA sdt[1]={
	{
		SHADER_DATA_TYPE_CONSTANT_BUFFER,
		0,
		0,
		SHADER_DATA_FLAG_VS|SHADER_DATA_FLAG_PS
	}
};



VertexShader vs;
PixelShader ps;
ConstantBuffer cb;
Matrix wm;
Matrix pm;
Camera c;
ObjectBuffer ob;



void init(void){
	vs=GEngine_load_vertex_shader(L"rsrc/phong.hlsl","vertex_shader","vs_4_0",vs_inp,2);
	ps=GEngine_load_pixel_shader(L"rsrc/phong.hlsl","pixel_shader","ps_4_0");
	cb=GEngine_create_constant_buffer(sizeof(struct VertexShaderInput));
	sdt->id=cb;
	c=GEngine_create_camera(5,5,0,5,0,-90,0,0);
	c->lock=true;
	c->enabled=true;
	GEngine_set_shader_data(sdt);
	wm=GEngine_identity_matrix();
	pm=GEngine_perspective_fov_matrix(GENGINE_PIDIV2,GEngine_aspect_ratio(),0.01f,1000);
	ob=GEngine_box_object_buffer();
}



void render(double dt){
	static double t=0;
	t+=dt*1e-6;
	Matrix cm=GEngine_update_camera(c,(float)(dt*1e-6));
	GEngine_set_color(0,0,0,255);
	if (cm==NULL){
		return;
	}
	wm=GEngine_y_rotation_matrix((float)(t/1000));
	struct VertexShaderInput cb1={
		GEngine_as_raw_matrix(wm),
		GEngine_as_raw_matrix(cm),
		GEngine_as_raw_matrix(pm)
	};
	GEngine_update_constant_buffer(cb,&cb1);
	GEngine_use_vertex_shader(vs);
	GEngine_use_pixel_shader(ps);
	GEngine_draw_object_buffer(ob);
	if (GEngine_is_pressed(0x1b)==true){
		GEngine_close();
	}
}



int main(int argc,const char** argv){
	GEngine_set_init_func(init);
	GEngine_set_render_func(render);
	atexit(GEngine_close);
	GEngine_create(L"HEY!");
	return 0;
}
