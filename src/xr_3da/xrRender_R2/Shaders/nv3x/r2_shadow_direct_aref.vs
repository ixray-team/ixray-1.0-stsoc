struct 	a2v
{
	float4 P:	 	POSITION;	// Object-space position
 	float4 tc0:		TEXCOORD0;	// Texture coordinates
};

struct 	v2p
{
	float4 	hpos: 		POSITION;	// Clip-space position 	(for rasterization)
	float 	depth:		TEXCOORD0;
	float2 	tc0:		TEXCOORD1;
};

//////////////////////////////////////////////////////////////////////////////////////////
uniform float3x4 	m_WV;
uniform float4x4 	m_WVP;

//////////////////////////////////////////////////////////////////////////////////////////
// Vertex
v2p 	main	( a2v  	I )
{
	v2p 	O;
	O.hpos 	= mul		(m_WVP,	I.P	);
	O.depth = O.hpos.z;
	O.tc0 	= I.tc0;

 	return	O;
}
