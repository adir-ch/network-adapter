
#ifndef RTROUTE_H 
#define RTROUTE_H 

#include <iostream>
#include <string>
#include "return_codes.h" 
#include "rt_request.h"

// pre defined classes 
class ErrorEventHandler;
class NetLinkAdapter;
class Request; 

/**
 * The class RTRoute_Represents an RPDB route. The class encapsulate 
 * all the basic functions for modifying the routes in the RPDB.
 * 
 * @author Adir Cohen
 * @version 1.0
 * @since 1.0
 */
class RTRoute { 

public:
    
    /** 
     * Default constructor. 
     * 
     * @param errorHandler The route error handler. 
     * @param aDeviceName The device to be modified. 
     */
    RTRoute(const ErrorEventHandler *anErrorHandler, 
            char *aDeviceName);
        
    
    /** 
     * The function initialize an RPDB route. 
     * 
     * @param aTable        The table which the routes will be inserted to.
     * @param anIPAddress   The subnet IP Address which the route will match.
     * @param anAddressMask The IP address subnet mask in CIDR. 
     * @param anNFMark      The nfMark to check when trying to match the route.
     * @param aGateWay      The new route gateway.
     * 
     * @return  True when the route initialized successfully, false otherwise.
     */
    bool init(unsigned int aTable, 
              unsigned int anIPAddress,
              unsigned int anAddressMask,
              unsigned int aRealm,
              unsigned int aGateWay);

    /**
     * Destructor
     */
    virtual ~RTRoute();

    /**
     * The function adds the route to the RPDB.
     * 
     * @param aUseRealmFlag When set to true, the aRealm will be added to 
     *                      the new route, otherwise false.
     * 
     * @return The operation status.
     */
    ReturnCode addNewRoute(bool aUseRealmFlag);

    /**
     * The function removes the route to the RPDB.
     * 
     * @param aUseRealmFlag When set to true, the aRealm will be added to 
     *                      the new route, otherwise false.
     * 
     * @return The operation status.
     */
    ReturnCode deleteRoute(bool aUseRealmFlag);

private:

    /**
     * Modifies the rule in RPDM.
     * 
     * @param aRequestType The modificaton type - add or remove. 
     * @param aUseRealmFlag When set to true, the aRealm will be added to 
     *                      the new route, otherwise false.
     * 
     * @return Status of the operation - 0 success < 0 failed.
     */
    ReturnCode modifyRoute(RTRequest::requestHeaderType aRequestType,
                           bool aUseRealmFlag);


    /** member variables */ 
    const ErrorEventHandler *m_errorEventHandler;
    NetLinkAdapter *m_nlAdapter;
    Request *m_request;
    
    /** route info */ 
    char  *m_deviceName; 
    unsigned int m_routeTable; 
    unsigned int m_destIPAddress;
    unsigned int m_destAddressMask;
    unsigned int m_realm;
    unsigned int m_gateWay;
};

#endif // RTROUTE_H
