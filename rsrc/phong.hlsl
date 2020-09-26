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
	float3 vd:POSITION1;
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
		normalize(cp.xyz-mul(p,wm).xyz),
		tx
	};
	return o;
}



float4 pixel_shader(float4 p:POSITION,float4 c:COLOR):SV_TARGET0{
	return c;
}



float4 pixel_shader_tex(float4 p:SV_POSITION,float4 wp:POSITION,float3 n:NORMAL,float3 vd:POSITION1,float2 tx:TEXCOORD0):SV_TARGET0{
	float3 ld=-normalize(lp-wp).xyz;
	float li=saturate(dot(n,ld));
	return saturate(ac+dc*(li*0.9+0.1)+(li>0?sc*saturate(pow(saturate(dot(normalize(2*li*n-ld),vd)),16/*se*/)):0));
}
