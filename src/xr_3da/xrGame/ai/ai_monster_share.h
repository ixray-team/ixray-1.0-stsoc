#pragma once

// Singleton template definition 
template <class T> class IShared {
	static T*	_self;
	static int	_refcount;
public:
					IShared	()	{}
			virtual	~IShared()	{_self=NULL;}
public:
	static T*		Instance() {
		if(!_self) _self=xr_new<T>(); 
		++_refcount;
		return _self;
	}
	void			FreeInst() {
		if(0 == --_refcount) {
			IShared<T> *ptr = this;
			xr_delete(ptr);
		} 
	}
};

template <class T> T*	IShared<T>::_self = NULL;
template <class T> int	IShared<T>::_refcount=0;

template<class T_shared> class CSharedObj : public IShared<CSharedObj<T_shared> >
{
	xr_map<CLASS_ID, T_shared*> _shared_tab;	
	typedef typename xr_map<CLASS_ID, T_shared*>::iterator SHARED_DATA_MAP_IT;
	
public:
				CSharedObj() {};
				~CSharedObj() {};
	
	friend class IShared<CSharedObj<T_shared> >;

public:
	// Access to data
	T_shared *get_shared(CLASS_ID id);
};

template<class T_shared> T_shared *CSharedObj<T_shared>::get_shared(CLASS_ID id) 
{
	SHARED_DATA_MAP_IT shared_it = _shared_tab.find(id);

	T_shared *_data;

	// if not found - create appropriate shared data object
	if (_shared_tab.end() == shared_it) {
		_data		= xr_new<T_shared>();
		_shared_tab.insert(mk_pair(id, _data));
	} else _data = shared_it->second;

	return _data;
}

class CSharedResource {
	bool	loaded;
public:
			CSharedResource	() {loaded = false;}

	bool	IsLoaded		() {return loaded;}
	void	SetLoad			(bool l = true) {loaded = l;}
};


template<class shared_struc> class CSharedClass {
public:
	shared_struc				*_sd;
	CSharedObj<shared_struc>	*pSharedObj;

			CSharedClass	() {pSharedObj	= CSharedObj<shared_struc>::Instance();}
			~CSharedClass	() {pSharedObj->FreeInst();}

	bool	IsLoaded		() {return _sd->IsLoaded();}
	void	Prepare			(CLASS_ID cls_id)	{_sd = pSharedObj->get_shared(cls_id);}
	void	Finish			()	{_sd->SetLoad();}

	
	void	load_shared		(CLASS_ID cls_id, LPCSTR section) {
		_sd = pSharedObj->get_shared(cls_id);
		if (!_sd->IsLoaded()) {
			load_shared(section);
			_sd->SetLoad();
		}
	}

	virtual void load_shared (LPCSTR section) {}
};


