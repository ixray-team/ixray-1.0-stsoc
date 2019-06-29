////////////////////////////////////////////////////////////////////////////
//	Module 		: object_saver_inline.h
//	Created 	: 21.01.2003
//  Modified 	: 09.07.2004
//	Author		: Dmitriy Iassenev
//	Description : Object saver inline functions
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_OBJECT_SAVER_INLINE
#define XRAY_OBJECT_SAVER_INLINE

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

#endif // XRAY_OBJECT_SAVER_INLINE