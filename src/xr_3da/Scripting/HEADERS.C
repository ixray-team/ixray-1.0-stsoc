/************************************************************************
       -= SEER - C-Scripting engine v 0.94a =-
State      : under implementation
Author     : Przemyslaw Podsiadly
             email: ppodsiad@elka.pw.edu.pl
             WWW:   http://home.elka.pw.edu.pl/~ppodsiad
File:headers.c
Desc:internal headers and functions
************************************************************************/
//when #include <stdarg.h> - from char * inside internal structs,
//#include "io.h" - from file / archive
//**************INTERNAL HEADER FILES:********************************

//STDARG.H - when #include stdarg
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "seer.h"

//#define OutputDebug

char *hdrSTDARG=
"\n"
"typedef void *va_list;\n";

/*
"
typedef void *va_list;

#define __dj_va_rounded_size(T)  \\
  (((sizeof (T) + sizeof (int) - 1) / sizeof (int)) * sizeof (int))

#define va_arg(ap, T) \\
    (ap = (va_list) ((char *) (ap) + __dj_va_rounded_size (T)), \\
     *((T *) (void *) ((char *) (ap) - __dj_va_rounded_size (T))))

#define va_end(ap)

#define va_start(ap, last_arg) \
 (ap = ((va_list) __builtin_next_arg (last_arg)))

";
*/
char *hdrInstances=
"#ifndef SeeR_Internal_Instances\n"
"#define scstatus_FREE    0\n"
"#define scstatus_RUNNING 1\n"
"#define scstatus_PAUSED  2\n"

"import void scKill_My_Forked_Instances();\n"
"import void scKill_Forked(int);\n"
"import int scGet_Forked_Status(int);\n"
"import int scGet_Actual_Priority();\n"
"#define SeeR_Internal_Instances\n"
"#endif\n"
;

char *hdrKernel=
"#ifndef SeeR_Internal_Kernel\n"
"#include Instances\n"
"//can call all imported kernel API function\n"
"#define scrights_KERNEL 256\n"
"//cannot ---\n"
"#define scrights_USER     0\n"
"//when no instance is being run:\n"
"#define scrights_TOP MAXINT\n"

"typedef void scInstance;\n"
"typedef void* scScript;\n"
"import scInstance* scCreate_Instance(scScript,char*,...);\n"
"import scInstance* scFork_Instance(scInstance*);\n"
"import void scFree_Instance(scInstance *);\n"

"import scScript scCompile_File(char *);\n"
"import scScript scCompile_Text(char *);\n"
"import scScript scLoad_Script(char *);\n"

"import scInstance* scGet_This();\n"
"import scInstance* scGet_Forked(scInstance*,int);\n"
"import scInstance* scGet_Actual_Forked(int);\n"
"import void scPause_Instance(scInstance*,int);\n"
"import void scKill_Instance(scInstance*);\n"
"import void scKill_Forked_Instances(scInstance*);\n"

"import int scGet_Script_Size(scScript);\n"
"import int scCall_Instance(scInstance*,int address,...);\n"
"import int scVCall_Instance(scInstance*,int,int,int*);\n"
"import int scStart_Instance(scInstance *,...);\n"
"import int scGet_Symbol(scInstance*,char*);\n"
"import int scLaunch_Instance(scInstance* inst,int spd,int address,...);\n"

"import void scSet_Priority(scInstance*, int priority_level);\n"
"import int scGet_Priority(scInstance*);\n"

"#define SeeR_Internal_Kernel\n"
"#endif\n"
;

void SAPI scExport_Kernel()
{
	scAdd_Internal_Header("Kernel",hdrKernel);
	//Kernel imports:
	scAddExtSym(scCreate_Instance);
	scAddExtSym(scFork_Instance);
	scAddExtSym(scFree_Instance);
	
	scAddExtSym(scCompile_File);
	scAddExtSym(scCompile_Text);
	scAddExtSym(scLoad_Script);
	
	scAddExtSym(scGet_This);
	scAddExtSym(scGet_Forked);
	scAddExtSym(scGet_Actual_Forked);
	scAddExtSym(scPause_Instance);
	scAddExtSym(scKill_Instance);
	scAddExtSym(scKill_Forked_Instances);
	
	scAddExtSym(scGet_Script_Size);
	scAddExtSym(scStart_Instance);
	scAddExtSym(scCall_Instance);
	scAddExtSym(scVCall_Instance);
	scAddExtSym(scGet_Symbol);
	scAddExtSym(scLaunch_Instance);
	
	scAddExtSym(scSet_Priority);
	scAddExtSym(scGet_Priority);
	
	scAddExtSym(scGet_Title);
	scAddExtSym(scGet_Author);
	scAddExtSym(scGet_Instance_Title);
}

