#include <windows.h>
#include <iostream>

using namespace std;

int main() {
	UINT nDevices = 0;
	GetRawInputDeviceList( NULL, &nDevices, sizeof( RAWINPUTDEVICELIST ) );

	if( nDevices < 1 ){
		cout << "ERR: 0 Devices?";
		cin.get();
		return 0;
	}
	
	PRAWINPUTDEVICELIST pRawInputDeviceList;
	pRawInputDeviceList = new RAWINPUTDEVICELIST[ sizeof( RAWINPUTDEVICELIST ) * nDevices ];

	if( pRawInputDeviceList == NULL ){
		cout << "ERR: Could not allocate memory for Device List.";
		cin.get();
		return 0;
	}
	
	int nResult;
	nResult = GetRawInputDeviceList( pRawInputDeviceList, &nDevices, sizeof( RAWINPUTDEVICELIST ) );

	if( nResult < 0 ){
		delete [] pRawInputDeviceList;
		cout << "ERR: Could not get device list.";
		cin.get();
		return 0;
	}

	for( UINT i = 0; i < nDevices; i++ ){
		UINT nBufferSize = 0;
		nResult = GetRawInputDeviceInfo( pRawInputDeviceList[i].hDevice, // Device
										 RIDI_DEVICENAME,				 // Get Device Name
										 NULL,							 // NO Buff, Want Count!
										 &nBufferSize );				 // Char Count Here!
		if( nResult < 0 ){
			cout << "ERR: Unable to get Device Name character count.. Moving to next device." << endl << endl;
			continue;
		}

		WCHAR* wcDeviceName = new WCHAR[ nBufferSize + 1 ];

		if( wcDeviceName == NULL ){
			cout << "ERR: Unable to allocate memory for Device Name.. Moving to next device." << endl << endl;
			continue;
		}

		nResult = GetRawInputDeviceInfo( pRawInputDeviceList[i].hDevice, // Device
										 RIDI_DEVICENAME,				 // Get Device Name
										 wcDeviceName,					 // Get Name!
										 &nBufferSize );				 // Char Count

		if( nResult < 0 ){
			cout << "ERR: Unable to get Device Name.. Moving to next device." << endl << endl;
			delete [] wcDeviceName;
			continue;
		}


		RID_DEVICE_INFO rdiDeviceInfo;
		rdiDeviceInfo.cbSize = sizeof( RID_DEVICE_INFO );
		nBufferSize = rdiDeviceInfo.cbSize;

		nResult = GetRawInputDeviceInfo( pRawInputDeviceList[i].hDevice,
										 RIDI_DEVICEINFO,
										 &rdiDeviceInfo,
										 &nBufferSize );

		if( nResult < 0 ) {
			cout << "ERR: Unable to read Device Info.. Moving to next device." << endl << endl;
			continue;
		}
		if( rdiDeviceInfo.dwType == RIM_TYPEMOUSE ){
			cout << endl << "Displaying device " << i+1 << " information. (MOUSE)" << endl;
			wcout << L"Device Name: " << wcDeviceName << endl;
			cout << "Mouse ID: " << rdiDeviceInfo.mouse.dwId << endl;
		}
		else if( rdiDeviceInfo.dwType == RIM_TYPEKEYBOARD ){
			cout << endl << "Displaying device " << i+1 << " information. (KEYBOARD)" << endl;
			wcout << L"Device Name: " << wcDeviceName << endl;
		}
		else {

			cout << endl << "Displaying device " << i+1 << " information. (HID)" << endl;
			wcout << L"Device Name: " << wcDeviceName << endl;
			cout << "Usage for the device: " << rdiDeviceInfo.hid.usUsage << endl;
			cout << "Usage Page for the device: " << rdiDeviceInfo.hid.usUsagePage << endl;
		}
		delete [] wcDeviceName;
	}

	delete [] pRawInputDeviceList;
	cout << endl << "Finished.";
	cin.get();
	return 0;
}