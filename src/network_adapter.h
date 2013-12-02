#ifndef NETWORKADAPTER_H
#define NETWORKADAPTER_H

#include <iostream> 
#include <string> 

#include "error_event_handler.h"
#include "return_codes.h"
   
                          
// pre defined classes 
class SubNet;
   
/**
 *  The struct represents a subnet.
 */
struct rt_subnet {
    /**
     *  The network IP adddress.
     */
    unsigned int network;
    /**
     *  The mask bit length in CIDR.
     */
    unsigned int sig;
};   


/**
 * The class encapsulates all the action needed for 
 * controlling linux Kernel using NetLink protocol.
 * The class contains function for controling the linux 
 * Policy Based Routing (RPDB).
 * Note: The class is not thread safe due to the use of
 *       the function send, recv (socket.h) 
 * 
 * @author Adir Cohen
 * @version 1.0
 * @since 1.0
 */
class NetworkAdapter : public ErrorEventHandler { 
    
public:
    
    /** Default constructor */ 
    NetworkAdapter();

    /** Destructor */ 
    virtual ~NetworkAdapter();

    /**
     * @see ErrorEventHandler::errorMessageEvent
     */
    void errorMessageEvent(const std::string& anErrorMessage) const;

    /**
     * The function adds a new rule to the RPDB table. 
     * 
     * @param aPriority     The Rule priority.
     *                      The priority value can be between 1 to 32765.
     * @param aTable        The table which the rules will be inserted to.
     * @param aDeviceName   The device that the rule will be activated on.
     * @param anRTSubnet    The subnet which the rule will match.
     * @param anNFMark      The nfMark to check when trying to match the rule.
     * 
     * @return          The operation end status - 0 no error, < 0 error. 
     */
    ReturnCode addNewRule(unsigned int aPriority, 
                          unsigned int aTable, 
                          char *aDeviceName, 
                          struct rt_subnet& anRTSubnet,
                          unsigned int anNFMark);

    /**
     * The function removes a rule to the RPDB table. 
     * 
     * @param aPriority     The Rule priority.
     *                      The priority value can be between 1 to 32765.
     * @param aTable        The table which the rules will be removed from.
     * @param aDeviceName   The device that the rule is activated on.
     * @param anRTSubnet    The subnet which the rule matches.
     * @param anNFMark      The nfMark to check when trying to match the rule.
     * 
     * @return         The operation end status - 0 no error, < 0 error.     
     */
    ReturnCode deleteRule(unsigned int aPriority, 
                          unsigned int aTable, 
                          char *aDeviceName, 
                          struct rt_subnet& anRTSubnet,
                          unsigned int anNFMark);
    

    /**
     * The function adds a new route to the RPDB.
     * 
     * @param aTable        The table which holds the new route.
     * @param aRealm        ????????????????????????????????
     * @param aDeviceName   The destination device.
     * @param anRTSubnet    The subnet from which the packets arrives from.
     *                      consist of two field's:
     *                      network - the network address.
     *                      sig - the network mask bits (CIDR).
     * @param aGateWay      The gateway of the packets.
     * @param aUseRealmFlag When set to true, the aRealm will be added to 
     *                      the new route, otherwise false.
     * 
     * @return        The operation end status - 0 no error, < 0 error.     
     */
    ReturnCode addNewRoute(unsigned int aTable,
                           unsigned int aRealm, 
                           char *aDeviceName, 
                           struct rt_subnet& anRTSubnet,
                           unsigned int aGateWay,
                           unsigned int aUseRealmFlag);
    
    /**
     * The function removes a route from the RPDB.
     * 
     * @param aTable        The table which holds the new route.
     * @param aRealm        ????????????????????????????????
     * @param aDeviceName   The destination device.
     * @param anRTSubnet    The subnet from which the packets arrives from.
     *                      consist of two field's:
     *                      network - the network address.
     *                      sig - the network mask bits (CIDR).
     * @param aGateWay      The gateway of the packets.
     * @param aUseRealmFlag The realm which will be attached with the packet that
     *                      was forwarded by this route.
     * 
     * @return        The operation end status - 0 no error, < 0 error.     
     */
    ReturnCode deleteRoute(unsigned int aTable,
                           unsigned int aRealm, 
                           char *aDeviceName, 
                           struct rt_subnet& anRTSubnet,
                           unsigned int aGateWay,
                           int aUseRealmFlag);

    
    /**
     * The function modifies ip address of a given interface.
     * 
     * @param anInterfaceName
     *                   The interface name.
     * @param aNewSubnet The new subnet.
     * 
     * @return The ip cmd end status.
     */
    int setInterfaceAddress(char *anInterfaceName,
                            const SubNet& aNewSubnet);
    

    ReturnCode setInterfaceQDisc();

};

#endif // NETWORKADAPTER_H

