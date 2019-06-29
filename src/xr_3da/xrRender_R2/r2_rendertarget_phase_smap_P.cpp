#include "stdafx.h"

void	CRenderTarget::phase_smap_point	(light* L, u32 pls_phase)
{
	// Targets
	u_setrt								(PSM_size,PSM_size,rt_smap_p->pRT[pls_phase], NULL, NULL, rt_smap_p_ZB);

	// Clear
	CHK_DX(HW.pDevice->Clear			( 0L, NULL, D3DCLEAR_ZBUFFER, 0xFFFFFFFF, 1.0f, 0L));

	// Stencil	- disable
	RCache.set_Stencil					( FALSE );

	// Misc		- draw only front faces
	RCache.set_CullMode					( CULL_CCW );
	RCache.set_ColorWriteEnable			();	//. should depend on HW-smap implementation
}
