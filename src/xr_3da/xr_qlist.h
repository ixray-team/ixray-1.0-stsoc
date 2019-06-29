#ifndef __XR_QLIST_H__
#define __XR_QLIST_H__

//-----------------------------------------------------------------------------------------------------------
//square _list
//-----------------------------------------------------------------------------------------------------------
template <class TYPE> class		CQList
{
	xr_vector<TYPE>	data;
public:
	int				z_count, x_count;
public:
					CQList				( );
					~CQList				( );

	IC		void	SetSize					( int x, int z );
	IC		TYPE	&operator()				( int x, int z )	
	{ 
		clamp(z,0,z_count-1); 
		clamp(x,0,x_count-1); 
		return data[z*x_count+x]; 
	}
};

//----------------------------------------------------------------------
// Class	: CXR_QList
//----------------------------------------------------------------------
template <class TYPE>
CQList<TYPE>::CQList	( ) {
	x_count						= 0;													// initial count
	z_count						= 0;													// initial count
}
//----------------------------------------------------------------------
// wt_array destructor
//----------------------------------------------------------------------
template <class TYPE>
CQList<TYPE>::~CQList	( )
{
	data.clear		();
}

//----------------------------------------------------------------------
// create array.
// after resize all data losing
//----------------------------------------------------------------------
template <class TYPE>
void CQList<TYPE>::SetSize		( int x, int z )
{
	VERIFY						( (x > 0) && (z > 0) );

	data.clear					();
	data.resize					(x*z);
	x_count						= x;
	z_count						= z;
}

#endif //__XR_QLIST_H__
