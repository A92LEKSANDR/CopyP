#include <iostream>
#include <cstring>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <unistd.h>     //for getcwd()
#include <limits.h>     //Path_max
//#include <string>

std::string getCurrentPath(){
    char buffer[PATH_MAX];
    
    if(getcwd(buffer,sizeof(buffer)) != nullptr){
        return std::string(buffer,strlen(buffer));      //safe construction dont use '/0'
    }else{
        perror("getcwd");
        return "";
    }
}

void copyToClipboard(Display* display, const std::string& path){
    Window window = XCreateSimpleWindow(display, DefaultRootWindow(display),0,0,1,1,0,0,0);

    Atom cpboard = XInternAtom(display, "CLIPBOARD", False);
    Atom targetsAtom  = XInternAtom(display, "TARGETS", False);
    Atom utf8Str = XInternAtom(display, "UFT8_STRING", False);
    Atom textAtom = XInternAtom(display, "TEXT", False);

    XSetSelectionOwner(display,cpboard,window,CurrentTime);

    if(XGetSelectionOwner(display,cpboard) != window){
        std::cerr << "Нет владельца буфера\n";
        return;
    }

    XEvent event;

    while(true){
    
        XNextEvent(display, &event);
        if(event.type == SelectionRequest){
            XSelectionRequestEvent* req = &event.xselectionrequest;

            XSelectionEvent selEvent = {0};
            selEvent.type = SelectionNotify;
            selEvent.display = req->display;
            selEvent.requestor = req->requestor;
            selEvent.selection = req->selection;
            selEvent.target = req->target;
            selEvent.property = None;
            selEvent.time = req->time;

            if (req->target == targetsAtom) {
                Atom availableTargets[] = {utf8Str, textAtom};
                XChangeProperty(display, req->requestor, req->property, XA_ATOM, 32, PropModeReplace, (unsigned char*)availableTargets, 2);
                selEvent.property = req->property;
            } else if (req->target == utf8Str || req->target == textAtom) {
                XChangeProperty(display, req->requestor, req->property, req->target, 8, PropModeReplace, (unsigned char*)path.c_str(), path.length());
                selEvent.property = req->property;
            }

            XSendEvent(display, req->requestor, False, 0, (XEvent*)&selEvent);
            XFlush(display);

       }
    }
}

int main() {
    Display* display = XOpenDisplay(nullptr);
    if (!display) {
        std::cerr << "Не удалось подключиться к дисплею X11\n";
        return 1;
    }

    std::string path = getCurrentPath();
    if (!path.empty()) {
        copyToClipboard(display, path);
    }

    XCloseDisplay(display);
    return 0;
}
