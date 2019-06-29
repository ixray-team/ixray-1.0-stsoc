struct av 
{
	float4 	pos	: POSITION;	// (float,float,float,1)
	int4 	misc	: TEXCOORD0;	// (u(Q),v(Q),frac,matrix-id)
};

struct vf
{
	float4 HPOS	: POSITION;
	float4 COL0	: COLOR0;
	float2 TEX0	: TEXCOORD0;
};

uniform float4 		consts; // {1/quant,1/quant,diffusescale,ambient}
uniform float4x4 	xform; 
uniform float4 		array	[200] : register(c12);

vf main (av v)
{
	vf 		o;

	// index
	int 	i 	= v.misc.w;
	float4  m0 	= array[i+0];
	float4  m1 	= array[i+1];
	float4  m2 	= array[i+2];
	float4  c0 	= array[i+3];

	// Transform to world coords
	float4 	pos;
 	pos.x 		= dot	(m0, v.pos);
 	pos.y 		= dot	(m1, v.pos);
 	pos.z 		= dot	(m2, v.pos);
	pos.w 		= 1;

	// Final out
	o.HPOS		= mul	(xform,pos);
	o.COL0		= c0;
	o.TEX0.xy	= (v.misc * consts).xy;

	return o;
}
