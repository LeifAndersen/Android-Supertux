#ifndef HEADER_SUPERTUX_ANDROIDLOG_HPP
#define HEADER_SUPERTUX_ANDROIDLOG_HPP

#include <android/log.h>
#include <ostream>
#include <sstream>
#include <iostream>

class StreamBuffer;

class droidStream
{
  public:
  static std::ostream output; 
    static StreamBuffer outputBuffer; /**< stream buffer used by output stream */

  friend class StreamBuffer;
};

class StreamBuffer : public std::stringbuf
{
  
public:
  int sync()
  {
    int result = std::stringbuf::sync();

	__android_log_print(ANDROID_LOG_INFO, "Supertux", str().c_str());
	str(std::string());
    return result;
    
  }
};




#endif