#include <PubSubClient.h>
#include <FloodingMesh.h>
#include <ESP8266WiFi.h>
#include "handlers.hpp"

#define ASCII_OFFSET    48

extern FloodingMesh mesh;
extern PubSubClient mqtt;

void (*command_handlers[HANDLERS_SIZE])(data_frame& frame) = {set_mode_command_handler, set_mode_command_handler,
																set_mode_command_handler, set_mode_command_handler,
																set_mode_command_handler, set_mode_command_handler,
																set_mode_command_handler, set_mode_command_handler};

int get_handler_index(char number) {
	return number - ASCII_OFFSET;
}

// void get_nodes_command_handler(data_frame& frame) {
// 	auto nodes = mesh.getNodeList(true);
// 	String str;
// 	for (auto &&id : nodes)
// 		str += String(id) + String(" ");

// 	mqtt.publish(report, str.c_str());
// }

void set_mode_command_handler(data_frame& frame) {
	char message[MESSAGE_SIZE] = {0};
	get_message(message, frame);
	mesh.broadcast(String(message));
}
