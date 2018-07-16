#include "hack.h"

bool Hack::init()
{
    //*/// root

    if (getuid() != 0)
    {
        endit("Cannot start as NON ROOT user.");
    }

    //*/// pid

    while (true)
    {
        if (remote::FindProcessByName(PROCESSNAME, &handle)) break;
        usleep(1000);
    }

    printab("Discovered with PID: ",handle.GetPid());

    //*/// memory regions

    client.start = 0;
    engine.start = 0;

    while (client.start == 0 || engine.start == 0)
    {
        if (!handle.IsRunning()) endit("Cannot find client_client.so and engine_client.so! (csgo stoped)");

        handle.ParseMaps();

        for (auto region : handle.regions)
        {
            if (region.filename.compare("client_panorama_client.so") == 0 && region.executable)
            {
                client = region;
            }
            if (region.filename.compare("engine_client.so") == 0 && region.executable)
            {
                engine = region;
            }
        }

        usleep(1);
        printa("region");
    }

    printhex("client.start: ",client.start);
    printhex("engine.start: ",engine.start);

    //*/// patterns

    // m_addressOfGlowPointer
    unsigned long foundGlowPointerCall = (long)client.find(handle,patternGlowPointerCall);
    unsigned long call = handle.GetCallAddress( (void*)(foundGlowPointerCall+1) );
    m_addressOfGlowPointer = handle.GetCallAddress( (void *)(call + 0x10) );
    printhex("m_addressOfGlowPointer: ",m_addressOfGlowPointer);

    // m_addressOfLocalPlayer
    unsigned long foundLocalPlayerLea = (long)client.find(handle,patternLocalPlayerLea);
    m_addressOfLocalPlayer = handle.GetCallAddress( (void *)(foundLocalPlayerLea + 8) );
    printhex("m_addressOfLocalPlayer: ",m_addressOfLocalPlayer);

    // m_addressIsConnected
    unsigned long foundIsConnectedMov = (long)engine.find(handle,patternIsConnectedMov);
    m_addressIsConnected = handle.GetCallAddress( (void *)(foundIsConnectedMov + 9) ) + 1;
    printhex("m_addressIsConnected: ",m_addressIsConnected);

    /*/// debug

    remote::MapModuleMemoryRegion *region = NULL;
    
    handle.ParseMaps();

    region = handle.GetRegionOfAddress((void*)m_addressIsConnected);
    region->print(handle,(void*)m_addressIsConnected);

    //*/// ok

    return true;
}

bool Hack::isOK()
{
    return handle.IsRunning();
}

void Hack::chConnected()
{
    /* m_addressIsConnected is outdated
    uint8_t b = 0;
    handle.Read((void *)m_addressIsConnected, &b, sizeof(b));
    isConnected = (b == 1);
    */
    isConnected = true;
}

bool Hack::players()
{

    struct GlowObjectDefinition_t g_glow[1024];
    std::array<EntityInfo, 64> entitiesLocal;

    //reset
    bzero(g_glow, sizeof(g_glow));
    for (int i = 0; i < 64; ++i)
    {
        entitiesLocal[i]=EntityInfo();
    }

    //local player
    unsigned long localPlayer = 0;
    handle.Read((void*) m_addressOfLocalPlayer, &localPlayer, sizeof(localPlayer));
    Entity local;
    if (localPlayer != 0) 
    {
        handle.Read((void *)(unsigned long)localPlayer, &local, sizeof(local));
    }

    //CGlowObjectManager
    CGlowObjectManager manager;
    if (!handle.Read((void*) m_addressOfGlowPointer, &manager, sizeof(manager))) 
    {
        endit("Failed to read glowClassAddress");
    }

    //g_glow
    size_t count = manager.m_GlowObjectDefinitions.Count;
    void *data_ptr = (void *) manager.m_GlowObjectDefinitions.DataPtr;

    printab("count ", count);
    printhex("data_ptr ",data_ptr);
    
    if (!handle.Read(data_ptr, g_glow, sizeof(GlowObjectDefinition_t) * count))
    {
        endit("Failed to read m_GlowObjectDefinitions");
    }

    //read
    for (unsigned int i = 0; i < count; i++)
    {
        if (g_glow[i].m_pEntity != NULL)
        {
            Entity ent;
            printab("ent.m_iTeamNum ",ent.m_iTeamNum);
            printab("ent.ID ",ent.ID);

            if (handle.Read(g_glow[i].m_pEntity, &ent, sizeof(ent)))
            {
                if ((ent.m_iTeamNum == 2 || ent.m_iTeamNum == 3) &&  (ent.ID <= 64 && ent.ID > 0))
                {
                    EntityInfo e;
                    e.entity = ent;
                    e.entityPtr = g_glow[i].m_pEntity;
                    e.isLocalPlayer = false;
                    entitiesLocal[ent.ID] = e;
                }
            } 
        }
    }

    //whoami
    {   
        EntityInfo e;
        e.entity = local;
        e.entityPtr = (void *)(unsigned long)localPlayer;
        e.isLocalPlayer = true;
        entitiesLocal[local.ID] = e;
    }

    writeEntities(entitiesLocal);

    return true;
}


//Maybe some lock mechanism??
void Hack::readEntities(std::array<EntityInfo, 64> &rentities)
{
    rentities = entities;
}

void Hack::writeEntities(std::array<EntityInfo, 64> &wentities)
{
    entities = wentities;
}