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
float _cc[4]={0,0,0,255};
uint32_t _ww=0;
uint32_t _wh=0;
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
ID3D11VertexShader* _vsl=NULL;
ID3D11PixelShader* _psl=NULL;
ID3D11Buffer* _cbl=NULL;
uint32_t _vsll=0;
uint32_t _psll=0;
uint32_t _cbll=0;



LRESULT CALLBACK _msg_cb(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp){
	switch (msg){
		case WM_DESTROY:
			PostQuitMessage(0);
			_w=NULL;
			return 0;
	}
	return DefWindowProc(hwnd,msg,wp,lp);
}



float GEngine_aspect_ratio(){
	return _ww/(float)_wh;
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
	o->_22=f/(f-n);
	o->_23=0;
	o->_30=0;
	o->_31=0;
	o->_32=0;
	o->_33=-n*f/(f-n);
	return o;
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
			IUnknown_Release(bbr);
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
			IUnknown_Release(dsr);
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
			_rf((double)((c.QuadPart-lt.QuadPart)*1000000/tf.QuadPart)/1000);
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



VertexShader GEngine_load_vertex_shader(const wchar_t* fp,const char* e,const char* v,VS_INPUT_LAYOUT* il,uint16_t ill){
	_vsll++;
	_vsl=realloc(_vsl,_vsll*sizeof(ID3D11VertexShader));
	ID3DBlob* b=NULL;
	ID3DBlob* err=NULL;
	HRESULT hr=D3DCompileFromFile(fp,NULL,NULL,e,v,D3DCOMPILE_ENABLE_STRICTNESS,0,&b,&err);
	if (FAILED(hr)){
		printf("Error loading VertexShader\n");
		if (err!=NULL){
			ID3D10Blob_Release(err);
		}
		return -1;
	}
	ID3D11VertexShader* vsp=NULL;
	hr=ID3D11Device_CreateVertexShader(_d3_d,ID3D10Blob_GetBufferPointer(b),ID3D10Blob_GetBufferSize(b),NULL,&vsp);
	for (size_t i=0;i<sizeof(ID3D11VertexShader);i++){
		*((char*)(_vsl+_vsll-1)+i)=*((char*)vsp+i);
	}
	if (FAILED(hr)){
		printf("Error creating VS\n");
		return -1;
	}
	ID3D11InputLayout* vl=NULL;
	hr=ID3D11Device_CreateInputLayout(_d3_d,il,ill,ID3D10Blob_GetBufferPointer(b),ID3D10Blob_GetBufferSize(b),&vl);
	if (FAILED(hr)){
		printf("Error creating VS Input Layout\n");
		return -1;
	}
	ID3D11DeviceContext_IASetInputLayout(_d3_dc,vl);
	ID3D10Blob_Release(b);
	return _vsll-1;
}



PixelShader GEngine_load_pixel_shader(const wchar_t* fp,const char* e,const char* v){
	_psll++;
	_psl=realloc(_psl,_psll*sizeof(ID3D11PixelShader));
	ID3DBlob* b=NULL;
	ID3DBlob* err=NULL;
	HRESULT hr=D3DCompileFromFile(fp,NULL,NULL,e,v,D3DCOMPILE_ENABLE_STRICTNESS,0,&b,&err);
	if (FAILED(hr)){
		printf("Error loading PixelShader\n");
		if (err!=NULL){
			ID3D10Blob_Release(err);
		}
		return -1;
	}
	ID3D11PixelShader* psp=NULL;
	hr=ID3D11Device_CreatePixelShader(_d3_d,ID3D10Blob_GetBufferPointer(b),ID3D10Blob_GetBufferSize(b),NULL,&psp);
	for (size_t i=0;i<sizeof(ID3D11PixelShader);i++){
		*((char*)(_psl+_psll-1)+i)=*((char*)psp+i);
	}
	if (FAILED(hr)){
		printf("Error creating PS\n");
		return -1;
	}
	ID3D10Blob_Release(b);
	return _psll-1;
}



ConstantBuffer GEngine_create_constant_buffer(uint16_t l){
	_cbll++;
	_cbl=realloc(_cbl,_cbll*sizeof(ID3D11Buffer));
	if (l%16!=0){
		l+=16-l%16;
	}
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
	for (size_t i=0;i<sizeof(ID3D11Buffer);i++){
		*((char*)(_cbl+_cbll-1)+i)=*((char*)cbp+i);
	}
	if (FAILED(hr)){
		printf("Error Creating Constant Buffer: %x\n",hr);
		return -1;
	}
	return _cbll-1;
}



void GEngine_set_shader_data(struct SHADER_DATA* dt){
	for (size_t i=0;i<sizeof(*dt)/sizeof(struct SHADER_DATA);i++){
		if ((dt+i)->t==SHADER_DATA_TYPE_CONSTANT_BUFFER){
			ID3D11Buffer* bf=_cbl+(dt+i)->id;
			if (((dt+i)->f&SHADER_DATA_FLAG_VS)!=0){
				ID3D11DeviceContext_VSSetConstantBuffers(_d3_dc,(dt+i)->r,0,&bf);
			}
			if (((dt+i)->f&SHADER_DATA_FLAG_PS)!=0){
				ID3D11DeviceContext_PSSetConstantBuffers(_d3_dc,(dt+i)->r,0,&bf);
			}
		}
		else{
			assert(0);
		}
	}
	// for (std::initializer_list<SHADER_DATA>::iterator i=dt.begin();i!=dt.end();i++){
	// 	if ((*i).type==SHADER_DATA_TYPE_CONSTANT_BUFFER){
	// 		if ((*i).fl&SHADER_DATA_FLAG_VS!=0){
	// 			this->_d3_dc->VSSetConstantBuffers((*i).r,1,&this->_cbl[(*i).id]);
	// 		}
	// 		if ((*i).fl&SHADER_DATA_FLAG_PS!=0){
	// 			this->_d3_dc->PSSetConstantBuffers((*i).r,1,&this->_cbl[(*i).id]);
	// 		}
	// 	}
	// 	else if ((*i).type==SHADER_DATA_TYPE_TEXTURE){
	// 		if ((*i).fl&SHADER_DATA_FLAG_VS!=0){
	// 			this->_d3_dc->VSSetShaderResources((*i).r,1,&this->_txsrl[(*i).id]);
	// 		}
	// 		if ((*i).fl&SHADER_DATA_FLAG_PS!=0){
	// 			this->_d3_dc->PSSetShaderResources((*i).r,1,&this->_txsrl[(*i).id]);
	// 		}
	// 	}
	// 	else if ((*i).type==SHADER_DATA_TYPE_SAMPLER_STATE){
	// 		if ((*i).fl&SHADER_DATA_FLAG_VS!=0){
	// 			this->_d3_dc->VSSetSamplers((*i).r,1,&this->_ssl[(*i).id]);
	// 		}
	// 		if ((*i).fl&SHADER_DATA_FLAG_PS!=0){
	// 			this->_d3_dc->PSSetSamplers((*i).r,1,&this->_ssl[(*i).id]);
	// 		}
	// 	}
	// 	else{
	// 		this->_err("Error applying Shader Data","Unknown Shader Data Type");
	// 		return;
	// 	}
	// }
}



void GEngine_close(void){
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
