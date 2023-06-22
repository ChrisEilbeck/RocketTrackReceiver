
#pragma once

#define MAX_TX_PACKET_LENGTH 64

extern uint8_t beaconid;
extern float beaconlat;
extern float beaconlon;
extern float beaconheight;
extern float beaconvoltage;
extern float beaconhacc;
extern uint8_t beaconnumsats;
extern uint8_t beacongpsfix;
extern uint16_t beaconcount;

void PackPacket(uint8_t *TxPacket,uint16_t *TxPacketLength);
void UnpackPacket(uint8_t *RxPacket,uint16_t RxPacketLength);
