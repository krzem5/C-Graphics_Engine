#define COBJMACROS
#include <windows.h>
#include <unknwn.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <g_engine.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"dxguid.lib")



HWND _w=NULL;
uint32_t _ww=0;
uint32_t _wh=0;
bool _wf=false;
float _cc[4]={0,0,0,255};
GEngine_init_func _if;
GEngine_render_func _rf;
ID3D11Device* _d3_d=NULL;
ID3D11Device1* _d3_d1=NULL;
ID3D11DeviceContext* _d3_dc=NULL;
ID3D11DeviceContext1* _d3_dc1=NULL;
IDXGISwapChain* _d3_sc=NULL;
IDXGISwapChain1* _d3_sc1=NULL;
ID3D11RenderTargetView* _d3_rt=NULL;
ID3D11Texture2D* _d3_ds=NULL;
ID3D11DepthStencilView* _d3_sv=NULL;
ID3D11VertexShader** _vsl=NULL;
ID3D11PixelShader** _psl=NULL;
ID3D11Buffer** _cbl=NULL;
size_t* _cblsz=NULL;
uint8_t* _cblp=NULL;
uint32_t _vsll=0;
uint32_t _psll=0;
uint32_t _cbll=0;
uint8_t _le=UCHAR_MAX;



float _float(float v){
	if (_le==UCHAR_MAX){
		uint16_t a=1;
		_le=(*((uint8_t*)(&a))==1?1:0);
	}
	if (_le==0){
		return v;
	}
	float o=0;
	uint8_t* op=(uint8_t*)&o;
	uint8_t* vp=(uint8_t*)&v;
	*op=*(vp+3);
	*(op+1)=*(vp+2);
	*(op+2)=*(vp+1);
	*(op+3)=*vp;
	return o;
}



uint16_t _uint16_t(uint16_t v){
	if (_le==UCHAR_MAX){
		uint16_t a=1;
		_le=(*((uint8_t*)(&a))==1?1:0);
	}
	if (_le==0){
		return v;
	}
	uint16_t o=0;
	uint8_t* op=(uint8_t*)&o;
	uint8_t* vp=(uint8_t*)&v;
	*op=*(vp+1);
	*(op+1)=*vp;
	return o;
}



LRESULT CALLBACK _msg_cb(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp){
	switch (msg){
		case WM_KILLFOCUS:
			_wf=false;
			return 0;
		case WM_SETFOCUS:
			_wf=true;
			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			_w=NULL;
			return 0;
	}
	return DefWindowProc(hwnd,msg,wp,lp);
}



struct _MODEL_BONE _load_model_bone(FILE* f){
	struct _MODEL_BONE o;
	o.nml=getc(f);
	o.nm=malloc(o.nml*sizeof(char));
	assert(fread_s((void*)o.nm,o.nml*sizeof(char),1,o.nml*sizeof(char),f)==o.nml*sizeof(char));
	// printf("%s\n",(char*)memcpy(calloc(o.nml+1),o.nm,o.nml));
	assert(fread_s((void*)(&o.l),sizeof(float),1,sizeof(float),f)==sizeof(float));
	o.l=_float(o.l);
	o.cl=getc(f);
	o.d=malloc(6*sizeof(float));
	assert(fread_s((void*)o.d,6*sizeof(float),1,6*sizeof(float),f)==6*sizeof(float));
	for (uint8_t i=0;i<6;i++){
		*(o.d+i)=_float(*(o.d+i))*(i>=3?GENGINE_PIDIV180:1);
	}
	o.tm=malloc(16*sizeof(float));
	assert(fread_s((void*)o.tm,16*sizeof(float),1,16*sizeof(float),f)==16*sizeof(float));
	for (uint8_t i=0;i<16;i++){
		*(o.tm+i)=_float(*(o.tm+i));
	}
	o.dt=malloc(16*sizeof(float));
	assert(fread_s((void*)o.dt,16*sizeof(float),1,16*sizeof(float),f)==16*sizeof(float));
	for (uint8_t i=0;i<16;i++){
		*(o.dt+i)=_float(*(o.dt+i));
	}
	o.wil=(((uint32_t)getc(f))<<24)|(((uint32_t)getc(f))<<16)|(((uint32_t)getc(f))<<8)|((uint32_t)getc(f));
	o.il=malloc(o.wil*sizeof(uint16_t));
	o.wl=malloc(o.wil*sizeof(float));
	assert(fread_s((void*)o.il,o.wil*sizeof(uint16_t),1,o.wil*sizeof(uint16_t),f)==o.wil*sizeof(uint16_t));
	assert(fread_s((void*)o.wl,o.wil*sizeof(float),1,o.wil*sizeof(float),f)==o.wil*sizeof(float));
	for (uint32_t i=0;i<o.wil;i++){
		*(o.il+i)=_uint16_t(*(o.il+i));
		*(o.wl+i)=_float(*(o.wl+i));
	}
	o.c=malloc(o.cl*sizeof(struct _MODEL_BONE));
	for (uint8_t i=0;i<o.cl;i++){
		*(o.c+i)=_load_model_bone(f);
	}
	return o;
}



float _mult_mat_x(float* m,float x,float y,float z){
	//  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
	// 00 01 02 03 10 11 12 13 20 21 22 23 30 31 32 33
	//
	// 00 01 02 03
	// 10 11 12 13
	// 20 21 22 23
	// 30 31 31 33
	//
	// 00*x+01*y+02*z+03
	return (*m)*x+(*(m+1))*y+(*(m+2))*z+(*(m+3));
}



