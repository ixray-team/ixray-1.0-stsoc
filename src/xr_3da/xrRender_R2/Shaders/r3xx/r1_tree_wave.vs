struct av 
{
	float4 	pos	: POSITION;	// (float,float,float,1)
	float3 	N	: NORMAL;	// (float,float,float,1)
	float4 	clr	: COLOR0;	// rgb
	float4 	misc	: TEXCOORD0;	// (u(Q),v(Q),frac,???)
};

struct vf
{
	float4 HPOS	: POSITION;
	float4 COL0	: COLOR0;
	float2 TEX0	: TEXCOORD0;
	float  fog	: FOG;
};

uniform float4 		consts; 		// {1/quant,1/quant,???,???}
uniform float4 		wave; 			// cx,cy,cz,tm
uniform float4 		wind; 			// direction2D
uniform float4		c_bias;			// + color
uniform float4		c_scale;		// * color
uniform float3x4	m_W;
uniform float4x4 	m_VP;
uniform float4 		v_eye;
uniform float2 		fog;
uniform	float3 		l_dir;
uniform	float3 		l_color;

float3 	v_unpack 	(float3 v)	{ return 2.f*v-1.f; }

vf main (av v)
{
	vf 		o;

	// Transform to world coords
	float3 	pos	= mul	(m_W, v.pos);

	// 
	float 	base 	= m_W._24;			// take base height from matrix
	float 	dp	= sin   (wave.w+dot(pos,(float3)wave));
	float 	H 	= pos.y - base;			// height of vertex (scaled, rotated, etc.)
	float 	frac 	= v.misc.z*consts.x;		// fractional (or rigidity)
	float 	inten 	= H * dp;			// intensity

	// Beizer
	float3 	ctrl1	= float3(0.f,		0.f, 		0.f		);
	float3  ctrl2	= float3(0.f,		H/2.f,		0.f		);
	float3 	ctrl3	= float3(wind.x*inten,	H,		wind.z*inten	);

	float3	temp	= lerp	(ctrl1, ctrl2, frac);
	float3 	temp2	= lerp	(ctrl2, ctrl3, frac);
	float3 	result 	= lerp	(temp,	temp2, frac);

	float4 	f_pos 	= float4(pos.x+result.x, pos.y /*base+result.y*/, pos.z+result.z, 1);

	// Calc fog
	float 	dist 	= dot(f_pos,v_eye);
	o.fog 		= 1-(dist-fog.x)*fog.y;

	// Final xform
	o.HPOS		= mul	(m_VP, f_pos);

	// Fake lighting
	float3 	norm 	= normalize 	(mul (m_W,  v_unpack(v.N)));
	float 	l_d 	= max 		(0, dot(norm,l_dir));
	float3  l_base 	= v.clr * c_scale + c_bias;
	float3 	l_final = l_base + l_base * l_color * l_d;
	o.COL0		= float4	(l_final.x,l_final.y,l_final.z,1);

	// final xform, color, tc
	o.TEX0.xy	= (v.misc * consts).xy;

	return o;
}
