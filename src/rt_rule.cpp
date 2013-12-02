

#include "rt_rule.h"
#include "net_link_adapter.h"
#include "error_event_handler.h"
#include "xslcp_log.h"

#include <arpa/inet.h>
#include <string.h>


// constructor                                                    
RTRule :: RTRule(const ErrorEventHandler *anErrorHandler, 
                 char *aDeviceName)
{
    m_errorEventHandler = anErrorHandler;
    m_nlAdapter = new NetLinkAdapter(anErrorHandler);
    m_deviceName = aDeviceName;
}
        
// destructor
RTRule :: ~RTRule()
{
    delete m_nlAdapter;
    if (m_request) {
        delete m_request;
    }
    
}

// initialize
bool RTRule :: init( unsigned int aPriority,
                     unsigned int aTable, 
                     unsigned int anIPAddress,
                     unsigned int anAddressMask,
                     unsigned int anNFMark)
{
    m_priority = aPriority;
    m_ruleTable = aTable;
    m_destIPAddress = anIPAddress;
    m_destAddressMask = anAddressMask;
    m_nfMark = anNFMark;
}


// adding a new rule 
ReturnCode RTRule :: addNewRule() {
    return modifyRule(RTRequest::ADD_NEW_RULE);
    
}


ReturnCode RTRule :: deleteRule(){
    return modifyRule(RTRequest::DELETE_RULE);
    
}

ReturnCode RTRule :: modifyRule(Request::requestHeaderType aRequestType) 
{
    
    XSLCP_FUNC;

    // building a rule request - scope should be RT_SCOPE_UNIVERSE    
    m_request = new RTRequest(m_errorEventHandler,
                              aRequestType,
                              NLM_F_REQUEST | NLM_F_ACK | NLM_F_CREATE | NLM_F_EXCL,
                              m_ruleTable,
                              m_destAddressMask,
                              RTN_UNICAST,
                              true);


    
   
    // adding attributes

    // source - from all sources  
    if (! m_request->addu32Attribute(sizeof(rrRequest), RTA_SRC, 0)){
        return RT_ATTRIBUTE_ERROR;
    }

    // destination 
    if (! m_request->addu32Attribute(sizeof(rrRequest), RTA_DST, m_destIPAddress)){
        return RT_ATTRIBUTE_ERROR;
    }
    
    // rule priority
    if (! m_request->addu32Attribute(sizeof(rrRequest), RTA_PRIORITY, m_priority)){
        return RT_ATTRIBUTE_ERROR;
    }

    // nfmark
    if (m_nfMark != 0) {
        if(! m_request->addu32Attribute(sizeof(rrRequest), RTA_PROTOINFO, m_nfMark))
            return RT_ATTRIBUTE_ERROR;
    }
        
    // device name 
    if (m_deviceName != NULL) {
        if (! m_request->addCustomLengthAttrbute(sizeof(rrRequest),  
                                                 RTA_IIF, 
                                                 m_deviceName,
                                                 strlen(m_deviceName) + 1)) 
        
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
    }

    return RT_NO_ERROR;

}
