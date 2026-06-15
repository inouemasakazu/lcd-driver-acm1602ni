/****************************************************************************************************
 * @file    acm1602ni.h
 * @brief   LCDデバイスの「ACM1602NI-FLW-FBW-M01」制御用デバイスドライバ
 * @details 外部公開するAPI、型定義、マクロを定義する
 *
 * @author  
 * @date    2026/06/14     新規作成
 ****************************************************************************************************/
#ifndef __ACM1602NI_H__
#define __ACM1602NI_H__

/****************************************************************************************************
 * Public include
 ****************************************************************************************************/
#include "stdint.h"

/****************************************************************************************************
 * Public define
 ****************************************************************************************************/

/*** 表示可能領域(2x16) ***/
#define ACM1602NI_LINE_MAX               2      /* 行数 */
#define ACM1602NI_COLUMN_MAX            16      /* 列数 */


/*** Instructions ***/
#define INSTRUCTIONS_CLEAR              0x01        /* Clear Display */
#define INSTRUCTIONS_HOME               0x02        /* Return Home */
#define INSTRUCTIONS_ENTRY_MODE         0x04        /* Entry Mode Set */
#define INSTRUCTIONS_DISPLAY_CONTROL    0x08        /* Display ON/OFF Control */
#define INSTRUCTIONS_SHIFT              0x10        /* Cursor or Display Shift */
#define INSTRUCTIONS_FUNCTION           0x20        /* Function Set */
#define INSTRUCTIONS_CGRAM              0x40        /* Set CGRAM Address */
#define INSTRUCTIONS_DDRAM              0x80        /* Set CDDAM Address */

/*** command ***/
#define COMMAND_DISPLAY_CLEAR           (INSTRUCTIONS_CLEAR)                    /* 表示を全消去 */
#define COMMAND_DISPLAY_HOME            (INSTRUCTIONS_HOME)                     /* カーソル位置をDDRAMの00Hに移動 */

/* 文字入力後のカーソル移動方向 */
#define COMMAND_DISPLAY_NONE            (INSTRUCTIONS_ENTRY_MODE | 0x00)        /* 表示のシフト有無：無し */
#define COMMAND_DISPLAY_RIGHT           (INSTRUCTIONS_ENTRY_MODE | 0x01)        /* 表示のシフト有無：右シフトあり */
#define COMMAND_DISPLAY_LEFT            (INSTRUCTIONS_ENTRY_MODE | 0x03)        /* 表示のシフト有無：左シフトあり */

#define COMMAND_DISPLAY_ON              (INSTRUCTIONS_DISPLAY_CONTROL | 0x04)   /* 表示ON  */
#define COMMAND_DISPLAY_OFF             (INSTRUCTIONS_DISPLAY_CONTROL | 0x00)   /* 表示OFF */

#define COMMAND_CURSOR_ON               (INSTRUCTIONS_DISPLAY_CONTROL | 0x06)   /* カーソル "_" 表示 */
#define COMMAND_CURSOR_BLINK_ON         (INSTRUCTIONS_DISPLAY_CONTROL | 0x07)   /* カーソル "_" 表示+点滅 */
#define COMMAND_CURSOR_OFF              (INSTRUCTIONS_DISPLAY_CONTROL | 0x04)   /* カーソル "_" 非表示 */

#define COMMAND_CURSOR_MOVE_RIGHT       (INSTRUCTIONS_SHIFT | 0x04)             /* カーソルを右に移動 */
#define COMMAND_CURSOR_MOVE_LEFT        (INSTRUCTIONS_SHIFT | 0x00)             /* カーソルを左に移動 */

#define COMMAND_SCREEN_MOVE_RIGHT       (INSTRUCTIONS_SHIFT | 0x0C)             /* 画面位置を右に移動(DDRAMごとずらす) */
#define COMMAND_SCREEN_MOVE_LEFT        (INSTRUCTIONS_SHIFT | 0x08)             /* 画面位置を左に移動(DDRAMごとずらす) */

#define COMMAND_CGRAM_SET               (INSTRUCTIONS_CGRAM)
#define COMMAND_DDRAM_SET               (INSTRUCTIONS_DDRAM)


/*** 処理結果 ***/
#define ACM1602NI_OK         0
#define ACM1602NI_NG         1

#define ACM1602NI_ERROR_ARG     -1
#define ACM1602NI_ERROR_CB      -2

/****************************************************************************************************
 * Public typedef
 ****************************************************************************************************/

/**
 * @brief I2C書き込みコールバック
 * @param address スレーブアドレス
 * @param pData   送信データを示すメモリ領域
 * @param length  送信データの長さ
 * @param wait    送信完了後の待ち
 */
typedef int (*i2c_cb_t)(uint8_t address, const uint8_t *pData, uint32_t length, uint32_t wait);

/****************************************************************************************************
 * Public Variables
 ****************************************************************************************************/

/****************************************************************************************************
 * Public Functions
 ****************************************************************************************************/

/**
 * @brief ACM1602NI初期化
 * @return 処理結果
 */
int acm1602ni_init(i2c_cb_t callback);

/**
 * @brief 現在の行列位置から文字列の書き込みを行う。
 *        行列位置の終端に到達した場合は先頭位置に循環する。
 * @param string 文字列のメモリ領域
 */
int acm1602ni_write_string(const char *string);

/**
 * @brief 指定した行列位置を起点に文字列の書き込みを行う。
 * @param string 文字列のメモリ領域
 * @param line   行位置
 * @param column 列位置
 */
int acm1602ni_write_string_at(const char *string, uint16_t line, uint16_t column);

/**
 * @brief DDRAM ADDRESS位置移動
 * @param line   移動する行位置
 * @param column 移動する列位置
 */
int acm1602ni_move_ddram_address(uint16_t line, uint16_t column);

/**
 * @brief コマンド設定
 *        ACM1602NIの制御状態を指示するコマンドを設定する
 * @param command コマンドコード
 */
int acm1602ni_command(uint8_t command);

#endif  /* __ACM1602NI_H__ */