////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_templates.h
//	Created 	: 21.01.2003
//  Modified 	: 21.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life templates
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_ALIFE_TEMPLATES
#define XRAY_AI_ALIFE_TEMPLATES

#include "ai_alife_interfaces.h"

// delete data
extern void delete_data(LPSTR &tpVector);
extern void delete_data(LPCSTR &tpVector);

template <class T1, class T2, class T3>
void delete_data(xr_map<T1,T2,T3> &tpMap)
{
	xr_map<T1,T2,T3>::iterator	I = tpMap.begin();
	xr_map<T1,T2,T3>::iterator	E = tpMap.end();
	for ( ; I != E; ++I) {
		delete_data				((*I).first);
		delete_data				((*I).second);
	}
	tpMap.clear					();
};

template <class T1, class T2>
void delete_data(xr_set<T1,T2> &tpSet)
{
	xr_set<T1,T2>::iterator	I = tpMap.begin();
	xr_set<T1,T2>::iterator	E = tpMap.end();
	for ( ; I != E; ++I)
		delete_data				(*I);
	tpSet.clear					();
};

template <class T> 
void delete_data(xr_vector<T> &tpVector)
{
	xr_vector<T>::iterator		I = tpVector.begin();
	xr_vector<T>::iterator		E = tpVector.end();
	for ( ; I != E; ++I)
		delete_data				(*I);
	tpVector.clear				();
};

template <class T> 
void delete_data(T *&tpData)
{
	delete_data					(*tpData);
	xr_delete					(tpData);
};

template <class T> 
void delete_data(T &tData)
{
};

// save data

template <class T1, class T2, class T3, class M>
void save_data(xr_map<T1,T2,T3> &tpMap, M &tStream, bool bSaveCount = true)
{
	if (bSaveCount)
		tStream.w_u32			((u32)tpMap.size());
	xr_map<T1,T2,T3>::iterator	I = tpMap.begin();
	xr_map<T1,T2,T3>::iterator	E = tpMap.end();
	for ( ; I != E; ++I)
		save_data				((*I).second,tStream,bSaveCount);
};

template <class T1, class T2, class M>
void save_data(xr_set<T1,T2> &tpSet, M &tStream, bool bSaveCount = true)
{
	if (bSaveCount)
		tStream.w_u32			((u32)tpSet.size());
	xr_set<T1,T2>::iterator	I = tpSet.begin();
	xr_set<T1,T2>::iterator	E = tpSet.end();
	for ( ; I != E; ++I)
		save_data				(*I,tStream,bSaveCount);
};

template <class T, class M>
void save_data(xr_vector<T> &tpVector, M &tStream, bool bSaveCount = true)
{
	if (bSaveCount)
		tStream.w_u32			((u32)tpVector.size());
	xr_vector<T>::iterator		I = tpVector.begin();
	xr_vector<T>::iterator		E = tpVector.end();
	for ( ; I != E; ++I)
		save_data				(*I,tStream,bSaveCount);
};

template <class M>
void save_data(xr_vector<bool> &baVector, M &tStream, bool bSaveCount = true)
{
	if (bSaveCount)
		tStream.w_u32			((u32)baVector.size());
	xr_vector<bool>::iterator 	I = baVector.begin();
	xr_vector<bool>::iterator 	E = baVector.end();
	u32							dwMask = 0;
	if (I != E) {
		for (int j=0; I != E; ++I, ++j) {
			if (j >= 32) {
				tStream.w_u32(dwMask);
				dwMask			= 0;
				j				= 0;
			}
			if (*I)
				dwMask			|= u32(1) << j;
		}
		tStream.w_u32		(dwMask);
	}
};

template <class M>
void save_data(IPureServerObject &tData, M &tStream,bool bSaveCount = true)
{
	tData.UPDATE_Write			(tStream);
}

template <typename T, class M>
void save_data(T &tData, M &tStream,bool bSaveCount = true)
{
	tStream.w					(&tData,sizeof(tData));
}

template <class M>
void save_data(LPSTR &tData, M &tStream,bool bSaveCount = true)
{
	tStream.w_string			(tData);
}

template <class T, class M>
void save_data(T *&tpData, M &tStream, bool bSaveCount = true)
{
	tpData->Save				(tStream);
};

template <class T, class M>
void save_data(T **&tpData, M &tStream, bool bSaveCount = true)
{
	save_data					(*tpData,tStream,bSaveCount);
};

template <class T1, class T2, class T3, class M>
void save_data(xr_map<T1,T2,T3> *&tpMap, M &tStream, bool bSaveCount = true)
{
	save_data					(*tpMap,tStream,bSaveCount);
};

template <class T1, class T2, class M>
void save_data(xr_set<T1,T2> *&tpSet, M &tStream, bool bSaveCount = true)
{
	save_data					(*tpSet,tStream,bSaveCount);
};

template <class T, class M>
void save_data(xr_vector<T> *&tpVector, M &tStream, bool bSaveCount = true)
{
	save_data					(*tpVector,tStream,bSaveCount);
};

