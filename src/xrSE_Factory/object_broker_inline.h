////////////////////////////////////////////////////////////////////////////
//	Module 		: object_broker_inline.h
//	Created 	: 21.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Object broker inline functions
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_OBJECT_BROKER_INLINE
#define XRAY_OBJECT_BROKER_INLINE

template <class T1, class T2, class T3>
IC	void delete_data(xr_map<T1,T2,T3> &tpMap)
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
IC	void delete_data(xr_set<T1,T2> &tpSet)
{
	xr_set<T1,T2>::iterator	I = tpMap.begin();
	xr_set<T1,T2>::iterator	E = tpMap.end();
	for ( ; I != E; ++I)
		delete_data				(*I);
	tpSet.clear					();
};

template <class T> 
IC	void delete_data(xr_vector<T> &tpVector)
{
	xr_vector<T>::iterator		I = tpVector.begin();
	xr_vector<T>::iterator		E = tpVector.end();
	for ( ; I != E; ++I)
		delete_data				(*I);
	tpVector.clear				();
};

template <class T> 
IC	void delete_data(T *&tpData)
{
	delete_data					(*tpData);
	xr_delete					(tpData);
};

template <class T> 
IC	void delete_data(T &tData)
{
};

template <class T1, class T2, class T3, class M>
IC	void save_data(xr_map<T1,T2,T3> &tpMap, M &tStream, bool bSaveCount)
{
	if (bSaveCount)
		tStream.w_u32			((u32)tpMap.size());
	xr_map<T1,T2,T3>::iterator	I = tpMap.begin();
	xr_map<T1,T2,T3>::iterator	E = tpMap.end();
	for ( ; I != E; ++I)
		save_data				((*I).second,tStream,bSaveCount);
};

template <class T1, class T2, class M>
IC	void save_data(xr_set<T1,T2> &tpSet, M &tStream, bool bSaveCount)
{
	if (bSaveCount)
		tStream.w_u32			((u32)tpSet.size());
	xr_set<T1,T2>::iterator	I = tpSet.begin();
	xr_set<T1,T2>::iterator	E = tpSet.end();
	for ( ; I != E; ++I)
		save_data				(*I,tStream,bSaveCount);
};

template <class T, class M>
IC	void save_data(xr_vector<T> &tpVector, M &tStream, bool bSaveCount)
{
	if (bSaveCount)
		tStream.w_u32			((u32)tpVector.size());
	xr_vector<T>::iterator		I = tpVector.begin();
	xr_vector<T>::iterator		E = tpVector.end();
	for ( ; I != E; ++I)
		save_data				(*I,tStream,bSaveCount);
};

template <class M>
IC	void save_data(xr_vector<bool> &baVector, M &tStream, bool bSaveCount)
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
IC	void save_data(IPureServerObject &tData, M &tStream,bool bSaveCount)
{
	tData.UPDATE_Write			(tStream);
}

template <typename T, class M>
IC	void save_data(T &tData, M &tStream,bool bSaveCount)
{
	tStream.w					(&tData,sizeof(tData));
}

template <class M>
IC	void save_data(LPSTR &tData, M &tStream,bool bSaveCount)
{
	tStream.w_string			(tData);
}

template <class T, class M>
IC	void save_data(T *&tpData, M &tStream, bool bSaveCount)
{
	tpData->save				(tStream);
};

template <class T, class M>
IC	void save_data(T **&tpData, M &tStream, bool bSaveCount)
{
	save_data					(*tpData,tStream,bSaveCount);
};

template <class T1, class T2, class T3, class M>
IC	void save_data(xr_map<T1,T2,T3> *&tpMap, M &tStream, bool bSaveCount)
{
	save_data					(*tpMap,tStream,bSaveCount);
};

template <class T1, class T2, class M>
IC	void save_data(xr_set<T1,T2> *&tpSet, M &tStream, bool bSaveCount)
{
	save_data					(*tpSet,tStream,bSaveCount);
};

template <class T, class M>
IC	void save_data(xr_vector<T> *&tpVector, M &tStream, bool bSaveCount)
{
	save_data					(*tpVector,tStream,bSaveCount);
};

template <class T1, class T2, class T3, class M, class P>
IC	void load_data(xr_map<T1,T2,T3> &tpMap, M &tStream, const P &predicate)
{
	tpMap.clear					();
	u32							dwCount	= tStream.r_u32();
	for (int i=0 ; i<(int)dwCount; ++i) {
		T2						T;
		load_data				(T,tStream);
		T1						temp = predicate(T);
		VERIFY					(tpMap.find(temp) == tpMap.end());
		tpMap.insert			(mk_pair(temp,T));
	}
};

