#ifndef GENGINE_H
#define GENGINE_H
#include <stdint.h>
#include <limits.h>
#include <signal.h>



#define bool _Bool
#define false 0
#define true 1
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



typedef void (*GEngine_init_func)(void);
typedef void (*GEngine_render_func)(double dt);



void GEngine_set_init_func(GEngine_init_func f);



void GEngine_set_render_func(GEngine_render_func f);



void GEngine_create(const wchar_t* nm);



bool GEngine_is_pressed(uint32_t k);



void GEngine_close(void);



#endif
