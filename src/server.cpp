#include "server.hpp"

#include "backend.hpp"

int main(int argc, char** argv) {

  auto args = arg_parser(argc, argv);

  std::string directory = "";
  if (args.find("directory") != args.end())
  {
    directory = args.at("directory");
  }
  else
  {
    directory = ".\\";
  }
  // You can use print statements as follows for debugging, they'll be visible when running tests.
  std::cout << "Logs from your program will appear here!\n";

  // Uncomment this block to pass the first stage

  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    std::cerr << "Failed to create server socket\n";
    return 1;
  }

  // Since the tester restarts your program quite often, setting REUSE_PORT
  // ensures that we don't run into 'Address already in use' errors
  int reuse = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) < 0) {
    std::cerr << "setsockopt failed\n";
    return 1;
  }

  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(4221);

  if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0) {
    std::cerr << "Failed to bind to port 4221\n";
    return 1;
  }

  int connection_backlog = 5;
  if (listen(server_fd, connection_backlog) != 0) {
    std::cerr << "listen failed\n";
    return 1;
  }
  std::cout << "Waiting for a client to connect...\n";

  Backend back_end(directory);
  while (true)
  {
    struct sockaddr_in client_addr;
    int client_addr_len = sizeof(client_addr);
    int client = accept(server_fd, (struct sockaddr*)&client_addr, (socklen_t*)&client_addr_len);
#ifndef NOTHREADING
    std::thread client_req(&Backend::handle_request, back_end, client);
    client_req.detach();
#else
    back_end.handle_request(client);
#endif
  }

  close(server_fd);
  return 0;
}
