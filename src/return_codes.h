#ifndef ERRORCODES_H
#define ERRORCODES_H

/** Error codes */ 
typedef enum {
        RT_NO_ERROR         =  0,
        RT_LOGIC_ERROR      = -2,
        RT_ATTRIBUTE_ERROR  = -3,
        RT_ROUTE_ERROR      = -4,
        RT_RULE_ERROR       = -5,
        RT_PROTOCOL_ERROR   = -6,
        RT_UNKOWN_ERROR     = -7,
        RT_NL_MESSAGE_ERROR = -8,
        RT_NL_EOF           = -9

} ReturnCode;

#endif // ERRORCODES_H
