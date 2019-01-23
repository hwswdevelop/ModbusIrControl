/* modbus.c file
 * 
 * Copyright (c) 2016-2018 Evgeny Sobolev
 * Contact:
 *	e-mail: evgeny@vrnnet.ru, hwswdevelop@gmail.com
 *  skype:  evgenysbl
 *  tel.:   +7(908)1375847
 */
#include "modbus.h"
#include "helper.h"
#include "leds.h"
#include "eeprom.h"
#include "button.h"
#include "ir.h"
#include "ds18b20.h"

void modbusException(uint8_t *data, uint8_t code);

#define MODBUS_MAX_VALID_ADDRESS	( 247 )

#define MAX_COIL_COUNT						( 0x07D0 )
#define COIL_COUNT								( LEDS_COUNT )

#define MAX_INPUT_COUNT						( 0x07D0 )
#define INPUT_COUNT								( BUTTONS_COUNT )

#define MAX_HOLDING_COUNT 				( 0x07D )
#define HOLDING_COUNT							( 22 )

#define REG_ADDR_MODBUS_ID				(0x00)
#define REG_ADDR_MODBUS_MODE			(0x01)
#define REG_ADDR_MODBUS_BAUD			(0x02)
#define REG_ADDR_MODBUS_PARITY		(0x03)
#define REG_ADDR_MODBUS_SAVE			(0x04)
#define REG_ADDR_BUTTONS_STATE		(0x05)
#define REG_ADDR_BUTTONS_PUSH			(0x06)
#define REG_ADDR_BUTTONS_PULL			(0x07)
#define REG_ADDR_BUTTON1_TIME			(0x08)
#define REG_ADDR_BUTTON2_TIME			(0x09)
#define REG_ADDR_BUTTON3_TIME			(0x0A)
#define REG_ADDR_TEMP							(0x0B)
#define REG_ADDR_LEDS							(0x0C)
#define REG_ADDR_IR_LEN						(0x0D)
#define REG_ADDR_IR_PULSE					(0x0E)
#define REG_ADDR_IR_PAUSE					(0x0F)
#define REG_ADDR_IR_DATA_FIRST		(0x10)
#define REG_ADDR_IR_DATA_LAST			(0x15)

#define MODBUS_CONFIG_SAVE_VAL		(0xA251)


void modbusException(uint8_t *data, const uint8_t code){
		data[1] |= MODBUS_EXCEPTION_BIT;
		data[2]  = code;
		modbusTx(data, 3);	
}

// 0x01 - Read Coils
static MODBUS_PROC modbusReadCoils(uint8_t *data, const uint8_t size){
		uint16_t addr  = BE16(data + 2);
		uint16_t count = BE16(data + 4);
	  if ( ( count < 1 ) || ( count > MAX_COIL_COUNT ) ){
				modbusException(data, MODBUS_ILLEGAL_DATA_VALUE);
		} else if ( (addr >= COIL_COUNT) || ( (addr + count) > COIL_COUNT ) ){
				modbusException(data, MODBUS_ILLEGAL_DATA_ADDRESS);
		} else {			
		  uint8_t ledStatus = 0;
			uint8_t coilNo;
			// Only 1 byte returned in any cases
			data[2] = 1;
			// update led status			
			for(coilNo = 0; coilNo < count; coilNo++){
				uint8_t ledNo = coilNo + addr;
				ledStatus |= ( getLedStatus(ledNo) << coilNo);
			}
			// return led satatus
			data[3] = ledStatus;			
			modbusTx(data, 4);
		}
		return MODBUS_NO_FUTURE_PROCESSING_REQUIRED;						
}

