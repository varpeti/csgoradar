#include <thread>
#include <iostream>

#include "hack.h"
#include "window.h"

#define endit(s) {std::cout << s << "\n";  return false;}

Hack hack;

void hackc()
{
    while(hack.isOK())
    {
        hack.chConnected(); //ingame check
        sleep(5);
    }
}

void hackr()
{
    while(hack.isOK())
    {
        if (!hack.isConnected) 
        {
            sleep(5);
            continue;
        }
        hack.players();

        usleep(100);
    }
}

void window()
{
    execQApp(&hack);
}

int main()
{
    
    if (!hack.init()) endit("FAIL init");

    std::thread thackc(hackc);
    std::thread thackr(hackr);

    window();

    thackc.join();

    std::cout << "OK" << "\n";

    getchar();
    return 0;
}
