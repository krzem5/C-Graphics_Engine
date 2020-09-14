#include <g_engine.h>
#include <stdio.h>
#include <stdlib.h>



struct VertexShaderInput{
	Matrix wm;
	Matrix vm;
	Matrix pm;
	uint8_t lc;
	uint8_t _[7];
	struct VertexShaderInputLight{
		Vector p;
		Vector c;
	} ll[8];
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
		"TEXCOORD",
		0,
		DXGI_FORMAT_R32G32_FLOAT,
		0,
		12,
		D3D11_INPUT_PER_VERTEX_DATA,
		0
	},
	{
		"NORMAL",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		20,
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
Matrix pm;



void init(void){
	vs=GEngine_load_vertex_shader(L"rsrc/phong.hlsl","vertex_shader","vs_4_0",vs_inp,3);
	ps=GEngine_load_pixel_shader(L"rsrc/phong.hlsl","pixel_shader","ps_4_0");
	cb=GEngine_create_constant_buffer(sizeof(struct VertexShaderInput));
	sdt->id=cb;
	GEngine_set_shader_data(sdt);
	pm=GEngine_perspective_fov_matrix(GENGINE_PIDIV4,GEngine_aspect_ratio(),0.01f,1000);
}



void render(double dt){
	static double t=0;
	t+=dt;
	GEngine_set_color((float)(((uint8_t)(t/10))%255)/255,0,0,255);
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
