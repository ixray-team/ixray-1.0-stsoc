#pragma once

#include "xrXMLParser.h"

class CUIXml :public CXml
{

protected:
	virtual shared_str correct_file_name	(LPCSTR path, LPCSTR fn);

};