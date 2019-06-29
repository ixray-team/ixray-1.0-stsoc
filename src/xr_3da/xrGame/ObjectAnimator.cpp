#include "stdafx.h"
#include "ObjectAnimator.h"
#include "motion.h"
 
bool st_AnimParam::Update(float dt){ 
	t+=dt; 
	if (t>max_t){ 
		if (bLoop){ t=min_t; return true; }
		else	  { t=max_t; return false;}
	}
	return true;
}
void st_AnimParam::Set(COMotion* M, bool _loop){ 
	t=0; 
	bLoop = _loop;
    min_t=(float)M->FrameStart()/M->FPS(); 
    max_t=(float)M->FrameEnd()/M->FPS();
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CObjectAnimator::CObjectAnimator()
{
}

CObjectAnimator::~CObjectAnimator()
{	
	for(MotionPairIt m_it=m_Motions.begin(); m_it!=m_Motions.end(); m_it++){
		free(m_it->first);
		_DELETE(m_it->second);
	}
	m_Motions.clear		();
}

void CObjectAnimator::SetActiveMotion(COMotion* mot, bool bLoop){
	m_ActiveMotion		=mot;
    if (m_ActiveMotion) m_MParam.Set(m_ActiveMotion, bLoop);
	vPosition.set		(0,0,0);
	mRotate.identity	();
}

void CObjectAnimator::LoadMotions(const char* fname){
	FILE_NAME			full_path;
	if (!Engine.FS.Exist( full_path, Path.Current, fname ))
		if (!Engine.FS.Exist( full_path, Path.Meshes, fname ))
			Device.Fatal("Can't find motion file '%s'.",fname);

	CFileStream F		(full_path);
	DWORD dwMCnt		= F.Rdword(); VERIFY(dwMCnt);
	for (DWORD i=0; i<dwMCnt; i++){
		COMotion* M		= new COMotion();
		M->Load			(F);
		m_Motions[strdup(M->Name())]=M;
	}
}

void CObjectAnimator::Load(CInifile* ini, const char * section)
{
	LPCSTR temp			= ini->ReadSTRING(section,"motions");
	LoadMotions			(temp);
	SetActiveMotion		(0,false);
}

void CObjectAnimator::OnMove()
{
	if (IsMotionActive()){
		Fvector R;
		m_ActiveMotion->Evaluate(m_MParam.Frame(),vPosition,R);
		if (!m_MParam.Update(Device.fTimeDelta)) StopMotion();
		mRotate.setHPB	(R.x,R.y,R.z);
	}
}

COMotion* CObjectAnimator::PlayMotion(LPCSTR name, bool bLoop)
{
	MotionPairIt I = m_Motions.find(LPSTR(name));
	if (I!=m_Motions.end())	SetActiveMotion(I->second, bLoop);
	else {
		Msg("OBJ ANIM::Cycle '%s' not found.",name);
		return NULL;
	}
	return I->second;
}

void CObjectAnimator::StopMotion(){
	SetActiveMotion		(0);
}

void CObjectAnimator::ManualUpdate(COMotion* M, float t){
	Fvector R;
	m_ActiveMotion->Evaluate(t,vPosition,R);
	mRotate.setHPB		(R.x,R.y,R.z);
}
/*
#include "stdafx.h"
#include "ObjectAnimator.h"
#include "motion.h"

//---------------------------------------------------------------------
class fMNameEQ {
	const char*	name;
public:
	fMNameEQ(char *N) : name(N) {};
	IC int operator() (CEMotion* M) { return strcmp(M->Name(),name); }
}; 
//---------------------------------------------------------------------
bool st_AnimParam::Update(float dt){ 
	t+=dt; 
	if (t>max_t){ 
		if (bLoop){ t=min_t; return true; }
		else	  { t=max_t; return false;}
	}
	return true;
}
void st_AnimParam::Set(CEMotion* M, bool _loop){ 
	t=0; 
	bLoop = _loop;
    min_t=(float)M->FrameStart()/M->FPS(); 
    max_t=(float)M->FrameEnd()/M->FPS();
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CObjectAnimator::CObjectAnimator()
{
}

CObjectAnimator::~CObjectAnimator()
{	
	for(MotionIt m_it=m_Motions.begin(); m_it!=m_Motions.end(); m_it++)
		_DELETE(*m_it);
	m_Motions.clear		();
}

void CObjectAnimator::SetActiveMotion(CEMotion* mot, bool bLoop){
	m_ActiveMotion		=mot;
    if (m_ActiveMotion) m_MParam.Set(m_ActiveMotion, bLoop);
	vPosition.set		(0,0,0);
	mRotate.identity	();
}

void CObjectAnimator::LoadMotions(const char* fname){
	FILE_NAME			full_path;
	if (!Engine.FS.Exist( full_path, LEVEL_PATH, fname ))
		if (!Engine.FS.Exist( full_path, MESHES_PATH, fname )){
			Msg("Can't find motion file '%s'.",fname);
			THROW;
		}

	CFileStream F		(full_path);
	DWORD dwMCnt		= F.Rdword(); VERIFY(dwMCnt);
	for (int i=0; i<dwMCnt; i++){
		CEMotion* M		= new CEMotion();
		M->Load			(F);
		m_Motions.push_back(M);
	}
}

void CObjectAnimator::Load(CInifile* ini, const char * section)
{
	LPCSTR temp			= ini->ReadSTRING(section,"motions");
	LoadMotions			(temp);
	SetActiveMotion		(0,false);
}

void CObjectAnimator::OnMove()
{
	if (IsMotionActive()){
		Fvector R;
		m_ActiveMotion->Evaluate(0,m_MParam.Frame(),vPosition,R);
		if (!m_MParam.Update(Device.fTimeDelta)) StopMotion();
		mRotate.setHPB	(R.x,R.y,R.z);
	}
}

CEMotion* CObjectAnimator::FindMotionByName(LPCSTR name){
	for (MotionIt m_it=m_Motions.begin(); m_it!=m_Motions.end(); m_it++)
		if (strcmp((*m_it)->Name(), name)==0) return *m_it;
	return 0;
}

CEMotion* CObjectAnimator::PlayMotion(LPCSTR name, bool bLoop)
{
	CEMotion* M = FindMotionByName(name);
	if (M)	SetActiveMotion(M, bLoop);
	else {
		Msg("OBJ ANIM::Cycle '%s' not found.",name);
		return NULL;
	}
	return M;
}

void CObjectAnimator::StopMotion(){
	SetActiveMotion		(0);
}

void CObjectAnimator::ManualUpdate(CEMotion* M, float t){
	Fvector R;
	m_ActiveMotion->Evaluate(0,t,vPosition,R);
	mRotate.setHPB		(R.x,R.y,R.z);
}
*/