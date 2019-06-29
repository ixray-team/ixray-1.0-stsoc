#ifndef PHDEFS_H
#define PHDEFS_H
class CPHElement;
class CPHJoint;
class CPhysicsShell;
class CPHFracture;
class CShellSplitInfo;
typedef std::pair<CPhysicsShell*,u16>	shell_root;
typedef std::pair<CPHElement*,CShellSplitInfo>		element_fracture;
DEFINE_VECTOR(CPHElement*,ELEMENT_STORAGE,ELEMENT_I)
DEFINE_VECTOR(CPHJoint*,JOINT_STORAGE,JOINT_I)
DEFINE_VECTOR(shell_root,PHSHELL_PAIR_VECTOR,SHELL_PAIR_I)
typedef xr_vector<shell_root>::reverse_iterator SHELL_PAIR_RI;
DEFINE_VECTOR(element_fracture,ELEMENT_PAIR_VECTOR,ELEMENT_PAIR_I)
typedef		xr_vector<CPHElement*>::reverse_iterator	ELEMENT_RI;
typedef		xr_vector<element_fracture>::reverse_iterator	ELEMENT_PAIR_RI;
DEFINE_VECTOR(CPHFracture,FRACTURE_STORAGE,FRACTURE_I)
typedef		xr_vector<CPHFracture>::reverse_iterator	FRACTURE_RI;
#endif