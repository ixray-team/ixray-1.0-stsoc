#ifndef EToolsH
#define EToolsH

#ifdef ETOOLS_EXPORTS
#define ETOOLS_API __declspec( dllexport )
#else
#define ETOOLS_API __declspec( dllimport )
#endif

#include "xrCDB.h"

extern "C" {
	// fast functions
	namespace ETOOLS{
		ETOOLS_API bool 			TestRayTriA		(const Fvector& C, const Fvector& D, Fvector** p, float& u, float& v, float& range, bool bCull);
		ETOOLS_API bool 			TestRayTriB		(const Fvector& C, const Fvector& D, Fvector* p, float& u, float& v, float& range, bool bCull);
		ETOOLS_API bool 			TestRayTri2		(const Fvector& C, const Fvector& D, Fvector* p, float& range);
		
		typedef  void __stdcall		pb_callback		(void* user_data, float& val);
		ETOOLS_API void				SimplifyCubeMap	(u32* src_data, u32 src_width, u32 src_height, u32* dst_data, u32 dst_width, u32 dst_height, float sample_factor=1.f, pb_callback cb=0, void* pb_data=0);

		ETOOLS_API CDB::COLLIDER*	get_collider	();
		ETOOLS_API CDB::MODEL*		create_model	(Fvector* V, int Vcnt, CDB::TRI* T, int Tcnt);
		ETOOLS_API void				destroy_model	(CDB::MODEL*&);
		ETOOLS_API CDB::RESULT*		r_begin			();
		ETOOLS_API CDB::RESULT*		r_end			(); 
		ETOOLS_API int				r_count			();
		ETOOLS_API void 			ray_options		(u32 flags);
		ETOOLS_API void				ray_query		(const CDB::MODEL *m_def, const Fvector& r_start,  const Fvector& r_dir, float r_range);
		ETOOLS_API void				ray_query_m		(const Fmatrix& inv_parent, const CDB::MODEL *m_def, const Fvector& r_start,  const Fvector& r_dir, float r_range);
		ETOOLS_API void 			box_options		(u32 flags);
		ETOOLS_API void				box_query		(const CDB::MODEL *m_def, const Fvector& b_center, const Fvector& b_dim);
		ETOOLS_API void				box_query_m		(const Fmatrix& inv_parent, const CDB::MODEL *m_def, const Fbox& src);
	};
};

#endif // EToolsH