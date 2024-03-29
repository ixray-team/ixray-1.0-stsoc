///////////////////////////////////////////////////////////////
// InfoPortionsDefs.h
// ����� ���������� ��� ������� ���������� � info_portion
///////////////////////////////////////////////////////////////

#pragma once

#include "alife_space.h"
#include "object_interfaces.h"
//#define NO_INFO_INDEX	(-1)
//typedef int				INFO_INDEX;
typedef shared_str		INFO_ID;


struct INFO_DATA : public IPureSerializeObject<IReader,IWriter>
{
	INFO_DATA():info_id(NULL),receive_time(0){};
	INFO_DATA(INFO_ID id, ALife::_TIME_ID time):info_id(id),receive_time(time){};

	virtual void load (IReader& stream);
	virtual void save (IWriter&);

	INFO_ID				info_id;
	//����� ��������� ����� ������ ����������
	ALife::_TIME_ID		receive_time;
};

using KNOWN_INFO_VECTOR = xr_vector<INFO_DATA>;
using KNOWN_INFO_VECTOR_IT = KNOWN_INFO_VECTOR::iterator;
