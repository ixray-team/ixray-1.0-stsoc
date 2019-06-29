////////////////////////////////////////////////////////////////////////////
//	Module 		: object_equality_checker.h
//	Created 	: 21.01.2003
//  Modified 	: 12.07.2004
//	Author		: Dmitriy Iassenev
//	Description : Object equality checker
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_OBJECT_EQUALITY_CHECKER
#define XRAY_OBJECT_EQUALITY_CHECKER

struct CEqualityChecker {
	template <typename T>
	struct CHelper {
		template <bool a>
		IC	static bool equal(const T &_1, const T &_2)
		{
			return			(_1 == _2);
		}

		template <>
		IC	static bool equal<true>(const T &_1, const T &_2)
		{
			return			(CEqualityChecker::equal(*_1,*_2));
		}
	};

	IC	static bool equal(LPCSTR &_1, LPCSTR &_2)
	{
		return						(!xr_strcmp(_1,_2));
	}

	IC	static bool equal(LPSTR &_1, LPSTR &_2)
	{
		return						(!xr_strcmp(_1,_2));
	}

	IC	static bool equal(ref_str &_1, ref_str &_2)
	{
		return						(!xr_strcmp(_1,_2));
	}

	template <typename T1, typename T2>
	IC	static bool equal(const std::pair<T1,T2> &_1, const std::pair<T1,T2> &_2)
	{
		return						(
			equal(_1.first,_2.first)
			&&
			equal(_1.second,_2.second)
		);
	}

	template <typename T, int size>
	IC	static bool equal(const svector<T,size> &_1, const svector<T,size> &_2)
	{
		if (_1.size() != _2.size())
			return					(false);
		
		svector<T,size>::const_iterator	I = _1.begin(), J = _2.begin();
		svector<T,size>::const_iterator	E = _1.end();
		for ( ; I != E; ++I, ++J)
			if (!equal(*I,*J))
				return				(false);
		return						(true);
	}

	template <typename T1, typename T2>
	IC	static bool equal(const std::queue<T1,T2> &__1, const std::queue<T1,T2> &__2)
	{
		std::queue<T1,T2>			_1 = __1;
		std::queue<T1,T2>			_2 = __2;
		
		if (_1.size() != _2.size())
			return					(false);

		for ( ; !_1.empty(); _1.pop(), _2.pop())
			if (!equal(_1.front(),_2.front()))
				return				(false);
		return						(true);
	}

	template <template <typename _1, typename _2> class T1, typename T2, typename T3>
	IC	static bool equal(const T1<T2,T3> &__1, const T1<T2,T3> &__2, bool)
	{
		T1<T2,T3>					_1 = __1;
		T1<T2,T3>					_2 = __2;

		if (_1.size() != _2.size())
			return					(false);

		for ( ; !_1.empty(); _1.pop(), _2.pop())
			if (!equal(_1.top(),_2.top()))
				return				(false);
		return						(true);
	}

	template <template <typename _1, typename _2, typename _3> class T1, typename T2, typename T3, typename T4>
	IC	static bool equal(const T1<T2,T3,T4> &__1, const T1<T2,T3,T4> &__2, bool)
	{
		T1<T2,T3,T4>				_1 = __1;
		T1<T2,T3,T4>				_2 = __2;

		if (_1.size() != _2.size())
			return					(false);

		for ( ; !_1.empty(); _1.pop(), _2.pop())
			if (!equal(_1.top(),_2.top()))
				return				(false);
		return						(true);
	}

	template <typename T1, typename T2>
	IC	static bool equal(const xr_stack<T1,T2> &_1, const xr_stack<T1,T2> &_2)
	{
		return					(equal(_1,_2,true));
	}

	template <typename T1, typename T2, typename T3>
	IC	static bool equal(const std::priority_queue<T1,T2,T3> &_1, const std::priority_queue<T1,T2,T3> &_2)
	{
		return					(equal(_1,_2,true));
	}

	struct CHelper3 {
		template <template <typename _1> class T1, typename T2>
		IC	static bool equal(const T1<T2> &_1, const T1<T2> &_2)
		{
			if (_1.size() != _2.size())
				return						(false);

			T1<T2>::const_iterator			I = _1.begin(), J = _2.begin();
			T1<T2>::const_iterator			E = _1.end();
			for ( ; I != E; ++I, ++J)
				if (!CEqualityChecker::equal(*I,*J))
					return					(false);
			return							(true);
		}

		template <template <typename _1, typename _2> class T1, typename T2, typename T3>
		IC	static bool equal(const T1<T2,T3> &_1, const T1<T2,T3> &_2)
		{
			if (_1.size() != _2.size())
				return					(false);

			T1<T2,T3>::const_iterator			I = _1.begin(), J = _2.begin();
			T1<T2,T3>::const_iterator			E = _1.end();
			for ( ; I != E; ++I, ++J)
				if (!CEqualityChecker::equal(*I,*J))
					return					(false);
			return							(true);
		}

		template <template <typename _1, typename _2, typename _3> class T1, typename T2, typename T3, typename T4>
		IC	static bool equal(const T1<T2,T3,T4> &_1, const T1<T2,T3,T4> &_2)
		{
			if (_1.size() != _2.size())
				return					(false);

			T1<T2,T3,T4>::const_iterator		I = _1.begin(), J = _2.begin();
			T1<T2,T3,T4>::const_iterator		E = _1.end();
			for ( ; I != E; ++I, ++J)
				if (!CEqualityChecker::equal(*I,*J))
					return					(false);
			return							(true);
		}

		template <template <typename _1, typename _2, typename _3, typename _4> class T1, typename T2, typename T3, typename T4, typename T5>
		IC	static bool equal(const T1<T2,T3,T4,T5> &_1, const T1<T2,T3,T4,T5> &_2)
		{
			if (_1.size() != _2.size())
				return					(false);

			T1<T2,T3,T4,T5>::const_iterator	I = _1.begin(), J = _2.begin();
			T1<T2,T3,T4,T5>::const_iterator	E = _1.end();
			for ( ; I != E; ++I, ++J)
				if (!CEqualityChecker::equal(*I,*J))
					return					(false);
			return							(true);
		}
	};

	template <typename T>
	struct CHelper4 {
		template <bool a>
		IC	static bool equal(const T &_1, const T &_2)
		{
			return(CHelper<T>::equal<object_type_traits::is_pointer<T>::value>(_1,_2));
		}

		template <>
		IC	static bool equal<true>(const T &_1, const T &_2)
		{
			return(CHelper3::equal(_1,_2));
		}
	};

	template <typename T>
	IC	static bool equal(const T &_1, const T &_2)
	{
		return						(CHelper4<T>::equal<object_type_traits::is_stl_container<T>::value>(_1,_2));
	}
};

IC	bool equal(LPCSTR p0, LPCSTR p1)
{
	return			(CEqualityChecker::equal(p0,p1));
}

template <typename T>
IC	bool equal(const T &p0, const T &p1)
{
	return			(CEqualityChecker::equal(p0,p1));
}

#endif // XRAY_OBJECT_EQUALITY_CHECKER