// load data
template <class T1, class T2, class T3, class M, class P>
void load_data(xr_map<T1,T2,T3> &tpMap, M &tStream, const P &predicate)
{
	tpMap.clear					();
	u32							dwCount	= tStream.r_u32();
	for (int i=0 ; i<(int)dwCount; ++i) {
		T2						T;
		load_data				(T,tStream);
		tpMap.insert			(mk_pair(predicate(T),T));
	}
};

template <class T1, class T2, class T3, class M>
void load_data(xr_map<T1,T2,T3> &tpMap, M &tStream, const T1 tfGetKey(const T2))
{
	tpMap.clear					();
	u32							dwCount	= tStream.r_u32();
	for (int i=0 ; i<(int)dwCount; ++i) {
		T2						T;
		load_data				(T,tStream);
		tpMap.insert			(mk_pair(tfGetKey(T),T));
	}
};

template <class T1, class T2, class T3, class M>
void load_data(xr_map<T1,T2*,T3> &tpMap, M &tStream, const T1 tfGetKey(const T2*))
{
	tpMap.clear					();
	u32							dwCount	= tStream.r_u32();
	for (int i=0 ; i<(int)dwCount; ++i) {
		T2						*T;
		load_data				(T,tStream);
		tpMap.insert			(mk_pair(tfGetKey(T),T));
	}
};

template <class T1, class T2, class T3, class M>
void load_data(xr_map<T1,T2,T3> &tpMap, M &tStream, bool bSaveCount = true)
{
	xr_map<T1,T2,T3>::iterator	I = tpMap.begin();
	xr_map<T1,T2,T3>::iterator	E = tpMap.end();
	for ( ; I != E; ++I)
		load_data				((*I).second,tStream,bSaveCount);
};

template <class T1, class T2, class M>
void load_data(xr_set<T1,T2> &tpSet, M &tStream, bool bSaveCount = true)
{
	xr_set<T1,T2>::iterator	I = tpSet.begin();
	xr_set<T1,T2>::iterator	E = tpSet.end();
	for ( ; I != E; ++I)
		load_data				(*I,tStream,bSaveCount);
};

template <class T, class M>
void load_data(xr_vector<T> &tpVector, M &tStream, bool bSaveCount = true)
{
	if (bSaveCount)
		tpVector.resize			(tStream.r_u32());
	xr_vector<T>::iterator		I = tpVector.begin();
	xr_vector<T>::iterator		E = tpVector.end();
	for ( ; I != E; ++I)
		load_data				(*I,tStream,bSaveCount);
};

template <class M>
void load_data(xr_vector<bool> &baVector, M &tStream, bool bSaveCount = true)
{
	if (bSaveCount)
		baVector.resize			(tStream.r_u32());
	xr_vector<bool>::iterator 	I = baVector.begin();
	xr_vector<bool>::iterator 	E = baVector.end();
	u32							dwMask = 0;
	for (int j=32; I != E; ++I, ++j) {
		if (j >= 32) {
			dwMask				= tStream.r_u32();
			j					= 0;
		}
		*I						= !!(dwMask & (u32(1) << j));
	}
};

template <class M>
void load_data(LPSTR &tpData, M &tStream, bool bSaveCount = true)
{
	string4096					S;
	tStream.r_string			(S);
	tpData						= xr_strdup(S);
};

template <class T, class M>
void load_data(T &tData, M &tStream, bool bSaveCount = true)
{
	tStream.r					(&tData,sizeof(tData));
}

template <class T, class M>
void load_data(T *&tpData, M &tStream, bool bSaveCount = true)
{
	if (bSaveCount)
		tpData = xr_new<T>();
	tpData->Load				(tStream);
};

template <class T, class M>
void load_data(T **&tpData, M &tStream, bool bSaveCount = true)
{
	if (bSaveCount)
		tpData = xr_new<T>();
	load_data					(*tpData,tStream,bSaveCount);
};

template <class T1, class T2, class T3, class M>
void load_data(xr_map<T1,T2,T3> *&tpMap, M &tStream, const T1 tfGetKey(const T2))
{
	if (bSaveCount)
		tpData = xr_new<T>();
	load_data					(*tpMap,tStream,tfGetKey);
};

template <class T1, class T2, class T3, class M, class P>
void load_data(xr_map<T1,T2,T3> *&tpMap, M &tStream, const P &predicate)
{
	if (bSaveCount)
		tpData = xr_new<T>();
	load_data					(*tpMap,tStream,predicate);
};

template <class T1, class T2, class T3, class M>
void load_data(xr_map<T1,T2,T3> *&tpMap, M &tStream, bool bSaveCount = true)
{
	if (bSaveCount)
		tpData = xr_new<T>();
	load_data					(*tpMap,tStream,bSaveCount);
};

template <class T, class M>
void load_data(xr_vector<T> *&tpVector, M &tStream, bool bSaveCount = true)
{
	if (bSaveCount)
		tpData = xr_new<T>();
	load_data					(*tpVector,tStream,bSaveCount);
};

#endif