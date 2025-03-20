#ifndef BOARD_HPP
#define BOARD_HPP



// Board addresses
# define MX3ADDR_TEST          0
# define MX3ADDR_SW            1
# define MX3ADDR_LED           2

# define MX3ADDR_FLASH_RFIFO   4
# define MX3ADDR_FLASH_PTR_L   5
# define MX3ADDR_FLASH_PTR_M   6
# define MX3ADDR_FLASH_PTR_H   7
# define MX3ADDR_RGBLED_R      8
# define MX3ADDR_RGBLED_G      9
# define MX3ADDR_RGBLED_B     10
# define MX3ADDR_SND_WFIFO    11
# define MX3ADDR_SND_ELTS_L   12
# define MX3ADDR_SND_ELTS_H   13
# define MX3ADDR_UPTIME_L     14
# define MX3ADDR_UPTIME_H     15
# define MX3ADDR_7SEG_DEC_L   16
# define MX3ADDR_7SEG_DEC_H   17
# define MX3ADDR_7SEG_HEX_L   18
# define MX3ADDR_7SEG_HEX_H   19
# define MX3ADDR_7SEG_MAP_0   20
# define MX3ADDR_7SEG_MAP_1   21
# define MX3ADDR_7SEG_MAP_2   22
# define MX3ADDR_7SEG_MAP_3   23
# define MX3ADDR_B_IMAT_LL    24
# define MX3ADDR_B_IMAT_LH    25
# define MX3ADDR_B_IMAT_HL    26
# define MX3ADDR_B_IMAT_HH    27

# define MX3ADDR_STATUS       31
# define MX3ADDR_LCD_START    32
# define MX3ADDR_GPR_START    64

/************************************
     32 addresses for LCD display
*************************************/


//Bit definitions for status register
#define MX3BIT_STATUS_AUD_EN    0x01
#define MX3BIT_STATUS_AUD_FREQ  0x02
#define MX3BIT_STATUS_7SEG_MODE 0x0C


//Command values
#define MX3CMD_NOP    0
#define MX3CMD_TEST   1
#define MX3CMD_WR1    2
#define MX3CMD_RD1    3
#define MX3CMD_WR_FF  4
#define MX3CMD_RD_FF  5
#define MX3CMD_WR_MAP 6
#define MX3CMD_RD_MAP 7
#define MX3CMD_WR1MAP 8

//Board related constants
#define MX3DAT_TESTCMD_RESP 42
#define MX3DAT_TESTREG_RESP 75



#endif