char *hdrStdLib=
"#ifndef SeeR_Internal_StdLib\n"
"#include Instances\n"

/* class string */
"class string{\n"

"public:\n"

" char *cstr;\n"

" c_import(string) string();\n"
" c_import(string_SZ) string(char *x);\n"
" c_import(string_I) string(int x);\n"
" c_import(stringCC) string(string& f);\n"

" c_import ~string();\n"

" c_import int length();\n"

" c_import(operator=(int)) string& operator=(int x);\n"
" c_import string& operator=(string& s);\n"
" c_import(operator=(char*)) string& operator=(char *s);\n"
// a=x=s; <=> a.operator=(x.operator=(s));

" c_import string operator+(string& x);\n"
" c_import(operator+(char*)) string operator+(char *s);\n"
// a=x+s; <=> a=x.operator+(s); <=> a.operator=(x.operator+(s));

" c_import string sub(int start,int len);\n"
" c_import string operator()(int start,int len);\n"

" c_import int cmp(string& x);\n"
//" c_import int operator<?>(string& x);\n"
" c_import BOOL operator==(string& x);\n"
" c_import BOOL operator!=(string& x);\n"
" c_import(operator==(char*)) BOOL operator==(char *x);\n"
" c_import BOOL operator>(string& x);\n"
" c_import BOOL operator<(string& x);\n"
" c_import BOOL operator>=(string& x);\n"
" c_import BOOL operator<=(string& x);\n"
" c_import void operator+=(string& x);\n"
" c_import(operator+=(char*)) void operator+=(char* x);\n"
// a+=x; <=> a.operator+=(x);

" c_import void del(int start,int len);\n"
" c_import void insert(int start,string& x);\n"
//index of first found occurance, or -1 if not
" c_import int find(string& x);\n"
//returns number of replaces
" c_import int replace(string& what,string& with,BOOL global=false);\n"

/* operators */
" c_import int toInt();\n"//toint
" c_import int operator(int)();\n"//toint
" c_import char& operator[](int idx);\n"


" c_import void reconstruct();\n"

"};\n"

// class hashInt
"class hashInt{\n"
"public:\n"
" c_import hashInt();\n"
" c_import ~hashInt();\n"

" c_import void*& operator[](int id);\n"

" c_import void add(int id,void *data);\n"
" c_import BOOL exists(int id);\n"
" c_import void *remove(int id);\n"
" c_import void *get(int id);\n"

" c_import BOOL empty();\n"

" c_import int first();\n"
" c_import int next();\n"

"};\n"

// class hashStr
"class hashStr{\n"
"public:\n"
" c_import hashStr();\n"
" c_import ~hashStr();\n"

" c_import void*& operator[](char *id);\n"

" c_import void add(char* id,void *data);\n"
" c_import BOOL exists(char* id);\n"
" c_import void *remove(char* id);\n"
" c_import void *get(char* id);\n"

" c_import BOOL empty();\n"

" c_import char *first();\n"
" c_import char *next();\n"
"};\n"

"struct vector{\n"
" double x,y,z;\n"
"public:\n"
" c_import(vectorVOID) vector();\n"
" c_import vector(double,double,double);\n"
" c_import(vectorCC) vector(vector& v);\n"
//" c_import ~vector();\n"

" c_import vector& operator=(vector& v);\n"

" c_import vector operator+(vector& v);\n"
" c_import vector operator-(vector& v);\n"
" c_import vector operator*(vector& v);\n"//cross product,not scalar

" c_import void operator+=(vector& v);\n"
" c_import void operator-=(vector& v);\n"
" c_import void operator*=(vector& v);\n"

