#include "vesc_uasrt.h"

uint8_t VESC_RX_Buff[256];
uint8_t VESC_RX_Flag = 0;

dataPackage data;

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
	uint8_t command[1];
	
	command[0] = id;
	
	Send_Pack_Data(command,1);
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

			data.tempFET            = buffer_get_float16(pdata, 10.0, &ind);
			data.tempMotor          = buffer_get_float16(pdata, 10.0, &ind);
			data.avgMotorCurrent 	= buffer_get_float32(pdata, 100.0, &ind);
			data.avgInputCurrent 	= buffer_get_float32(pdata, 100.0, &ind);
			ind += 8; // skip 8 bytes
			data.dutyCycleNow 		= buffer_get_float16(pdata, 1000.0, &ind);
			data.rpm 				= buffer_get_int32(pdata, &ind);
			data.inpVoltage 		= buffer_get_float16(pdata, 10.0, &ind);
			data.ampHours 			= buffer_get_float32(pdata, 10000.0, &ind);
			data.ampHoursCharged 	= buffer_get_float32(pdata, 10000.0, &ind);
			ind += 8; // skip 8 bytes
			data.tachometer 		= buffer_get_int32(pdata, &ind);
			data.tachometerAbs 		= buffer_get_int32(pdata, &ind);

		break;
	}
	
	return 0;
}

