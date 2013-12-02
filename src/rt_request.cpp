#include "rt_request.h" 
#include "request.h" 

#include <asm/types.h>
#include <linux/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>

                                                       
// constructor 
RTRequest :: RTRequest(const ErrorEventHandler *anErrorHandler,
                       Request::requestHeaderType aRequestHeaderType,
                       unsigned short aRequestHeaderFlags,                       
                       unsigned int aRouteTable, 
                       unsigned int anAddressMask,
                       unsigned short aRequestRouteType,
                       bool aScopeFlag)
                       : Request(anErrorHandler, aRequestHeaderType, aRequestHeaderFlags, &m_request.header)
{
    
    // building request header
    m_request.header.nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg));

    // building request data info 
    m_request.data.rtm_family   = AF_INET;
    m_request.data.rtm_table =    aRouteTable;
    m_request.data.rtm_dst_len =  anAddressMask;
    m_request.data.rtm_type     = aRequestRouteType;
    m_request.data.rtm_protocol = RTPROT_BOOT;
    
    // when using gateway rtm_scope must be set to RT_SCOPE_UNIVERSE
    if (aScopeFlag) 
        m_request.data.rtm_scope = RT_SCOPE_UNIVERSE;
    else 
        m_request.data.rtm_scope = RT_SCOPE_LINK;

    
    // zeroz the attributes buffer 
    memset(m_request.attributesBuffer, 0, sizeof(m_request.attributesBuffer));
}

// destructor 
RTRequest :: ~RTRequest(){
}

// returns a formatted request for netlink protocol
char* RTRequest :: getFormattedRequest() {
    return (char*)&m_request.header;
}