float _mult_mat_y(float* m,float x,float y,float z){
	//  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
	// 00 01 02 03 10 11 12 13 20 21 22 23 30 31 32 33
	//
	// 00 01 02 03
	// 10 11 12 13
	// 20 21 22 23
	// 30 31 31 33
	//
	// 10*x+11*y+12*z+13
	return (*(m+4))*x+(*(m+5))*y+(*(m+6))*z+(*(m+7));
}



float _mult_mat_z(float* m,float x,float y,float z){
	//  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
	// 00 01 02 03 10 11 12 13 20 21 22 23 30 31 32 33
	//
	// 00 01 02 03
	// 10 11 12 13
	// 20 21 22 23
	// 30 31 31 33
	//
	// 20*x+21*y+22*z+23
	return (*(m+8))*x+(*(m+9))*y+(*(m+10))*z+(*(m+11));
}



void _draw_model_bones(struct _MODEL_BONE* b,float* t,uint32_t* vll,float** vl,uint32_t* ill,uint16_t** il){
	float* nt=memcpy(malloc(6*sizeof(float)),t,6*sizeof(float));
	for (uint8_t i=3;i<6;i++){
		*(nt+i)+=*(b->d+i);
	}
	if (b->l!=0){
		(*vll)+=77;
		(*ill)+=30;
		*vl=realloc(*vl,(*vll)*sizeof(float));
		*il=realloc(*il,(*ill)*sizeof(uint16_t));
		float rx=sinf(*(nt+3))*cosf(*(nt+4));
		float ry=cosf(*(nt+3));
		float rz=sinf(*(nt+3))*sinf(*(nt+4));
		float rm=sqrtf(rx*rx+ry*ry+rz*rz);
		rx/=rm;
		ry/=rm;
		rz/=rm;
		float prx=0;
		float pry=0;
		float prz=0;
		if (rx!=0){
			prx=-rz;
			prz=rx;
		}
		else if (ry!=0){
			prx=ry;
			pry=-rx;
		}
		else{
			pry=rz;
			prz=-ry;
		}
		float ax=*b->d+rx;
		float ay=*(b->d+1)+ry;
		float az=*(b->d+2)+rz;
		float am=sqrtf(ax*ax+ay*ay+az*az);
		ax/=am;
		ay/=am;
		az/=am;
		uint32_t i=(*vll)-77;
		for (float a=0;a<GENGINE_2PI;a+=GENGINE_PI/5){
			float sa=sinf(a);
			float ca=cosf(a);
			float nx=prx*(ca+ax*ax*(1-ca))+pry*(ax*ay*(1-ca)-az*sa)+prz*(ax*az*(1-ca)+ay*sa);
			float ny=prx*(ay*ax*(1-ca)+az*sa)+pry*(ca+ay*ay*(1-ca))+prz*(ay*az*(1-ca)-ax*sa);
			float nz=prx*(az*ax*(1-ca)-ay*sa)+pry*(az*ay*(1-ca)+ax*sa)+prz*(ca+az*az*(1-ca));
			float nm=sqrtf(nx*nx+ny*ny+nz*nz);
			nx/=nm;
			ny/=nm;
			nz/=nm;
			*(*vl+i)=*nt+_mult_mat_x(b->tm,nx,ny,nz);
			*(*vl+i+1)=*(nt+1)+_mult_mat_y(b->tm,nx,ny,nz);
			*(*vl+i+2)=*(nt+2)+_mult_mat_z(b->tm,nx,ny,nz);
			*(*vl+i+3)=0.6f;
			*(*vl+i+4)=0.6f;
			*(*vl+i+5)=0.6f;
			*(*vl+i+6)=1;
			i+=7;
		}
		*(*vl+i)=*nt+*b->d+_mult_mat_x(b->tm,rx,ry,rz);
		*(*vl+i+1)=*(nt+1)+*(b->d+1)+_mult_mat_y(b->tm,rx,ry,rz);
		*(*vl+i+2)=*(nt+2)+*(b->d+2)+_mult_mat_z(b->tm,rx,ry,rz);
		*(*vl+i+3)=1;
		*(*vl+i+4)=1;
		*(*vl+i+5)=1;
		*(*vl+i+6)=1;
		i=(*ill)-30;
		uint16_t bv=(uint16_t)((*vll)-77)/7;
		for (uint8_t j=0;j<10;j++){
			*(*il+i)=(uint16_t)(bv+j);
			*(*il+i+1)=(uint16_t)(bv+(j+1)%10);
			*(*il+i+2)=(uint16_t)(bv+10);
			i+=3;
		}
		*nt+=*b->d+_mult_mat_x(b->tm,rx,ry,rz);
		*(nt+1)+=*(b->d+1)+_mult_mat_y(b->tm,rx,ry,rz);
		*(nt+2)+=*(b->d+2)+_mult_mat_z(b->tm,rx,ry,rz);
	}
	for (uint8_t i=0;i<b->cl;i++){
		_draw_model_bones(b->c+i,nt,vll,vl,ill,il);
	}
	free(nt);
}



uint32_t GEngine_window_width(){
	return _ww;
}



uint32_t GEngine_window_height(){
	return _wh;
}



float GEngine_aspect_ratio(){
	return _ww/(float)_wh;
}



