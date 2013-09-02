#pragma once

//#define RAW_DATA_SIMULATION_TEST 1

// Standard BiSync Serial Control chars
#define BSC_CODE_ASCII_ENQ 0x05
#define BSC_CODE_ASCII_DLE 0x10
#define BSC_CODE_ASCII_ACK0 0x30
#define BSC_CODE_ASCII_ACK1 0x31
#define BSC_CODE_ASCII_EOT 0x04
#define BSC_CODE_ASCII_STX 0x02
#define BSC_CODE_ASCII_ETX 0x03
#define BSC_CODE_ASCII_NAK 0x15

//Next define depends on our device (SAE) address (in fact 0xC = 12)
#define BSC_CODE_POLLING_SAE 0xCC	// this is our poll address
#define BSC_CODE_SELECTING_SAE 0x8C	// this is our selecting address

//Times out (ms)
#define BSC_MASTER_TIMEOUT 100
#define BSC_ANSWER_TIMEOUT 40
#define BSC_INTERCHAR_TIMEOUT 15

// Look out (Survey) parameters
#define BSC_SURV_TIME_MS 5000		// MAX T_SURV = 10 sec	
#define BSC_SURV_REPERAT 5		// N_SURV = 5

// BSC messages defines
#define REP_SURV 101
#define INT_SURV 1
#define INT_SURV_DATA_SIZE 24
#define INT_SURV_BUF_SIZE INT_SURV_DATA_SIZE*2
