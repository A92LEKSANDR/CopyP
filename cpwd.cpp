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
        return std::string("cd ") + std::string(buffer,strlen(buffer));      //safe construction dont use '/0'
    }else{
        perror("getcwd");
        return "";
    }
}

void copyToClipBoard(const std::string& text){
    const char* waylandSession = getenv("WAYLAND_DISPLAY");
    const char* x11Session = getenv("DISPLAY");

    std::string command;
    
    if (waylandSession) {
        // Если используется Wayland
        command = "echo -n \"" + text + "\" | wl-copy";
    } else if (x11Session) {
        // Если используется X11
        command = "echo -n \"" + text + "\" | xclip -selection clipboard";
    } else {
        std::cerr << "Неизвестное окружение. Не удалось определить Wayland или X11.\n";
        return;
    }
    // Выполняем команду для копирования в буфер обмена
    int result = system(command.c_str());
    if (result != 0) {
        std::cerr << "Ошибка при копировании в буфер обмена\n";
    }
}

int main() {
    
    std::string path = getCurrentPath();
    
    if(!path.empty()){
        copyToClipBoard(path);
    }
    return 0;
}
