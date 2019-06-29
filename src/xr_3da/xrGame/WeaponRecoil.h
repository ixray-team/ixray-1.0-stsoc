// WeaponRecoil.h:	 класс, который реализует отдачу оружия
//					 (типа внешнего эффектора на камеру)
//////////////////////////////////////////////////////////////////////

class CWeaponRecoil
{
	Fvector	vDispersionDir;
	float	fAngleCurrent;
	float	fAngleNoiseH;
	float	fAngleNoiseP;
	
	float	fRelaxSpeed;
	float	fMaxAngle;
	BOOL	bActive;

public:
	CWeaponRecoil			(float max_angle, float relax_time);
	virtual ~CWeaponRecoil	();

	BOOL	IsActive() {return bActive;}
	void	Process	(Fvector &dangle);
	void	Shot	(float angle);
};