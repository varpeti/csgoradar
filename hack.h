#ifndef __HACK_H_
#define __HACK_H_

#include <iostream>

#include "remote.h"
#include "csgo.h"

#define endit(s) {std::cout << s << "\n";  return false;}
#define printa(a) {std::cout << a << "\n";}
#define printab(a,b) {std::cout << a << b <<"\n";}
#define printhex(a,b) {std::cout << a << std::hex << b <<"\n";}

struct EntityInfo
{
    Entity entity;
    void* entityPtr;
    bool isLocalPlayer;
};

class Hack
{
    public:
        bool isConnected;
        
    private:
        remote::Handle handle;

        remote::MapModuleMemoryRegion client;
        remote::MapModuleMemoryRegion engine;

        unsigned long m_addressOfGlowPointer;
        unsigned long m_addressOfLocalPlayer;
        unsigned long m_addressIsConnected;

        std::array<EntityInfo,64> entities;

    public:
        bool init();
        bool players();
        bool isOK();
        void chConnected();
        void readEntities(std::array<EntityInfo,64> &rentities);

    private:
        void writeEntities(std::array<EntityInfo,64> &wentities);

    
};

#endif