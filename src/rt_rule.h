

#ifndef RTRULE_H 
#define RTRULE_H 

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
class RTRule { 

public:
    
    /** 
     * Default constructor. 
     * 
     * @param errorHandler The rule error handler. 
     * @param aDeviceName The device to be modified. 
     */
    RTRule(const ErrorEventHandler *anErrorHandler, 
           char *aDeviceName);
        
    
    /** 
     * The function initialize an RPDB rule. 
     * 
     * @param aPriority     The rule priority - can be an unsigned number between 1 to 32765.
     * @param aTable        The table which the rules will be inserted to.
     * @param anIPAddress   The subnet IP Address which the rule will match.
     * @param anAddressMask The IP address subnet mask in CIDR. 
     * @param anNFMark      The nfMark to check when trying to match the rule.
     * 
     * @return  True when the rule initialized successfully, false otherwise.
     */
    bool init(unsigned int aPriority,
              unsigned int aTable, 
              unsigned int anIPAddress,
              unsigned int anAddressMask,
              unsigned int anNFMark);

    /**
     * Destructor
     */
    virtual ~RTRule();

    /**
     * The function adds the route to the RPDB.
     * 
     * @return The operation status.
     */
    ReturnCode addNewRule();

    /**
     * The function removes the route to the RPDB.
     * 
     * @return The operation status.
     */
    ReturnCode deleteRule();

private:

    /**
     * Modifies the rule in RPDM.
     * 
     * @param aRequestType The modificaton type - add or remove. 
     * 
     * @return Status of the operation - 0 success < 0 failed.
     */
    ReturnCode modifyRule(RTRequest::requestHeaderType aRequestType);
    
    /** member variables */ 
    const ErrorEventHandler *m_errorEventHandler;
    NetLinkAdapter *m_nlAdapter;
    Request *m_request;
    
    /** route info */ 
    char  *m_deviceName; 
    unsigned int m_priority;
    unsigned int m_ruleTable; 
    unsigned int m_destIPAddress;
    unsigned int m_destAddressMask;
    unsigned int m_nfMark;
};

#endif // RTRULE_H
