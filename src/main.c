#include <g_engine.h>
#include <stdio.h>



void init(void){
	printf("BBB\n");
}



void render(double dt){
	// printf("%lf\n",dt);
}



int main(int argc,const char** argv){
	GEngine_set_init_func(init);
	GEngine_set_render_func(render);
	GEngine_create(L"HEY!");
	getchar();
	GEngine_close();
	return 0;
}