// 0x05 - Write Single Coil
static MODBUS_PROC  modbusWriteSingleCoil(uint8_t *data, const uint8_t size){
	  uint16_t addr  = BE16(data + 2);
		uint16_t val   = BE16(data + 4);
		if (val != 0x00 && val != 0xFF00){
			modbusException(data, MODBUS_ILLEGAL_DATA_VALUE);
		} else if (addr >= COIL_COUNT){
			modbusException(data, MODBUS_ILLEGAL_DATA_ADDRESS);
		} else {
			if (val == 0){
				ledsOnOff(addr, LED_OFF);
			} else {
				ledsOnOff(addr, LED_ON);
			}
			modbusTx(data, 6);
		}
		return MODBUS_NO_FUTURE_PROCESSING_REQUIRED;		
}

// 0x0F - Write Multiple Coils
static MODBUS_PROC  modbusWriteMultipleCoils(uint8_t *data, const uint8_t size){
		uint16_t addr  = BE16(data + 2);
		uint16_t count = BE16(data + 4);
		uint8_t  byteCount = LE8(data + 6);

	  if ( ( count < 1 ) ||
				 ( count > MAX_COIL_COUNT ) ||
				 ( byteCount != ((count + 7) >> 3) ) ){
				modbusException(data, MODBUS_ILLEGAL_DATA_VALUE);
		} else if ( ( addr >= COIL_COUNT ) || ( (addr + count) > COIL_COUNT) ){
				modbusException(data, MODBUS_ILLEGAL_DATA_ADDRESS);
		} else {
			uint8_t coilNo;			
			for(coilNo = 0; coilNo < count; coilNo++){
				uint8_t ledNo = coilNo + addr;
				uint8_t val = (data[7] >> coilNo) & 0x01;
				if (val == 0){
					ledsOnOff(ledNo, LED_OFF);
				} else {
					ledsOnOff(ledNo, LED_ON);
				}
			}
			modbusTx(data, 6);
		}
		return MODBUS_NO_FUTURE_PROCESSING_REQUIRED;						
}




// 0x02 - Read Discrete Inputs
static MODBUS_PROC modbusReadDiscreteInputs(uint8_t *data, const uint8_t size){
		uint16_t addr  = BE16(data + 2);
		uint16_t count = BE16(data + 4);
		if ( (count < 1) || (count > MAX_INPUT_COUNT)){
				modbusException(data, MODBUS_ILLEGAL_DATA_VALUE);
		} else if ( ( addr >= INPUT_COUNT ) || ( (addr + count) > INPUT_COUNT ) ){
				modbusException(data, MODBUS_ILLEGAL_DATA_ADDRESS);
		}	else {
			// Anyway only 1 byte returned
			uint8_t val = 0;
			uint8_t inputNo;
			data[2] = 1;
			for(inputNo = 0; inputNo < count; inputNo++){
					uint8_t swInputNo = inputNo + addr;
					uint8_t inputVal = getButtonState(swInputNo);
					val |= (inputVal << inputNo);
			}			
			data[3] = val;
			modbusTx(data, 4);
		}		
		return MODBUS_NO_FUTURE_PROCESSING_REQUIRED;
}


extern MODBUS_CONFIG gModbusConfig;


