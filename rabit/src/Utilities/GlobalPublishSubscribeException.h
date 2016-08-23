#ifndef GLOBAL_PUBLISH_SUBSCRIBE_EXCEPTION
/* ****************************************************************
 * Rabit Multi-Threaded Management System
 * Athrs: Randal Direen PhD
 *        Harry Direen PhD
 * www.direentech.com
 * Date: June 2016
 *
 *******************************************************************/

#define GLOBAL_PUBLISH_SUBSCRIBE_EXCEPTION

#include <iostream>
#include <sstream>
#include <exception>


namespace Rabit
{

    class GlobalPublishSubscribeException : public std::exception
    {

    private:
        std::string _message;

    public:

        GlobalPublishSubscribeException()
        {
            _message = "Attempting to get copy of global message without a reference to the global message established";
        }

        virtual const char *what() const throw()
        {
            return _message.c_str();
        }
    };
}

#endif //GLOBAL_PUBLISH_SUBSCRIBE_EXCEPTION
