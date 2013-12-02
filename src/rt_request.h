
#ifndef RTREQUEST_H
#define RTREQUEST_H

#define __KERNEL_STRICT_NAMES 

#include "request.h"
   
// system includes 
#include <asm/types.h>
#include <linux/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>


// netlink route or rule request
struct rrRequest {
    struct nlmsghdr header;
    struct rtmsg    data; 
    char            attrBuf[BUFSIZE];
};              

/**
 * The class represents a Rule or Route modification 
 * request.
 * 
 * @author Adir Cohen
 * @version 1.0
 * @since 1.0
 */
class RTRequest : public Request {

public:
    
     

    /**
     * Cnstructor
     * 
     * @param anErrorHandler The request error handler.
     * @param aRequestFlags The request flags as defined in netlink(7) for <i>nlmsg_type</i>.
     * @param aRequestHeaderFlags Additional flags as defined in netlink(7) for <i>nlmsg_flags</i>.
     *
     * @param aRouteTable The table where the route will be added or removed. 
     * @param anAddressMask The mask len of the destination address.
     * @param aRequestRouteType The route type as defined in rtnetlink(7) for <i>rtmsg - rtm_type</i> message.
     * @param aScopeFlag Set to true when specifing a route gateway, false otherwise. 
     *
     */
    RTRequest(const ErrorEventHandler *anErrorHandler,
              Request::requestHeaderType aRequestHeaderType,
              unsigned short aRequestHeaderFlags,
              unsigned int aRouteTable, 
              unsigned int anAddressMask,
              unsigned short aRequestRouteType,
              bool aScopeFlag);

    /** 
     * Destructor 
     */
    ~RTRequest();

    /**
     * @see Request::getFormattedRequest
     */
    char* getFormattedRequest();

    int getRequestSize() { return sizeof(m_request); }

private:

    // Message variables 
    struct {
        struct nlmsghdr header;
        struct rtmsg    data;
        char            attributesBuffer[BUFSIZE];
    } m_request;
    
    

};

#endif // RTREQUEST_H
