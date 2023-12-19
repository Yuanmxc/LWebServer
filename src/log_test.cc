#include <unistd.h>
#include "../log/logging.h"
#include "../log/logfile.h"

using namespace std;

std::unique_ptr<ws::detail::logfile> g_logFile;

void outputFunc(const char* msg, int len)
{
  g_logFile->append(msg, len);
}

void flushFunc()
{
  g_logFile->flush();
}

int main(int argc, char* argv[])
{
  char name[256] = { 0 }; 
  strncpy(name, argv[0], sizeof name - 1);
  g_logFile.reset(new ws::detail::logfile(::basename(name), 200*1000));
  ws::detail::logging::setOutput(outputFunc);
  ws::detail::logging::setFlush(flushFunc); 
  std::string line = "1234567890 abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

  for (int i = 0; i < 2000000; ++i)
  {
    ws::detail::log_INFO(__FILE__, __LINE__, errno).stream() << line << i << ":" << "\n";

    //usleep(1000);
  }
}