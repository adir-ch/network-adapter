
#include "net_link_adapter.h" 
#include "error_event_handler.h" 
#include "request.h"
#include "xslcp_log.h"

#define __KERNEL_STRICT_NAMES

#include <linux/if.h>
#include <linux/if_ether.h>
#include <sys/ioctl.h>
#include <linux/socket.h>
#include <linux/rtnetlink.h>
#include <linux/netlink.h>
#include <linux/netdevice.h>
#include <string.h>
#include <errno.h>
#include <asm/types.h>
#include <unistd.h>

int NetLinkAdapter::m_messageCounter = 10;

// Constructor 
NetLinkAdapter :: NetLinkAdapter(const ErrorEventHandler *anErrorHandler) 
{
    XSLCP_FUNC;
    m_errorHandler = anErrorHandler;
    m_pid = getpid();


    // Creating a netlink socket
    m_rtNetlinkSocket = 0;
    m_rtNetlinkSocket = socket (PF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (m_rtNetlinkSocket < 0) {
        m_errorHandler->errorMessageEvent("Unable to create netlink socket !!!");
    }
}


// Destructor
NetLinkAdapter :: ~NetLinkAdapter() {
    if (m_rtNetlinkSocket)
        close(m_rtNetlinkSocket);
}

// Netlink send message 
int NetLinkAdapter::netlinkSend(void *aNetLinkMessageHeader)
{
    XSLCP_FUNC;

    struct nlmsghdr *header = (struct nlmsghdr*) aNetLinkMessageHeader;

    if (m_rtNetlinkSocket == 0)
        return -1;

    header->nlmsg_seq = ++m_messageCounter;
    header->nlmsg_pid = m_pid;

    int sendStatus = send(m_rtNetlinkSocket, header, header->nlmsg_len,0);

    return sendStatus;
}


// Netlink receive  message 
int NetLinkAdapter :: netLinkRecv(void *aNetLinkMessageHeader, int &aNetLinkResponse) 
{
    XSLCP_FUNC;

    struct nlmsghdr *sentMessage = (struct nlmsghdr*) aNetLinkMessageHeader;
    int readLength = 0;

    // set recv messages buffer
    char messageBuffer[RECV_BUFFER_SIZE];
    memset(messageBuffer, 0, RECV_BUFFER_SIZE);
    bool messageFlag = true; 

    while (messageFlag) {

        // reading frm socket 
        if ( (readLength = recv(m_rtNetlinkSocket, messageBuffer, RECV_BUFFER_SIZE, 0)) < 0 ) {
            m_errorHandler->errorMessageEvent("Unable to read response from netlink socket");
            return -1;
        }

        struct nlmsghdr *currentMessage = (struct nlmsghdr*)messageBuffer;
       
        while (1) {
            
            if (validateRecvMessage(currentMessage, readLength, sentMessage) < 0) {
                return -1;
            }
           
            if (currentMessage->nlmsg_type == NLMSG_ERROR) {
                aNetLinkResponse = parseNetLinkError(currentMessage);
                return 0;
            }

            if (currentMessage->nlmsg_type != NLMSG_DONE) {
                currentMessage = NLMSG_NEXT(currentMessage, currentMessage->nlmsg_len);
            } else {
                messageFlag = false;
                break;
            }
        }
    }

    return 0;
}

int NetLinkAdapter :: validateRecvMessage(struct nlmsghdr *aMessageHeader,
                                          int aReadLength,
                                          struct nlmsghdr *aSentMessageHeader) {

    // checking read length
    if (aReadLength <= 0) {
        m_errorHandler->errorMessageEvent("Netlink EOF");
        return -1;
    }

    int messageLen = aMessageHeader->nlmsg_len;
    int dataLen = messageLen - sizeof(*aMessageHeader);

    if (dataLen < 0 || messageLen > aReadLength) {
        char *msg;
        sprintf(msg,"Malformed message: len=%d\n", messageLen); 
        m_errorHandler->errorMessageEvent(msg);
        return -1;
    }
   

    // checking that the message is correct
    if (!NLMSG_OK(aMessageHeader, aReadLength)) {
        m_errorHandler->errorMessageEvent("Malformed netlink message");
        return -1;
    }

    if (aMessageHeader->nlmsg_pid != aSentMessageHeader->nlmsg_pid || 
        aMessageHeader->nlmsg_seq != aSentMessageHeader->nlmsg_seq) {
        m_errorHandler->errorMessageEvent("Unknown message received from netlink");
        return -1;
    }

    return 0;
}


// Netlink talk (send and receive)
bool NetLinkAdapter :: netLinkTalk(void *aNetLinkMessageHeader) 
{

    XSLCP_FUNC;

    //char *errorMessage; 
    std::string errorMessage;

    // sending message
    int status = netlinkSend(aNetLinkMessageHeader);
    if (status < 0) {
        errorMessage = strerror(errno);
        m_errorHandler->errorMessageEvent("Error while sending message to netlink: " + errorMessage);
        return false; 
    }

    // reading received message
    int ackStatus = 0;
    status = netLinkRecv(aNetLinkMessageHeader, ackStatus);
    if (status < 0) {
        errorMessage = strerror(errno);
        m_errorHandler->errorMessageEvent("Error while receiving message from netlink: " + errorMessage);
        return false; 
    }

    if (ackStatus == 0)
        return true;

    return false;
}


int NetLinkAdapter :: getIfIndexByName(char *anIfName) 
{

    XSLCP_FUNC;

    struct ifreq ifr;
    int sd;

    /* packet socket */
    sd = socket(PF_PACKET, SOCK_RAW, 0);

    if (sd < 0) {
        m_errorHandler->errorMessageEvent("Error while opening socket for ioctl");
        return -1;
    }

    strcpy(ifr.ifr_name, anIfName);
    int status = ioctl(sd, SIOCGIFINDEX, &ifr);

    if (status < 0) {
        m_errorHandler->errorMessageEvent("Error while getting info from ioctl");
        return -1;
    }

    close(sd);
    return ifr.ifr_ifindex;
}


int NetLinkAdapter :: parseNetLinkError(struct nlmsghdr *header)
{

    XSLCP_FUNC;

    int dataSize = header->nlmsg_len - sizeof(struct nlmsghdr);
    if (dataSize < sizeof(struct nlmsgerr)) {
        m_errorHandler->errorMessageEvent("Malformed netlink error message");
        return -1; 
    }

    struct nlmsgerr *error = (struct nlmsgerr *) NLMSG_DATA(header);
    char errorMessage[BUFSIZE];

    sprintf(errorMessage, 
            "%s(%d) -- sent message type was %d", 
            strerror(-error->error),
            -error->error,
            error->msg.nlmsg_type); 

    m_errorHandler->errorMessageEvent(errorMessage); 
    return error->error;
}


int NetLinkAdapter :: parse_rtattr(struct rtattr *tb[], 
                                   int max, 
                                   struct rtattr *rta, 
                                   int len)
{
    while (RTA_OK(rta, len)) {
        if (rta->rta_type <= max)
            tb[rta->rta_type] = rta;
        rta = RTA_NEXT(rta,len);
    }
    if (len)
        m_errorHandler->errorMessageEvent("Malformed attribute");
    return 0;
}
