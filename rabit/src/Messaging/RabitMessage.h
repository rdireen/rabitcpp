#ifndef RABIT_MESSAGE
#define RABIT_MESSAGE

#include <chrono>
#include <string>
#include <memory>

namespace Rabit{

  class RabitMessage {
  private:
    typedef std::chrono::high_resolution_clock::time_point TimeStamp;
    typedef std::shared_ptr<RabitMessage> RabitMessage_Sptr;
    TimeStamp _timeStamp;
    std::string _messageTypeName;
  public:
    RabitMessage(std::string messageTypeName){
      _messageTypeName = messageTypeName;
    }

    virtual void CopyMessage(RabitMessage_Sptr msg) = 0;

    virtual void Clear() = 0;

    virtual std::string ToString() const = 0;

    virtual RabitMessage_Sptr Clone() = 0;

    void SetTimeNow(){
      _timeStamp = std::chrono::high_resolution_clock::now();
    }

    const TimeStamp& GetTimeStamp() const{
      return _timeStamp;
    }

    bool PostMessage(){

    }

    bool ForceFetchMessage(){

    }

    bool FetchMessage(){

    }

    /*
     * If equal in time, this function returns true;
     */
    bool CompareTime(const std::shared_ptr<RabitMessage>& rabitMessage){
      if(this->GetTimeStamp() == rabitMessage->GetTimeStamp())
        return true;
      else
        return false;
    }

    std::string GetMessageTypeName() const{
      return _messageTypeName;
    }
  };
  

}



#endif //RABIT_MESSAGE
