
#define __KERNEL_STRICT_NAMES

#include "request.h"
#include "error_event_handler.h" 
#include "xslcp_log.h"

#include <asm/types.h>
#include <linux/socket.h>
#include <linux/rtnetlink.h>
#include <linux/netlink.h>
#include <unistd.h>

// Constructor
Request :: Request (const ErrorEventHandler *anErrorHandler,
                    unsigned short aRequestHeaderType,
                    unsigned short aRequestHeaderFlags,
                    struct nlmsghdr *aRequestHeader) 
{
    m_errorHandler = anErrorHandler;
    m_requestHeader = aRequestHeader;
    m_requestHeader->nlmsg_flags = aRequestHeaderFlags;
   
    switch (aRequestHeaderType) {
    case Request::ADD_NEW_ROUTE:
        m_requestHeader->nlmsg_type = RTM_NEWROUTE;
        break;
    case Request::DELETE_ROUTE:
        m_requestHeader->nlmsg_type = RTM_DELROUTE;
        break;
    case Request::ADD_NEW_RULE:
        m_requestHeader->nlmsg_type = RTM_NEWRULE;
        break;
    case Request::DELETE_RULE:
        m_requestHeader->nlmsg_type = RTM_DELRULE;
        break;
    default:
        m_errorHandler->errorMessageEvent("Unsupported request type !!!");
        break;
    }

}

// Destructor 
Request::~Request(){
}


// adding attribute
bool Request :: addu32Attribute(int aMessageMaxSize, 
                                unsigned short anAttributeType,
                                unsigned int anAttributeData)
{
    
    XSLCP_FUNC;

    int dataLength = RTA_LENGTH(4);
	
	if (NLMSG_ALIGN(m_requestHeader->nlmsg_len) + dataLength > aMessageMaxSize)
		return false;

    /** building attributest */
    struct rtattr *rtAttrib;
    rtAttrib = (struct rtattr *)(((char *) m_requestHeader) + NLMSG_ALIGN(m_requestHeader->nlmsg_len));
    rtAttrib->rta_type = anAttributeType;
    rtAttrib->rta_len = RTA_LENGTH(4);
    memcpy(RTA_DATA(rtAttrib), &anAttributeData, sizeof(4));
    m_requestHeader->nlmsg_len = NLMSG_ALIGN(m_requestHeader->nlmsg_len) + RTA_LENGTH(4);

    // cheking that the attribute is in the correct size 
    if (!RTA_OK(rtAttrib, rtAttrib->rta_len)) {
        m_errorHandler->errorMessageEvent("Unable to add the attribute !!!");
        return false; 
    }

    return true;
}

bool Request :: addCustomLengthAttrbute(int aMessageMaxSize,
                                        int anAttributeType,
                                        const void *anAttributeData,
                                        int anAttributeDataLen)

{

    XSLCP_FUNC;

    int dataLen = RTA_LENGTH(anAttributeDataLen);
	struct rtattr *rta;

	if (NLMSG_ALIGN(m_requestHeader->nlmsg_len) + dataLen > aMessageMaxSize)
		return false;

	rta = (struct rtattr*)(((char*)m_requestHeader) + NLMSG_ALIGN(m_requestHeader->nlmsg_len));
	rta->rta_type = anAttributeType;
	rta->rta_len = dataLen;
	memcpy(RTA_DATA(rta), anAttributeData, dataLen);
	m_requestHeader->nlmsg_len = NLMSG_ALIGN(m_requestHeader->nlmsg_len) + dataLen;
	return true;
}




