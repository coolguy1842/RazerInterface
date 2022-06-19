#include "keyboard.h"
#include "dbus_handler.h"
#include "devicemanager.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int keyboard_from_id(char* id, struct keyboard* keyboard) {
    keyboard->id = id;
    keyboard->name = devicemanager_get_device_name(id);
    device_lighting_init(id, &keyboard->lighting);

    return 0;
}

void keyboard_draw(struct keyboard* keyboard) {
    unsigned char* bytes;
    int bytesSize;

    // get bytes 
    lighting_colour_bytes(&keyboard->lighting, &bytes, &bytesSize);

    // create connection and error variable
    DBusError err;
    DBusConnection* con;

    // initialize error variable
    dbus_error_init(&err);

    // connect to the bus
    con = dbus_bus_get(DBUS_BUS_SESSION, &err);
    
    // check if error or if session bus == null
    if(dbus_error_is_set(&err)) {
        // print error message 
        fprintf(stderr, "Connection Error (%s)\n", err.message); 

        // free error variable
        dbus_error_free(&err); 
    }

    // exit if bus did not connect 
    if(!con) {
        return;
    }

    // create pending call
    DBusPendingCall* pending;
    dbus_bool_t dbret;

    // create char* for path and devpath
    char* devPathStr = "/org/razer/device/";
    char* pathStr = malloc(strlen(devPathStr) + strlen(keyboard->id) + 1);
    
    // copy devpathstr contents to pathstr
    strcpy(pathStr, devPathStr);
    // append device id to pathstr
    strcat(pathStr, keyboard->id);


    // create message and send then wait till reply
    DBusMessage* message = dbus_message_new_method_call("org.razer", pathStr, "razer.device.lighting.chroma", "setKeyRow");
    
    // add byte array to arguments
    dbus_message_append_args(message, DBUS_TYPE_ARRAY, DBUS_TYPE_BYTE, &bytes, bytesSize, DBUS_TYPE_INVALID);
    free(bytes);
    
    // send message then wait
    dbus_connection_send_with_reply(con, message, &pending, 50);
    
    // wait until reply
    dbus_pending_call_block(pending);

    // set msg to reply
    DBusMessage* reply = dbus_pending_call_steal_reply(pending);

    // free the error variable
    dbus_error_free(&err); 

    // flush connection
    dbus_connection_flush(con);

    DBusMessage* msg;

    // draw keyboard colours
    devicemanager_get_device_property(keyboard->id, "razer.device.lighting.chroma", "setCustom", &msg); // set custom effect again to load new buffer
    dbus_message_unref(msg);
    // reset lighting array to all unset
    device_lighting_reset(&keyboard->lighting);
}

void keyboard_set_key_light(struct keyboard* keyboard, int key, int red, int green, int blue) {
    // set rgb at key
    device_lighting_set_key(&keyboard->lighting, key, red, green, blue);
}

struct rgb keyboard_get_key_light(struct keyboard* keyboard, int key) {
    // get rgb at key
    return device_lighting_get_key(&keyboard->lighting, key);
}