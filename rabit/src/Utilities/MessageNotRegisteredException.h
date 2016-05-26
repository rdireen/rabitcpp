#ifndef GLOBAL_PUBLISH_SUBSCRIBE_EXCEPTION
#define GLOBAL_PUBLISH_SUBSCRIBE_EXCEPTION

#include <iostream>
#include <sstream>
#include <exception>


namespace Rabit{

  class MessageNotRegisteredException : public std::exception{

  private:
    std::string _message;

  public:

    MessageNotRegisteredException(){
      _message = "Attempting to access message that does not exist in global workspace.";
    }

    MessageNotRegisteredException(std::string messageName){
      std::ostringstream os;
      os << messageName << ": Attempting to access message that does not exist in global workspace.";
      _message = os.str();
    }

    virtual const char* what() const throw(){
      return _message.c_str();
    }
  };
}

#endif //GLOBAL_PUBLISH_SUBSCRIBE_EXCEPTION
