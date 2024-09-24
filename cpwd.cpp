#include <iostream>
#include <cstring>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <unistd.h>     //for getcwd()
#include <limits.h>     //Path_max
//#include <string>

std::string getCurrentPath(const char command = '\0'){
    char buffer[PATH_MAX];
    
    if(getcwd(buffer,sizeof(buffer)) != nullptr){
        if(command == 'c'){
            return std::string("cd ") + std::string(buffer,strlen(buffer));      
        }
        else if(command == '\0'){
            return std::string(buffer,strlen(buffer));
        }else
        {
            return "";
        }      //safe construction dont use '/0'
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

int main(int argc, char** argv) {
    
    if(argc == 1){
        std::string path = getCurrentPath();
        copyToClipBoard(path);
        if(!path.empty()){
            copyToClipBoard(path);
        }
    }

    for(int i = 1; i < argc; ++i){
        std::string argument = argv[i];
        
        if(argument == "--help"){
            std::cout << "Это справка программы cpwd. Доступные команды:\n"
                      << "-cd: Будет подставлен \"cd+путь директории\"\n";
        }else if(argument == "-cd"){
            std::string path  = getCurrentPath('c');
            copyToClipBoard(path);
        }
         else {
            std::cerr << "Ошибка: неподдерживаемый аргумент \"" << argument << "\".\n"
                          << "Используйте --help для получения справки.\n";                          
                return 1; // Завершаем с ненулевым кодом ошибки  
        }
        }

       return 0;
}
