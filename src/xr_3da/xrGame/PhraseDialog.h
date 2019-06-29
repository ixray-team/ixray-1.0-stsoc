#pragma once

#include "shared_data.h"
#include "phrase.h"
#include "graph_abstract.h"
#include "PhraseDialogDefs.h"
#include "xml_str_id_loader.h"

typedef CGraphAbstract<CPhrase*, float, int> CPhraseGraph;


struct SPhraseDialogData : CSharedResource
{
	SPhraseDialogData ();
	virtual ~SPhraseDialogData ();

	//заголовок диалога, если NULL, то принимается за стартовую фразу
	shared_str		m_sCaption;

	//однонаправленый граф фраз
	//описывает все возможные варианты развития диалога
	CPhraseGraph	m_PhraseGraph;

	//список скриптовых предикатов, выполнение, которых необходимо
	//для начала диалога
	CPhraseScript	m_PhraseScript;

	//произвольное число - приоритет диалога (0 по умолчанию), может быть отрицательным
	//в окне выбора у актера диалоги будут сортироваться по этому значению от меньшего (снизу) к большему (сверху)
	int	m_iPriority;
};

DEFINE_VECTOR(CPhrase*, PHRASE_VECTOR, PHRASE_VECTOR_IT);

class CPhraseDialog;
class CPhraseDialogManager;

class CPhraseDialog	: public CSharedClass<SPhraseDialogData, shared_str, false>,
					  public CXML_IdToIndex<shared_str, int, CPhraseDialog>
{
private:
	typedef CSharedClass<SPhraseDialogData, shared_str, false>				inherited_shared;
	typedef CXML_IdToIndex<shared_str, int, CPhraseDialog>					id_to_index;

	friend id_to_index;
public:
			 CPhraseDialog	(void);
	virtual ~CPhraseDialog	(void);

	//переобределяем copy constructor и оператор ==,
	//чтоб не ругался компилятор
			CPhraseDialog	(const CPhraseDialog& pharase_dialog) {*this = pharase_dialog;}
			CPhraseDialog&	operator = (const CPhraseDialog& pharase_dialog) {*this = pharase_dialog; return *this;}

	
	virtual void			Load				(shared_str dialog_id);

	//связь диалога между двумя DialogManager
	virtual void			Init				(CPhraseDialogManager* speaker_first, CPhraseDialogManager* speaker_second);

	IC		bool			IsInit				() {return ((FirstSpeaker()!=NULL)&& (SecondSpeaker()!=NULL));}

	//реинициализация диалога
	virtual void Reset  ();

	//список предикатов начала диалога
	virtual bool			Precondition		(const CGameObject* pSpeaker1, const CGameObject* pSpeaker2);

	//список доступных в данный момент фраз
	virtual const PHRASE_VECTOR& PhraseList		() const			{return m_PhraseVector;}
			bool				allIsDummy		();
	//сказать фразу и перейти к следующей стадии диалога
	//если вернули false, то считаем, что диалог закончился
	//(сделано статическим, так как мы должны передавать имеенно DIALOG_SHARED_PTR&,
	//а не обычный указатель)
	static bool				SayPhrase			(DIALOG_SHARED_PTR& phrase_dialog, int phrase_id);

	virtual LPCSTR			GetPhraseText		(int phrase_id, bool current_speaking = true);
	virtual LPCSTR			GetLastPhraseText	() {return GetPhraseText(m_iSaidPhraseID, false);}
	virtual int				GetLastPhraseID		() {return m_iSaidPhraseID;}

	//заголовок, диалога, если не задан, то 0-я фраза
	virtual LPCSTR			DialogCaption		();
	virtual int				Priority			();


	virtual bool			IsFinished			()	const {return m_bFinished;}
	
	IC	CPhraseDialogManager* FirstSpeaker		()	const {return m_pSpeakerFirst;}
	IC	CPhraseDialogManager* SecondSpeaker		()	const {return m_pSpeakerSecond;}
	   
		//кто собирается говорить и кто слушать
		CPhraseDialogManager* CurrentSpeaker	()	const;
	    CPhraseDialogManager* OtherSpeaker		()	const;
		//кто последний сказал фразу
		CPhraseDialogManager* LastSpeaker		()	const {return m_bFirstIsSpeaking?SecondSpeaker():FirstSpeaker();}

	IC bool					FirstIsSpeaking		()	const {return m_bFirstIsSpeaking;}
	IC bool					SecondIsSpeaking	()	const {return !m_bFirstIsSpeaking;}

	IC bool					IsWeSpeaking		(CPhraseDialogManager* dialog_manager) const  {return (FirstSpeaker()==dialog_manager && FirstIsSpeaking()) ||
																							(SecondSpeaker()==dialog_manager && SecondIsSpeaking());}
	CPhraseDialogManager*	OurPartner			(CPhraseDialogManager* dialog_manager) const;

protected:
	//идентификатор диалога
	shared_str				m_DialogId;

	//ID последней сказанной фразы в диалоге, -1 если такой не было
	int						m_iSaidPhraseID;
	//диалог закончен
	bool					m_bFinished;

	//список указателей на фразы доступные в данный момент
	PHRASE_VECTOR			m_PhraseVector;

	//указатели на собеседников в диалоге
	CPhraseDialogManager*	m_pSpeakerFirst;
	CPhraseDialogManager*	m_pSpeakerSecond;
	bool					m_bFirstIsSpeaking;

	const SPhraseDialogData* data		() const	{ VERIFY(inherited_shared::get_sd()); return inherited_shared::get_sd();}
	SPhraseDialogData*		data		()			{ VERIFY(inherited_shared::get_sd()); return inherited_shared::get_sd();}

	//загрузка диалога из XML файла
	virtual void			load_shared	(LPCSTR);
	
	//рекурсивное добавление фраз в граф
	void					AddPhrase	(XML_NODE* phrase_node, int phrase_id, int prev_phrase_id);
public:
	CPhrase*				AddPhrase	(LPCSTR text, int phrase_id, int prev_phrase_id, int goodwil_level);
	void					SetCaption	(LPCSTR str);
	void					SetPriority	(int val);

protected:

	//буфферные данные для рекурсивной функции
	CUIXml					uiXml;
	XML_NODE*				phrase_list_node;

	static void				InitXmlIdToIndex();
};
