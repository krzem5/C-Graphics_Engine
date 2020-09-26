#pragma pack_matrix(row_major)



cbuffer core:register(b0){
	matrix wm;
	matrix cm;
	matrix pm;
	float4 cp;
	float4 lp;
};



cbuffer phong:register(b1){
	float4 ac;
	float4 dc;
	float4 sc;
	float4 d;
	float4 s;
	float df;
	float se;
}



struct VS_OUT{
	float4 p:SV_POSITION;
	float4 c:COLOR;
};



struct VS_OUT_TEX{
	float4 p:SV_POSITION;
	float4 wp:POSITION;
	float3 n:NORMAL;
	float2 tx:TEXCOORD0;
};



VS_OUT vertex_shader(float4 p:POSITION,float4 c:COLOR){
	VS_OUT o={
		mul(mul(mul(p,wm),cm),pm),
		c
	};
	return o;
}



VS_OUT_TEX vertex_shader_tex(float4 p:POSITION,float3 n:NORMAL,float2 tx:TEXCOORD0){
	VS_OUT_TEX o={
		mul(mul(mul(p,wm),cm),pm),
		mul(p,wm),
		mul(float4(n,1),wm).xyz,
		tx
	};
	return o;
}



float4 pixel_shader(float4 p:POSITION,float4 c:COLOR):SV_TARGET0{
	return c;
}



float4 pixel_shader_tex(float4 p:SV_POSITION,float4 wp:POSITION,float3 n:NORMAL,float2 tx:TEXCOORD0):SV_TARGET0{
	/////
	// float3 lightDir=normalize(p-wp).xyz;
	// float diffuseLighting=saturate(dot(n,-lightDir));
	// diffuseLighting*=((length(lightDir)*length(lightDir))/dot(lp-wp,lp-wp));
	// float3 h=normalize(normalize(cp-wp).xyz-lightDir);
	// float specLighting=pow(saturate(dot(h,n)),2.0f);
	/////
	// float3 lightDir=normalize(p-wp).xyz;
	// float diffuseLighting=saturate(dot(n,-lightDir));
	// diffuseLighting*=((length(lightDir)*length(lightDir))/dot(lp-wp,lp-wp));
	// // float3 h=normalize(normalize(cp-wp).xyz-lightDir);
	// // float specLighting=pow(saturate(dot(h,n)),2.0f);
	// return saturate(ac+diffuseLighting*df);
	/////
	// float4 df_c=saturate(dot((lp-wp).xyz,n)*df);
	// return saturate(ac+df_c);
	/////
	float3 ld=normalize(lp-wp).xyz;
	return saturate(ac+dc*(saturate(dot(n,-ld))*0.9+0.1));
}
