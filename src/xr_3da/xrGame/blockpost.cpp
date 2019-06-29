-- blockpost script

toilet_is_busy = false


function go_to_toilet(obj, toilet)
	if math.Random(0,100) < 50 then
		mtype = walk
	else
		mtype = run
	end

	obj:command(
		action(
			move(standing,walk,mtype,toilet),
			look(),
			anim(),
			sound(),
			use(),
			-1
		)
	)
	obj:command(
		action(
			move(),
			look(toilet),
			anim(),
			sound(),
			use(),
			-1
		)
	)
	obj:command(
		action(
			move(),
			look(),
			anim("toilet_usage" + math.Random(0,4)),
			sound(),
			use(),
			-1
		)
	)
end

function shtab(obj)
	obj:command(
		action(
			move(standing,walk,line,level.object("stolik0000")),
			look(),
			anim(),
			sound(),
			use(),
			-1
		)
	)
	obj:command(
		action(
			move(),
			look(level.object("stolik0000")),
			anim(),
			sound(),
			use(),
			-1
		)
	)
	obj:command(
		action(
			move(),
			look(),
			anim("sest_za_stol"),
			sound(),
			use(),
			-1
		)
	)
	obj:command(
		action(
			move(),
			look(),
			anim("pisat_za_stolom"),
			sound(),
			use(),
			TIME + ANIM + SOUND
			math.random(20000,50000)
		)
	)
	wait(obj::anim)


	obj:command(anim("pisat_za_stolom"), math.random(20000,50000) )
	wait(obj or (obj1.move and obj2.anim) or environment:rain or time(math.random(20000,50000)));


	if math.Random(0,100) < 30 then
		obj:command(
			action(
				move(),
				look(),
				anim("pisat_za_stolom"),
				sound("phone_call",level.object("phone0000")),
				use(),
				math.random(1000,3000)
			)
		)
		obj:command(
			action(
				move(),
				look(),
				anim("podniat_trubku"),
				sound("phone_call",level.object("phone0000")),
				use(),
				-1
			)
		)
	
		s = "phone_talk" + math.random(0,10)
		
		obj:command(
			action(
				move(),
				look(),
				anim(s),
				sound(s),
				use(),
				-1
			)
		)
		obj:command(
			action(
				move(),
				look(),
				anim("polozhit_trubku"),
				sound(),
				use(),
				-1
			)
		)
	else
		if toilet_is_busy == false then
			toilet_is_busy = true
			obj:command(
				action(
					move(),
					look(),
					anim("vstat_iz_za_stola"),
					sound(s),
					use(),
					-1
				)
			)
			go_to_toilet(obj,level.object("toilet0000"))
		end
	end
end

function vishka(obj)
end

function patrol(obj)
end

function stolik(obj)
end

function bezdelniki(obj)
end

function koster(obj)
end

st0 = level.object("m_stalker_e0000")
st1 = level.object("m_stalker_e0001")
st2 = level.object("m_stalker_e0002")
st3 = level.object("m_stalker_e0003")
st4 = level.object("m_stalker_e0004")
st5 = level.object("m_stalker_e0005")

repeat
	shtab		(st0)
	vishka		(st1)
	patrol		(st2,st3,st4)
	stolik		(st5,st6,st7)
	bezdelniki	(st8,st9,st10)
	koster		(st11,st12,st13)

	
until false


