struct 	a2v
{
	float4 P:	 	POSITION;	// Object-space position
};

struct 	v2p
{
	float4 	hpos: 		POSITION;	// Clip-space position 	(for rasterization)
	float4 	tc:		TEXCOORD0;
};

//////////////////////////////////////////////////////////////////////////////////////////
uniform float3x4 	m_W;
uniform float3x4 	m_V;
uniform float4x4 	m_P;
uniform float3x4 	m_WV;
uniform float4x4 	m_VP;
uniform float4x4 	m_WVP;
uniform float4 		light_position;
uniform float4x4	m_tex;

//////////////////////////////////////////////////////////////////////////////////////////
// Vertex
v2p 	main	( a2v  	I )
{
	/*
	// xform into view-space (and deform)
	float3 	p 	=  mul 	(m_WV,	I.P	);
	p.z		+= max	(near.z - p.z, 0);		// if >0 we need to move it
	float 	lrange 	=  1.f/light_position.w;

	// ensure that it is still inside sphere
	float3 	dir 	= p 	- (float3) light_position;
	float 	len 	= max	(length(dir) - lrange, 0);   	// if >0 we need to move it inside
	p		= p  	- dir*len;

	// move again relative to view-plane
	p.z		+= max	(near.z - p.z, 0);		// if >0 we need to move it

	v2p 		O;
	O.hpos 		= mul	(m_P,	float4(p.x,p.y,p.z,1)	);
	O.tc 		= float2(0,0);
	*/

	/*
	v2p 		O;
	float4 P 	= mul	(m_WVP,	  I.P		        );
	float  w 	= max	(0.00001f,P.w			);	// don't allow back-project
	float4 Pscr 	= float4(P.x/w,   P.y/w,  0.f, 1.f 	);	// just 2D coords
	float4 T 	= mul	(m_tex,   Pscr		        );
	O.hpos 		= Pscr;						// Z-doesn't matter ???, just stencil test
	O.tc 		= T;
	*/

	v2p 		O;
	float4 P 	= mul	(m_WVP,	  I.P		        );
	float4 T 	= mul	(m_tex,   I.P		        );
	O.hpos 		= P;					// Z-doesn't matter ???, just stencil test
	O.tc 		= T;
 	return	O;
}
