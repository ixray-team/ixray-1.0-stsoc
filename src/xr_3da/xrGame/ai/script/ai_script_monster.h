////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script_monster.h
//	Created 	: 06.10.2003
//  Modified 	: 06.10.2003
//	Author		: Dmitriy Iassenev
//	Description : Script behaviour interface for all the monsters
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../../GameObject.h"
#include "../../script_callback.h"
#include "../../ai_sounds.h"
#include "../../script_monster_space.h"

class CScriptEntityAction;
class CEntity;
class CScriptGameObject;
class CMotionDef;

using namespace ScriptMonster;

class CScriptMonster : virtual public CGameObject {
public:
	struct CSavedSound {
		CScriptGameObject	*m_lua_game_object;
		int				m_sound_type;
		Fvector			m_position;
		float			m_sound_power;

		IC				CSavedSound(
							CScriptGameObject	*lua_game_object,
							int				sound_type,
							const Fvector	&position,
							float			sound_power
						) :
						m_lua_game_object(lua_game_object),
						m_sound_type(sound_type),
						m_position(position),
						m_sound_power(sound_power)
		{
		}
	};
protected:
	typedef CGameObject inherited;
	xr_deque<CScriptEntityAction*>	m_tpActionQueue;
	bool							m_bScriptControl;
	shared_str						m_caScriptName;
	CMotionDef						*m_tpNextAnimation;
	CScriptEntityAction				*m_tpCurrentEntityAction;
public:
	CMotionDef						*m_tpScriptAnimation;
protected:
	CScriptCallback					m_tpCallbacks[ScriptMonster::eActionTypeCount];
	CScriptCallback					m_tSoundCallback;
	CScriptCallback					m_tHitCallback;
	ref_sound						*m_current_sound;
	xr_vector<CSavedSound>			m_saved_sounds;

public:
								CScriptMonster			();
	virtual						~CScriptMonster			();
			void				init					();
	virtual	void				reinit					();
	virtual BOOL				net_Spawn				(LPVOID DC);
	virtual void				net_Destroy				();
			const Fmatrix		GetUpdatedMatrix		(shared_str caBoneName, const Fvector &tPositionOffset, const Fvector &tAngleOffset);
			void				vfUpdateParticles		();
			void				vfUpdateSounds			();
	virtual	void				vfFinishAction			(CScriptEntityAction		*tpEntityAction);
	virtual	void				SetScriptControl		(const bool			bScriptControl, shared_str	caSciptName);
	virtual	bool				GetScriptControl		() const;
	virtual	LPCSTR				GetScriptControlName	() const;
	
	virtual bool				CheckObjectVisibility	(const CGameObject	*tpObject);
	virtual bool				CheckTypeVisibility		(const char* section_name);

	virtual bool				CheckIfCompleted		() const										{return false;};
	virtual	CScriptEntityAction	*GetCurrentAction		();
	virtual	void				AddAction				(const CScriptEntityAction*tpEntityAction, bool bHighPriority = false);
	virtual void				UseObject				(const CObject		*tpObject);
	virtual void				ProcessScripts			();
	virtual	void				ResetScriptData			(void				*P = 0);
	virtual	bool				bfAssignMovement		(CScriptEntityAction		*tpEntityAction);
	virtual	bool				bfAssignWatch			(CScriptEntityAction		*tpEntityAction);
	virtual	bool				bfAssignAnimation		(CScriptEntityAction		*tpEntityAction);
	virtual	bool				bfAssignSound			(CScriptEntityAction		*tpEntityAction);
	virtual	bool				bfAssignParticles		(CScriptEntityAction		*tpEntityAction);
	virtual	bool				bfAssignObject			(CScriptEntityAction		*tpEntityAction);
	virtual bool				bfAssignMonsterAction	(CScriptEntityAction		*tpEntityAction);

	virtual void				set_callback			(const luabind::object &lua_object, LPCSTR method, const CScriptMonster::EActionType tActionType);
	virtual void				set_callback			(const luabind::functor<void> &lua_function, const CScriptMonster::EActionType tActionType);
	virtual void				clear_callback			(const CScriptMonster::EActionType tActionType);

	virtual void				set_sound_callback		(const luabind::object &lua_object, LPCSTR method);
	virtual void				set_sound_callback		(const luabind::functor<void> &lua_function);
	virtual void				clear_sound_callback	(bool member_callback);
	virtual void				sound_callback			(const CObject *object, int sound_type, const Fvector &position, float sound_power);

	virtual void				set_hit_callback		(const luabind::object &lua_object, LPCSTR method);
	virtual void				set_hit_callback		(const luabind::functor<void> &lua_function);
	virtual void				clear_hit_callback		(bool member_callback);
	virtual void				hit_callback			(float amount, const Fvector &vLocalDir, const CObject *who, s16 element);

	virtual	void				callback				(const EActionType tActionType);

	virtual LPCSTR				GetPatrolPathName		();
	virtual BOOL				UsedAI_Locations		();
	virtual void				shedule_Update			(u32				DT);
	virtual void				UpdateCL				();
			bool				bfScriptAnimation		();
			u32					GetActionCount			() const;
	const CScriptEntityAction	*GetActionByIndex		(u32 action_index) const;

	virtual CEntity				*GetCurrentEnemy		();
	virtual CEntity				*GetCurrentCorpse		();
	virtual int					get_enemy_strength		();

			void				process_sound_callbacks	();
	virtual CScriptMonster*		cast_script_monster		()	{return this;}
private:
			void				FreeAll					();
};