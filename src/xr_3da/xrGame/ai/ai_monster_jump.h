#pragma once
#include "ai_monster_defs.h"

class CCustomMonster;

enum EJumpStateType {
	JT_GLIDE,				
	JT_CUSTOM,
};

struct SJumpState{
	CMotionDef		*motion;
	EJumpStateType	type;
	bool			change;

	struct {
		float	linear;
		float	angular;
	} speed;
};


class CJumping {
	
	DEFINE_VECTOR(SJumpState, JUMP_STATE_VECTOR, JUMP_STATE_VECTOR_IT);

	CCustomMonster *pMonster;

	JUMP_STATE_VECTOR		bank;
	JUMP_STATE_VECTOR_IT	ptr_cur;
	CMotionDef				*cur_motion;

	bool			active;					// состояние прыжка активно?
	Fvector			from_pos;				// стартовая позиция
	Fvector			target_pos;				// целевая позиция
	float			target_yaw;				// угол вектора направления
	TTime			time_started;			// время начала прыжка
	TTime			time_next_allowed;		// время следующего разрешенного прыжка
	float			ph_time;				// расcчетное время прыжка

	const CObject	*entity;				// указатель на объект-цель (0 - нет)

	// LTX section params
	float			m_fJumpFactor;			// коэффициент-делитель времени прыжка
	float			m_fJumpMinDist;			// мин. дистанция, возможная для прыжка
	float			m_fJumpMaxDist;			// макс. дистанция возможная для прыжка
	float			m_fJumpMaxAngle;		// макс. угол возможный для прыжка между монстром и целью
	TTime			m_dwDelayAfterJump;		// задержка после прыжка
	float			m_fTraceDist;			// 

public:
					CJumping				();
				
	// Инициализация параметров прыжка. Необходимо вызывать в Monster::Init
			void	Init					(CCustomMonster *pM); 
	// Загрузка параметров из ltx. Необходимо вызывать в Monster::Load
			void	Load					(LPCSTR section);
	
	// Добавление параметров прыжка
			void	AddState				(CMotionDef *motion, EJumpStateType type, bool change, float linear, float angular);			
			
	// Общая проверка на прыжок
			bool	Check					(Fvector from_pos, Fvector to_pos, const CObject *pO);

	// Подготовка текущей анимации прыжка
			bool	PrepareAnimation		(CMotionDef **m);
	
			void	OnAnimationEnd			();
	// Обновляет состояние прыжка в каждом фрейме (вызывается из UpdateCL)
			void	Update					();
			bool	IsActive				() {return active;}
	// Выполнить проверку на возможность прыжка в текущий момент времени
	virtual bool	CanJump					() {return true;}

			bool	IsGlide					() {return ((active) ? JT_GLIDE == ptr_cur->type : false);}
			const CObject	*GetEnemy		() {return entity;}

	virtual void	OnJumpStart				() {};
	virtual void	OnJumpStop				() {};

private:
			void	Reset					();
			// Начало прыжка
			void	Start					();
			void	Execute					();
			// Завершение прыжка			
			void	Stop					();

			void	ApplyParams				();
			// Переключить следующее состояние прыжка				
			void	NextState				();

};


