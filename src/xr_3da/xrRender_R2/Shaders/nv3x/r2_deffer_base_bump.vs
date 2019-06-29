struct 	a2v
{
  float4 P: 		POSITION;	// Object-space position
  float3 N: 		NORMAL;		// Object-space normal
  float3 T:		TANGENT;	// tangent
  float3 B:		BINORMAL;	// binormal	(calculate in vs??? or even ps???)
  float4 tc0:		TEXCOORD0;	// Texture coordinates
  float4 hemi:		COLOR0;		// Hemisphere
};

struct 	v2p_out
{
  float4 hpos: 		POSITION;	// Clip-space position 	(for rasterization)
  float4 hemi:		COLOR0;		// Hemisphere, replicated
  float2 tc0: 		TEXCOORD0;	// Texture coordinates 	(for sampling diffuse map)
  float3 Pe: 		TEXCOORD1;	// Eye-space position 	(for lighting)
  float3 M1:		TEXCOORD2;	// nmap 2 eye - 1
  float3 M2:		TEXCOORD3;	// nmap 2 eye - 2
  float3 M3:		TEXCOORD4;	// nmap 2 eye - 3
};

//////////////////////////////////////////////////////////////////////////////////////////
uniform float3x4 	m_WV;
uniform float4x4 	m_WVP;
uniform float3 		c_pos;

//////////////////////////////////////////////////////////////////////////////////////////
// Vertex
float3 	v_unpack 	(float3 v)	{ return 2.f*v-1.f; }
float3 	v_unpack_s 	(float3 v)	{ return 4.f*v-2.f; }

v2p_out main	( a2v  	I )
{
	// Eye-space pos/normal
	v2p_out 	O;
	O.hpos 		= mul	(m_WVP,			I.P		);
	O.Pe 		= mul	(m_WV, 			I.P		);
	O.tc0 		= I.tc0;
	O.hemi		= I.hemi;

	// Calculate the 3x3 transform from tangent space to eye-space
	// TangentToEyeSpace = object2eye * tangent2object
	//		     = object2eye * transpose(object2tangent) (since the inverse of a rotation is its transpose)
	float3 	N 	= v_unpack_s(I.N);	// just scale
	float3 	T 	= v_unpack_s(I.T);	// just scale (assume normal in the -.5f, .5f)
	float3 	B 	= v_unpack_s(I.B);	// just scale
	float3x3 xform	= mul	((float3x3)m_WV, float3x3(
						T.x,B.x,N.x,
						T.y,B.y,N.y,
						T.z,B.z,N.z
				));
	// The pixel shader operates on the bump-map in [0..1] range
	// Remap this range in the matrix, anyway we are pixel-shader limited :)
	// ...... [ 2  0  0  0]
	// ...... [ 0  2  0  0]
	// ...... [ 0  0  2  0]
	// ...... [-1 -1 -1  1]
	// issue: strange, but it's slower :(
	// issue: interpolators? dp4? VS limited? black magic? 

	// Feed this transform to pixel shader
	O.M1 			= xform[0]; 
	O.M2 			= xform[1]; 
	O.M3 			= xform[2]; 

	return	O;
}
