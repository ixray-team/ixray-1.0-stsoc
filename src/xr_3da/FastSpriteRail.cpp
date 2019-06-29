// FastSpriteRail.cpp: implementation of the CFastSpriteRail class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FastSpriteRail.h"

#define NUM_SPRITES			200
#define NUM_CIRCLES			10
#define RAIL_SPRITE_SIZE	.05f	//
#define RAIL_CIRCLE_SIZE	.1f		// real (meters) coords
#define RAIL_TTL			1.f		// TimeToLive
#define RAIL_SCALE			.5f		// at end of TTL circle size will be
									//       RAIL_CIRCLE_SIZE+RAIL_SCALE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFastSpriteRail::CFastSpriteRail(Fvector &vStart, Fvector &vEnd) : CFastSprite("rail")
{
	fTTL	= RAIL_TTL;

	float angle     = 0;
	float angle_add	= (PI_MUL_2*NUM_CIRCLES)/NUM_SPRITES;

	for (int i=0; i<NUM_SPRITES; i++, angle+=angle_add) {
		FSprite S;
		float d = float(i)/float(NUM_SPRITES);
		float r = (1-d)*RAIL_CIRCLE_SIZE;
		S.pos.x = r*_cos(angle);
		S.pos.y = r*_sin(angle);
		S.pos.z = d;
		S.alpha = 1.0f;
		S.angle = angle; //rnd()*PI_MUL_2;
		S.size	= RAIL_SPRITE_SIZE;
		sprites.push_back(S);
	}

	// ****************** build transform
	Fvector vScale,D,N;
	Fmatrix mRotation,mScale,mTranslate;

	D.sub(vEnd,vStart);
	float len = D.magnitude();
	D.div(len);

	// scale
	vScale.set(1.f,1.f,len);
	mScale.scale(vScale);

	// translate
	mTranslate.translate(vStart);

	// rotation
	Fvector	right, y;
	y.set(0,1,0);
	right.crossproduct(y,D);
	N.crossproduct(D,right);
	N.normalize();
	mRotation.rotation(D,N);

	mTransformNoScale.mul_43(mTranslate,mRotation);
	mTransform.mul_43(mTransformNoScale,mScale);

	// ***************** Calc sphere & radius
	bv_Radius		= len/2;
	bv_Position.add(vStart,vEnd);
	bv_Position.div(2.0f);
}

CFastSpriteRail::~CFastSpriteRail()
{

}

void CFastSpriteRail::Update()
{
	CFastSprite::Update();

	fTTL -= Device.fTimeDelta;
	float alpha = fTTL/RAIL_TTL;
	float angle = Device.fTimeDelta*4.f;

	// apply alpha
	VSprites::iterator i = sprites.begin();
	for (;i<sprites.end(); i++) {
		i->alpha = alpha;
		i->angle+= angle;
	}

	// apply scale
	Fvector vScale;
	Fmatrix mScale;
	float	fScale	= 1+(1-alpha)*RAIL_SCALE;
	vScale.set		(fScale,fScale,bv_Radius*2);
	mScale.scale	(vScale);
	mTransform.mul_43(mTransformNoScale,mScale);
}
