#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////
// Реализация невидимости (мерцания)
///////////////////////////////////////////////////////////////////////////////////////////////

class CBloodsuckerVisibility {
	bool				flagVisibility;					// true - если видим

	// параметры мерцания
	bool				bCurVisibility;					// текущая видимость
	TTime				timeBlinkInterval;				// время кванта мерцания
	TTime				timeStartBlinking;				// время начала изменения видимости монстра
	TTime				timeBlinking;					// время мерцания
	TTime				timeLastBlink;					// время последнего переключения кванта мерцания
	TTime				timeBlinkIntervalCurrent;		// текущее время кванта мерцания
	bool				bBlinking;						// сейчас в состоянии мерцания

	// параметры смены видимости
	TTime				timeStartRestore;				// время начала восстановления (после невидимости идёт процесс восстановления, после которого монстр опять сможет стать нивидимым)
	TTime				timeRestoreInterval;			// время восстановления

	TTime				timeStartInvisible;
	TTime				timeInvisibleInterval;
	TTime				timeInvisibilityMin;
	TTime				timeInvisibilityMax;
		
	TTime				timeCurrent;					// текущее время

public:
				CBloodsuckerVisibility	();
	
		void	Load					(LPCTSTR section);
		bool	Switch					(bool bVis);	// возвращает true, если переключение успешно
		bool	Update					();				// возвращает новое состояние видимости
		
	IC	bool	IsCurrentVisible		() {return bCurVisibility;}
	IC	bool	IsInvisibilityReady		() {return (timeStartRestore + timeRestoreInterval < timeCurrent);}	
		TTime	GetInvisibleInterval	() {return timeInvisibleInterval;}
			
	// Temp 
	IC	bool	IsActiveBlinking		() {if (timeStartBlinking + timeBlinking > timeCurrent) return true; else return false;}
};



