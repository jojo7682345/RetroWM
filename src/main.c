#include <stdio.h>
#include "actions/actions.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XInput.h>
#include "io/io.h"
#include "util.h"

int main(int argc, char* argv[]){
    
    char* action;
    if (argc > 1) {
	    action = argv[1];
	    while(action[0] == '-'){
            action++;
        } 
    } else {
	    action = "version";
    }

    if(checkCommand(action,"version","ver","v")){
        return listVersion();
    }

    if(checkCommand(action,"help","h","?")){
        return showHelp();
    }

    Display* display;

    display = XOpenDisplay(NULL);
    if(display==NULL){
        fprintf(stderr,"Unable to connect to X server\n");
        goto out;
    }

    int xi_opcode;
    int event;
    int error;

    if (!XQueryExtension(display, "XInputExtension", &xi_opcode, &event, &error)) {
        printf("X Input extension not available.\n");
        goto out;
    }

    if (!xinput_version(display)) {
	fprintf(stderr, "%s extension not available\n", INAME);
	goto out;
    }

    if (is_xwayland(display)){
        fprintf(stderr, "WARNING: running xinput against an Xwayland server. See the xinput man page for details.\n");
    }   

    entry	*driver = drivers;

    while(driver->func_name) {
	if (strcmp(driver->func_name, action) == 0) {
	    int	r = (*driver->func)(display, argc-2, argv+2,
				    driver->func_name, driver->arg_desc);
	    XSync(display, False);
	    XCloseDisplay(display);
	    return r;
	}
	driver++;
    }

out:
    if (display)
        XCloseDisplay(display);
    return EXIT_FAILURE;
}