" c_import BOOL operator==(vector& v);\n"
" c_import BOOL operator!=(vector& v);\n"
" c_import BOOL operator<(vector& v);\n"
" c_import BOOL operator>(vector& v);\n"
" c_import BOOL operator<=(vector& v);\n"
" c_import BOOL operator>=(vector& v);\n"
" c_import void normalize();\n"
" c_import double length();\n"

"};\n"

"#define SeeR_Internal_StdLib\n"
"#endif\n";

/************STRING*****************************************/

/* in seer.h
typedef struct {
 char *cstr;
} scintString;
*/

//" c_import string(char *x);\n"
void string_string_char(scintString* _this,char *x)
{if (!x) _this->cstr=NULL;
    else _this->cstr=xr_strdup(x);
#ifdef OutputDebug
 printf("string(%d,%d:%s);\n",_this,_this->cstr,_this->cstr);
#endif
}

//" c_import string();\n"
void string_string(scintString* _this)
{_this->cstr=NULL;
#ifdef OutputDebug
 printf("string(%d);\n",_this);
#endif
}

scintString* string_operatorSET_int(scintString* _this,int x)
{char temp[256];
 if (_this->cstr) xr_free(_this->cstr);
 itoa(x,temp,10);
 _this->cstr=xr_strdup(temp);
 return _this;
}

void string_string_int(scintString* _this,int x)
{_this->cstr=NULL;
 string_operatorSET_int(_this,x);
}


//" c_import ~string();\n"
void string_destroy(scintString* _this)
{if (_this->cstr)
 {
  xr_free(_this->cstr);
  _this->cstr=NULL;
 }
}

//" c_import string(string& f);\n"
void string_copy(scintString* _this,scintString* f)
{
 _this->cstr=NULL;
 if (f->cstr)
 _this->cstr=xr_strdup(f->cstr);
}
//" c_import string& operator=(string& s);\n"
// a=x=s; <=> a.operator=(x.operator=(s));
scintString* string_operatorSET(scintString* _this,scintString* f)
{
 if (_this->cstr) {
#ifdef OutputDebug
  printf("set:xr_free(%d);",_this->cstr);
#endif
  xr_free(_this->cstr);}
 _this->cstr=NULL;
 if (f->cstr)
 _this->cstr=xr_strdup(f->cstr);
#ifdef OutputDebug
 printf("%d.set(%d,%s)\n",_this,f,*f);
#endif
 return _this;
}

//" c_import string& operator=(char * f);\n"
scintString* string_operatorSET_char(scintString* _this,char* f)
{scintString t;
 t.cstr=f;
 return string_operatorSET(_this,&t);
}
//" c_import string operator+(string& x);\n"
// a=x+s; <=> a=x.operator+(s); <=> a.operator=(x.operator+(s));
scintString string_operatorPLUS(scintString* _this,scintString* x)
{scintString v;
 int i;
 v.cstr=xr_malloc((i=strlen(_this->cstr))+strlen(x->cstr)+1);
 strcpy(v.cstr,_this->cstr);
 strcpy(v.cstr+i,x->cstr);
// printf("add:%s+%s=%s\n",*_this,*x,v);
#ifdef OutputDebug
 printf("add(%d,%d)\n",_this,x);
 printf("%s\n",*x);
#endif
 return v;
}

//" c_import string operator+(char* x);\n"
scintString string_operatorPLUS_char(scintString* _this,char *x)
{scintString v;
 int i;
 v.cstr=xr_malloc((i=strlen(_this->cstr))+strlen(x)+1);
 strcpy(v.cstr,_this->cstr);
 strcpy(v.cstr+i,x);
// printf("add:%s+%s=%s\n",*_this,*x,v);
#ifdef OutputDebug
 printf("add_C(%d,%s)\n",_this,x);
#endif
 return v;
}

//" c_import string operator()(int start,int len);\n"
scintString string_sub(scintString* _this,int start,int len)
{scintString v;
 int i;
 if (_this->cstr) i=strlen(_this->cstr);
 else i=0;
 if (start>i) i=0;
 if (i&&start+len>i) len=i-start;
 if (!i||!len)
 {
  v.cstr=xr_malloc(4);
  v.cstr[0]=0;
 }
 else
 {
  v.cstr=xr_malloc(len+1);
  memcpy(v.cstr,_this->cstr+start,len);
  v.cstr[len]=0;
 }
 return v;
}

