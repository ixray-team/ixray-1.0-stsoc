#if !defined(_SSCONNECTION_H_INCLUDED_)
#define _SSCONNECTION_H_INCLUDED_

#include "stdafx.h"

class CSSConnection
{
protected:
	CComBSTR		mstr_baseLocalDirectory;
	CComBSTR		mstr_baseSourcesafeProject;

	// Visual SourceSafe Database
//	IVSSDatabasePtr mp_vssDatabase;
	IVSSDatabase*  mp_vssDatabase;
	BOOL mb_connected;

public:

	CSSConnection():mb_connected(FALSE)
	{
		mstr_baseSourcesafeProject = _T("$");	// root
		mstr_baseLocalDirectory = _T("");

	}

	virtual ~CSSConnection()
	{
	}

	int		get_ref(IUnknown* a) {
		a->AddRef();
		return a->Release();
	}
	BOOL b_Connect(LPCTSTR psz_User, LPCTSTR psz_Password, LPCTSTR psz_VSSini=NULL)
	{
		mb_connected = FALSE;


		 CLSID clsid;
		 IClassFactory *pClf;
     
		try
		{

			v_TestHr(CLSIDFromProgID(L"SourceSafe", &clsid ));
			v_TestHr(CoGetClassObject( clsid, CLSCTX_ALL, NULL, IID_IClassFactory, (void**)&pClf ));

			v_TestHr(pClf->CreateInstance( NULL, IID_IVSSDatabase, (void **) &mp_vssDatabase));

         
			v_TestHr(mp_vssDatabase->Open((CComBSTR)psz_VSSini, 
				(CComBSTR)psz_User, 
				(CComBSTR)psz_Password));

			mb_connected = TRUE;
		}
		catch (...)
		{
			b_DisplayAnyError();
		}

		return mb_connected;
	}

	BOOL b_IsConnected()
	{
		return mb_connected;
	}

	CString str_GetBaseSourcesafeProject()
	{
		CW2A pszA( mstr_baseSourcesafeProject );
		return CString(pszA);
//		return CString(static_cast<LPCTSTR>(mstr_baseSourcesafeProject));
	}

	CString str_GetBaseLocalDirectory()
	{
		CW2A pszA( mstr_baseLocalDirectory );
		return CString(pszA);
//		return CString(static_cast<LPCTSTR>(mstr_baseLocalDirectory));
	}

	IVSSDatabasePtr p_GetSourcesafeDatabase()
	{
		get_ref(mp_vssDatabase);
		return mp_vssDatabase;
	}

	CString str_GetSrcSafeIni()
	{
		ATLASSERT(mp_vssDatabase!=NULL);
		CComBSTR str_ini;
		mp_vssDatabase->get_SrcSafeIni(&str_ini);

		CW2A pszA( str_ini );
		return CString(pszA);
//		return CString(static_cast<LPCTSTR>(str_ini));
	}

	CString str_GetUser()
	{
		ATLASSERT(mp_vssDatabase!=NULL);
		CComBSTR str_user;
		mp_vssDatabase->get_UserName(&str_user);

		CW2A pszA( str_user );
		return CString(pszA);
//		return CString(static_cast<LPCTSTR>(str_user));
	}

	BOOL b_SetSourcesafeProject(LPCTSTR psz_BaseSourcesafeProject=NULL)
	{
		try
		{
			mstr_baseSourcesafeProject = psz_BaseSourcesafeProject;

			IVSSItemPtr vss_Item = NULL;
			if (psz_BaseSourcesafeProject==NULL)
			{
				mp_vssDatabase->get_CurrentProject(&mstr_baseSourcesafeProject);
			}

			mp_vssDatabase->get_VSSItem(mstr_baseSourcesafeProject,FALSE, &vss_Item);
			if (vss_Item == NULL)
			{
				CString str_error;
				str_error.Format(TEXT("%s was not found in the Sourcesafe database."), mstr_baseSourcesafeProject);
				throw str_error;
			}

			int i_type;
			vss_Item->get_Type(&i_type);
			if (i_type != VSSITEM_PROJECT)
			{
				CString str_error;
				str_error.Format(TEXT("%s is not a Sourcesafe Project."), mstr_baseSourcesafeProject);
				throw str_error;
			}

			vss_Item->get_Spec(&mstr_baseSourcesafeProject);
			vss_Item->get_LocalSpec(&mstr_baseLocalDirectory);
			mp_vssDatabase->put_CurrentProject(mstr_baseSourcesafeProject);

			return TRUE;
		}
		catch (...)
		{
			b_DisplayAnyError();
		}

		return FALSE;
	}

};


#endif // _SSCONNECTION_H_INCLUDED_