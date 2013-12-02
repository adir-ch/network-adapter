#ifndef REQUEST_H
#define REQUEST_H
 
#include <asm/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>



// pre defined classes 
class ErrorEventHandler;

// constants 
const int BUFSIZE = 512;




/**
 * The class represents a basic request. 
 * 
 * @author Adir Cohen
 * @version 1.0
 * @since 1.0
 */
class Request { 

public:


    typedef enum {
        ADD_NEW_ROUTE,
        DELETE_ROUTE,
        ADD_NEW_RULE,
        DELETE_RULE
    } requestHeaderType;


    /** destructor */ 
    virtual ~Request(); 
       
    /**
     * The function adds a new attribute, with an 
     * unsigned int value to the request.
     * 
     * @param aMessageMaxSize The message max size.
     * @param anAttributeType The attribute type as defined in rtnetlink(7) for <i>rta_type</i>.
     * @param anAttributeData The attribute value.
     * 
     * @return True if the attribute added ok, false otherwise.
     */
    bool addu32Attribute(int aMessageMaxSize,
                         unsigned short anAttributeType,
                         unsigned int anAttributeData);

       
    /** 
     * The function return the request in a format which defined in 
     * the specific protocol.
     * 
     * @param anNLmsgHeader The request header.
     */
    virtual char* getFormattedRequest() = 0;

    
    /**
     * The function adds a custom length attribute to the 
     * request message
     * 
     * @param aMessageMaxSize The netlink message max size.
     * @param anAttributeType The attribute type as defined in rtnetlink(7) for <i>rta_type</i>
     * @param anAttributeData The attribute value.
     * @param anAttributeDataLen The attribute value length.
     * 
     * @return True if the attribute added ok, false otherwise.
     */
     bool addCustomLengthAttrbute(int aMessageMaxSize,
                                  int anAttributeType,
                                  const void *anAttributeData,
                                  int anAttributeDataLen);

   
protected:

    /** 
     * Default constructor 
     */
    Request();

    /**
     * Constructor
     * 
     * @param anErrorHandler The request error handler.
     * @param aRequestHeaderType The request flags as defined in netlink(7) for <i>nlmsg_type</i>.
     * @param aRequestHeaderFlags Additional flags as defined in netlink(7) for <i>nlmsg_flags</i>.
     * @param aRequestHeader The message header.
     */
    Request(const ErrorEventHandler *anErrorHandler,
            unsigned short aRequestHeaderType,
            unsigned short aRequestHeaderFlags, 
            struct nlmsghdr *aRequestHeader);


    /** member variables */ 
    
    // Error handler 
    const ErrorEventHandler *m_errorHandler;

    struct nlmsghdr *m_requestHeader;

};

#endif //REQUEST_H
