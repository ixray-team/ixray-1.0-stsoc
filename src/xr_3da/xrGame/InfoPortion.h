///////////////////////////////////////////////////////////////
// InfoPortion.h
// струтура, предстваляющая сюжетную информацию
///////////////////////////////////////////////////////////////


#pragma once

#include "shared_data.h"

#include "PhraseScript.h"

#include "xml_str_id_loader.h"
#include "encyclopedia_article_defs.h"

#include "GameTaskDefs.h"
#include "PhraseDialogDefs.h"


//////////////////////////////////////////////////////////////////////////
// SInfoPortionData: данные для InfoProtion
//////////////////////////////////////////////////////////////////////////
struct SInfoPortionData : CSharedResource
{
	SInfoPortionData ();
	virtual ~SInfoPortionData ();

	//массив с именами диалогов, которые могут быть инициированы
	//из этого InfoPortion
	DIALOG_ID_VECTOR m_DialogNames;

	//список статей в энциклопедии, которые становятся известными 
	ARTICLE_ID_VECTOR m_Articles;
	//список статей в энциклопедии, которые становятся неизвестными (на тот случай если
	//нужно заменить одну статью другой)
	ARTICLE_ID_VECTOR m_ArticlesDisable;
    	
	//присоединенные задания
	TASK_ID_VECTOR	m_GameTasks;

	//скриптовые действия, которые активируется после того как 
	//информацию получает персонаж
	CPhraseScript m_PhraseScript;

	//массив с индексами тех порций информации, которые
	//исчезнут, после получения этой info_portion
	DEFINE_VECTOR(INFO_ID, INFO_ID_VECTOR, INFO_ID_VECTOR_IT);
	INFO_ID_VECTOR m_DisableInfo;
};


class CInfoPortion;

//квант  - порция информации
class CInfoPortion : public CSharedClass<SInfoPortionData, INFO_ID, false>,
					 public CXML_IdToIndex<INFO_ID,	int, CInfoPortion>
{
private:
	typedef CSharedClass<SInfoPortionData, INFO_ID, false> inherited_shared;
	typedef CXML_IdToIndex<INFO_ID, int, CInfoPortion>		id_to_index;

	friend id_to_index;
public:
				CInfoPortion	(void);
	virtual		~CInfoPortion	(void);

	//инициализация info данными
	//если info с таким id раньше не использовался
	//он будет загружен из файла
	virtual void Load	(INFO_ID info_str_id);
//	virtual void Load	(INFO_INDEX info_index);

//	const LOCATIONS_VECTOR&							MapLocations()	const {return info_data()->m_MapLocations;}
	const ARTICLE_ID_VECTOR&						Articles	()	const {return info_data()->m_Articles;}
	const ARTICLE_ID_VECTOR&						ArticlesDisable	()	const {return info_data()->m_ArticlesDisable;}
	const TASK_ID_VECTOR&							GameTasks	()	const {return info_data()->m_GameTasks;}
	const DIALOG_ID_VECTOR&							DialogNames	()	const {return info_data()->m_DialogNames;}
	const SInfoPortionData::INFO_ID_VECTOR&			DisableInfos()	const {return info_data()->m_DisableInfo;}
	
			void									RunScriptActions		(const CGameObject* pOwner)	{info_data()->m_PhraseScript.Action(pOwner, NULL, -1);}

	//текстовое представление информации
			shared_str								GetText () const ;

protected:
    INFO_ID		m_InfoId;

	void load_shared	(LPCSTR);
	SInfoPortionData* info_data() { VERIFY(inherited_shared::get_sd()); return inherited_shared::get_sd();}
	const SInfoPortionData* info_data() const { VERIFY(inherited_shared::get_sd()); return inherited_shared::get_sd();}

	static void InitXmlIdToIndex();
};