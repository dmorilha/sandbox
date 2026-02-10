#include <iostream>
#include <sstream>
#include <string>

#include <cstring>
#include <cstdio>
#include <cstdlib>

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char * * argv) {
  std::string body = "<h1>Hello World</h1>";
  if (1 < argc) {
    body = std::string(argv[1]);
  }

  std::stringstream stream;
  stream << "HTTP/1.0 200 OK" "\n"
    "Content-Type: text/html" "\n"
    "Content-Length: " << body.size() << "\n"
    "\n"
    << body;

  const std::string response = stream.str();

  const int sockfd = socket(/* ipv4 */ AF_INET, /* tcp */ SOCK_STREAM, 0);

  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY; /* any interface */
  serv_addr.sin_port = htons(80); /* port 80 */

  bind(sockfd, reinterpret_cast<struct sockaddr *>(&serv_addr), sizeof(serv_addr));
  listen(sockfd, 5);

  while (true) {
    // Daemon main loop: accept connections, process, etc.
    const int newsockfd = accept(sockfd, nullptr, nullptr);
    if (newsockfd >= 0) {
      std::cout << "new request" << std::endl;
      write(newsockfd, response.c_str(), response.size());
      close(newsockfd);
    }
    usleep(10'000); /* sleep for 10 milli */
  }
  return 0;
}
