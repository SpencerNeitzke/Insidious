//
//  usb.cpp
//  Insidious
//

#include <iostream>
#import "usb.hpp"

typedef struct DeviceInfo {
    io_object_t             notification;
    IOUSBDeviceInterface    **deviceInterface;
    CFStringRef             deviceName;
    UInt32                  locationID;
} DeviceInfo;

static IONotificationPortRef    gNotifyPort;
static io_iterator_t            gAddedIter;
static CFRunLoopRef             gRunLoop;

//================================================================================================
//
//  DeviceNotification
//
//  This routine will get called whenever any kIOGeneralInterest notification happens.  We are
//  interested in the kIOMessageServiceIsTerminated message so that's what we look for.  Other
//  messages are defined in IOMessage.h.
//
//================================================================================================
void DeviceNotification(void *refCon, io_service_t service, natural_t messageType, void *messageArgument)
{
    kern_return_t   kr;
    DeviceInfo   *deviceInfoRef = (DeviceInfo *) refCon;
    
    if (messageType == kIOMessageServiceIsTerminated) {
        fprintf(stderr, "Device removed: ");
        CFShow(deviceInfoRef->deviceName);
        fprintf(stderr, "\n");
        
        // Free the data we're no longer using now that the device is going away
        CFRelease(deviceInfoRef->deviceName);
        
        if (deviceInfoRef->deviceInterface) {
            kr = (*deviceInfoRef->deviceInterface)->Release(deviceInfoRef->deviceInterface);
        }
        
        kr = IOObjectRelease(deviceInfoRef->notification);
        free(deviceInfoRef);
    }
}

//================================================================================================
//
//  DeviceAdded
//
//  This routine is the callback for our IOServiceAddMatchingNotification.  When we get called
//  we will look at all the devices that were added and we will:
//
//  1.  Create some private data to relate to each device (in this case we use the service's name
//      and the location ID of the device
//  2.  Submit an IOServiceAddInterestNotification of type kIOGeneralInterest for this device,
//      using the refCon field to store a pointer to our private data.  When we get called with
//      this interest notification, we can grab the refCon and access our private data.
//
//================================================================================================
void DeviceAdded(void *refCon, io_iterator_t iterator)
{
    kern_return_t       kr;
    io_service_t        usbDevice;
    
    while ((usbDevice = IOIteratorNext(iterator))) {
        io_name_t       deviceName;
        CFStringRef     deviceNameAsCFString;   
        DeviceInfo   *deviceInfoRef = NULL;
        
        fprintf(stderr, "Device connected: ");
        
        //CFStringRef bsdName = (CFStringRef)IORegistryEntrySearchCFProperty(usbDevice, kIOServicePlane, CFSTR(kIOBSDNameKey), kCFAllocatorDefault, kIORegistryIterateRecursively);
        //fprintf(stderr, "BSD name: %s", CFStringGetCStringPtr(bsdName, kCFStringEncodingMacRoman));
        
        // Create a buffer to hold the data
        deviceInfoRef = (DeviceInfo*)malloc(sizeof(DeviceInfo));
        bzero(deviceInfoRef, sizeof(DeviceInfo));
        
        // Get the device's name
        kr = IORegistryEntryGetName(usbDevice, deviceName);
        if (KERN_SUCCESS != kr) {
            deviceName[0] = '\0';
        }
        
        deviceNameAsCFString = CFStringCreateWithCString(kCFAllocatorDefault, deviceName, kCFStringEncodingASCII);
        CFShow(deviceNameAsCFString);       
        deviceInfoRef->deviceName = deviceNameAsCFString;
        
        // Register for a device removal notification
        kr = IOServiceAddInterestNotification(gNotifyPort,                      // notifyPort
                                              usbDevice,                        // service
                                              kIOGeneralInterest,               // interestType
                                              DeviceNotification,               // callback
                                              deviceInfoRef,                   // refCon
                                              &(deviceInfoRef->notification)   // notification
                                              );
        
        if (KERN_SUCCESS != kr) {
            fprintf(stderr, "IOServiceAddInterestNotification returned 0x%08x.\n", kr);
        }
        
        kr = IOObjectRelease(usbDevice);
    }
}

USBScanner::USBScanner() {
}

void USBScanner::scan() {
    CFMutableDictionaryRef  matchingDict;
    CFRunLoopSourceRef      runLoopSource;
    kern_return_t           kr;
    
    // Interested in IOUSBDevice and its subclasses
    matchingDict = IOServiceMatching(kIOUSBDeviceClassName);
    if (matchingDict == NULL) {
        fprintf(stderr, "IOServiceMatching returned NULL.\n");
        return;
    }
    
    // Add async notifications to run loop
    gNotifyPort = IONotificationPortCreate(kIOMasterPortDefault);
    runLoopSource = IONotificationPortGetRunLoopSource(gNotifyPort);
    
    gRunLoop = CFRunLoopGetCurrent();
    CFRunLoopAddSource(gRunLoop, runLoopSource, kCFRunLoopDefaultMode);
    
    // Register for device connection notifications
    kr = IOServiceAddMatchingNotification(gNotifyPort,                  // notifyPort
                                          kIOFirstMatchNotification,    // notificationType
                                          matchingDict,                 // matching
                                          DeviceAdded,                  // callback
                                          NULL,                         // refCon
                                          &gAddedIter                   // notification
                                          );        
    
    // Detect already connected devices    
    DeviceAdded(NULL, gAddedIter);  
    
    // Start the run loop, now we'll receive notifications
    fprintf(stderr, "Starting run loop.\n");
    CFRunLoopRun();
}