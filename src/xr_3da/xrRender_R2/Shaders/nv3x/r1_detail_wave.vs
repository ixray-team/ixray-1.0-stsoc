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
uniform float4 		wave; 	// cx,cy,cz,tm
uniform float4 		dir2D; 
uniform float4x4 	m_WVP; 
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

	// 
	float 	base 	= m1.w;
	float 	dp	= sin   (dot(pos,wave));
	float 	H 	= pos.y - base;			// height of vertex (scaled)
	float 	frac 	= v.misc.z*consts.x;		// fractional
	float 	inten 	= H * dp;

	// Beizer
	float3 	ctrl1	= float3(0.f,		0.f, 		0.f		);
	float3  ctrl2	= float3(0.f,		H/2.f,		0.f		);
	float3 	ctrl3	= float3(dir2D.x*inten,	H,		dir2D.z*inten	);

	float3	temp	= lerp	(ctrl1, ctrl2, frac);
	float3 	temp2	= lerp	(ctrl2, ctrl3, frac);
	float3 	result 	= lerp	(temp,	temp2, frac);

	// Final xform
	pos		= float4(pos.x+result.x, pos.y, pos.z+result.z, 1);
	o.HPOS		= mul	(m_WVP,pos);

	// Fake lighting
	float 	dpc 	= max 	(0.f, dp);
	o.COL0		= c0  * (consts.w+consts.z*dpc*frac);

	// final xform, color, tc
	o.TEX0.xy	= (v.misc * consts).xy;

	return o;
}
