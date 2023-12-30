#include "vesc_uasrt.h"
#include "flag_bit.h"

uint8_t VESC_RX_Buff[256];
uint8_t VESC_RX_Flag = 0;

// Access ADC values here to determine riding state
extern float ADC1_Val, ADC2_Val;

dataPackage data;
lcmConfig_t lcmConfig;
uint8_t errCode = 0;

uint8_t protocol_buff[256]; // send buffer
/**************************************************
 * @brief  :Send_Pack_Data()
 * @note : Send a packet of data
 * @param *payload: The starting address of the data packet to be sent by the payload
 * @param len: packet length
 * @retval None
 **************************************************/
void Send_Pack_Data(uint8_t *payload,uint16_t len) 
{
//	uint8_t protocol_buff[256]; // send buffer
	uint8_t count = 0;
	uint16_t crcpayload = crc16(payload, len);  // calculate checksum
	
	/*
		protocol format
	
		Start byte (one byte) + data packet length (one or two bytes) + data packet (N bytes) + checksum (two bytes) + stop byte (one byte)
	
		Start byte: 0x02 packet length 1-256 bytes
					0x03 Packet length exceeds 256 bytes
	
		Data packet length: start byte 0x02 data packet occupies one byte
	                Start byte 0x03 data packet occupies two bytes
	
		Packet :  	 The first byte of the packet is the packet ID
	
		Checksum :		 CRC check two bytes 
	    
		Stop Byte :    Fixed 0x03

	*/
	
	if (len <= 256) // The data packet length is not greater than 256 bytes
	{
		protocol_buff[count++] = 2;
		protocol_buff[count++] = len;
	}
	else // The packet length is greater than 256 bytes
	{
		protocol_buff[count++] = 3;
		protocol_buff[count++] = (uint8_t)(len >> 8);
		protocol_buff[count++] = (uint8_t)(len & 0xFF);
	}

	memcpy(&protocol_buff[count], payload, len);  // Copy the data packet into the protocol

	count += len;
	protocol_buff[count++] = (uint8_t)(crcpayload >> 8);
	protocol_buff[count++] = (uint8_t)(crcpayload & 0xFF);
	protocol_buff[count++] = 3;
	
	USART1_Send_Bytes(protocol_buff,count);
}

/**************************************************
 * @brief  :Get_Vesc_Pack_Data()
 * @note : Get a packet of data
 * @param id: packet id
 * @retval None
 **************************************************/
void Get_Vesc_Pack_Data(COMM_PACKET_ID id)
{
	uint8_t command[12];
	int len = 1;
	
	command[0] = id;
	
	if (id == COMM_CUSTOM_APP_DATA) {
		command[1] = 101;
		command[2] = 24; // FLOAT_COMMAND_POLL
		len = 3;
	}

	if (id == COMM_CUSTOM_DEBUG) {
		command[0] = COMM_CUSTOM_APP_DATA;
		command[1] = 101;
		command[2] = 28; // FLOAT_COMMAND_LCM_DEBUG
		command[3] = Power_Flag;
		command[4] = Charge_Flag;
		command[5] = Buzzer_Flag;
		command[6] = Lightbar_Battery_Flag;
		command[7] = Sensor_Activation_Display_Flag;
		command[8] = Light_Profile;
		command[9] = errCode;
		len = 10;
	}
	
	Send_Pack_Data(command, len);
}

/**************************************************
 * @brief  :buffer_get_int16()
 * @note : Two bytes in the buffer spell an int16_t
 * @param *buffer: address
 * @param *index: address offset
 * @retval None
 **************************************************/
int16_t buffer_get_int16(const uint8_t *buffer, int32_t *index) {
	int16_t res =	((uint16_t) buffer[*index]) << 8 |
					((uint16_t) buffer[*index + 1]);
	*index += 2;
	return res;
}
/**************************************************
 * @brief  :buffer_get_uint16()
 * @note : Two bytes in the buffer spell an uint16_t
 * @param *buffer: address
 * @param *index: address offset
 * @retval None
 **************************************************/
uint16_t buffer_get_uint16(const uint8_t *buffer, int32_t *index) {
	uint16_t res = 	((uint16_t) buffer[*index]) << 8 |
					((uint16_t) buffer[*index + 1]);
	*index += 2;
	return res;
}
/**************************************************
 * @brief  :buffer_get_int32()
 * @note : Two bytes in the buffer spell an int32_t
 * @param *buffer: address
 * @param *index: address offset
 * @retval None
 **************************************************/
