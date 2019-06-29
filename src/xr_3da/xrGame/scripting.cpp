state_set	();
state_call	();

class DumbToilet  extends Trigger
{
	bool			bEmpty;
};

class DumbSoldier extends Soldier
{
	function bool isNeutral		(Object O)				{ return !isActor(O);			}
	function bool isInteresting	(Object O)				{ return !isActor(O) && O->canBePickedUp(); }
	function bool isDangerous	(Object O)				{ return isEnemy(O);			}
	function bool isSoldier		(Object O)				{ return isSameClass(O,this);	}

	state s_generic
	{
		event(see)							e_see			(Object O)
		{
			if (isActor(O))					state_call	(s_normal_ai);
		}
		event(timer,random_minutes(10,30))	e_toilet
		{
			if				(state==s_toilet)	return;
			state_set		(s_toilet);
		}
	}

	state s_attention		extends s_generic
	{
		code	(Object O)
		{
			cmd_stay		();
			cmd_animation	(“attention”);
			cmd_look		(O);
			wait			(random_sec(10,30));
		}
	}

	state s_interest		extends s_generic
	{
		code	(Object O)
		{
			wait			(cmd_walk(O)+cmd_look(O));
			wait			(cmd_pickup(O));
		}
	}

	state s_patrol			extends s_generic
	{
		event(near,15m) 				touch		(Object O)
		{
			if (isNeutral(O))			state_call	(attention(O));
		}
		event(hear) 					hear 		(Object O)
		{
		}
		event(timer,random(seconds(10),seconds(30)))	go_to_idle
		{
			state_set	(s_idle);
		}
		code()
		{
			waypoint 	= select_nearest_waypoint	();
			while (1)
			{
				wait	( cmd_move(waypoint) + cmd_lookat(waypoint) );
				waypoint = waypoint.next;
			}
		}
	}

	state s_idle			extends s_generic
	{
		event(touch,15m) 	touch	(Object O)
		{
			if (isSoldier(O) && isSameState(O,this))	{
				O.state_set	(speak_to_soldier(this));
				state_set	(speak_to_soldier(O));
			}
		}
		event(timer,random(seconds(10),seconds(30)))	go_to_idle
		{
			state_set		(s_idle);
		}
		code()
		{
			animation		(“êóðèòü”);
			wait 			(random_minutes(10,15));
		}
	}
	state s_toilet			extends s_generic
	{
		if (state.prev		== s_idle)
		{
		} else {
			Object O			= Level.find("base_toilet");
			if (!O.bEmpty)		
			{
				wait			(cmd_walk(O)+cmd_look(O));
				cmd_stay		();
				cmd_animation	(“want_to_toilet”);
				wait			(O.bEmpty);
			}
			cmd_animation		(“go_to_toilet”);
		}
	}
}


/////////////////////////////////////////////////////////////////////////
class Trigger 
{
public:
	bool	bActive;
	bool	bSignaled;
};

class TriggerAttention		: public Trigger
{
public:
	void	update			()
	{
		get_nearest ();
		foreach if isNeutral	signal();
	}
};

class TriggerTimer			: public Trigger
{
public:
	void	update			()
	{
		if (timeelapsed)	signal();
	}
};

class SoldierController		: public Controller
{
	TriggerSight*			tEnemy;
	TriggerAttention*		tAttention;
	TriggerNear*			tInterest;
	TriggerTimer*			tIdle;
	TriggerTimer*			tToilet;
	TriggerNear*			tAgression;

	void					triggered		()
	{
		if (tEnemy)							set_normal_ai	();
		if (tAttention->bSignaled)			set_state		state_attention	();
		if (tInterest->bSignaled)			set_state		state_attention	();
	}
	void					export			()
	{
		add_trigger();
	}
	void					state_attention	()	
	{
	}
	void					state_interest	()
	{
	}
	void					state_patrol	()
	{
	}
	void					state_idle		()
	{
	}
};

/*
FSM:
	condition(states,triggers,variables)	code();
	condition(states,triggers,variables)	code();
	condition(states,triggers,variables)	code();
	condition(states,triggers,variables)	code();

states: 
	from where it can go to us

triggers:
	boolean - analyzers/reactors to environment, who is near, who is seen, time

variables:
	can be modified inside code to set something or somehow communicate between states
*/
