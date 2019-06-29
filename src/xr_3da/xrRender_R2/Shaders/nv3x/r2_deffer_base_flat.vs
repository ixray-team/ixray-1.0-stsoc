struct 	a2v
{
  float4 P: 		POSITION;	// Object-space position
  float3 N: 		NORMAL;		// Object-space normal
  float3 T:		TANGENT;	// tangent
  float3 B:		BINORMAL;	// binormal	(calculate in vs??? or even ps???)
  float2 tc0:		TEXCOORD0;	// Texture coordinates
  float4 hemi:		COLOR0;		// Hemisphere
};

struct 	v2p_out
{
  float4 hpos: 		POSITION;	// Clip-space position 	(for rasterization)
  float4 hemi:		COLOR0;		// Hemisphere, replicated
  float2 tc0: 		TEXCOORD0;	// Texture coordinates 	(for sampling diffuse map)
  float3 Pe: 		TEXCOORD1;	// Eye-space position 	(for lighting)
  float3 Ne: 		TEXCOORD2;	// Eye-space normal	(for lighting)
};

//////////////////////////////////////////////////////////////////////////////////////////
uniform float3x4 	m_WV;
uniform float4x4 	m_WVP;
uniform float3 		c_pos;

//////////////////////////////////////////////////////////////////////////////////////////
// Vertex
float3 	v_unpack 	(float3 v)	{ return 2.f*v-1.f; }

v2p_out main	( a2v  	I )
{
	// distortion
	/*
	float3 		c_dir	= I.P-c_pos;
	float 		c_len 	= length(c_dir);
	float 		shear 	= 1.f	/ sqrt	(c_len/30);
	float3 		P 	= c_pos + c_dir*shear;
	*/

	// Eye-space pos/normal
	v2p_out 	O;
	O.hpos 		= mul	(m_WVP,			I.P		); // float4 (P.x,P.y,P.z,1)	);
	O.Pe 		= mul	(m_WV, 			I.P		);
	O.Ne 		= mul	((float3x3)m_WV,	v_unpack(I.N)	);
	O.tc0 		= I.tc0;
	O.hemi 		= I.hemi;

	return	O;
}