int32_t buffer_get_int32(const uint8_t *buffer, int32_t *index) {
	int32_t res =	((uint32_t) buffer[*index]) << 24 |
					((uint32_t) buffer[*index + 1]) << 16 |
					((uint32_t) buffer[*index + 2]) << 8 |
					((uint32_t) buffer[*index + 3]);
	*index += 4;
	return res;
}
/**************************************************
 * @brief  :buffer_get_uint32()
 * @note : Two bytes in the buffer spell an uint32_t
 * @param *buffer: address
 * @param *index: address offset
 * @retval None
 **************************************************/
uint32_t buffer_get_uint32(const uint8_t *buffer, int32_t *index) {
	uint32_t res =	((uint32_t) buffer[*index]) << 24 |
					((uint32_t) buffer[*index + 1]) << 16 |
					((uint32_t) buffer[*index + 2]) << 8 |
					((uint32_t) buffer[*index + 3]);
	*index += 4;
	return res;
}
/**************************************************
 * @brief  :buffer_get_float16()
 * @note   The four bytes of the buffer spell a float16
 * @param *buffer: address
 * @param scale: scale denominator
 * @param *index: address offset
 * @retval None
 **************************************************/
float buffer_get_float16(const uint8_t *buffer, float scale, int32_t *index) {
    return (float)buffer_get_int16(buffer, index) / scale;
}
/**************************************************
 * @brief  :buffer_get_float32()
 * @note   The four bytes of the buffer spell a float32
 * @param buffer: address
 * @param scale: scale denominator
 * @param index: address offset
 * @retval None
 **************************************************/
float buffer_get_float32(const uint8_t *buffer, float scale, int32_t *index) {
    return (float)buffer_get_int32(buffer, index) / scale;
}

/**************************************************
 * @brief  :Protocol_Parse()
 * @note : protocol analysis
 * @param message: the start address of the message received data
 * @retval 0 parsing success 1 parsing failure
 **************************************************/
uint8_t Protocol_Parse(uint8_t *message)
{
	uint8_t  start = 0;
	uint8_t *pdata;
	uint16_t counter = 0;
	uint16_t len = 0; 
	uint16_t crcpayload;
	uint8_t id;
	int32_t ind = 0;
		
	start = message[counter++];
	
	switch(start)
	{
		case 0x02:
			len = message[counter++];
		break;
		
		case 0x03:
			
		break;
		
	}
	
	crcpayload = crc16(&message[counter], len);
	
	if(crcpayload != (((uint16_t)message[counter+len])<<8|
		             ((uint16_t)message[counter+len+1])))
	{
		return 1; // crc is wrong
	}
	
	id = message[counter++];
	pdata = &message[counter];  
	
	switch(id)
	{
		case COMM_GET_VALUES: 

			ind += 8;
			data.avgInputCurrent 	= buffer_get_float32(pdata, 100.0, &ind); // negative input current implies braking
			ind += 8; // Skip id/iq currents
			data.dutyCycleNow 		= buffer_get_float16(pdata, 10.0, &ind);	// duty as value 0..100
			data.rpm 				= buffer_get_int32(pdata, &ind);
			data.inpVoltage 		= buffer_get_float16(pdata, 10.0, &ind);
			
			if ((data.rpm > 100) || (data.rpm < -100) || (data.avgInputCurrent > 1) || (data.avgInputCurrent < -1)) {
				data.state = RUNNING;
			}
			else {
				// Use this fault as a placeholder (we only care that the board is stopped anyways)
				data.state = FAULT_STARTUP;
			}

		break;

		   case COMM_CUSTOM_APP_DATA:
      
		  if (len < 12) {
				break;
}
		  uint8_t magicnr = pdata[ind++];
		  uint8_t floatcmd = pdata[ind++];
		  if ((magicnr != 101) || (floatcmd != FLOAT_COMMAND_LCM_POLL)) {
				break;
        }
      data.floatPackageSupported = true;
			uint8_t state = pdata[ind++];
			data.state = state & 0xF;
			//data.switchstate = (state >> 4) & 0x7;
			data.isHandtest = (state & 0x80) > 0;
			data.fault = pdata[ind++];
			data.dutyCycleNow = pdata[ind++];
			data.rpm = buffer_get_float16(pdata, 1.0, &ind);
			data.avgInputCurrent = buffer_get_float16(pdata, 1.0, &ind);
			data.inpVoltage = buffer_get_float16(pdata, 10.0, &ind);

			uint8_t lcmset = pdata[ind++];
			if ((lcmset > 0) && (len >= 17)) {
				lcmConfig.isSet = true;
				lcmConfig.headlightBrightness = pdata[ind++];
				lcmConfig.statusbarBrightness = pdata[ind++];
				lcmConfig.statusbarMode = pdata[ind++];
				lcmConfig.dutyBeep = pdata[ind++];
				lcmConfig.boardOff = pdata[ind++];
			}
}
	if (data.rpm > 500)
		data.isForward = true;
	if (data.rpm < -500)
		data.isForward = false;
	
	return 0;
}

