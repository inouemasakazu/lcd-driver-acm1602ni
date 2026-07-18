/****************************************************************************************************
 * FILE NAME   : examples.c
 * Description : LCDのサンプルプログラム
 ****************************************************************************************************/

/****************************************************************************************************
 * Private include
 ****************************************************************************************************/
#include    "examples.h"

#include    "if_lcd.h"

#include    "stm32f4xx_hal.h"

#include    "stdio.h"
#include    "string.h"

/****************************************************************************************************
 * Private define
 ****************************************************************************************************/
#define     EXAMPLES_INIT       (0)     /* APP初期化 */
#define     EXAMPLES_STATE1     (1)     /* APP状態1 */
#define     EXAMPLES_STATE2     (2)     /* APP状態2 */

/****************************************************************************************************
 * Private typedef
 ****************************************************************************************************/

/****************************************************************************************************
 * Private Global Variables
 ****************************************************************************************************/

/****************************************************************************************************
 * Private Prototype Declaration
 ****************************************************************************************************/

/****************************************************************************************************
 * Function Name : examples_init
 * Description   : LCDディスプレイの初期化
 *---------------------------------------------------------------------------------------------------
 * Input  : None
 * Output : None
 * Return : None
 ****************************************************************************************************/
void examples_init( void )
{
    /* LCDデバイス初期化 */
    iLcd_setFunction( 1 );       /* 制御・表示タイプの設定 */
    iLcd_setCursorShift( 1 );    /* カーソルのシフト設定 */
    iLcd_setOnOffControl( 1 );   /* 表示・点滅設定 */
    iLcd_setEntryMode( 2 );      /* エントリーモード */

    /* ディスプレイクリア */
    iLcd_displayClear();

    /* バックライトON */
    iLcd_backLightControl( BACK_LIGHT_ON );
}

/****************************************************************************************************
 * Function Name : examples_main1
 * Description   : サンプル用の実例2
 *                 日時表示を行います。更新する日時はダミーです。
 *---------------------------------------------------------------------------------------------------
 * Input  : None
 * Output : None
 * Return : None
 ****************************************************************************************************/
void examples_main1( void )
{
    static short state = EXAMPLES_INIT;
    short timer = 1000;         /* 1sec = 1000ms */

    /* 日時表示用に静的に確保 */
    static short year = 2025;
    static short mon  =    9;
    static short day  =   13;
    static short hour =   12;
    static short min  =    0;
    static short sec  =    0;

    char date[20];
    char time[20];

    switch (state)
    {
    case EXAMPLES_INIT:
        /* 起動時の初期表示 */
        iLcd_writeDisplay("Wake UP!", 0, 0);
        iLcd_writeDisplay("LCD examples 1!", 1, 0);
        HAL_Delay( timer + 500 );

        /* 表示クリア */
        iLcd_displayClear();

        state = EXAMPLES_STATE1;
        break;

    case EXAMPLES_STATE1:
        /* 時間表示更新 */
        snprintf(date, 17, "DATE:%4d/%2d/%2d", year, mon, day);
        snprintf(time, 17, "TIME:%2d:%2d:%2d", hour, min, sec);

        iLcd_writeDisplay(date, 0, 0);
        iLcd_writeDisplay(time, 1, 0);
        HAL_Delay( timer );

        state = EXAMPLES_STATE2;
        break;

    case EXAMPLES_STATE2:
        /* 時刻更新 */
        sec++;
        if (sec > 59)
        {
            sec = 0;
            min++;
        }

        if (min > 59)
        {
            min = 0;
            hour++;
        }

        if (hour > 23)
        {
            hour = 0;
        }

        state = EXAMPLES_STATE1;
        break;

    default:
        break;
    }
}

/****************************************************************************************************
 * Function Name : examples_main2
 * Description   : サンプル用の実例2
 *                 インフォメーションメッセージの表示を行います。
 *---------------------------------------------------------------------------------------------------
 * Input  : None
 * Output : None
 * Return : None
 ****************************************************************************************************/
void examples_main2( void )
{
    static short state = EXAMPLES_INIT;
    short timer = 1000;         /* 1sec = 1000ms */

    char msg[] = { "Today's weather will be fine." };
    char buf[] = { "                " };                /* 更新用フレームバッファ */
    static short begin = 0;
    short idx;

    switch (state)
    {
    case EXAMPLES_INIT:
        /* 起動時の初期表示 */
        iLcd_writeDisplay("Wake UP!", 0, 0);
        iLcd_writeDisplay("LCD examples 2!", 1, 0);
        HAL_Delay( timer + 500 );

        /* 表示クリア */
        iLcd_displayClear();

        iLcd_writeDisplay("Information Msg", 0, 0);

        state = EXAMPLES_STATE1;
        break;

    case EXAMPLES_STATE1:
        /* 表示更新 */
        for (idx = 0; idx < 16; idx++)
        {
            if (msg[begin + idx] != '\0')
            {
                buf[idx] = msg[begin + idx];
            }
            else
            {
                break;
            }
        }

        iLcd_writeDisplay(buf, 1, 0);
        HAL_Delay( timer );         /* 1秒よりちょっと短くする */

        state = EXAMPLES_STATE2;
        break;

    case EXAMPLES_STATE2:
        /* 開始index更新 */
        begin++;
        if (begin > strlen( msg ))
        {
            begin = 0;
        }

        state = EXAMPLES_STATE1;
        break;

    default:
        break;
    }
}
