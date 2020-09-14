#ifndef GENGINE_H
#define GENGINE_H
#include <d3d11_1.h>
#include <stdint.h>
#include <limits.h>
#include <signal.h>



#define bool _Bool
#define false 0
#define true 1
#define GENGINE_2PI ((float)6.283185307f)
#define GENGINE_PI ((float)3.141592654f)
#define GENGINE_PIDIV2 ((float)1.570796327f)
#define GENGINE_PIDIV4 ((float)0.785398163f)
#define GENGINE_1DIVPI ((float)0.318309886f)
#define GENGINE_1DIV2PI ((float)0.159154943f)
#ifdef NULL
#undef NULL
#endif
#define NULL ((void*)0)
#define assert(x) \
	do{ \
		if (!(x)){ \
			printf("%s:%i (%s): %s: Assertion Failed\n",__FILE__,__LINE__,__func__,#x); \
			raise(SIGABRT); \
		} \
	} while (0)
#define GEngine_free_matrix(m) ((m!=NULL?free(m):NULL)



struct _VECTOR;
struct _MATRIX;
typedef uint32_t VertexShader;
typedef uint32_t PixelShader;
typedef uint32_t ConstantBuffer;
typedef struct _VECTOR* Vector;
typedef struct _MATRIX* Matrix;
typedef D3D11_INPUT_ELEMENT_DESC VS_INPUT_LAYOUT;
typedef void (*GEngine_init_func)(void);
typedef void (*GEngine_render_func)(double dt);



enum SHADER_DATA_TYPE{
	SHADER_DATA_TYPE_CONSTANT_BUFFER=0x01,
	SHADER_DATA_TYPE_TEXTURE=0x02,
	SHADER_DATA_TYPE_SAMPLER_STATE=0x03
};



enum SHADER_DATA_FLAGS{
	SHADER_DATA_FLAG_VS=0x01,
	SHADER_DATA_FLAG_PS=0x02
};



struct _VECTOR{
	float x;
	float y;
	float z;
	float w;
};



struct _MATRIX{
	float _00;
	float _01;
	float _02;
	float _03;
	float _10;
	float _11;
	float _12;
	float _13;
	float _20;
	float _21;
	float _22;
	float _23;
	float _30;
	float _31;
	float _32;
	float _33;
};



struct SHADER_DATA{
	enum SHADER_DATA_TYPE t;
	ConstantBuffer id;
	uint16_t r;
	uint8_t f;
};



float GEngine_aspect_ratio();



Matrix GEngine_perspective_fov_matrix(float fov,float a,float n,float f);



void GEngine_set_init_func(GEngine_init_func f);



void GEngine_set_render_func(GEngine_render_func f);



void GEngine_create(const wchar_t* nm);



bool GEngine_is_pressed(uint32_t k);



void GEngine_set_color(float r,float g,float b,float a);



VertexShader GEngine_load_vertex_shader(const wchar_t* fp,const char* e,const char* v,VS_INPUT_LAYOUT* il,uint16_t ill);



PixelShader GEngine_load_pixel_shader(const wchar_t* fp,const char* e,const char* v);



ConstantBuffer GEngine_create_constant_buffer(uint16_t l);



void GEngine_set_shader_data(struct SHADER_DATA* dt);



void GEngine_close(void);



#endif