Matrix GEngine_identity_matrix(){
	Matrix o=malloc(sizeof(struct _MATRIX));
	o->_00=1;
	o->_01=0;
	o->_02=0;
	o->_03=0;
	o->_10=0;
	o->_11=1;
	o->_12=0;
	o->_13=0;
	o->_20=0;
	o->_21=0;
	o->_22=1;
	o->_23=0;
	o->_30=0;
	o->_31=0;
	o->_32=0;
	o->_33=1;
	return o;
}



Matrix GEngine_y_rotation_matrix(float a){
	Matrix o=malloc(sizeof(struct _MATRIX));
	float c=cosf(a);
	float s=sinf(a);
	o->_00=c;
	o->_01=0;
	o->_02=-s;
	o->_03=0;
	o->_10=0;
	o->_11=1;
	o->_12=0;
	o->_13=0;
	o->_20=s;
	o->_21=0;
	o->_22=c;
	o->_23=0;
	o->_30=0;
	o->_31=0;
	o->_32=0;
	o->_33=1;
	return o;
}



Matrix GEngine_perspective_fov_matrix(float fov,float a,float n,float f){
	Matrix o=malloc(sizeof(struct _MATRIX));
	float fov2=fov/2;
	float cs=cosf(fov2)/sinf(fov2);
	float r=f/(f-n);
	o->_00=cs/a;
	o->_01=0;
	o->_02=0;
	o->_03=0;
	o->_10=0;
	o->_11=cs;
	o->_12=0;
	o->_13=0;
	o->_20=0;
	o->_21=0;
	o->_22=r;
	o->_23=1;
	o->_30=0;
	o->_31=0;
	o->_32=-n*r;
	o->_33=0;
	return o;
}



Matrix GEngine_look_at_matrix(float ex,float ey,float ez,float dx,float dy,float dz,float ux,float uy,float uz){
	float dm=sqrtf(dx*dx+dy*dy+dz*dz);
	dx/=dm;
	dy/=-dm;
	dz/=dm;
	ey=-ey;
	float xx=uy*dz-uz*dy;
	float xy=ux*dx-ux*dz;
	float xz=ux*dy-uy*dx;
	float xm=sqrtf(xx*xx+xy*xy+xz*xz);
	xx/=xm;
	xy/=xm;
	xz/=xm;
	float yx=xy*dz-xz*dy;
	float yy=xz*dx-xx*dz;
	float yz=xx*dy-xy*dx;
	Matrix o=malloc(sizeof(struct _MATRIX));
	o->_00=xx;
	o->_01=yx;
	o->_02=dx;
	o->_03=0;
	o->_10=-xy;
	o->_11=-yy;
	o->_12=-dy;
	o->_13=0;
	o->_20=xz;
	o->_21=yz;
	o->_22=dz;
	o->_23=0;
	o->_30=-(xx*ex+xy*ey+xz*ez);
	o->_31=-(yx*ex+yy*ey+yz*ez);
	o->_32=-(dx*ex+dy*ey+dz*ez);
	o->_33=1;
	return o;
}



Camera GEngine_create_camera(float ms,float rs,float x,float y,float z,float rx,float ry,float rz){
	Camera o=malloc(sizeof(struct _CAMERA));
	o->x=x;
	o->y=y;
	o->z=z;
	o->rx=(rx-90)*GENGINE_PIDIV180;
	o->ry=ry*GENGINE_PIDIV180;
	o->rz=rz*GENGINE_PIDIV180;
	o->ms=ms;
	o->rs=rs;
	o->lock=false;
	o->enabled=false;
	o->_fs=false;
	o->_dx=x;
	o->_dy=y;
	o->_dz=z;
	o->_drx=rx;
	o->_dry=ry;
	o->_drz=rz;
	return o;
}

#define abs(x) ((x)<0?-(x):(x))
#define ease(a,b) (abs((a)-(b))<=1e-3?(b):(a)+0.45f*((b)-(a)))

Matrix GEngine_update_camera(Camera c,float dt){
	if (_wf==false){
		c->_fs=false;
		return NULL;
	}
	if (c->_fs==false&&c->lock==true){
		SetCursorPos(_ww/2,_wh/2);
		c->_fs=true;
		return NULL;
	}
	POINT mp;
	GetCursorPos(&mp);
	if (c->enabled==true){
		c->_drx+=((int32_t)_wh/2-mp.y)*c->rs*dt;
		c->_dry+=((int32_t)_ww/2-mp.x)*c->rs*dt;
		if (c->_drx>89.999f){
			c->_drx=89.999f;
		}
		if (c->_drx<-89.999f){
			c->_drx=-89.999f;
		}
		float drxr=(c->_drx-90)*GENGINE_PIDIV180;
		float dryr=c->_dry*GENGINE_PIDIV180;
		float drzr=c->_drz*GENGINE_PIDIV180;
		float dx=cosf(dryr)*c->ms*dt;
		float dz=sinf(dryr)*c->ms*dt;
		if (GEngine_is_pressed(0x57)){
			c->_dx-=dx;
			c->_dz-=dz;
		}
		if (GEngine_is_pressed(0x53)){
			c->_dx+=dx;
			c->_dz+=dz;
		}
		if (GEngine_is_pressed(0x41)){
			c->_dx+=dz;
			c->_dz-=dx;
		}
		if (GEngine_is_pressed(0x44)){
			c->_dx-=dz;
			c->_dz+=dx;
		}
		if (GEngine_is_pressed(VK_SPACE)){
			c->_dy+=c->ms*dt;
		}
		if (GEngine_is_pressed(VK_LSHIFT)){
			c->_dy-=c->ms*dt;
		}
		c->x=ease(c->x,c->_dx);
		c->y=ease(c->y,c->_dy);
		c->z=ease(c->z,c->_dz);
		c->rx=ease(c->rx,drxr);
		c->ry=ease(c->ry,dryr);
		c->rz=ease(c->rz,drzr);
		// c->x=(c->x-c->_dx>-1e-5&&c->x-c->_dx<1e-5?c->_dx:c->x+0.45f*(c->_dx-c->x));
		// c->y=(c->y-c->_dy>-1e-5&&c->y-c->_dy<1e-5?c->_dy:c->y+0.45f*(c->_dy-c->y));
		// c->z=(c->z-c->_dz>-1e-5&&c->z-c->_dz<1e-5?c->_dz:c->z+0.45f*(c->_dz-c->z));
		// c->rx=(c->rx-drxr>-1e-5&&c->rx-drxr<1e-5?drxr:c->rx+0.45f*(drxr-c->rx));
		// c->ry=(c->ry-dryr>-1e-5&&c->ry-dryr<1e-5?dryr:c->ry+0.45f*(dryr-c->ry));
		// c->rz=(c->rz-drzr>-1e-5&&c->rz-drzr<1e-5?drzr:c->rz+0.45f*(drzr-c->rz));
	}
	if (c->lock==true){
		SetCursorPos(_ww/2,_wh/2);
	}
	return GEngine_look_at_matrix(c->x,c->y,c->z,sinf(c->rx)*cosf(c->ry),cosf(c->rx),sinf(c->rx)*sinf(c->ry),0,1,0);
}



