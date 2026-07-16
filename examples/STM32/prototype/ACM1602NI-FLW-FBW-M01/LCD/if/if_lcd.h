/****************************************************************************************************
 * FILE NAME   : if_lcd.h
 * Description : LCD制御用インターフェース
 *               このファイルではLCD制御用IF機能を定義しています。
 *---------------------------------------------------------------------------------------------------
 * HISTORY     : 2025/09/13 : 新規作成
 ****************************************************************************************************/
#ifndef __IF_LCD_H__
#define __IF_LCD_H__

/****************************************************************************************************
 * Public include
 ****************************************************************************************************/

/****************************************************************************************************
 * Public define
 ****************************************************************************************************/
/*** LCDへの表示データサイズ ***/
#define     DISPLAY_ROW_SIZE        (2)         /* 表示列数 */
#define     DISPLAY_COL_SIZE        (16 + 1)    /* 表示行数 */

/*** 制御アドレス ***/
#define     I2C_SLAVE_ADDRESS       (0x50 << 1) /* スレーブアドレス  */
    #define     WRITE_BIT                   (0x00)      /* write bit[0] */
    #define     READ_BIT                    (0x01)      /* read  bit[0] */

/*** コントロールBYTE(コマンド) ***/
#define     CONTROL_BYTE_CMD        (0x00)      /* コマンドBYTE送信時の指定値 */
    /* コマンドBYTE */
    #define     CMD_BYTE_DISPLAY_CLR        (0x01)      /* Clear Display */
    #define     CMD_BYTE_DISPLAY_HOME       (0x02)      /* Return Home */
    #define     CMD_BYTE_ENTRY_MODE         (0x04)      /* Entry Mode */
    #define     CMD_BYTE_ONOFF_CONTROL      (0x08)      /* Display On/Off Control */
    #define     CMD_BYTE_CURSOR_SHIFT       (0x10)      /* Cursor or Display Shift */
    #define     CMD_BYTE_FUNCTION_SET       (0x20)      /* Function Set */
    #define     CMD_BYTE_SET_CGRAM_ADDR     (0x40)      /* Set CGRAM ADDRESS */
    #define     CMD_BYTE_SET_DDRAM_ADDR     (0x80)      /* Set DDRAM ADDRESS */

/*** コントロールBYTE(データ) ***/
#define     CONTROL_BYTE_DATA       (0x80)      /* データBYTE送信時の指定値 */

/*** バックライト点灯状態 ***/
#define     BACK_LIGHT_OFF          (0)
#define     BACK_LIGHT_ON           (1)

/****************************************************************************************************
 * Public typedef
 ****************************************************************************************************/

/****************************************************************************************************
 * Public Variables
 ****************************************************************************************************/

/****************************************************************************************************
 * Public Functions
 ****************************************************************************************************/
/* データ(表示)コマンド */
void iLcd_writeDisplay( char *msg, short row, short col );

/* 制御コマンド */
void iLcd_writeControl( char cmd );

void iLcd_moveDDramAddress( short row, short col );
void iLcd_displayClear( void );
void iLcd_displayHome( void );
void iLcd_setEntryMode( short type );
void iLcd_setOnOffControl( short type );
void iLcd_setCursorShift( short type );
void iLcd_setFunction( short type );

/* バックライト制御 */
void iLcd_backLightControl( short light);

#endif  /* __IF_LCD_H__ */