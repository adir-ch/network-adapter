
#include "rt_route.h"
#include "net_link_adapter.h"
#include "error_event_handler.h"
#include "xslcp_log.h"

#include <arpa/inet.h>


// constructor                                                    
RTRoute :: RTRoute(const ErrorEventHandler *anErrorHandler, 
                   char *aDeviceName)
{
    m_errorEventHandler = anErrorHandler;
    m_nlAdapter = new NetLinkAdapter(anErrorHandler);
    m_deviceName = aDeviceName;
}
        
// destructor
RTRoute :: ~RTRoute()
{
    delete m_nlAdapter;
    if (m_request) {
        delete m_request;
    }
    
}

// initialize
bool RTRoute :: init( unsigned int aTable, 
                      unsigned int anIPAddress,
                      unsigned int anAddressMask,
                      unsigned int aRealm,
                      unsigned int aGateWay)
{
    m_routeTable = aTable;
    m_destIPAddress = anIPAddress;
    m_destAddressMask = anAddressMask;
    m_realm = aRealm;
    m_gateWay = aGateWay;
}


// adding a new route 
ReturnCode RTRoute :: addNewRoute(bool aUseRealmFlag) {

    return modifyRoute(RTRequest::ADD_NEW_ROUTE, aUseRealmFlag);
}

// removing route
ReturnCode RTRoute :: deleteRoute(bool aUseRealmFlag){
    return modifyRoute(RTRequest::DELETE_ROUTE, aUseRealmFlag);
 
}


ReturnCode RTRoute :: modifyRoute(RTRequest::requestHeaderType aRequestType,
                                  bool aUseRealmFlag)
{
    
    XSLCP_FUNC;

    bool useGateWayFlag = true;
    if (m_gateWay <= 0) 
        useGateWayFlag = false;

    m_request = new RTRequest(m_errorEventHandler,
                              aRequestType,
                              NLM_F_REQUEST | NLM_F_ACK | NLM_F_CREATE | NLM_F_EXCL,
                              m_routeTable,
                              m_destAddressMask,
                              RTN_UNICAST,
                              useGateWayFlag);


    
   
    // adding attributes
    if (! m_request->addu32Attribute(sizeof(rrRequest), RTA_DST, m_destIPAddress)){
        return RT_ATTRIBUTE_ERROR;
    }
    
    if (aUseRealmFlag) {
        if(! m_request->addu32Attribute(sizeof(rrRequest), RTA_FLOW, m_realm))
            return RT_ATTRIBUTE_ERROR;
    }
    
    if (m_deviceName != NULL) {
        if (! m_request->addu32Attribute(sizeof(rrRequest), 
                                     RTA_OIF, m_nlAdapter->getIfIndexByName(m_deviceName)))
            return RT_ATTRIBUTE_ERROR;
    }
    

    if (m_gateWay != 0) {
        m_gateWay = htonl(m_gateWay);
        if (! m_request->addCustomLengthAttrbute(sizeof(rrRequest), RTA_GATEWAY, &m_gateWay, 4))
            return RT_ATTRIBUTE_ERROR;
    }
   


    // creating netlink request 
    struct nlmsghdr *header = (struct nlmsghdr*)m_request->getFormattedRequest();

    if (header->nlmsg_len == 0) {
        m_errorEventHandler->errorMessageEvent("Unable to create netlink request");
        return RT_LOGIC_ERROR;
    }
    
    // sending the request
    if (! m_nlAdapter->netLinkTalk(header)) {
        return RT_ROUTE_ERROR;
        // sending the request
    }
    
    return RT_NO_ERROR;
}

