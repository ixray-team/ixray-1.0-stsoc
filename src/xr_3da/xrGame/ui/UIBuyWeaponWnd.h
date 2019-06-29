// UIBuyWeaponWnd.h:	окошко, для покупки оружия в режиме CS
//						
//////////////////////////////////////////////////////////////////////

#pragma once

#include "UIBuyWeaponStd.h"

#include "UIDialogWnd.h"
#include "UIDragDropList.h"
#include "UIDragDropItemMP.h"

#include "UIProgressBar.h"

#include "UIPropertiesBox.h"
#include "UIOutfitSlot.h"
#include "UIOutfitInfo.h"

#include "UICharacterInfo.h"
#include "UIItemInfo.h"
#include "UITabControl.h"
#include "UIListItem.h"
#include "UIMultiTextStatic.h"


#include "UIBuyWeaponTab.h"
#include "UIBag.h"
//#include "UIAutobuyIndication.h"
//#include "UIWpnParams.h"

extern const u32	cDetached;
extern const u32	cAttached;
extern const u32	cUnableToBuy;
extern const u32	cUnableByRank;
extern const u32	cAbleToBuy;
extern const u32	cAbleToBuyOwned;
extern const float	fRealItemSellMultiplier;

class CUIBuyWeaponWnd: public CUIDialogWnd
{
	friend class CUIDragDropItemMP;
private:
	typedef CUIDialogWnd inherited;
public:
	CUIBuyWeaponWnd(LPCSTR strSectionName, LPCSTR strPricesSection);
	virtual ~CUIBuyWeaponWnd();

	virtual void Init(LPCSTR strSectionName, LPCSTR strPricesSection);
	void InitInventory();

	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void *pData);
	virtual bool OnMouse(float x, float y, EUIMessages mouse_action);
	virtual void OnMouseScroll(float iDirection);


	virtual void Update();
			void UpdateOutfit();
	virtual void Draw();

	virtual void Show();
	virtual void Hide();

protected:
	CUIStatic			UIBackground;
	CUIBag				UIBagWnd;
	CUIStatic			UIDescWnd;
	CUIStatic			UIDescRankIcon;
	CUIStatic			UIPersonalWnd;
	CUIBuyWeaponTab		UITabControl;
	CUIStatic			UIStaticBelt;
	CUIStatic			UIStaticBottom;
	CUIStatic			UIStaticDesc;
	// Индикаторы денег
	CUIStatic			UITotalMoneyHeader;
	CUIStatic			UITotalMoney;
	CUIStatic			UIItemCostHeader;
	CUIStatic			UIItemCost;
	//слоты для оружия
	CUIDragDropList		UITopList[MP_SLOTS_NUM]; 
	//отдельный слот для костюмов
	CUIStatic			UIOutfitIcon;		
	//pop-up меню вызываемое по нажатию правой кнопки
	CUIPropertiesBox	UIPropertiesBox;
//	CUIAutobuyIndication UIAutobuyIndication;

	//элемент с которым работают в текущий момент
	PIItem m_pCurrentItem;
	CUIDragDropItemMP* m_pCurrentDragDropItem;

	// Информация о вещи
	CUIItemInfo UIItemInfo;

	static bool SlotProc0(CUIDragDropItem* pItem, CUIDragDropList* pList);
	static bool SlotProc1(CUIDragDropItem* pItem, CUIDragDropList* pList);
	static bool SlotProc2(CUIDragDropItem* pItem, CUIDragDropList* pList);
	static bool SlotProc3(CUIDragDropItem* pItem, CUIDragDropList* pList);
	static bool SlotProc4(CUIDragDropItem* pItem, CUIDragDropList* pList);

	static bool OutfitSlotProc(CUIDragDropItem* pItem, CUIDragDropList* pList);
	static bool BagProc(CUIDragDropItem* pItem, CUIDragDropList* pList);
	static bool BeltProc(CUIDragDropItem* pItem, CUIDragDropList* pList);
	//для запуска меню по правой клавиши
	void ActivatePropertiesBox();

	//описание возоможных дейстивий над предметами инвентаря
	enum {BUY_ITEM_ACTION, CANCEL_BUYING_ACTION,
		ATTACH_SILENCER_ADDON,
		ATTACH_SILENCER_ADDON_PISTOL,
		ATTACH_GRENADE_LAUNCHER_ADDON,
		ATTACH_SCOPE_ADDON, 
		DETACH_SILENCER_ADDON,
		DETACH_SILENCER_ADDON_PISTOL,
		DETACH_GRENADE_LAUNCHER_ADDON,
		DETACH_SCOPE_ADDON};

	//выбросить элемент
	void DropItem();

	//перемещение вещи
	bool ToSlot();
	bool ToBag();
	bool ToBelt();

	//запуск и остановка меню работы с артефактами
	void StartArtefactMerger();
	void StopArtefactMerger();

	shared_str		m_StrSectionName;

	bool SlotToSection(const u32 SlotNum);
	bool BeltToSection(CUIDragDropItemMP* pDDItemMP);
	bool CanPutInSlot(CUIDragDropItemMP* pDDItemMP, const u32 slotNum);
	bool CanPutInBelt(CUIDragDropItemMP* pDDItemMP);
	bool CanAttachAddOn(CUIDragDropItemMP* pAddonOwner, CUIDragDropItemMP* pAddOn);
	// for buy presets

//	typedef struct {
//		int m_price;
//		xr_vector<shared_str> m_list;
//	} Preset;

