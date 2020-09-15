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
#define GENGINE_PIDIV180 ((float)0.017453293f)
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
#define GEngine_raw_vector(x,y,z,w) ((RawVector){(x),(y),(z),(w)})
#define GEngine_raw_identity_matrix() ((RawMatrix){1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1})
#define GEngine_raw_matrix(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p) ((RawMatrix){(a),(b),(c),(d),(e),(f),(g),(h),(i),(j),(k),(l),(m),(n),(o),(p)})
#define GEngine_as_raw_matrix(m) (*((RawMatrix*)(m)))
#define GEngine_free_matrix(m) ((m)!=NULL?(free((m)),NULL):NULL)



struct _VECTOR;
struct _MATRIX;
struct _CAMERA;
struct _OBJECT_BUFFER;



typedef uint32_t VertexShader;
typedef uint32_t PixelShader;
typedef uint32_t ConstantBuffer;
typedef struct _VECTOR* Vector;
typedef struct _VECTOR RawVector;
typedef struct _MATRIX* Matrix;
typedef struct _MATRIX RawMatrix;
typedef struct _CAMERA* Camera;
typedef struct _OBJECT_BUFFER* ObjectBuffer;
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



struct _CAMERA{
	float x;
	float y;
	float z;
	float rx;
	float ry;
	float rz;
	float ms;
	float rs;
	bool lock;
	bool enabled;
	bool _fs;
	float _dx;
	float _dy;
	float _dz;
	float _drx;
	float _dry;
	float _drz;
};



struct _OBJECT_BUFFER{
	uint32_t vll;
	float* vl;
	uint32_t ill;
	uint32_t* il;
	uint32_t st;
	ID3D11Buffer* _vb;
	ID3D11Buffer* _ib;
};



struct SHADER_DATA{
	enum SHADER_DATA_TYPE t;
	ConstantBuffer id;
	uint16_t r;
	uint8_t f;
};



uint32_t GEngine_window_width();



uint32_t GEngine_window_height();



float GEngine_aspect_ratio();



Matrix GEngine_identity_matrix();



Matrix GEngine_y_rotation_matrix(float a);



Matrix GEngine_perspective_fov_matrix(float fov,float a,float n,float f);



Camera GEngine_create_camera(float ms,float rs,float x,float y,float z,float rx,float ry,float rz);



Matrix GEngine_update_camera(Camera c,float dt);



ObjectBuffer GEngine_create_object_buffer();



ObjectBuffer GEngine_box_object_buffer();



void GEngine_update_object_buffer(ObjectBuffer ob);



void GEngine_draw_object_buffer(ObjectBuffer ob);



void GEngine_set_init_func(GEngine_init_func f);



void GEngine_set_render_func(GEngine_render_func f);



void GEngine_create(const wchar_t* nm);



bool GEngine_is_pressed(uint32_t k);



void GEngine_set_color(float r,float g,float b,float a);



VertexShader GEngine_load_vertex_shader(const wchar_t* fp,const char* e,const char* v,VS_INPUT_LAYOUT* il,uint16_t ill);



PixelShader GEngine_load_pixel_shader(const wchar_t* fp,const char* e,const char* v);



ConstantBuffer GEngine_create_constant_buffer(uint16_t l);



void GEngine_update_constant_buffer(ConstantBuffer cb,void* dt);



void GEngine_set_shader_data(struct SHADER_DATA* dt);



void GEngine_use_vertex_shader(VertexShader vs);



void GEngine_use_pixel_shader(PixelShader ps);



void GEngine_close(void);



#endif