//" c_import BOOL operator<?>(string& x);\n"
int string_cmp(scintString* _this,scintString* x)
{
        if (!_this->cstr||!x->cstr)
                return (_this->cstr==x->cstr);
        return strcmp(_this->cstr,x->cstr);
}
//" c_import BOOL operator==(string& x);\n"
BOOL string_eq(scintString* _this,scintString* x)
{return string_cmp(_this,x)==0;
}

//" c_import BOOL operator!=(string& x);\n"
BOOL string_ne(scintString* _this,scintString* x)
{return string_cmp(_this,x)!=0;
}

//" c_import BOOL operator==(char* x);\n"
BOOL string_eq_char(scintString* _this,char* x)
{return strcmp(_this->cstr,x)==0;
}

//" c_import BOOL operator>(string& x);\n"
BOOL string_gt(scintString* _this,scintString* x)
{return string_cmp(_this,x)>0;
}

//" c_import BOOL operator<(string& x);\n"
BOOL string_lt(scintString* _this,scintString* x)
{return string_cmp(_this,x)<0;
}

//" c_import BOOL operator>=(string& x);\n"
BOOL string_ge(scintString* _this,scintString* x)
{return string_cmp(_this,x)>=0;
}

//" c_import BOOL operator<=(string& x);\n"
BOOL string_le(scintString* _this,scintString* x)
{return string_cmp(_this,x)<=0;
}

//" c_import void del(int start,int len);\n"
void string_del(scintString* _this,int start,int len)
{int j,i=strlen(_this->cstr);
 if (start>i) return;
 if (start+len>i) {_this->cstr[start]=0;return;}
 for(j=start+len;j<=i;j++)
  _this->cstr[j-len]=_this->cstr[j];
}

//" c_import void insert(int start,string& x);\n"
void string_insert(scintString* _this,int start,scintString* x)
{
 char *c;
 int i=string_length(_this);
 if (start>i) start=i;
 i+=string_length(x);
 c=xr_malloc(i+1);
 if (_this->cstr) strcpy(c,_this->cstr);
 strcpy(c+start,x->cstr);
 if (_this->cstr)
  strcpy(c+strlen(c),_this->cstr+start);
 string_destroy(_this);
 _this->cstr=c;
}

//index of first found occurance
int string_find(scintString* _this,scintString* x)
{char *s;
 s=strstr(_this->cstr,x->cstr);
 if (!s) return -1;
 return (int)s-(int)_this->cstr;
}

//returns number of replaces
int string_replace(scintString* _this,scintString* what,scintString* with,BOOL global)
{//TODO
 int n=0,p,dx=0,len1,len2;
 char *np=_this->cstr,*onp=np;
 _this->cstr=np;

 if (!what||!what->cstr||!_this->cstr||!with||!with->cstr) return 0;
 len1=strlen(what->cstr);
 len2=strlen(with->cstr);
// printf("Replace `%s' with `%s'(global=%d)\n",what->cstr,with->cstr,global);

 while((p=string_find(_this,what))!=-1)
 {
  n++;
  _this->cstr=onp;
  p+=dx;
  string_del(_this,p,len1);
  string_insert(_this,p,with);
  if (!global) return 1;
  onp=_this->cstr;
  dx=p+len2;
  np=onp+dx;
  if (p+len2>(int)strlen(_this->cstr)) break;
  _this->cstr=np;
 }
 _this->cstr=onp;
 return n;
}

//" c_import void operator+=(string& x);\n"
// a+=x; <=> a.operator+=(x);
void string_append(scintString* _this,scintString* x)
{int i;
 char *z;
#ifdef OutputDebug
 printf("append(%s,%s)",*_this,*x);
#endif
 if (!x->cstr) return;
 if (!_this->cstr) {_this->cstr=xr_strdup(x->cstr);return;}
 i=strlen(_this->cstr);
// _this->cstr=xr_realloc(_this->cstr,i+strlen(x->cstr)+1);
 z=xr_malloc(i+strlen(x->cstr)+1);
 strcpy(z,_this->cstr);
 xr_free(_this->cstr);
 _this->cstr=z;
 strcpy(_this->cstr+i,x->cstr);
#ifdef OutputDebug
 printf("=%s\n",*_this);
#endif

}