//	Preset   m_presets[3];
//		void FillUpPresets();
//		void ParseStrToVector(const char* str, xr_vector<shared_str>& vector);
//		void UpdatePresetPrices();
//		void UpdatePresetPrice(Preset& preset);
//		int  FindBestBuy();
//		void PerformAutoBuy();
		int  GetPriceOfOwnItems();
		int  GetPriceOfItemInSlot(int slot);
static	int  GetItemPrice(CUIDragDropItemMP *pDDItemMP);
//		void 
	CUI3tButton UIBtnOK;
	CUI3tButton UIBtnCancel;
//	CUI3tButton UIBtnAutobuy;
	CUI3tButton UIBtnClear;

	CUI3tButton UIBtn_PistolBullet;
	CUI3tButton UIBtn_PistolSilencer;
	CUI3tButton UIBtn_RifleBullet;
	CUI3tButton UIBtn_RifleSilencer;
	CUI3tButton UIBtn_RifleScope;
	CUI3tButton UIBtn_RifleGranadelauncer;
	CUI3tButton UIBtn_RifleGranade;

	// Дополнительные функции для получения информации о вещах в слотах
	// Params:	slotNum	- номер слота (раздела) в котором ищем 
	//			idx		- порядковый индекс оружия в слоте (для всех слотов кроме пояса должно быть == 0)
	// Return:	Указатель на затребованную вещь, либо NULL, если не найдено
	CUIDragDropItemMP * GetWeapon(u32 slotNum, u32 idx = 0);

	friend void WpnDrawIndex(CUIDragDropItem *pDDItem);

//	int			m_iMoneyAmount;
	bool		m_bIgnoreMoney;
	bool		m_bIgnoreMoneyAndRank;

	// Координаты дефолтного скина на текстуре с иконками сталкеров.
//	float		m_iIconTextureX, m_iIconTextureY;
	xr_string	m_current_skin;		
	void		FillItemInfo(CUIDragDropItemMP *pDDItemMP);
	void		BuyReaction();

	void		HighlightCurAmmo();

public:
	bool		CheckBuyAvailabilityInSlots();
	bool		CanBuyAllItems();
	void		IgnoreMoney(bool ignore);
	void		IgnoreMoneyAndRank	(bool ignore);
	int			GetMoneyAmount() const;
	bool		HasEnoughMoney(CUIDragDropItemMP* pItem);
	void		SetMoneyAmount(int moneyAmount);
	int			GetMoneyLeft	(int ItemCost);

	// Процедура запоминания индекса скина в списке skins типа сетевой игры, после выбора его игроком
	void		SetSkin			(u8 SkinID);	

	CUIDragDropItemMP * IsItemAnAddon(CUIDragDropItemMP *pPossibleAddon, CUIDragDropItemMP::AddonIDs &ID);
	bool		IsItemAnAddonSimple(CUIDragDropItemMP *pPossibleAddon) const;
	CUIDragDropItemMP * GetAddonByID(CUIDragDropItemMP *pAddonOwner, CUIDragDropItemMP::AddonIDs ID);

	virtual bool OnKeyboard(int dik, EUIMessages keyboard_action);
			void OnTabChange();
			void OnMenuLevelChange();
			void OnDDItemDbClick();
			void OnDDItemDrag();
			void OnBtnOkClicked();
			void OnBtnCancelClicked();
			void OnBtnClearClicked();
			void OnBtnAutobuyClicked();
			void OnBtnAutobuyFocusReceive();
			void OnBtnAutobuyFocusLost();
			void OnBtnBulletBuy(int slot);
			void OnBtnSilencerBuy(int slot);
			void OnBtnRifleScope();
			void OnBtnRifleGranadelauncher();
			void OnBtnRifleGranade();
			void UpdateBuyButtons();

			void SetCurrentDDItem(CUIWindow* pWnd);

			bool ClearTooExpensiveItems();
			bool ClearSlot_ifTooExpensive(int slot);

	const u8	GetWeaponIndex(u32 slotNum);
	const u8	GetWeaponIndexInBelt(u32 indexInBelt, u8 &sectionId, u8 &itemId);
	const u8	GetItemIndex(u32 slotNum, u32 idx, u8 &sectionNum);
	
	void		GetWeaponIndexByName(const xr_string sectionName, u8 &grpNum, u8 &idx);
	char*		GetWeaponNameByIndex(u8 grpNum, u8 idx);

	// Получить данные о аддонах к оружию. Младшие 3 бита, если установлены в 1 означают:
	// 2 - Silencer, 1 - Grenade Launcher, 0 - Scope
	const u8	GetWeaponAddonInfoByIndex(u8 idx);
	const u8	GetBeltSize();
	void		SetRank(int rank) {UIBagWnd.SetRank(rank);}
//	void		ReInitItems	(LPCSTR strSectionName);

	// Процедуры принудительного перемещения оружия (эмуляция даблклика).
	// Params:	grpNum					- номер группы секций
	//			uIndexInSlot			- порядковый уровень оружия в списке группы секций
	//			bRealRepresentationSet	- новое флага , который указывает на наличие у вещи реального 
	//									  представления
//	void		MoveWeapon(const u8 grpNum, const u8 uIndexInSlot, bool bRealRepresentationSet) {}
	// Params:	sectionName		- имя конфигурационной секции оружия
//	void		MoveWeapon(const char *sectionName, bool bRealRepresentationSet) {}

	void		SectionToSlot(const u8 grpNum, u8 uIndexInSlot, bool bRealRepresentationSet);
	void		SectionToSlot(LPCSTR name, const u8 addon_info, bool bRealRepresentationSet);
	void		SectionToSlot(const char *sectionName, bool bRealRepresentationSet);
	void		ClearSlots();
	void		ClearRealRepresentationFlags();

	void		ReloadItemsPrices	();
};