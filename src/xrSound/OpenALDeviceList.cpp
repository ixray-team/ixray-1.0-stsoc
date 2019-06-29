/*
 * Copyright (c) 2005, Creative Labs Inc.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided
 * that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright notice, this list of conditions and
 * 	     the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright notice, this list of conditions
 * 	     and the following disclaimer in the documentation and/or other materials provided with the distribution.
 *     * Neither the name of Creative Labs Inc. nor the names of its contributors may be used to endorse or
 * 	     promote products derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include "stdafx.h"

#include "OpenALDeviceList.h"
#include <al.h>
#include <alc.h>


/* 
 * Init call
 */
ALDeviceList::ALDeviceList()
{
	Enumerate();
}

/* 
 * Exit call
 */
ALDeviceList::~ALDeviceList()
{
}

void ALDeviceList::Enumerate()
{
	char *devices;
	int major, minor, index;
	const char *defaultDeviceName;
	const char *actualDeviceName;

	// have a set of vectors storing the device list, selection status, spec version #, and XRAM support status
	// -- empty all the lists and reserve space for 10 devices
	m_devices.clear		();
	m_defaultDeviceIndex	= 0;

	// grab function pointers for 1.0-API functions, and if successful proceed to enumerate all devices
	if (alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT")) {
		defaultDeviceName	= (char *)alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
		devices				= (char *)alcGetString(NULL, ALC_DEVICE_SPECIFIER);
		defaultDeviceName	= (char *)alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
		index				= 0;
		// go through device list (each device terminated with a single NULL, list terminated with double NULL)
		while (*devices != NULL) {
			ALCdevice *device		= alcOpenDevice(devices);
			if (device) {
				ALCcontext *context = alcCreateContext(device, NULL);
				if (context) {
					if (xr_strcmp(defaultDeviceName, devices) == 0) {
						m_defaultDeviceIndex = index;
					}
					alcMakeContextCurrent(context);
					// if new actual device name isn't already in the list, then add it...
					actualDeviceName = alcGetString(device, ALC_DEVICE_SPECIFIER);
					bool bNewName = true;
					for (int i = 0; i < GetNumDevices(); i++) {
						if (xr_strcmp(GetDeviceName(i), actualDeviceName) == 0) {
							bNewName = false;
						}
					}
					if ((bNewName) && (actualDeviceName != NULL) && (xr_strlen(actualDeviceName) > 0)) {
						alcGetIntegerv					(device, ALC_MAJOR_VERSION, sizeof(int), &major);
						alcGetIntegerv					(device, ALC_MINOR_VERSION, sizeof(int), &minor);
						m_devices.push_back				(ALDeviceDesc(actualDeviceName,minor,major));
						m_devices.back().xram			= (alIsExtensionPresent("EAX-RAM") == TRUE);
						m_devices.back().eax			= (alIsExtensionPresent("EAX2.0") == TRUE);
						m_devices.back().eax_unwanted	= ((0==xr_strcmp(actualDeviceName,"Generic Hardware"))||
														   (0==xr_strcmp(actualDeviceName,"Generic Software")));
					}
					alcDestroyContext(context);
				}
				alcCloseDevice(device);
			}
			devices		+= xr_strlen(devices) + 1;
			index += 1;
		}
	}

	ResetFilters();
}

/*
 * Returns the major and minor version numbers for a device at a specified index in the complete list
 */
void ALDeviceList::GetDeviceVersion(int index, int *major, int *minor)
{
	*major = m_devices[index].major_ver;
	*minor = m_devices[index].minor_ver;
	return;
}

/* 
 * Deselects devices which don't have the specified minimum version
 */
void ALDeviceList::FilterDevicesMinVer(int major, int minor)
{
	int dMajor, dMinor;
	for (unsigned int i = 0; i < m_devices.size(); i++) {
		GetDeviceVersion(i, &dMajor, &dMinor);
		if ((dMajor < major) || ((dMajor == major) && (dMinor < minor))) 
			m_devices[i].selected = false;
	}
}

/* 
 * Deselects devices which don't have the specified maximum version
 */
void ALDeviceList::FilterDevicesMaxVer(int major, int minor)
{
	int dMajor, dMinor;
	for (unsigned int i = 0; i < m_devices.size(); i++) {
		GetDeviceVersion(i, &dMajor, &dMinor);
		if ((dMajor > major) || ((dMajor == major) && (dMinor > minor))) {
			m_devices[i].selected = false;
		}
	}
}

/* 
 * Deselects devices which don't have XRAM support
 */
void ALDeviceList::FilterDevicesXRAMOnly()
{
	for (unsigned int i = 0; i < m_devices.size(); i++) {		
		if (m_devices[i].xram == false) {
			m_devices[i].selected = false;
		}
	}
}

/*
 * Resets all filtering, such that all devices are in the list
 */
void ALDeviceList::ResetFilters()
{
	for (int i = 0; i < GetNumDevices(); i++)
		m_devices[i].selected = true;
	m_filterIndex = 0;
}

/*
 * Gets index of first filtered device
 */
int ALDeviceList::GetFirstFilteredDevice()
{
	for (int i = 0; i < GetNumDevices(); i++) {
		if (m_devices[i].selected == true)
			break;
	}
	m_filterIndex = i + 1;
	return i;
}

/*
 * Gets index of next filtered device
 */
int ALDeviceList::GetNextFilteredDevice()
{
	for (int i = m_filterIndex; i < GetNumDevices(); i++) {
		if (m_devices[i].selected == true)
			break;
	}
	m_filterIndex = i + 1;
	return i;
}