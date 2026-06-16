/****************************************************************************************************
 * @file    acm1602ni.c
 * @brief   LCDデバイスの「ACM1602NI-FLW-FBW-M01」制御用デバイスドライバ
 * @details このファイルではデバイスドライバモジュールを定義している。
 *
 * @author  
 * @date    2026/06/14     新規作成
 ****************************************************************************************************/

/****************************************************************************************************
 * Private include
 ****************************************************************************************************/
#include "acm1602ni.h"

#include <stddef.h>

/****************************************************************************************************
 * Private define
 ****************************************************************************************************/
/*** SLAVE ADDRESS ***/
#define SLAVE_ADDRESS           0x50        /* スレーブアドレス(7bitアドレスモード) */

#define SLAVE_WRITE             ((SLAVE_ADDRESS << 1) | 0x00)       /* デバイスへの書き込み */
#define SLAVE_READ              ((SLAVE_ADDRESS << 1) | 0x01)       /* デバイスへの読み込み */


/*** CONTROL BYTE ***/
#define RS_BIT                  0x80        /* b[7] 0:command / 1:RAM DATA  */

#define CONTROL_BYTE_COMMAND    (0x00)
#define CONTROL_BYTE_RAMDATA    (RS_BIT)


/*** DDRAM ADDRESS ***/
#define DDRAM_LINE_SIZE         ACM1602NI_LINE_MAX
#define DDRAM_COLUMN_SIZE       ACM1602NI_COLUMN_MAX


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


/****************************************************************************************************
 * Private typedef
 ****************************************************************************************************/
typedef struct
{
    i2c_cb_t i2c_cb;
} acm1602ni_if_t;

/****************************************************************************************************
 * Private Global Variables
 ****************************************************************************************************/

/*** DDRAM ADDRESS(2x16) ***/
static const uint8_t s_matrix_ddram_address[DDRAM_LINE_SIZE][DDRAM_COLUMN_SIZE] = {
    { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F },
    { 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F }
};

static acm1602ni_if_t acm1602ni_if;

/****************************************************************************************************
 * Private Prototype Declaration
 ****************************************************************************************************/

/**
 * @brief DDRAM ADDRESS位置移動
 * @param line   移動する行位置
 * @param column 移動する列位置
 */
static int acm1602ni_move_ddram_address(uint16_t line, uint16_t column);

/**
 * @brief コマンド設定
 *        ACM1602NIの制御状態を指示するコマンドを設定する
 * @param command コマンドコード
 */
static int acm1602ni_command(uint8_t command);

/*** Low Level Layer function ***/
static int acm1602ni_send_i2c(const uint8_t *data, uint32_t wait);

/**
 * @brief ACM1602NI初期化
 *        コールバックの登録・LCDの起動時処理を行う。
 * @param callback コールバック処理のポインタ
 * @return 処理結果
 */
int acm1602ni_init(i2c_cb_t callback)
{
    int success = ACM1602NI_OK;

    if (callback == NULL)
    {
        /* コールバックない場合はドライバ利用不可 */
        success = ACM1602NI_ERROR_CB;
    }
    else
    {
        uint8_t code = 0x00;

        acm1602ni_if.i2c_cb = callback;

        /* ACM1602NIの機能設定は固定(CGRAM未対応) */
        code = code | 0x10;     /* (DL)Data Length = 4bit mode */
        code = code | 0x08;     /* (N)Display Line = 2Line mode */
        code = code | 0x00;     /* (F)Font Size    = 5*8  dots */

        /* 起動時の初期状態設定 */
        acm1602ni_command(INSTRUCTIONS_FUNCTION | code);
        acm1602ni_command(COMMAND_DISPLAY_NONE);
        acm1602ni_command(COMMAND_CURSOR_MOVE_RIGHT);
        acm1602ni_command(COMMAND_DISPLAY_ON);
        acm1602ni_command(COMMAND_DISPLAY_CLEAR);
    }

    return success;
}

/**
 * @brief 現在の行列位置から文字列の書き込みを行う。
 *        行列位置の終端に到達した場合は先頭位置に循環する。
 * @param string 文字列のメモリ領域
 * @return 処理結果
 */
int acm1602ni_write_string(const char *string)
{
    int success = ACM1602NI_OK;
    uint8_t buf[2] = {0};

    buf[0] = CONTROL_BYTE_RAMDATA;

    if (string == NULL)
    {
        /* 引数異常 */
        success = ACM1602NI_ERROR_ARG;
    }
    else
    {
        while (*string != '\0')
        {
            buf[1] = *string++;

            /* RAM DATA送信 */
            success = acm1602ni_send_i2c(buf, 1);
            if (success == ACM1602NI_ERROR_CB)
            {
                /* 送信コールバック未登録 */
                break;
            }
        }
    }

    return success;
}

/**
 * @brief 指定した行列位置を起点に文字列の書き込みを行う。
 * @param string 文字列のメモリ領域
 * @param line   行位置
 * @param column 列位置
 * @return 処理結果
 */
