struct av 
{
	float4 	P	: POSITION;	// (float,float,float,1)
	float3 	N	: NORMAL;	// (float,float,float,1)
	float4 	color	: COLOR0;	// rgb
	float4 	misc	: TEXCOORD0;	// (u(Q),v(Q),frac,???)
};

struct vf
{
	float4 HPOS: 	POSITION;
	float4 hemi:	COLOR0;		// Hemisphere, replicated
	float2 tc0: 	TEXCOORD0;	// Texture coordinates 	(for sampling diffuse map)
	float3 Pe: 	TEXCOORD1;	// Eye-space position 	(for lighting)
	float3 Ne: 	TEXCOORD2;	// Eye-space normal	(for lighting)
};

uniform float4 		consts; 	// {1/quant,1/quant,???,???}
uniform float3x4	m_WV;
uniform float4x4 	m_WVP;

float3 	v_unpack 	(float3 v)	{ return 2.f*v-1.f; }

vf main (av v)
{
	vf 		o;

	// Final xform(s)
	o.HPOS		= mul	(m_WVP, 	v.P		);
	o.Pe 		= mul	(m_WV,  	v.P		);
	o.Ne 		= mul	((float3x3)m_WV,v_unpack(v.N)	);
	o.tc0.xy	= (v.misc * consts).xy;
	o.hemi 		= v.color;

	return o;
}
