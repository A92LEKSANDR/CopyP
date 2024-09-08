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
    std::string system_command = "echo -n \"" + text + "\" | wl-copy";
    system(system_command.c_str());  // Выполнение команды для копирования в буфер обмена
}

int main() {
    
    std::string path = getCurrentPath();
    
    if(!path.empty()){
        copyToClipBoard(path);
    }
    return 0;
}
