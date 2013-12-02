#include "network_adapter.h"
#include "rt_route.h"
#include "request.h"
#include "rt_request.h" 
#include "net_link_adapter.h"
#include "xslcp_log.h" 

//------------

#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

int main(int argc, char *argv[]){

    XSLCP_OPENLOG("XSLCP", LOG_CONS | LOG_PID, LOG_LOCAL5);
    XSLCP_FUNC;

    unsigned int net = 0;
    inet_aton("1.1.1.1", (struct in_addr*)&net);

    rt_subnet dest;
    dest.network = net;
    dest.sig = 32;


    unsigned int gateway = 0;
    inet_aton("1.1.1.2", (struct in_addr*)&gateway);
    
    rt_subnet gw;
    gw.network = gateway;
    gw.sig = 32;
    
    unsigned int table = 10, realm = 5, useRealm = 1;
        
    NetworkAdapter adapter;
    
    adapter.addNewRoute(table, realm, "eth0", gw, 0, useRealm);
    adapter.addNewRoute(table, realm, "lo", dest, 0, 0);
    adapter.addNewRule (10, 1, "eth0", dest, 3);
    
    
    adapter.deleteRoute(table, realm, "eth0", gw, 0, useRealm);
    adapter.deleteRoute(table, realm, "lo", dest, 0, 0);
    adapter.deleteRule (10, 1, "eth0", dest, 3);

        

}