uint8_t readReg(const uint8_t addr, uint16_t *value){
	  uint8_t res = 0;
		switch(addr){
			// Modbus 
			case REG_ADDR_MODBUS_ID: 			*value = gModbusConfig.id;			break;
			case REG_ADDR_MODBUS_MODE: 		*value = gModbusConfig.mode;   break;
			case REG_ADDR_MODBUS_BAUD: 		*value = gModbusConfig.baud;   break;
			case REG_ADDR_MODBUS_PARITY: 	*value = gModbusConfig.parity; break;
			case REG_ADDR_MODBUS_SAVE: 		*value = 0; break;
			
			// Buttons
			case REG_ADDR_BUTTONS_STATE: 
			{ 
				uint8_t buttonNo, val = 0;
				for( buttonNo = 0; buttonNo < INPUT_COUNT; buttonNo++ ) 
					val |= getButtonState(buttonNo) << buttonNo;
				*value = val;
			}
			break;
			
			case REG_ADDR_BUTTONS_PUSH:
			case REG_ADDR_BUTTONS_PULL:
			{
				uint8_t  buttonNo = 0;
				uint16_t val = 0;
				for( buttonNo = 0; buttonNo < INPUT_COUNT; buttonNo++ ) {
					uint8_t pushed = getButtonPushed(buttonNo);
					uint8_t pulled = getButtonPulled(buttonNo);
					if ( ( addr == REG_ADDR_BUTTONS_PUSH ) && pushed ) val |= 1 << buttonNo;
					if ( ( addr == REG_ADDR_BUTTONS_PULL ) && pulled ) val |= 1 << buttonNo;
				}
				*value = val;
			}
			break;
			
			case REG_ADDR_BUTTON1_TIME:
			case REG_ADDR_BUTTON2_TIME:
			case REG_ADDR_BUTTON3_TIME:
			{
				uint8_t buttonNo = addr - REG_ADDR_BUTTON1_TIME;
				*value = getButtonPushTimeMs(buttonNo);
			}
			break;
			
			case REG_ADDR_TEMP: *value = ds18b20GetTemp(); break;
			
			case REG_ADDR_LEDS:
			{
				// Leds enable/disable
				uint8_t ledNo = 0;
				uint8_t val = 0;
				for(ledNo = 0; ledNo < LEDS_COUNT; ledNo++)
					val |= getLedStatus(ledNo) << ledNo;
				*value = val;
			}
			break;
			
			// IR
			case REG_ADDR_IR_LEN: 	*value = getIrRxLen(); break;
			case REG_ADDR_IR_PULSE: if (getIrRxLen()) *value = getIrRxPulse(); break;
			case REG_ADDR_IR_PAUSE: if (getIrRxLen()) *value = getIrRxPause(); break;
			
			default: 
			if ( ( REG_ADDR_IR_DATA_FIRST <= addr ) && ( addr <= REG_ADDR_IR_DATA_LAST ) ){
			  uint16_t val = 0;
				uint8_t irAddr = (addr - REG_ADDR_IR_DATA_FIRST) << 1;
				if ( irAddr < ((getIrRxLen() + 7) >> 3)){
					val = ((uint16_t)getIrRxByte(irAddr)) << 8;
					val |= getIrRxByte(irAddr + 1);
				}
				*value = val;
			} else res = MODBUS_ILLEGAL_DATA_ADDRESS;
		}		
		return res;
}

typedef struct {
	uint16_t 	txPulseTime;
	uint16_t 	txPauseTime;
	uint8_t 	txBitCount;
	uint8_t		txData[12];
} IR_TX, *pIR_TX;

IR_TX 	gIrTx;