int acm1602ni_write_string_at(const char *string, uint16_t line, uint16_t column)
{
    int success = 0;

    if (string == NULL)
    {
        /* 引数異常 */
        success = ACM1602NI_ERROR_ARG;
    }
    else
    {
        /* 描画位置更新 */
        success = acm1602ni_move_ddram_address(line, column);
        if (success == ACM1602NI_OK)
        {
            /* 文字列書き込み */
            success = acm1602ni_write_string(string);
            if (success == ACM1602NI_ERROR_CB)
            {
                /* 送信コールバック未登録 */
            }
        }
        else
        {
            /* 書き込み位置の不正 */
            success = ACM1602NI_NG;
        }
    }

    return success;
}

/**
 * @brief LCD画面の表示内容をすべて消す
 */
int acm1602ni_display_clear(void)
{
    return acm1602ni_command(COMMAND_DISPLAY_CLEAR);
}

/**
 * @brief LCD画面の表示をONにする
 * @return 処理結果
 */
int acm1602ni_display_on(void)
{
    return acm1602ni_command(COMMAND_DISPLAY_ON);
}

/**
 * @brief LCD画面の表示をOffにする
 * @return 処理結果
 */
int acm1602ni_display_off(void)
{
    return acm1602ni_command(COMMAND_DISPLAY_OFF);
}

/**
 * @brief LCD画面にカーソル"_"を表示する
 * @param is_blink カーソルの点滅設定
 * @return 処理結果
 */
int acm1602ni_cursor_on(bool is_blink)
{
    uint8_t code = COMMAND_CURSOR_ON;

    if (is_blink)
    {
        /* カーソル点滅あり */
        code = COMMAND_CURSOR_BLINK_ON;
    }

    return acm1602ni_command(code);
}

/**
 * @brief LCD画面にカーソル"_"を非表示にする
 * @return 処理結果
 */
int acm1602ni_cursor_off(void)
{
    return acm1602ni_command(COMMAND_DISPLAY_OFF);
}


/**
 * @brief LCD画面にカーソル"_"を行列の0番目に移動する
 */
int acm1602ni_cursor_home(void)
{
    return acm1602ni_command(COMMAND_DISPLAY_HOME);
}

/**
 * @brief LCD画面上のカーソル"_"位置と文字列の書き込み位置移動
 *        カーソルの表示有無にかかわらず、APIの実行後は指定した位置より文字列の書き込みが行われる
 * @param line   移動する行位置
 * @param column 移動する列位置
 * @return 処理結果
 */
int acm1602ni_cursor_move(uint16_t line, uint16_t column)
{
    /* カーソル位置の移動 */
    return acm1602ni_move_ddram_address(line, column);
}

/**
 * @brief DDRAM ADDRESS位置移動
 *        デバイスの表示可能範囲内のみ位置移動を行う
 * @param line   移動する行位置
 * @param column 移動する列位置
 * @return 処理結果
 */
static int acm1602ni_move_ddram_address(uint16_t line, uint16_t column)
{
    int success = ACM1602NI_OK;

    if ((line < DDRAM_LINE_SIZE) && (column < DDRAM_COLUMN_SIZE))
    {
        uint8_t command = 0;

        command = COMMAND_DDRAM_SET;
        command = command | s_matrix_ddram_address[line][column];

        /* DDRAM ADDRESS位置の移動 */
        success = acm1602ni_command(command);
    }
    else
    {
        /* DDRAM ADDRESSの指定可能範囲外 */
        success = ACM1602NI_ERROR_ARG;
    }

    return success;
}

/**
 * @brief コマンド設定
 *        ACM1602NIの制御状態を指示するコマンドを設定する
 * @param command コマンドコード
 * @return 処理結果
 */
static int acm1602ni_command(uint8_t command)
{
    int success = ACM1602NI_OK;

    uint8_t buf[2] = {0};
    uint32_t wait = 1;      /* wait[ms] */

    buf[0] = CONTROL_BYTE_COMMAND;
    buf[1] = command;

    /* 送信後の待機時間[ms]をコマンド毎に設定 */
    if (((command & COMMAND_DISPLAY_CLEAR) != 0x00) && ((command & 0xfe) == 0x00))
    {
        /* command = Clear Display */
        wait = 3;               /* wait 2.16ms */
    }
    else if (((command & COMMAND_DISPLAY_HOME) != 0x00) && ((command & 0xfd) == 0x00))
    {
        /* command = Return Home */
        wait = 3;               /* wait 2.16ms */
    }
    else
    {
        /* command = Entry Mode Set */
        /* command = Display ON/OFF Control */
        /* command = Cursor of Display Shift */
        /* command = Function Set */
        /* command = Set CGRAM Adress */
        /* command = Set DDRAM Adress */
        wait = 1;               /* wait 53us */
    }

    /* command送信 */
    success = acm1602ni_send_i2c(buf, wait);

    return success;
}

/**
 * @brief I2C送信処理
 * @param data デバイスに送信するデータ(文字 or コマンド)
 * @param wait 送信実行後の待ち時間
 * @return 処理結果
 */
static int acm1602ni_send_i2c(const uint8_t *data, uint32_t wait)
{
    int success = ACM1602NI_OK;

    if (acm1602ni_if.i2c_cb != NULL)
    {
        /* 送信コールバック実行 */
        success = acm1602ni_if.i2c_cb(SLAVE_WRITE, data, 2, wait);
    }
    else
    {
        /* 送信コールバック未登録 */
        success = ACM1602NI_ERROR_CB;
    }

    return success;
}