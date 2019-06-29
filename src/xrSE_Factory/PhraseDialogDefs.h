///////////////////////////////////////////////////////////////
// PhraseDialogDefs.h
// общие объ€влени€ дл€ классов работающих с диалогом
///////////////////////////////////////////////////////////////

#pragma once

#include "boost/shared_ptr.hpp"

//умный указатель дл€ диалога, так как он может 
//совмесно использоватьс€ несколькими объектами ()
class CPhraseDialog;
typedef boost::shared_ptr<CPhraseDialog> DIALOG_SHARED_PTR;


/*
//возможные типы диалогов
typedef enum {
	eDialogTypeAI		= 0x0,	//дл€ разговора 2х NPC
	eDialogTypePDA		= 0x1,
	eDialogTypeActor	= 0x2
} EDialogType;
*/

//тип id фразы
typedef int PHRASE_ID;

typedef shared_str	PHRASE_DIALOG_ID;

DEFINE_VECTOR(PHRASE_DIALOG_ID,		DIALOG_ID_VECTOR,		DIALOG_ID_IT);
//DEFINE_VECTOR(PHRASE_DIALOG_INDEX,	DIALOG_INDEX_VECTOR,	DIALOG_INDEX_IT);

//#define  NO_PHRASE_DIALOG	PHRASE_DIALOG_INDEX(-1)

#define  NO_PHRASE				-1
#define  START_PHRASE			0
#define  START_PHRASE_STR		"0"