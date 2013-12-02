#ifndef ERROREVENTHANDLER_H
#define ERROREVENTHANDLER_H

#include <string>


/**
 * The defines a set of function for handeling error 
 * events. 
 * To support error events, you must inherit the class and 
 * override the function <code>errorMessageEvent</code>. 
 * 
 * @author Adir Cohen
 * @version 1.0
 * @since 1.0
 */
class ErrorEventHandler {
    
public:

    /** Destructor */ 
    virtual ~ErrorEventHandler(); 

    /** 
     * The function which will be invoked when an error
     * event occurs.
     * 
     * @param anErrorMessage The error message.
     */
    virtual void errorMessageEvent(const std::string& anErrorMessage) const = 0;


protected:

    /**
     * Default constructor
     */
    ErrorEventHandler(); 
    
};

#endif // ERROREVENTHANDLER_H
