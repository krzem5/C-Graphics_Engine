#include <g_engine.h>
#include <windows.h>
#include <stdio.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dcompiler.lib")



HWND _w=NULL;
uint32_t _ww=0;
uint32_t _wh=0;
GEngine_init_func _if;
GEngine_render_func _rf;
ID3D11Device* _d3_d=NULL;
ID3D11DeviceContext* _d3_dc=NULL;



LRESULT CALLBACK _msg_cb(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp){
	switch (msg){
		case WM_CREATE:
			D3D_FEATURE_LEVEL fl;
			D3D11CreateDevice(NULL,D3D_DRIVER_TYPE_HARDWARE,0,D3D11_CREATE_DEVICE_DEBUG,NULL,0,D3D11_SDK_VERSION,&_d3_d,&fl,&_d3_dc);
			DXGI_SWAP_CHAIN_DESC scd={
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
			IDXGISwapChain* swapchain;
			ID3D11Device* dev;
			ID3D11DeviceContext* devcon;
			D3D11CreateDeviceAndSwapChain(NULL,D3D_DRIVER_TYPE_HARDWARE,NULL,0,NULL,6,D3D11_SDK_VERSION,&scd,&swapchain,&dev,NULL,&devcon);
			printf("INIT\n");
			_if();
			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	return DefWindowProc(hwnd,msg,wp,lp);
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
	while (msg.message!=WM_QUIT){
		if (PeekMessage(&msg,_w,0,0,PM_REMOVE)>0){
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		QueryPerformanceCounter(&c);
		if (lt.QuadPart==0){
			_rf(0);
		}
		else{
			_rf((double)((c.QuadPart-lt.QuadPart)*1000000/tf.QuadPart)/1000);
		}
		lt=c;
	}
}



bool GEngine_is_pressed(uint32_t k){
	return (GetKeyState(k)<0?true:false);
}



void GEngine_close(void){
	DestroyWindow(_w);
	_w=NULL;
}