uint8_t writeReg(const uint8_t addr, const uint16_t value){
	  uint8_t res = 0;
		switch(addr){
			
			case REG_ADDR_MODBUS_ID: 
				if ( value > 0 && value <= MODBUS_MAX_VALID_ADDRESS ){				
					gModbusConfig.id = (uint8_t)value; 
				} else res = MODBUS_ILLEGAL_DATA_VALUE;
			break;
			
			case REG_ADDR_MODBUS_MODE: 
				if (value >= MODBUS_MODE_ASCII && value <= MODBUS_MODE_RTU){
					gModbusConfig.mode = value;
					rs485_init(&gModbusConfig);
				} else res = MODBUS_ILLEGAL_DATA_VALUE;
			break;
		
			case REG_ADDR_MODBUS_BAUD:
				if (value >= RS485_BAUD_4800 && value <= RS485_BAUD_115200) {
					gModbusConfig.baud = value;
					rs485_init(&gModbusConfig);
				} else res = MODBUS_ILLEGAL_DATA_VALUE;
			break;
			
			case REG_ADDR_MODBUS_PARITY:
			  if (value >= RS485_NO_PARITY && value <= RS485_ODD ){
					gModbusConfig.parity = value;
					rs485_init(&gModbusConfig);
				} else res = MODBUS_ILLEGAL_DATA_VALUE;
			break;
						
			case REG_ADDR_MODBUS_SAVE:
				if (value == MODBUS_CONFIG_SAVE_VAL){
					saveSerialConfig();
			  } else if (value) {
					res = MODBUS_ILLEGAL_DATA_VALUE;	
				}
			break;
			
	    // ************* BUTTONS ************** 			
			case REG_ADDR_BUTTONS_STATE: break;// Buttons state
			
			case REG_ADDR_BUTTONS_PUSH:
			case REG_ADDR_BUTTONS_PULL:
			// Pushed pulled
			{
				uint8_t buttonNo;
				for( buttonNo = 0; buttonNo < INPUT_COUNT; buttonNo++ ){
					if ( (value >> buttonNo) & 0x01){ 
						clearButtonPushed(buttonNo);
						clearButtonPulled(buttonNo);
						clearButtonPushTime(buttonNo);
					}						
				}
			}
			break;
			
			case REG_ADDR_BUTTON1_TIME:
			case REG_ADDR_BUTTON2_TIME:
			case REG_ADDR_BUTTON3_TIME:			
			// Button push time
			break;
			
			case REG_ADDR_TEMP:
			// Temperature
			break;
			
			// ************* LEDS ************** 
			case REG_ADDR_LEDS:
			{ 
				// Leds enable/disable
				uint8_t val = value;
				uint8_t ledNo;
				for(ledNo = 0; ledNo < LEDS_COUNT; ledNo++)
				   ledsOnOff(ledNo, (val >> ledNo) & 0x01);
			}
			break;
			
			// ************* IR ************** 
			case REG_ADDR_IR_LEN:
			{
				if ( (0 < value) && (value < 32) ){
					// Clear receive
					clearIrRx(); break;
				} else {
					// Transmit
					gIrTx.txBitCount = value;	
				}
			}
			case REG_ADDR_IR_PULSE: gIrTx.txPulseTime = value; break;
			case REG_ADDR_IR_PAUSE: gIrTx.txPauseTime = value; break;
						
			default:
				if( (REG_ADDR_IR_DATA_FIRST <= addr) && (addr <= REG_ADDR_IR_DATA_LAST)){
					// Calculate current register number
					uint8_t irRegNo = (addr - REG_ADDR_IR_DATA_FIRST);
					// Calc bytes count to transmit
					uint8_t irTxByteCount = (gIrTx.txBitCount + 7) >> 3;
					// Calc reg when transmit will be started
					uint8_t txStartRegNo =  irTxByteCount ? (irTxByteCount - 1) >> 1 : 0;
					// Calc byte no to write into tx buffer
					uint8_t irTxAddr = irRegNo << 1;
					// Write data to tx buffer
					gIrTx.txData[irTxAddr] = (uint8_t)(value >> 8);
					gIrTx.txData[irTxAddr + 1] = (uint8_t)(value);
				  // Transmit ?
					if ( txStartRegNo == irRegNo ){
						// Transmit frame
						irTxFrame(gIrTx.txPulseTime, gIrTx.txPauseTime, gIrTx.txData, gIrTx.txBitCount);
					}
				} else res = MODBUS_ILLEGAL_DATA_ADDRESS;
		}		
		return res;
}



