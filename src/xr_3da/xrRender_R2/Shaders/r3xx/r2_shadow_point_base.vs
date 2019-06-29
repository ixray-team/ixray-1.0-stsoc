struct 	a2v
{
	float4 P:	 	POSITION;	// Object-space position
 	float4 tc0:		TEXCOORD0;	// Texture coordinates
};

struct 	v2p
{
	float4 	hpos: 		POSITION;	// Clip-space position 	(for rasterization)
	float3 	lpos:		TEXCOORD0;
};

//////////////////////////////////////////////////////////////////////////////////////////
uniform float3x4 	m_W;
uniform float3x4 	m_WV;
uniform float4x4 	m_WVP;

//////////////////////////////////////////////////////////////////////////////////////////
// Vertex
v2p 	main	( a2v  	I )
{
	v2p 	O;
	O.hpos 	= mul		(m_WVP,	I.P	);
	O.lpos 	= mul 		(m_WV, 	I.P	);

 	return	O;
}
