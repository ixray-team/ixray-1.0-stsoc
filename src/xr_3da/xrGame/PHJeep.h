///////////////////////////////////////////////////////////////////////////////////
#ifndef PH_JEEP
#define PH_JEEP

class CPHJeep {
	dGeomID GeomsGroup;
	static const	u32 NofGeoms=8;
	static const	u32 NofBodies=5;
	static const	u32 NofJoints=4;


	dBodyID Bodies[NofBodies];
	dGeomID Geoms[NofGeoms];
	dJointID Joints[NofJoints];
	CPhysicsRefObject* m_ref_object;
	dVector3 startPosition;
	bool	 weels_limited;
	bool	 bActive;
	void CreateDynamicData();
public:

	dReal MassShift;

	CPHJeep()
	{
		weels_limited=true;
		bActive=false;
		m_ref_object=NULL;
		MassShift=0.25f;
	}
	void SetPhRefObject(CPhysicsRefObject * ref_object);
	void applyImpulseTrace		(int part,const Fvector& pos, const Fvector& dir, float val){
		if(!bActive) return;
		val/=fixed_step;
		if(part<0||part>NofBodies-1) return;
		Fvector body_pos;
		body_pos.set(pos);
		if(part==0){ body_pos.sub(DynamicData.BoneTransform.c);
		dBodyAddForceAtRelPos       (Bodies[part], dir.x*val,dir.y*val,dir.z*val,pos.z, pos.y, -pos.x);
		}
		else
			dBodyAddForceAtRelPos       (Bodies[part], dir.x*val,dir.y*val,dir.z*val,pos.x, pos.z, -pos.y);
		//else		body_pos.sub(DynamicData[part-1].BoneTransform.c);
	}
	void Create(dSpaceID space, dWorldID world);
	void Create1(dSpaceID space, dWorldID world);
	void Destroy();
	void Steer1(const char& velocity, const char& steering);
	void Steer(const char& steering);
	float GetSteerAngle();
	void LimitWeels();
	void Drive(const char& velocity,dReal force=500.f);
	void Drive();
	void NeutralDrive();
	void JointTune(dReal step);
	void Revert();
	void SetStartPosition(Fvector pos)
	{
		if(!bActive) return;
		dBodySetPosition(Bodies[0],pos.x,pos.y,pos.z);
	}
	void SetPosition(Fvector pos){
		if(!bActive) return;
		const dReal* currentPos=dBodyGetPosition(Bodies[0]);	
		Fvector v={pos.x-currentPos[0],pos.y-currentPos[1],pos.z-currentPos[2]};
		dBodySetPosition(Bodies[0],pos.x,pos.y,pos.z);
		for(unsigned int i=1;i<NofBodies; i++ ){
			const dReal* currentPos=dBodyGetPosition(Bodies[i]);
			dVector3 newPos={currentPos[0]+v.x,currentPos[1]+v.y,currentPos[2]+v.z};
			dBodySetPosition(Bodies[i],newPos[0],newPos[1],newPos[2]);
		}
	}
	void SetRotation(dReal* R){
		if(!bActive) return;
		const dReal* currentPos0=dBodyGetPosition(Bodies[0]);	
		const dReal* currentR=dBodyGetRotation(Bodies[0]);
		dMatrix3 relRot;
		dMULTIPLYOP1_333(relRot,=,R,currentR);
		dBodySetRotation(Bodies[0],R);


		for(unsigned int i=1;i<NofBodies; i++ ){
			const dReal* currentPos=dBodyGetPosition(Bodies[i]);
			dVector3 relPos={currentPos[0]-currentPos0[0],currentPos[1]-currentPos0[1],currentPos[2]-currentPos0[2]};
			dVector3 relPosRotated;
			dMULTIPLYOP0_331(relPosRotated,=,relRot,relPos);
			dVector3 posChange={relPosRotated[0]-relPos[0],relPosRotated[1]-relPos[1],relPosRotated[2]-relPos[2]};
			dVector3 newPos={currentPos[0]+posChange[0],currentPos[1]+posChange[1],currentPos[2]+posChange[2]};
			dBodySetPosition(Bodies[i],newPos[0],newPos[1],newPos[2]);

			const dReal* currentR=dBodyGetRotation(Bodies[i]);
			dMatrix3 newRotation;
			dMULTIPLYOP0_333(newRotation,=,currentR,relRot);
			dBodySetRotation(Bodies[i],newRotation);
		}

	}
	Fvector GetVelocity(){

		if(!bActive) 
		{
			Fvector ret;
			ret.set(0,0,0);
			return ret;
		}
		Fvector ret;
		const dReal* vel=dBodyGetLinearVel(Bodies[0]);
		ret.x=vel[0];
		ret.y=vel[1];
		ret.z=vel[2];
		return ret;
	}
	Fvector GetAngularVelocity(){

		if(!bActive) 
		{
			Fvector ret;
			ret.set(0,0,0);
			return ret;
		}
		Fvector ret;
		const dReal* vel=dBodyGetAngularVel(Bodies[0]);
		ret.x=vel[0];
		ret.y=vel[1];
		ret.z=vel[2];
		return ret;
	}

	PHDynamicData DynamicData;
	dVector3 jeepBox;
	dVector3 cabinBox;
	dReal VelocityRate;
	dReal DriveForce;

	dReal DriveVelocity;
	char  DriveDirection;
	bool Breaks;
};

#endif