#include "util/androidlog.hpp"
 
StreamBuffer  droidStream::outputBuffer;
std::ostream droidStream::output(&outputBuffer);