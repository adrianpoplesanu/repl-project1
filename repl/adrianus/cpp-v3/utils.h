#ifndef __UTILS_H
#define __UTILS_H

std::string read_file_content(std::string);
void write_file_content(std::string, std::string);
void append_file_content(std::string, std::string);

void start_socket_socket();
void start_client_socket();

#endif