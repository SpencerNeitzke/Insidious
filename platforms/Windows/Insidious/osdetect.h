#include <windows.h>
#include <iostream>
#include <dos.h>
#include <string.h> 
#include <sstream>
using namespace std;

string osdetect() {

OSVERSIONINFO osver;
osver.dwOSVersionInfoSize = sizeof(osver);
if (GetVersionEx(&osver)) {
	ostringstream Major;
	ostringstream Minor;
	Major << osver.dwMajorVersion;
	Minor << osver.dwMinorVersion;
	string MajorStr = Major.str();
	string MinorStr = Minor.str();
	string ver = MajorStr+"."+MinorStr;

	if(ver == "5.0") {
		return ("Windows 2000");
	}

	if(ver == "5.1") {
		return ("Windows XP");
	}

	if(ver == "5.2") {
		return ("Windows XP 64-bit Edition or Windows Server 2003");
	}

	if(ver == "6.0") {
		return ("Windows Vista or Windows Server 2008");
	}

	if(ver == "6.1") {
		return ("Windows 7 or Windows Server 2008 RC2");
	}

	if(ver == "6.2") {
		return ("Windows 8");
	}

	if(ver == "") {
		return ("No version found");
	}
}
}