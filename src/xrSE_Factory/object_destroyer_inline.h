////////////////////////////////////////////////////////////////////////////
//	Module 		: object_destroyer_inline.h
//	Created 	: 21.01.2003
//  Modified 	: 09.07.2004
//	Author		: Dmitriy Iassenev
//	Description : Object destroyer inline functions
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_OBJECT_DESTROYER_INLINE
#define XRAY_OBJECT_DESTROYER_INLINE

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
	xr_set<T1,T2>::iterator	I = tpSet.begin();
	xr_set<T1,T2>::iterator	E = tpSet.end();
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

IC	void delete_data(LPSTR &tpData)
{
	xr_free						(tpData);
}

#endif // XRAY_OBJECT_DESTROYER_INLINE