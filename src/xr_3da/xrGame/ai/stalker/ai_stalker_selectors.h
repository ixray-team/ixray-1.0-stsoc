#pragma once

class CStalkerSelectorFreeHunting : public CAISelectorBase
{
	public:
		CStalkerSelectorFreeHunting();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

class CStalkerSelectorReload : public CAISelectorBase
{
	public:
		CStalkerSelectorReload();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

class CStalkerSelectorRetreat : public CAISelectorBase
{
	public:
		CStalkerSelectorRetreat();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

class CStalkerSelectorAttack : public CAISelectorBase
{
	public:
		CStalkerSelectorAttack();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