Model GEngine_load_model(const char* fp){
	FILE* f=NULL;
	assert(fopen_s(&f,fp,"rb")==0);
	Model o=malloc(sizeof(struct _MODEL));
	o->bl=(uint8_t)getc(f);
	o->b=malloc(o->bl*sizeof(struct _MODEL_BONE));
	o->dtll=(((uint32_t)getc(f))<<24)|(((uint32_t)getc(f))<<16)|(((uint32_t)getc(f))<<8)|((uint32_t)getc(f));
	o->ill=(((uint32_t)getc(f))<<24)|(((uint32_t)getc(f))<<16)|(((uint32_t)getc(f))<<8)|((uint32_t)getc(f));
	o->dtl=malloc(o->dtll*sizeof(float)*8);
	o->il=malloc(o->ill*sizeof(uint16_t));
	assert(fread_s((void*)o->dtl,o->dtll*sizeof(float)*8,1,o->dtll*sizeof(float)*8,f)==o->dtll*sizeof(float)*8);
	assert(fread_s((void*)o->il,o->ill*sizeof(uint16_t),1,o->ill*sizeof(uint16_t),f)==o->ill*sizeof(uint16_t));
	for (uint32_t i=0;i<o->dtll*8;i++){
		*(o->dtl+i)=_float(*(o->dtl+i));
	}
	for (uint32_t i=0;i<o->ill;i++){
		*(o->il+i)=_uint16_t(*(o->il+i));
	}
	for (uint8_t i=0;i<o->bl;i++){
		*(o->b+i)=_load_model_bone(f);
	}
	fclose(f);
	o->_vb=NULL;
	o->_ib=NULL;
	GEngine_update_model(o);
	return o;
}



void GEngine_update_model(Model m){
	D3D11_BUFFER_DESC bd={
		m->dtll*sizeof(float),
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_VERTEX_BUFFER,
		0,
		0,
		0
	};
	D3D11_SUBRESOURCE_DATA dt={
		m->dtl,
		0,
		0
	};
	HRESULT hr=ID3D11Device_CreateBuffer(_d3_d,&bd,&dt,&m->_vb);
	if (FAILED(hr)){
		printf("ERR4\n");
		return;
	}
	bd.Usage=D3D11_USAGE_DEFAULT;
	bd.ByteWidth=m->ill*sizeof(uint16_t);
	bd.BindFlags=D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags=0;
	dt.pSysMem=m->il;
	hr=ID3D11Device_CreateBuffer(_d3_d,&bd,&dt,&m->_ib);
	if (FAILED(hr)){
		printf("ERR5\n");
		return;
	}
}



