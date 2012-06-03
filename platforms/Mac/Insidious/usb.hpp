//
//  usb.hpp
//  Insidious
//

#ifndef Insidious_usb_hpp
#define Insidious_usb_hpp

#include <Foundation/Foundation.h>
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/IOMessage.h>
#include <IOKit/IOCFPlugIn.h>
#include <IOKit/usb/IOUSBLib.h>
#include <IOKIT/IOBSD.h>

void DeviceNotification(void *refCon, io_service_t service, natural_t messageType, void *messageArgument);
void DeviceAdded(void *refCon, io_iterator_t iterator);

class USBScanner {
public:
    USBScanner();
    void scan();
};

#endif
