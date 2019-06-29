#ifndef	SHADOW_H
#define SHADOW_H

#include "common.h"

#define	KERNEL	.6f
//////////////////////////////////////////////////////////////////////////////////////////
// software
//////////////////////////////////////////////////////////////////////////////////////////
half 	sample_sw	(float2 tc, float2 shift, float depth_cmp)
{
	static const float 	ts = KERNEL / float(SMAP_size);
	tc 	+= 	shift*ts;

	float  	texsize = SMAP_size;
	float  	offset 	= 0.5f/texsize;
	float2 	Tex00 	= tc + float2(-offset, -offset);
	float2 	Tex01 	= tc + float2(-offset,  offset);
	float2 	Tex10 	= tc + float2( offset, -offset);
	float2 	Tex11 	= tc + float2( offset,  offset);
	float4 	depth 	= float4(
		depth_cmp-tex2D	(s_smap, Tex00).x,
		depth_cmp-tex2D	(s_smap, Tex01).x,
		depth_cmp-tex2D	(s_smap, Tex10).x,
		depth_cmp-tex2D	(s_smap, Tex11).x);
	half4 compare 	= step	(depth,0);
	float2 fr 	= frac	(Tex00*texsize);
	half2 ifr 	= half2	(1,1) - fr;
	half4 fr4 	= half4	(ifr.x*ifr.y, ifr.x*fr.y, fr.x*ifr.y,  fr.x*fr.y);
	return dot	(compare, fr4);
}
half 	shadow_sw	(float4 tc)	{ 
	float2	tc_dw	= tc.xy / tc.w;
	half4	s;
	s.x	= sample_sw	(tc_dw,float2(-1,-1),tc.z); 
	s.y	= sample_sw	(tc_dw,float2(+1,-1),tc.z); 
	s.z	= sample_sw	(tc_dw,float2(-1,+1),tc.z); 
	s.w	= sample_sw	(tc_dw,float2(+1,+1),tc.z);
	return	dot	(s, 1.h/4.h);
}

//////////////////////////////////////////////////////////////////////////////////////////
// hardware
//////////////////////////////////////////////////////////////////////////////////////////
half  	sample_hw	(float4 tc,float4 shift){
	static const float 	ts = KERNEL / float(SMAP_size);
	return tex2Dproj	(s_smap,tc + tc.w*shift*ts).x;
}
half 	shadow_hw	(float4 tc)		{
  half  s0	= sample_hw	(tc,float4(-1,-1,0,0)); 
  half  s1	= sample_hw	(tc,float4(+1,-1,0,0)); 
  half  s2	= sample_hw	(tc,float4(-1,+1,0,0)); 
  half  s3	= sample_hw	(tc,float4(+1,+1,0,0));
	return	(s0+s1+s2+s3)/4.h;
}

//////////////////////////////////////////////////////////////////////////////////////////
// select hardware or software shadowmaps
//////////////////////////////////////////////////////////////////////////////////////////
#ifdef	USE_HWSMAP
	half 	shadow		(float4	tc)	{ return shadow_hw	(tc);	}
#else
	half 	shadow		(float4 tc) 	{ return shadow_sw	(tc);	}
#endif

//////////////////////////////////////////////////////////////////////////////////////////
// testbed

uniform sampler2D	jitter0;
uniform sampler2D	jitter1;
uniform sampler2D	jitter2;
uniform sampler2D	jitter3;
uniform half4 		jitterS;
half4 	test 		(float4 tc, half2 offset)
{
	float4	tcx	= float4 (tc.xy + tc.w*offset, tc.zw);
	return 	tex2Dproj (s_smap,tcx);
}
half 	shadowtest 	(float4 tc, float4 tcJ)			// jittered sampling
{
	half4	r;

	const 	float 	scale 	= (2.7f/float(SMAP_size));
	half4	J0 	= tex2Dproj	(jitter0,tcJ)*scale;
	half4	J1 	= tex2Dproj	(jitter1,tcJ)*scale;

		r.x 	= test 	(tc,J0.xy).x;
		r.y 	= test 	(tc,J0.wz).y;
		r.z	= test	(tc,J1.xy).z;
		r.w	= test	(tc,J1.wz).x;

	return	dot(r,1.h/4.h);
}
half 	shadowtest_sun 	(float4 tc, float4 tcJ)			// jittered sampling
{
	half4	r;

	const 	float 	scale 	= (2.0f/float(SMAP_size));
	half4	J0 	= tex2D	(jitter0,tcJ)*scale;
	half4	J1 	= tex2D	(jitter1,tcJ)*scale;

		r.x 	= test 	(tc,J0.xy).x;
		r.y 	= test 	(tc,J0.wz).y;
		r.z	= test	(tc,J1.xy).z;
		r.w	= test	(tc,J1.wz).x;

	return	dot(r,1.h/4.h);
}

//////////////////////////////////////////////////////////////////////////////////////////
#ifdef  USE_SUNMASK	
uniform float3x4	m_sunmask	;		// ortho-projection
half 	sunmask		(float4 P)	{		// 
	float2 		tc	= mul	(m_sunmask, P);	//
	return 		tex2D 	(s_lmap,tc).w;		// A8 
	
}
#else
half 	sunmask		(float4 P)	{ return 1.h; }	// 
#endif

//////////////////////////////////////////////////////////////////////////////////////////
uniform float4x4	m_shadow;

#endif
