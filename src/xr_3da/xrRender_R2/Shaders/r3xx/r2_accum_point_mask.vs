struct 	a2v
{
	float4 P:	 	POSITION;	// Object-space position
};

struct 	v2p
{
	float4 	hpos: 		POSITION;	// Clip-space position 	(for rasterization)
};

//////////////////////////////////////////////////////////////////////////////////////////
uniform float3x4 	m_W;
uniform float3x4 	m_V;
uniform float4x4 	m_P;
uniform float3x4 	m_WV;
uniform float4x4 	m_VP;
uniform float4x4 	m_WVP;
uniform float4 		light_position;
uniform float4		near;

//////////////////////////////////////////////////////////////////////////////////////////
// Vertex
v2p 	main	( a2v  	I )
{
	v2p 		O;
	O.hpos 		= mul	(m_WVP,	I.P	);
 	return	O;
}