void string_append_char(scintString* _this,char* x)
{scintString t;
#ifdef OutputDebug
 printf("append_C:%s\n",x);
#endif
 t.cstr=x;
 string_append(_this,&t);
}

//" c_import int operator(int)();\n"//toint
int string_toInt(scintString* _this)
{int i=0;
 char *e;
 i=strtoul(_this->cstr,&e,0);
 return i;
}

//" c_import char operator[](int idx);\n"
char* string_operatorIDX(scintString* _this,int idx)
{int i=strlen(_this->cstr);
 if (idx>i) idx=i;
 return &(_this->cstr[idx]);
}

void string_reconstruct(scintString* _this)
{//nothing right now
}

int string_length(scintString* _this)
{if (!_this->cstr) return 0;
 return strlen(_this->cstr);
}

/********VECTOR************************************************/
typedef struct{
double x,y,z;
} scintVector;

//" c_import vector();\n"
void vector_vectorVOID(scintVector *_this)
{_this->x=_this->y=_this->z=0.0;
}
//" c_import vector(double,double,double);\n"
void vector_vector(scintVector *_this,double x,double y,double z)
{_this->x=x;_this->y=y;_this->z=z;
}
//" c_import ~vector();\n"
void vector_destroy(scintVector *_this)
{}

//" c_import vector& operator=(vector& v);\n"
scintVector *vector_operatorSET(scintVector *_this,scintVector *v)
{*_this=*v;
 return _this;
}

//" c_import vector operator+(vector& v);\n"
scintVector vector_operatorPLUS(scintVector *_this,scintVector *v)
{scintVector a;
 a=*_this;
 a.x+=v->x;
 a.y+=v->y;
 a.z+=v->z;
 return a;
}

//" c_import vector operator-(vector& v);\n"
scintVector vector_operatorMINUS(scintVector *_this,scintVector *v)
{scintVector a;
 a=*_this;
 a.x-=v->x;
 a.y-=v->y;
 a.z-=v->z;
 return a;
}
//" c_import vector operator*(vector& v);\n"//cross-product
scintVector vector_operatorMUL(scintVector *_this,scintVector *v)
{scintVector a;
 a.x = _this->y * v->z - _this->z * v->y;
 a.y = _this->z * v->x - _this->x * v->z;
 a.z = _this->x * v->y - _this->y * v->x;
 return a;
}

//" c_import void operator+=(vector& v);\n"
void vector_operatorPLUSSET(scintVector *_this, scintVector *v)
{
 _this->x+=v->x;
 _this->y+=v->y;
 _this->z+=v->z;
}
//" c_import void operator-=(vector& v);\n"
void vector_operatorMINUSSET(scintVector *_this, scintVector *v)
{
 _this->x-=v->x;
 _this->y-=v->y;
 _this->z-=v->z;
}
//" c_import void operator*=(vector& v);\n"
void vector_operatorMULSET(scintVector *_this, scintVector *v)
{
 *_this=vector_operatorMUL(_this,v);
}
//" c_import BOOL operator==(vector& v);\n"
BOOL vector_operatorEQ(scintVector *_this, scintVector *v)
{
 return (_this->x==v->x && _this->y==v->y &&_this->z==v->z );
}
//" c_import BOOL operator!=(vector& v);\n"
BOOL vector_operatorNE(scintVector *_this, scintVector *v)
{
 return (!vector_operatorEQ(_this,v));
}
//" c_import BOOL operator<(vector& v);\n"
BOOL vector_operatorLT(scintVector *_this, scintVector *v)
{
 return (_this->x<v->x && _this->y<v->y &&_this->z<v->z );
}
//" c_import BOOL operator>(vector& v);\n"
BOOL vector_operatorGT(scintVector *_this, scintVector *v)
{
 return (_this->x>v->x && _this->y>v->y &&_this->z>v->z );
}
//" c_import BOOL operator<=(vector& v);\n"
BOOL vector_operatorLE(scintVector *_this, scintVector *v)
{
 return (_this->x<=v->x && _this->y<=v->y &&_this->z<=v->z );
}
//" c_import BOOL operator>=(vector& v);\n"
BOOL vector_operatorGE(scintVector *_this, scintVector *v)
{
 return (_this->x>=v->x && _this->y>=v->y &&_this->z>=v->z );
}

