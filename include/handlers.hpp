#ifndef HANDLERS_HPP
#define HANDLERS_HPP

#include <Arduino.h>

#include "data.hpp"

#define HANDLERS_SIZE       8

// void get_nodes_command_handler(data_frame& frame);
void set_mode_command_handler(data_frame& frame);
int get_handler_index(char number);

extern void (*command_handlers[HANDLERS_SIZE])(data_frame& frame);

#endif /* HANDLERS_HPP */