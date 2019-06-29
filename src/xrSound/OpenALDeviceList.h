#ifndef ALDEVICELIST_H
#define ALDEVICELIST_H

struct ALDeviceDesc{
	xr_string			name;
	int					minor_ver;
	int					major_ver;
	struct{
		u8				selected	:1;
		u8				xram		:1;
		u8				eax			:1;
		u8				eax_unwanted:1;
	};
						ALDeviceDesc			(LPCSTR nm, int mn, int mj){name=nm;minor_ver=mn;major_ver=mj;selected=false;xram=false;eax=false;eax_unwanted=true;}
};

class ALDeviceList
{
private:
	xr_vector<ALDeviceDesc>	m_devices;
	int					m_defaultDeviceIndex;
	int					m_filterIndex;
	void				Enumerate				();
public:
						ALDeviceList			();
						~ALDeviceList			();

	int					GetNumDevices			()				{return m_devices.size();}
	const ALDeviceDesc&	GetDeviceDesc			(int index)		{return m_devices[index];}
	const char*			GetDeviceName			(int index)		{return m_devices[index].name.c_str();}
	void				GetDeviceVersion		(int index, int *major, int *minor);
	int					GetDefaultDevice		()				{return m_defaultDeviceIndex;};
	void				FilterDevicesMinVer		(int major, int minor);
	void				FilterDevicesMaxVer		(int major, int minor);
	void				FilterDevicesXRAMOnly	();
	void				ResetFilters			();
	int					GetFirstFilteredDevice	();
	int					GetNextFilteredDevice	();
};

#endif // ALDEVICELIST_H
