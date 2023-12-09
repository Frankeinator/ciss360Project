#ifndef DATA_H
#define DATA_H
#include <vector>
#include <cstdint>
#include <string>

class Data
{
  public:
    enum class Type { INT, STRING, NONE };
  private:
    Type currentType;

    //
    std::vector < int32_t >* intData = nullptr;
    std::vector < std::string >* stringData = nullptr;
    
};


#endif
