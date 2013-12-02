#ifndef NETLINKADAPTER_H
#define NETLINKADAPTER_H

  
#include <asm/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <string>
       
//pre defined classes 
class ErrorEventHandler;

const int RECV_BUFFER_SIZE = 8192;

/**
 * The class responsible for communicating with the Linux Kernel 
 * by sending and receiving a netlink packets. 
 * 
 * @author Adir Cohen
 * @version 1.0
 * @since 1.0
 */
class NetLinkAdapter {  

public:
    
    /** 
     * Default Constructor.
     * @param anErrorHandler The error handler.
     */ 
    NetLinkAdapter(const ErrorEventHandler *anErrorHandler);

    /** Destructor */ 
    virtual ~NetLinkAdapter();

    
    

    /**
     * The function converts an interface name to an interface 
     * index.
     * 
     * @param anIfName The name of the interface
     * 
     * @return The index of the interface.
     */
    int getIfIndexByName(char *anIfName);

       
    /**
     * The function parses a netlink error message.
     * 
     * @param aHeader The netlink error message header.
     * 
     * @return The error code.
     */
    int parseNetLinkError(struct nlmsghdr *aHeader);


    /**
     * The function parses at array of attributes. 
     * 
     * @param tb     The attributes array
     * @param max
     * @param rta    The first attribute
     * @param len    The first attribute length
     * 
     * @return 
     */
    int parse_rtattr(struct rtattr *tb[], 
                     int max, 
                     struct rtattr *rta, 
                     int len);

    /**
     * Sending and receiving message to netlink
     * 
     * @param aNetlinkMessage The net link message buffer.
     * 
     * @return True if the session ended ok, false otherwise. 
     */
    bool netLinkTalk(void *aNetLinkMessageHeader);

private:

    /**
     * Sending a netlink message.
     * 
     * @param aNetlinkMessage The message buffer.
     * 
     * @return The number of sent bytes.
     */
    int netlinkSend(void *aNetLinkMessageHeader);

    /**
     * Receives a netlink message, and sets the the - aNetLinkResponse with 
     * the netlink return ack code.
     * 
     * @param aMessageBuffer The sent message buffer.
     * @param aNetLinkResponse The variable will be set with the netlink ack status.
     * 
     * @return The number of sent bytes.
     */
    int netLinkRecv(void *aNetLinkMessageHeader, int &aNetLinkResponse);

    /**
     * Validates that the received message is ok.
     * 
     * @param aNetLinkMessageBuffer The read message buffer.
     * @param aReadLength The read message length.
     * @param aSentMessageHeader The sent message header. 
     * 
     * @return 0 - when message was ok, -1 otherwise..
     */
    int validateRecvMessage(struct nlmsghdr *aMessageHeader,
                            int aReadLength,
                            struct nlmsghdr *aSentMessageHeader);


    /** member variables */ 
    static int m_messageCounter;
    const ErrorEventHandler *m_errorHandler; 
    int m_rtNetlinkSocket;
    int m_pid;

};



#endif // NETLINKADAPTER_H
