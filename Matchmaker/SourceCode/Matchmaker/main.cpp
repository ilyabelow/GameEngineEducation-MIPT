#include <iostream>
#include <enet/enet.h>
#include "Matchmaker.h"


int main()
{
    if (enet_initialize() != 0)
    {
        std::cerr  << "An error occurred while initializing ENet.\n";
        return EXIT_FAILURE;
    }
    
    Matchmaker mm;
    mm.Run();
    
    enet_deinitialize();
}