//" c_import double length();\n"
double vector_length(scintVector *_this)
{
   return sqrt(_this->x*_this->x + _this->y*_this->y + _this->z*_this->z);
}

//" c_import void normalize();\n"
void vector_normalize(scintVector *_this)
{
   double length = 1.0 / vector_length(_this);

   _this->x *= length;
   _this->y *= length;
   _this->z *= length;
}

void scExport_Stdlib()
{
 scAdd_Internal_Header("stdlib",hdrStdLib);
//string:
  scAdd_External_Symbol("string.string",string_string);
  scAdd_External_Symbol("string.string_SZ",string_string_char);
  scAdd_External_Symbol("string.string_I",string_string_int);
  scAdd_External_Symbol("string.~string",string_destroy);
  scAdd_External_Symbol("string.length",string_length);
  scAdd_External_Symbol("string.operator=(int)",string_operatorSET_int);
  scAdd_External_Symbol("string.stringCC",string_copy);
  scAdd_External_Symbol("string.operator=",string_operatorSET);
  scAdd_External_Symbol("string.operator=(char*)",string_operatorSET_char);
  scAdd_External_Symbol("string.operator+",string_operatorPLUS);
  scAdd_External_Symbol("string.operator+(char*)",string_operatorPLUS_char);
  scAdd_External_Symbol("string.sub",string_sub);
  scAdd_External_Symbol("string.operator()",string_sub);
  scAdd_External_Symbol("string.cmp",string_cmp);
//  scAdd_External_Symbol("string.operator<?>",string_cmp);
  scAdd_External_Symbol("string.operator==",string_eq);
  scAdd_External_Symbol("string.operator!=",string_ne);
  scAdd_External_Symbol("string.operator==(char*)",string_eq_char);
  scAdd_External_Symbol("string.operator>",string_gt);
  scAdd_External_Symbol("string.operator<",string_lt);
  scAdd_External_Symbol("string.operator>=",string_ge);
  scAdd_External_Symbol("string.operator<=",string_le);
  scAdd_External_Symbol("string.del",string_del);
  scAdd_External_Symbol("string.insert",string_insert);
  scAdd_External_Symbol("string.find",string_find);
  scAdd_External_Symbol("string.replace",string_replace);
  scAdd_External_Symbol("string.operator+=",string_append);
  scAdd_External_Symbol("string.operator+=(char*)",string_append_char);
  scAdd_External_Symbol("string.toInt",string_toInt);
  scAdd_External_Symbol("string.operator(int)",string_toInt);
  scAdd_External_Symbol("string.operator[]",string_operatorIDX);
  scAdd_External_Symbol("string.reconstruct",string_reconstruct);
//vector:
  scAdd_External_Symbol("vector.vectorVOID",vector_vectorVOID);
  scAdd_External_Symbol("vector.vector",vector_vector);
  scAdd_External_Symbol("vector.vectorCC",vector_operatorSET);
  scAdd_External_Symbol("vector.~vector",vector_destroy);
  scAdd_External_Symbol("vector.operator=",vector_operatorSET);
  scAdd_External_Symbol("vector.operator+",vector_operatorPLUS);
  scAdd_External_Symbol("vector.operator-",vector_operatorMINUS);
  scAdd_External_Symbol("vector.operator*",vector_operatorMUL);
  scAdd_External_Symbol("vector.operator+=",vector_operatorPLUSSET);
  scAdd_External_Symbol("vector.operator-=",vector_operatorMINUSSET);
  scAdd_External_Symbol("vector.operator*=",vector_operatorMULSET);
  scAdd_External_Symbol("vector.operator==",vector_operatorEQ);
  scAdd_External_Symbol("vector.operator!=",vector_operatorNE);
  scAdd_External_Symbol("vector.operator<",vector_operatorLT);
  scAdd_External_Symbol("vector.operator>",vector_operatorGT);
  scAdd_External_Symbol("vector.operator<=",vector_operatorLE);
  scAdd_External_Symbol("vector.operator>=",vector_operatorGE);
  scAdd_External_Symbol("vector.normalize",vector_normalize);
  scAdd_External_Symbol("vector.length",vector_length);
}
