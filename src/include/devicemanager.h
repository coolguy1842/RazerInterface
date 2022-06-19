#ifndef __DEVICE_MANAGER_H__
#define __DEVICE_MANAGER_H__
#include <dbus/dbus.h>
#include "devicematrix.h"

typedef enum DeviceType { NONE, KEYBOARD, MOUSE } DeviceType;


void devicemanager_get_devices(char*** deviceIDs, int* deviceIDsLen);
int devicemanager_has_device(char* device);


int devicemanager_get_device_property(char* device, char* interface, char* property, DBusMessage** msg);
int devicemanager_call_device_method_no_args(char* device, char* interface, char* property);

DeviceType devicemanager_get_device_type(char* device);
char* devicemanager_get_device_name(char* device);

int devicemanager_get_device_matrix(char* device, struct matrix* matrix);

#endif