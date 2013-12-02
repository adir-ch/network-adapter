
// System includes          
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>

// xSL includes 
#include "network_adapter.h" 
#include "rt_route.h"
#include "rt_rule.h"
#include "xslcp_log.h"
#include "tl/ip_addr.h"

// constructor
NetworkAdapter :: NetworkAdapter() {
}

// destructor
NetworkAdapter :: ~NetworkAdapter(){
}

// error event handler 
void NetworkAdapter::errorMessageEvent(const std::string& anErrorMessage) const {
    //XSLCP_LOG("%s", anErrorMessage.c_str());
    std::cout<<"RT Message: "<< anErrorMessage << std::endl;
}

// adding a new rule 
ReturnCode NetworkAdapter :: addNewRule(unsigned int aPriority, 
                                        unsigned int aTable, 
                                        char *aDeviceName, 
                                        struct rt_subnet& anRTSubnet,
                                        unsigned int anNFMark)
{
    XSLCP_FUNC;
    
    RTRule *rule = new RTRule(this, aDeviceName);
    rule->init(aPriority,aTable,anRTSubnet.network, anRTSubnet.sig,anNFMark);
    ReturnCode status = rule->addNewRule();
    if (status == RT_NO_ERROR) 
        errorMessageEvent("Rule added");
    else 
        errorMessageEvent("Rule was not added");

    delete rule;
    return status;
}

// deleting a rule 
ReturnCode NetworkAdapter :: deleteRule(  unsigned int aPriority, 
                                          unsigned int aTable, 
                                          char  *aDeviceName, 
                                          struct rt_subnet& anRTSubnet,
                                          unsigned int anNFMark)
{
    
    XSLCP_FUNC;
    
    RTRule *rule = new RTRule(this, aDeviceName);

    rule->init(aPriority,aTable,anRTSubnet.network, anRTSubnet.sig,anNFMark);
    ReturnCode status = rule->deleteRule();
    if (status == RT_NO_ERROR) 
        errorMessageEvent("Rule added");
    else 
        errorMessageEvent("Rule was not added");

    delete rule;
    return status;
}
    

// adding a new route 
ReturnCode NetworkAdapter :: addNewRoute( unsigned int aTable,
                                          unsigned int aRealm, 
                                          char *aDeviceName, 
                                          struct rt_subnet& anRTSubnet,
                                          unsigned int aGateWay,
                                          unsigned int aUseRealmFlag)
{
      
    XSLCP_FUNC;

    RTRoute *route = new RTRoute(this, aDeviceName);

    // initializing route 
    route->init(aTable, anRTSubnet.network, anRTSubnet.sig, aRealm, aGateWay);
    
    // adding route 
    ReturnCode status = route->addNewRoute(aUseRealmFlag);
    
    if (status == RT_NO_ERROR) 
        errorMessageEvent("Route added");
    else 
        errorMessageEvent("Route was not added");

    delete route;
    return status;
}
    

// delete a route 
ReturnCode NetworkAdapter :: deleteRoute(unsigned int aTable,
                                         unsigned int aRealm, 
                                         char *aDeviceName, 
                                         struct rt_subnet& anRTSubnet,
                                         unsigned int aGateWay,
                                         int aUseRealmFlag)
{

    XSLCP_FUNC;

    RTRoute *route = new RTRoute(this, aDeviceName);

    // initializing route 
    route->init(aTable, anRTSubnet.network, anRTSubnet.sig, aRealm, aGateWay);

    // adding route 
    ReturnCode status = route->deleteRoute(aUseRealmFlag);
    
    if (status == RT_NO_ERROR) 
        errorMessageEvent("Route added");
    else 
        errorMessageEvent("Route was not added");

    delete route;
    return status;
}


int NetworkAdapter :: setInterfaceAddress(char *anInterfaceName,
                                          const SubNet& aNewSubnet)
{
    // cmd: ip addr add <ip>/<mask> dev <dev name> brd+ 
    
    if (anInterfaceName == NULL) {
        errorMessageEvent("Illegal interface name");
        return -1;
    }

    if (aNewSubnet.isV4() == 0) {
        errorMessageEvent("Only ipV4 is currently supported");
        return RT_LOGIC_ERROR;
    }

    char *cmd;
    sprintf(cmd, "ip addr add %s dev %s brd+", aNewSubnet.sprint().c_str(), anInterfaceName);
    printf("cmd: %s \n", cmd);

    if (system(NULL) == 0) {
        errorMessageEvent("Unable to find shell");
        return RT_LOGIC_ERROR;
    }

    int exitStatus = system(cmd);
    if (exitStatus == -1) {
        errorMessageEvent("Unable to execute ip commnand");
        return -1;
    } else {
        return WEXITSTATUS(exitStatus);
    } 
}