template <class T1, class T2, class T3, class M>
IC	void load_data(xr_map<T1,T2,T3> &tpMap, M &tStream, const T1 tfGetKey(const T2))
{
	tpMap.clear					();
	u32							dwCount	= tStream.r_u32();
	for (int i=0 ; i<(int)dwCount; ++i) {
		T2						T;
		load_data				(T,tStream);
		T1						temp = tfGetKey(T);
		VERIFY					(tpMap.find(temp) == tpMap.end());
		tpMap.insert			(mk_pair(temp,T));
	}
};

template <class T1, class T2, class T3, class M>
IC	void load_data(xr_map<T1,T2*,T3> &tpMap, M &tStream, const T1 tfGetKey(const T2*))
{
	tpMap.clear					();
	u32							dwCount	= tStream.r_u32();
	for (int i=0 ; i<(int)dwCount; ++i) {
		T2						*T;
		load_data				(T,tStream);
		T1						temp = tfGetKey(T);
		VERIFY					(tpMap.find(temp) == tpMap.end());
		tpMap.insert			(mk_pair(temp,T));
	}
};

template <class T1, class T2, class T3, class M>
IC	void load_data(xr_map<T1,T2,T3> &tpMap, M &tStream, bool bSaveCount)
{
	xr_map<T1,T2,T3>::iterator	I = tpMap.begin();
	xr_map<T1,T2,T3>::iterator	E = tpMap.end();
	for ( ; I != E; ++I)
		load_data				((*I).second,tStream,bSaveCount);
};

template <class T1, class T2, class M>
IC	void load_data(xr_set<T1,T2> &tpSet, M &tStream, bool bSaveCount)
{
	xr_set<T1,T2>::iterator	I = tpSet.begin();
	xr_set<T1,T2>::iterator	E = tpSet.end();
	for ( ; I != E; ++I)
		load_data				(*I,tStream,bSaveCount);
};

template <class T, class M>
IC	void load_data(xr_vector<T> &tpVector, M &tStream, bool bSaveCount)
{
	if (bSaveCount)
		tpVector.resize			(tStream.r_u32());
	xr_vector<T>::iterator		I = tpVector.begin();
	xr_vector<T>::iterator		E = tpVector.end();
	for ( ; I != E; ++I)
		load_data				(*I,tStream,bSaveCount);
};

template <class M>
IC	void load_data(xr_vector<bool> &baVector, M &tStream, bool bSaveCount)
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
IC	void load_data(LPSTR &tpData, M &tStream, bool bSaveCount)
{
	string4096					S;
	tStream.r_string			(S);
	tpData						= xr_strdup(S);
};

template <class T, class M>
IC	void load_data(T &tData, M &tStream, bool bSaveCount)
{
	tStream.r					(&tData,sizeof(tData));
}

template <class T, class M>
IC	void load_data(T *&tpData, M &tStream, bool bSaveCount)
{
	if (bSaveCount)
		tpData = xr_new<T>();
	tpData->load				(tStream);
};

template <class T, class M>
IC	void load_data(T **&tpData, M &tStream, bool bSaveCount)
{
	if (bSaveCount)
		tpData = xr_new<T>();
	load_data					(*tpData,tStream,bSaveCount);
};

template <class T1, class T2, class T3, class M>
IC	void load_data(xr_map<T1,T2,T3> *&tpMap, M &tStream, const T1 tfGetKey(const T2))
{
	if (bSaveCount)
		tpData = xr_new<T>();
	load_data					(*tpMap,tStream,tfGetKey);
};

template <class T1, class T2, class T3, class M, class P>
IC	void load_data(xr_map<T1,T2,T3> *&tpMap, M &tStream, const P &predicate)
{
	if (bSaveCount)
		tpData = xr_new<T>();
	load_data					(*tpMap,tStream,predicate);
};

template <class T1, class T2, class T3, class M>
IC	void load_data(xr_map<T1,T2,T3> *&tpMap, M &tStream, bool bSaveCount)
{
	if (bSaveCount)
		tpData = xr_new<T>();
	load_data					(*tpMap,tStream,bSaveCount);
};

template <class T, class M>
IC	void load_data(xr_vector<T> *&tpVector, M &tStream, bool bSaveCount)
{
	if (bSaveCount)
		tpData = xr_new<T>();
	load_data					(*tpVector,tStream,bSaveCount);
};

#endif // XRAY_OBJECT_BROKER_INLINE