// 0x03 - Read Holding Registers
static MODBUS_PROC modbusReadHoldingRegisters(uint8_t *data, const uint8_t size){
		uint16_t addr  = BE16(data + 2);
		uint16_t count = BE16(data + 4);
		if (count < 1 || count > MAX_HOLDING_COUNT){
			modbusException(data, MODBUS_ILLEGAL_DATA_VALUE);
		} else if ( (addr >= HOLDING_COUNT) || 
								( (addr + count) > HOLDING_COUNT) ) {
			modbusException(data, MODBUS_ILLEGAL_DATA_ADDRESS);
		} else {
			uint8_t 	regInd;
			uint8_t 	index = 3;
			for(regInd = 0; regInd < count; regInd++ ){
					uint8_t  regAddr = regInd + addr;
					uint8_t  res;
					uint16_t val = 0;
					res = readReg(regAddr, &val);	
					if (res){
						modbusException(data, res);
						return MODBUS_NO_FUTURE_PROCESSING_REQUIRED;
					} else {
						data[index++] = val >> 8;
						data[index++] = val & 0xFF;
					}
			}
			data[2] = index - 3;
			modbusTx(data, index);
		}
		
		return MODBUS_NO_FUTURE_PROCESSING_REQUIRED;
}



// 0x06 - Write Single Register
static MODBUS_PROC  modbusWriteSingleRegister(uint8_t *data, const uint8_t size){
		uint16_t addr  = BE16(data + 2);
		uint16_t value = BE16(data + 4);

    if (addr >= HOLDING_COUNT){
				modbusException(data, MODBUS_ILLEGAL_DATA_ADDRESS);
		} else {
				uint8_t  res;
				res = writeReg(addr, value);
				if (res){
						modbusException(data, res);
						return MODBUS_NO_FUTURE_PROCESSING_REQUIRED;
				} else {
					  readReg(addr, &value);
            data[4] = value >> 8;
						data[5] = value & 0xFF;
						modbusTx(data, 6);
				}
		}
		return MODBUS_NO_FUTURE_PROCESSING_REQUIRED;						
}

// 0x10 - Write Multiple registers
static MODBUS_PROC  modbusWriteMultipleRegisters(uint8_t *data, const uint8_t size){
		uint16_t addr  = BE16(data + 2);
		uint16_t count = BE16(data + 4);
		uint8_t  bytes = LE8(data + 6);
		if (count < 1 || count > MAX_HOLDING_COUNT){
			modbusException(data, MODBUS_ILLEGAL_DATA_VALUE);
		} else if ( (addr >= HOLDING_COUNT) || 
								((addr + count) > HOLDING_COUNT) || 
								( bytes != count * 2 ) ) {
			modbusException(data, MODBUS_ILLEGAL_DATA_ADDRESS);
		} else {
			uint8_t index  = 7;
			uint8_t regInd = 0;
			for(regInd = 0; regInd < count; regInd++){
				uint8_t regAddr = addr + regInd;
				uint16_t value;
				uint8_t  res;
				value = ((uint16_t)data[index++]) << 8;
				value |= data[index++];
				res = writeReg(regAddr, value);
				if (res){
						modbusException(data, res);
						return MODBUS_NO_FUTURE_PROCESSING_REQUIRED;
				}
			}
			modbusTx(data, 6);
		}
		return MODBUS_NO_FUTURE_PROCESSING_REQUIRED;						
}



MODBUS_PROC modbusRx(uint8_t *data, const uint8_t size){
		// MODBUS_PROCESSING_REQUIRED
		uint8_t moduleAddr = LE8(data);
		uint8_t	res = MODBUS_NO_FUTURE_PROCESSING_REQUIRED;
		
		if ( ( moduleAddr == 0 ) || (moduleAddr == gModbusConfig.id) ){
				uint8_t func = LE8(data + 1);
				switch(func){
					case 0x01:	res = modbusReadCoils(data, size); break;
					case 0x02:	res = modbusReadDiscreteInputs(data, size); break;
					case 0x03:	res = modbusReadHoldingRegisters(data, size); break;
					case 0x05:	res = modbusWriteSingleCoil(data, size); break;
					case 0x06:	res = modbusWriteSingleRegister(data, size); break;
					case 0x0F:	res = modbusWriteMultipleCoils(data, size); break;
					case 0x10:	res = modbusWriteMultipleRegisters(data, size); break;
					default:
						modbusException(data, MODBUS_ILLEGAL_FUNCTION);
				}
		}
		
		return res;
}

