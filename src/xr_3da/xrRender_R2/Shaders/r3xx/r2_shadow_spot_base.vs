struct 	a2v
{
	float4 P:	 	POSITION;	// Object-space position
 	float4 tc0:		TEXCOORD0;	// Texture coordinates
};

struct 	v2p
{
	float4 	hpos: 		POSITION;	// Clip-space position 	(for rasterization)
	float 	depth:		TEXCOORD0;
};

//////////////////////////////////////////////////////////////////////////////////////////
uniform float3x4 	m_WV;
uniform float4x4 	m_WVP;
uniform float2		c_hpos;

//////////////////////////////////////////////////////////////////////////////////////////
// Vertex
v2p 	main	( a2v  	I )
{
	v2p 	O;
	float4 	hpos 	= mul		(m_WVP,	I.P	);
	O.hpos 		= hpos;
	O.depth 	= O.hpos.z/O.hpos.w;

 	return	O;
}
