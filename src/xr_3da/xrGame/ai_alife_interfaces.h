////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_interfaces.h
//	Created 	: 05.01.2003
//  Modified 	: 05.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life interfaces
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_ALIFE_INTERFACES
#define XRAY_AI_ALIFE_INTERFACES

class NET_Packet;

interface IPureALifeLObject {
public:
	IC virtual						~IPureALifeLObject()								= 0;
	virtual void					Load(IReader	&tFileStream)						= 0;
};

interface IPureALifeSObject {
public:
	IC virtual						~IPureALifeSObject()								= 0;
	virtual void					Save(IWriter	&tMemoryStream)						= 0;
};

interface IPureALifeLSObject : public IPureALifeLObject, public IPureALifeSObject {
};

interface IPureServerObject : public IPureALifeLSObject {
public:
	virtual void					STATE_Write	(NET_Packet &tNetPacket)				= 0;
	virtual void					STATE_Read	(NET_Packet &tNetPacket, u16 size)		= 0;
	virtual void					UPDATE_Write(NET_Packet &tNetPacket)				= 0;
	virtual void					UPDATE_Read	(NET_Packet &tNetPacket)				= 0;
};

interface IPureSchedulableObject {
	IC virtual						~IPureSchedulableObject()							= 0;
	virtual void					Update		()										= 0;
};

IC IPureALifeLObject::~IPureALifeLObject()
{
}

IC IPureALifeSObject::~IPureALifeSObject()
{
}

IC IPureSchedulableObject::~IPureSchedulableObject()
{
}

#endif