void GEngine_draw_model(Model m){
	unsigned int off=0;
	unsigned int st=8*sizeof(float);
	ID3D11DeviceContext_IASetVertexBuffers(_d3_dc,0,1,&m->_vb,&st,&off);
	ID3D11DeviceContext_IASetIndexBuffer(_d3_dc,m->_ib,DXGI_FORMAT_R16_UINT,0);
	ID3D11DeviceContext_IASetPrimitiveTopology(_d3_dc,D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	ID3D11DeviceContext_DrawIndexed(_d3_dc,m->ill,0,0);
}



void GEngine_draw_model_bones(Model m){
	uint32_t vll=0;
	float* vl=NULL;
	uint32_t ill=0;
	uint16_t* il=NULL;
	for (uint8_t i=0;i<m->bl;i++){
		float t[]={0,0,0,0,0,0};
		_draw_model_bones(m->b+i,t,&vll,&vl,&ill,&il);
	}
	D3D11_BUFFER_DESC bd={
		vll*sizeof(float),
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_VERTEX_BUFFER,
		0,
		0,
		0
	};
	D3D11_SUBRESOURCE_DATA dt={
		vl,
		0,
		0
	};
	ID3D11Buffer* vb=NULL;
	HRESULT hr=ID3D11Device_CreateBuffer(_d3_d,&bd,&dt,&vb);
	if (FAILED(hr)){
		printf("ERR4\n");
		return;
	}
	bd.Usage=D3D11_USAGE_DEFAULT;
	bd.ByteWidth=ill*sizeof(uint16_t);
	bd.BindFlags=D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags=0;
	dt.pSysMem=il;
	ID3D11Buffer* ib=NULL;
	hr=ID3D11Device_CreateBuffer(_d3_d,&bd,&dt,&ib);
	if (FAILED(hr)){
		printf("ERR5\n");
		return;
	}
	unsigned int off=0;
	unsigned int st=7*sizeof(float);
	ID3D11DeviceContext_IASetVertexBuffers(_d3_dc,0,1,&vb,&st,&off);
	ID3D11DeviceContext_IASetIndexBuffer(_d3_dc,ib,DXGI_FORMAT_R16_UINT,0);
	ID3D11DeviceContext_IASetPrimitiveTopology(_d3_dc,D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	ID3D11DeviceContext_DrawIndexed(_d3_dc,ill,0,0);
	free(vl);
	free(il);
}



ObjectBuffer GEngine_create_object_buffer(){
	ObjectBuffer o=malloc(sizeof(struct _OBJECT_BUFFER));
	o->vll=0;
	o->vl=NULL;
	o->ill=0;
	o->il=NULL;
	o->st=3*sizeof(float);
	o->_vb=NULL;
	o->_ib=NULL;
	return o;
}



ObjectBuffer GEngine_box_object_buffer(){
	ObjectBuffer o=malloc(sizeof(struct _OBJECT_BUFFER));
	o->vll=56;
	o->vl=malloc(56*sizeof(float));
	*o->vl=-1;
	*(o->vl+1)=1;
	*(o->vl+2)=-1;
	*(o->vl+3)=0;
	*(o->vl+4)=0;
	*(o->vl+5)=1;
	*(o->vl+6)=1;
	*(o->vl+7)=1;
	*(o->vl+8)=1;
	*(o->vl+9)=-1;
	*(o->vl+10)=0;
	*(o->vl+11)=1;
	*(o->vl+12)=0;
	*(o->vl+13)=1;
	*(o->vl+14)=1;
	*(o->vl+15)=1;
	*(o->vl+16)=1;
	*(o->vl+17)=0;
	*(o->vl+18)=1;
	*(o->vl+19)=1;
	*(o->vl+20)=1;
	*(o->vl+21)=-1;
	*(o->vl+22)=1;
	*(o->vl+23)=1;
	*(o->vl+24)=1;
	*(o->vl+25)=0;
	*(o->vl+26)=0;
	*(o->vl+27)=1;
	*(o->vl+28)=-1;
	*(o->vl+29)=-1;
	*(o->vl+30)=-1;
	*(o->vl+31)=1;
	*(o->vl+32)=0;
	*(o->vl+33)=1;
	*(o->vl+34)=1;
	*(o->vl+35)=1;
	*(o->vl+36)=-1;
	*(o->vl+37)=-1;
	*(o->vl+38)=1;
	*(o->vl+39)=1;
	*(o->vl+40)=0;
	*(o->vl+41)=1;
	*(o->vl+42)=1;
	*(o->vl+43)=-1;
	*(o->vl+44)=1;
	*(o->vl+45)=1;
	*(o->vl+46)=1;
	*(o->vl+47)=1;
	*(o->vl+48)=1;
	*(o->vl+49)=-1;
	*(o->vl+50)=-1;
	*(o->vl+51)=1;
	*(o->vl+52)=0;
	*(o->vl+53)=0;
	*(o->vl+54)=0;
	*(o->vl+55)=1;
	o->ill=36;
	o->il=malloc(36*sizeof(uint32_t));
	*o->il=3;
	*(o->il+1)=1;
	*(o->il+2)=0;
	*(o->il+3)=2;
	*(o->il+4)=1;
	*(o->il+5)=3;
	*(o->il+6)=0;
	*(o->il+7)=5;
	*(o->il+8)=4;
	*(o->il+9)=1;
	*(o->il+10)=5;
	*(o->il+11)=0;
	*(o->il+12)=3;
	*(o->il+13)=4;
	*(o->il+14)=7;
	*(o->il+15)=0;
	*(o->il+16)=4;
	*(o->il+17)=3;
	*(o->il+18)=1;
	*(o->il+19)=6;
	*(o->il+20)=5;
	*(o->il+21)=2;
	*(o->il+22)=6;
	*(o->il+23)=1;
	*(o->il+24)=2;
	*(o->il+25)=7;
	*(o->il+26)=6;
	*(o->il+27)=3;
	*(o->il+28)=7;
	*(o->il+29)=2;
	*(o->il+30)=6;
	*(o->il+31)=4;
	*(o->il+32)=5;
	*(o->il+33)=7;
	*(o->il+34)=4;
	*(o->il+35)=6;
	o->st=7*sizeof(float);
	o->_vb=NULL;
	o->_ib=NULL;
	GEngine_update_object_buffer(o);
	return o;
}



void GEngine_update_object_buffer(ObjectBuffer ob){
	D3D11_BUFFER_DESC bd={
		ob->vll*sizeof(float),
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_VERTEX_BUFFER,
		0,
		0,
		0
	};
	D3D11_SUBRESOURCE_DATA dt={
		ob->vl,
		0,
		0
	};
	HRESULT hr=ID3D11Device_CreateBuffer(_d3_d,&bd,&dt,&ob->_vb);
	if (FAILED(hr)){
		printf("ERR4\n");
		return;
	}
	bd.Usage=D3D11_USAGE_DEFAULT;
	bd.ByteWidth=ob->ill*sizeof(uint32_t);
	bd.BindFlags=D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags=0;
	dt.pSysMem=ob->il;
	hr=ID3D11Device_CreateBuffer(_d3_d,&bd,&dt,&ob->_ib);
	if (FAILED(hr)){
		printf("ERR5\n");
		return;
	}
}



void GEngine_draw_object_buffer(ObjectBuffer ob){
	unsigned int off=0;
	ID3D11DeviceContext_IASetVertexBuffers(_d3_dc,0,1,&ob->_vb,&ob->st,&off);
	ID3D11DeviceContext_IASetIndexBuffer(_d3_dc,ob->_ib,DXGI_FORMAT_R32_UINT,0);
	ID3D11DeviceContext_IASetPrimitiveTopology(_d3_dc,D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	ID3D11DeviceContext_DrawIndexed(_d3_dc,ob->ill,0,0);
}



void GEngine_set_init_func(GEngine_init_func f){
	_if=f;
}



void GEngine_set_render_func(GEngine_render_func f){
	_rf=f;
}



void GEngine_create(const wchar_t* nm){
	WNDCLASSEXW wc={
		sizeof(WNDCLASSEX),
		CS_DBLCLKS|CS_OWNDC|CS_HREDRAW|CS_VREDRAW,
		&_msg_cb,
		0,
		0,
		GetModuleHandle(0),
		0,
		0,
		0,
		0,
		nm,
		0
	};
	RegisterClassExW(&wc);
	assert(_w==NULL);
	_w=CreateWindowExW(wc.style,wc.lpszClassName,nm,WS_VISIBLE,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,NULL,NULL,GetModuleHandle(0),NULL);
	SetWindowPos(_w,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	SetWindowLongPtr(_w,GWL_STYLE,WS_VISIBLE);
	SetWindowLongPtr(_w,GWL_EXSTYLE,0);
	MONITORINFO mf={
		sizeof(mf)
	};
	GetMonitorInfo(MonitorFromWindow(_w,MONITOR_DEFAULTTONEAREST),&mf);
	_ww=mf.rcMonitor.right-mf.rcMonitor.left;
	_wh=mf.rcMonitor.bottom-mf.rcMonitor.top;
	SetWindowPos(_w,NULL,mf.rcMonitor.left,mf.rcMonitor.top,_ww,_wh,SWP_NOZORDER|SWP_NOACTIVATE|SWP_FRAMECHANGED);
	SetCapture(_w);
	ShowWindow(_w,SW_SHOW);
	UpdateWindow(_w);
	SetCursor(NULL);
	LARGE_INTEGER tf;
	LARGE_INTEGER lt={0};
	QueryPerformanceFrequency(&tf);
	MSG msg={0};
	LARGE_INTEGER c;
	while (msg.message!=WM_QUIT&&_w!=NULL){
		if (PeekMessage(&msg,_w,0,0,PM_REMOVE)>0){
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		QueryPerformanceCounter(&c);
		if (lt.QuadPart==0){
			D3D_FEATURE_LEVEL fl;
			D3D_FEATURE_LEVEL sfl[]={
				D3D_FEATURE_LEVEL_11_1
			};
			D3D11CreateDevice(NULL,D3D_DRIVER_TYPE_HARDWARE,0,D3D11_CREATE_DEVICE_DEBUG,sfl,1,D3D11_SDK_VERSION,&_d3_d,&fl,&_d3_dc);
			IDXGIFactory1* dxgi_f=NULL;
			IDXGIDevice* dxgi_d=NULL;
			HRESULT hr=ID3D11Device_QueryInterface(_d3_d,&IID_IDXGIDevice,(void**)&dxgi_d);
			if (SUCCEEDED(hr)){
				IDXGIAdapter* dxgi_a=NULL;
				hr=IDXGIDevice_GetAdapter(dxgi_d,&dxgi_a);
				if (SUCCEEDED(hr)){
					hr=IDXGIAdapter_GetParent(dxgi_a,&IID_IDXGIFactory1,(void**)&dxgi_f);
					IDXGIAdapter_Release(dxgi_a);
				}
				IDXGIDevice_Release(dxgi_d);
			}
			IDXGIFactory2* dxgi_f2=NULL;
			hr=IDXGIFactory1_QueryInterface(dxgi_f,&IID_IDXGIFactory2,(void**)&dxgi_f2);
			assert(hr==S_OK);
			if (dxgi_f2!=NULL){
				hr=ID3D11Device_QueryInterface(_d3_d,&IID_ID3D11Device1,(void**)&_d3_d1);
				if (SUCCEEDED(hr)){
					ID3D11DeviceContext_QueryInterface(_d3_dc,&IID_ID3D11DeviceContext1,(void**)&_d3_dc1);
				}
				DXGI_SWAP_CHAIN_DESC1 sc_d={
					_ww,
					_wh,
					DXGI_FORMAT_R8G8B8A8_UNORM,
					false,
					{
						1,
						0
					},
					DXGI_USAGE_RENDER_TARGET_OUTPUT,
					1,
					DXGI_SCALING_STRETCH,
					DXGI_SWAP_EFFECT_DISCARD,
					DXGI_ALPHA_MODE_UNSPECIFIED,
					0
				};
				IUnknown* du;
				ID3D11Device_QueryInterface(_d3_d,&IID_IUnknown,(void**)&du);
				hr=IDXGIFactory2_CreateSwapChainForHwnd(dxgi_f2,du,_w,&sc_d,NULL,NULL,&_d3_sc1);
				IUnknown_Release(du);
				if (SUCCEEDED(hr)){
					hr=IDXGISwapChain1_QueryInterface(_d3_sc1,&IID_IDXGISwapChain,(void**)&_d3_sc);
					assert(!FAILED(hr));
				}
				IDXGIFactory2_Release(dxgi_f2);
			}
			else{
				DXGI_SWAP_CHAIN_DESC sc_d={
					{
						_ww,
						_wh,
						{
							60,
							1
						},
						DXGI_FORMAT_UNKNOWN,
						DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
						DXGI_MODE_SCALING_UNSPECIFIED
					},
					{
						1,
						0
					},
					DXGI_USAGE_RENDER_TARGET_OUTPUT,
					1,
					_w,
					true,
					DXGI_SWAP_EFFECT_DISCARD,
					0
				};
				IUnknown* du;
				ID3D11Device_QueryInterface(_d3_d,&IID_IUnknown,(void**)&du);
				IDXGIFactory2_CreateSwapChain(dxgi_f,du,&sc_d,&_d3_sc);
				IUnknown_Release(du);
			}
			IDXGIFactory2_MakeWindowAssociation(dxgi_f,_w,DXGI_MWA_NO_ALT_ENTER);
			IDXGIFactory2_Release(dxgi_f);
			ID3D11Texture2D* bb=NULL;
			assert(_d3_sc!=NULL);
			hr=IDXGISwapChain_GetBuffer(_d3_sc,0,&IID_ID3D11Texture2D,(void**)&bb);
			if (FAILED(hr)){
				printf("ERR6\n");
				break;
			}
			ID3D11Resource* bbr;
			ID3D11Texture2D_QueryInterface(bb,&IID_ID3D11Resource,(void**)&bbr);
			hr=ID3D11Device_CreateRenderTargetView(_d3_d,bbr,NULL,&_d3_rt);
			ID3D11Resource_Release(bbr);
			if (FAILED(hr)){
				printf("ERR7\n");
				break;
			}
			ID3D11Texture2D_Release(bb);
			D3D11_TEXTURE2D_DESC dd={
				_ww,
				_wh,
				1,
				1,
				DXGI_FORMAT_D24_UNORM_S8_UINT,
				{
					1,
					0
				},
				D3D11_USAGE_DEFAULT,
				D3D11_BIND_DEPTH_STENCIL,
				0,
				0
			};
			hr=ID3D11Device_CreateTexture2D(_d3_d,&dd,NULL,&_d3_ds);
			if (FAILED(hr)){
				printf("Error creating Depth Stensil Texture\n");
				break;
			}
			D3D11_DEPTH_STENCIL_VIEW_DESC d_dsv={
				dd.Format,
				D3D11_DSV_DIMENSION_TEXTURE2D,
				0
			};
			d_dsv.Texture2D.MipSlice=0;
			ID3D11Resource* dsr;
			ID3D11Texture2D_QueryInterface(_d3_ds,&IID_ID3D11Resource,(void**)&dsr);
			hr=ID3D11Device_CreateDepthStencilView(_d3_d,dsr,&d_dsv,&_d3_sv);
			ID3D11Resource_Release(dsr);
			if (FAILED(hr)){
				printf("ERR9\n");
				break;
			}
			ID3D11DeviceContext_OMSetRenderTargets(_d3_dc,1,&_d3_rt,_d3_sv);
			D3D11_VIEWPORT vp={
				0,
				0,
				(float)_ww,
				(float)_wh,
				0,
				1
			};
			ID3D11DeviceContext_RSSetViewports(_d3_dc,1,&vp);
			_if();
			_rf(0);
		}
		else{
			ID3D11DeviceContext_ClearRenderTargetView(_d3_dc,_d3_rt,_cc);
			ID3D11DeviceContext_ClearDepthStencilView(_d3_dc,_d3_sv,D3D11_CLEAR_DEPTH,1.0f,0);
			_rf((double)((c.QuadPart-lt.QuadPart)*1000000/tf.QuadPart));
		}
		if (_w==NULL){
			break;
		}
		IDXGISwapChain_Present(_d3_sc,true,DXGI_PRESENT_DO_NOT_WAIT);
		lt=c;
	}
}



bool GEngine_is_pressed(uint32_t k){
	return (GetKeyState(k)<0?true:false);
}



void GEngine_set_color(float r,float g,float b,float a){
	*_cc=r;
	*(_cc+1)=g;
	*(_cc+2)=b;
	*(_cc+3)=a;
}



VertexShader GEngine_load_vertex_shader(const BYTE* dt,size_t ln,VS_INPUT_LAYOUT* il,uint16_t ill){
	_vsll++;
	_vsl=realloc(_vsl,_vsll*sizeof(ID3D11VertexShader*));
	ID3D11VertexShader* vsp=NULL;
	HRESULT hr=ID3D11Device_CreateVertexShader(_d3_d,dt,ln,NULL,&vsp);
	*(_vsl+_vsll-1)=vsp;
	if (FAILED(hr)){
		printf("Error creating VS\n");
		return -1;
	}
	ID3D11InputLayout* vl=NULL;
	hr=ID3D11Device_CreateInputLayout(_d3_d,il,ill,dt,ln,&vl);
	if (FAILED(hr)){
		printf("Error creating VS Input Layout\n");
		assert(0);
		return -1;
	}
	ID3D11DeviceContext_IASetInputLayout(_d3_dc,vl);
	return _vsll-1;
}



PixelShader GEngine_load_pixel_shader(const BYTE* dt,size_t ln){
	_psll++;
	_psl=realloc(_psl,_psll*sizeof(ID3D11PixelShader*));
	ID3D11PixelShader* psp=NULL;
	HRESULT hr=ID3D11Device_CreatePixelShader(_d3_d,dt,ln,NULL,&psp);
	*(_psl+_psll-1)=psp;
	if (FAILED(hr)){
		printf("Error creating PS\n");
		return -1;
	}
	return _psll-1;
}



ConstantBuffer GEngine_create_constant_buffer(uint16_t l){
	_cbll++;
	_cbl=realloc(_cbl,_cbll*sizeof(ID3D11Buffer*));
	_cblsz=realloc(_cblsz,_cbll*sizeof(size_t));
	_cblp=realloc(_cblp,_cbll*sizeof(uint8_t));
	if (l%16!=0){
		l+=16-l%16;
		*(_cblp+_cbll-1)=16-l%16;
	}
	else{
		*(_cblp+_cbll-1)=0;
	}
	*(_cblsz+_cbll-1)=l;
	D3D11_BUFFER_DESC bd={
		l,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_CONSTANT_BUFFER,
		0,
		0,
		0
	};
	ID3D11Buffer* cbp=NULL;
	HRESULT hr=ID3D11Device_CreateBuffer(_d3_d,&bd,NULL,&cbp);
	*(_cbl+_cbll-1)=cbp;
	if (FAILED(hr)){
		printf("Error Creating Constant Buffer: %x\n",hr);
		return -1;
	}
	return _cbll-1;
}



void GEngine_update_constant_buffer(ConstantBuffer cb,void* dt){
	ID3D11Resource* cbbr=NULL;
	ID3D11Buffer_QueryInterface(*(_cbl+cb),&IID_ID3D11Resource,(void**)&cbbr);
	if (*(_cblp+cb)==0){
		ID3D11DeviceContext_UpdateSubresource(_d3_dc,cbbr,0,NULL,dt,0,0);
	}
	else{
		void* t=malloc(*(_cblsz+cb));
		memcpy(t,dt,*(_cblsz+cb)-*(_cblp+cb));
		memset((char*)t+*(_cblsz+cb)-*(_cblp+cb),0,*(_cblp+cb));
		ID3D11DeviceContext_UpdateSubresource(_d3_dc,cbbr,0,NULL,t,0,0);
		free(t);
	}
	ID3D11Resource_Release(cbbr);
}



void GEngine_set_shader_data(struct SHADER_DATA* dt){
	for (size_t i=0;i<sizeof(*dt)/sizeof(struct SHADER_DATA);i++){
		if ((dt+i)->t==SHADER_DATA_TYPE_CONSTANT_BUFFER){
			if (((dt+i)->f&SHADER_DATA_FLAG_VS)!=0){
				ID3D11DeviceContext_VSSetConstantBuffers(_d3_dc,(dt+i)->r,1,_cbl+(dt+i)->id);
			}
			if (((dt+i)->f&SHADER_DATA_FLAG_PS)!=0){
				ID3D11DeviceContext_PSSetConstantBuffers(_d3_dc,(dt+i)->r,1,_cbl+(dt+i)->id);
			}
		}
		else{
			assert(0);
		}
	}
}



void GEngine_use_vertex_shader(VertexShader vs){
	ID3D11DeviceContext_VSSetShader(_d3_dc,*(_vsl+vs),NULL,0);
}



void GEngine_use_pixel_shader(PixelShader ps){
	ID3D11DeviceContext_PSSetShader(_d3_dc,*(_psl+ps),NULL,0);
}



void GEngine_close(void){
	if (_vsl!=NULL){
		free(_vsl);
	}
	if (_psl!=NULL){
		free(_psl);
	}
	if (_cbl!=NULL){
		for (size_t i=0;i<_cbll;i++){
			ID3D11Buffer_Release(*(_cbl+i));
		}
		free(_cbl);
		free(_cblsz);
		free(_cblp);
	}
	if (_d3_d!=NULL){
		ID3D11Device_Release(_d3_d);
		_d3_d=NULL;
	}
	if (_d3_d1!=NULL){
		ID3D11Device1_Release(_d3_d1);
		_d3_d1=NULL;
	}
	if (_d3_dc!=NULL){
		ID3D11DeviceContext_Release(_d3_dc);
		_d3_dc=NULL;
	}
	if (_d3_dc1!=NULL){
		ID3D11DeviceContext1_Release(_d3_dc1);
		_d3_dc1=NULL;
	}
	if (_d3_sc!=NULL){
		IDXGISwapChain_Release(_d3_sc);
		_d3_sc=NULL;
	}
	if (_d3_sc1!=NULL){
		IDXGISwapChain_Release(_d3_sc1);
		_d3_sc1=NULL;
	}
	if (_d3_rt!=NULL){
		ID3D11RenderTargetView_Release(_d3_rt);
		_d3_rt=NULL;
	}
	if (_d3_ds!=NULL){
		ID3D11DepthStencilView_Release(_d3_ds);
		_d3_ds=NULL;
	}
	if (_d3_sv!=NULL){
		ID3D11DepthStencilView_Release(_d3_sv);
		_d3_sv=NULL;
	}
	if (_w!=NULL){
		DestroyWindow(_w);
		_w=NULL;
	}
}
