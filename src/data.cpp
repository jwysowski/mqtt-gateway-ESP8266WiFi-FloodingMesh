#include "data.hpp"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

const char *alive = "gate/alive";
const char *command = "gate/command";
const char *temperature = "gate/temperature";
const char *humidity = "gate/humidity";
const char *report = "gate/report";

const char *get_nodes_command = "get_nodes";

const char *temp_msg = "Temperature";
const char *hum_msg = "Humidity";

void get_message(char *msg, data_frame &frame) {
	msg[0] = START_SIGN;
	msg[1] = frame.data_type;
	msg[2] = '\0';
	
	if (frame.data_type == TEMPERATURE_TYPE)
		strcat(msg, frame.measurement.temperature);
	else
		strcat(msg, frame.measurement.humidity);

	strcat(msg, frame.node_id);
	
	sprintf(frame.checksum, "%x", checksum(frame));
	strcat(msg, frame.checksum);
	msg[MESSAGE_SIZE - 2] = END_SIGN;
	msg[MESSAGE_SIZE - 1] = '\0';
}


bool validate(data_frame &frame) {
	uint16_t frame_checksum = strtoul(frame.checksum, nullptr, 16);
	return frame_checksum == checksum(frame);
}

void decode_msg(const char *msg, data_frame &frame) {
	int start = 0;
	while (start < MESSAGE_SIZE) {
		if (msg[start] == START_SIGN)
			break;

		start++;
	}

	frame.data_type = msg[start + 1];
	
	if (frame.data_type == TEMPERATURE_TYPE) {
		memcpy(frame.measurement.temperature, msg + start + 2, DATA_SIZE);
		frame.measurement.temperature[DATA_SIZE] = '\0';
	}

	if (frame.data_type == HUMIDITY_TYPE) {
		memcpy(frame.measurement.humidity, msg + start + 2, DATA_SIZE);
		frame.measurement.humidity[DATA_SIZE] = '\0';
	}
	
	else {
		memcpy(frame.measurement.target, msg + start + 2, DATA_SIZE);
		frame.measurement.target[DATA_SIZE] = '\0';
	}

	memcpy(frame.node_id, msg + start + 2 + DATA_SIZE, NODE_ID_SIZE);
	frame.node_id[NODE_ID_SIZE] = '\0';

	memcpy(frame.checksum, msg + start + 2 + DATA_SIZE + NODE_ID_SIZE, CHECKSUM_SIZE);
	frame.checksum[CHECKSUM_SIZE] = '\0';
}

uint16_t checksum(data_frame &frame) {
	uint16_t checksum = 0;

	checksum += START_SIGN;

	checksum += frame.data_type;
	if (frame.data_type == TEMPERATURE_TYPE)
		for (int i = 0; i < DATA_SIZE; i++)
			checksum += frame.measurement.temperature[i];

	if (frame.data_type == HUMIDITY_TYPE)
		for (int i = 0; i < DATA_SIZE; i++)
			checksum += frame.measurement.humidity[i];

	else
		for (int i = 0; i < DATA_SIZE; i++)
			checksum += frame.measurement.target[i];

	for (int i = 0; i < NODE_ID_SIZE; i++)
		checksum += frame.node_id[i];

	return checksum % CHECKSUM_MOD;
}
