#include "pico/stdlib.h"
#include "keyboard.h"
#include "ps2dev.h"

Keyboard::Keyboard(PS2dev& ps2)
{
    uint i;
    uint row;
    uint col;

    _ps2 = &ps2;

    for (i = 0; i < 8 ; i++)
    {
        col = _cols[i];

        gpio_init(col);
        gpio_set_dir(col, GPIO_OUT);
        gpio_put(col, 0);
    }

    for (i = 0; i < 14 ; i++)
    {
        row = _rows[i];

        gpio_init(row);
        gpio_set_dir(row, GPIO_IN);
        gpio_pull_down(row);
    }
}

void Keyboard::process_keys()
{
    int i;
    int j;
    int col;
    int row;
    bool old_key_state;
    bool new_key_state;

    for (i = 0; i < 8 ; i++)
    {
        col = _cols[i];

        gpio_put(col, 1);

        for (j = 0; j < 14 ; j++)
        {
            row = _rows[j];

            old_key_state = _key_states[col][row];
            new_key_state = gpio_get(row);

            if (new_key_state != old_key_state)
            {
                _debounce_counts[col][row]++;

                if (_debounce_counts[col][row] >= DEBOUNCE_SCANS)
                {
                    ps2_send_key(col, row, new_key_state);
                    _key_repeat_time = make_timeout_time_ms(REPEAT_DELAY_MS);
                    _key_states[col][row] = new_key_state;
                    _debounce_counts[col][row] = 0;
                }
            }
            else
            {
                _debounce_counts[col][row] = 0;

                if (new_key_state == 1)
                {
                    ps2_repeat_key(col, row);
                }
            }
        }

        gpio_put(col, 0);

        sleep_us(5);
    }
}

void Keyboard::ps2_repeat_key(int col, int row)
{
    unsigned char scan_code;
    bool is_special;
    uint32_t now;

    get_ps2_scan_code(col, row, &scan_code, &is_special);

    if (
            scan_code != _ps2->LEFT_CONTROL
            && scan_code != _ps2->RIGHT_CONTROL
            && scan_code != _ps2->LEFT_SHIFT
            && scan_code != _ps2->RIGHT_SHIFT
            && scan_code != _ps2->LEFT_ALT
            && scan_code != _ps2->RIGHT_ALT
            && scan_code != _ps2->NUM_LOCK
            && scan_code != _ps2->SCROLL_LOCK
            && scan_code != _ps2->CAPS_LOCK
        )
    {
        now = get_absolute_time();

        if (absolute_time_diff_us(now, _key_repeat_time) <= 0)
        {
            ps2_send_key(col, row, 1);
            _key_repeat_time = make_timeout_time_ms(REPEAT_MS);
        }
    }
}

void Keyboard::ps2_send_key(int col, int row, bool key_state)
{
    unsigned char scan_code;
    bool is_special;
    int (PS2dev::*key_func)(unsigned char code);

    if ((col == MAT_COL_D && row == MAT_ROW_1)
        || (row == MAT_COL_G && row == MAT_ROW_3))
    {
        _ps2->keyboard_pausebreak();
    }
    else if (col == MAT_COL_E && row == MAT_ROW_3)
    {
        if (key_state == 1)
        {
            _ps2->keyboard_press_printscreen();
        }
        else
        {
            _ps2->keyboard_release_printscreen();
        }
    }
    else
    {
        get_ps2_scan_code(col, row, &scan_code, &is_special);

        key_func = (key_state == 1)
            ? (is_special
                ? &PS2dev::keyboard_press_special
                : &PS2dev::keyboard_press)
            : (is_special
                ? &PS2dev::keyboard_release_special
                : &PS2dev::keyboard_release);

        (_ps2->*key_func)(scan_code);
    }
}

void Keyboard::get_ps2_scan_code(int col, int row, unsigned char *scan_code, bool *is_special)
{
    switch(col)
    {
        case MAT_COL_A:
            switch(row)
            {
                case MAT_ROW_1: *scan_code = _ps2->NUMPAD_EIGHT;  *is_special = 0; break;  // 8 (KP)
                case MAT_ROW_2: *scan_code = _ps2->F9;            *is_special = 0; break;  // F9
                case MAT_ROW_3: return;
                case MAT_ROW_4: *scan_code = _ps2->EIGHT;         *is_special = 0; break;  // 8 *
                case MAT_ROW_5: *scan_code = _ps2->RIGHT_SHIFT;   *is_special = 0; break;  // R SHIFT
                case MAT_ROW_6: *scan_code = _ps2->RIGHT_ALT;     *is_special = 1; break;  // ALT
                case MAT_ROW_7: *scan_code = _ps2->DIVIDE;        *is_special = 1; break;  // / (KP)
                case MAT_ROW_8: *scan_code = _ps2->I;             *is_special = 0; break;  // I
                case MAT_ROW_9: *scan_code = _ps2->CAPS_LOCK;     *is_special = 0; break;  // CAPS LOCK
                case MAT_ROW_10: *scan_code = _ps2->K;            *is_special = 0; break;  // K
                case MAT_ROW_11: *scan_code = _ps2->LEFT_CONTROL; *is_special = 0; break;  // CTRL
                case MAT_ROW_12: *scan_code = _ps2->LEFT_WIN;     *is_special = 1; break;  // GRAPH
                case MAT_ROW_13: *scan_code = _ps2->COMMA;        *is_special = 0; break;  // , <
                case MAT_ROW_14: *scan_code = _ps2->LEFT_SHIFT;   *is_special = 0; break;  // L SHIFT
            }
            break;

        case MAT_COL_B:
            switch(row)
            {
                case MAT_ROW_1: *scan_code = _ps2->TAB;           *is_special = 0; break;  // TAB
                case MAT_ROW_2: *scan_code = _ps2->F2;            *is_special = 0; break;  // F2
                case MAT_ROW_3: *scan_code = _ps2->F10;           *is_special = 0; break;  // F10
                case MAT_ROW_4: *scan_code = _ps2->ONE;           *is_special = 0; break;  // 1 !
                case MAT_ROW_5: *scan_code = _ps2->NINE;          *is_special = 0; break;  // 9 (
                case MAT_ROW_6: *scan_code = _ps2->ASTERISK;      *is_special = 0; break;  // * (KP)
                case MAT_ROW_7: return;
                case MAT_ROW_8: *scan_code = _ps2->Q;             *is_special = 0; break;  // Q
                case MAT_ROW_9: *scan_code = _ps2->O;             *is_special = 0; break;  // O
                case MAT_ROW_10: *scan_code = _ps2->A;            *is_special = 0; break;  // A
                case MAT_ROW_11: *scan_code = _ps2->L;            *is_special = 0; break;  // L
                case MAT_ROW_12: *scan_code = _ps2->NUMPAD_SEVEN; *is_special = 0; break;  // 7 (KP)
                case MAT_ROW_13: *scan_code = _ps2->Z;            *is_special = 0; break;  // Z
                case MAT_ROW_14: *scan_code = _ps2->PERIOD;       *is_special = 0; break;  // . >
            }
            break;

        case MAT_COL_C:
            switch(row)
            {
                case MAT_ROW_1: *scan_code = _ps2->ESCAPE;       *is_special = 0; break;  // ESC
                case MAT_ROW_2: *scan_code = _ps2->F3;           *is_special = 0; break;  // F3
                case MAT_ROW_3: *scan_code = _ps2->F11;          *is_special = 0; break;  // F11
                case MAT_ROW_4: *scan_code = _ps2->TWO;          *is_special = 0; break;  // 2 @
                case MAT_ROW_5: *scan_code = _ps2->ZERO;         *is_special = 0; break;  // 0 )
                case MAT_ROW_6: *scan_code = _ps2->NUMPAD_ONE;   *is_special = 0; break;  // 1 (KP)
                case MAT_ROW_7: return;
                case MAT_ROW_8: *scan_code = _ps2->W;            *is_special = 0; break;  // W
                case MAT_ROW_9: *scan_code = _ps2->P;            *is_special = 0; break;  // P
                case MAT_ROW_10: *scan_code = _ps2->S;           *is_special = 0; break;  // S
                case MAT_ROW_11: *scan_code = _ps2->SEMI_COLON;  *is_special = 0; break;  // ; :
                case MAT_ROW_12: *scan_code = _ps2->NUMPAD_FOUR; *is_special = 0; break;  // 4 (KP)
                case MAT_ROW_13: *scan_code = _ps2->X;           *is_special = 0; break;  // X
                case MAT_ROW_14: *scan_code = _ps2->SLASH;       *is_special = 0; break;  // / ?
            }
            break;

        case MAT_COL_D:
            switch(row)
            {
                case MAT_ROW_1: return;
                case MAT_ROW_2: *scan_code = _ps2->F4;           *is_special = 0; break;  // F4
                case MAT_ROW_3: *scan_code = _ps2->F12;          *is_special = 0; break;  // F12
                case MAT_ROW_4: *scan_code = _ps2->THREE;        *is_special = 0; break;  // 3 #
                case MAT_ROW_5: *scan_code = _ps2->MINUS;        *is_special = 0; break;  // - _
                case MAT_ROW_6: *scan_code = _ps2->NUMPAD_TWO;   *is_special = 0; break;  // 2 (KP)
                case MAT_ROW_7: return;
                case MAT_ROW_8: *scan_code = _ps2->E;            *is_special = 0; break;  // E
                case MAT_ROW_9: *scan_code = _ps2->OPEN_BRACKET; *is_special = 0; break;  // [
                case MAT_ROW_10: *scan_code = _ps2->D;           *is_special = 0; break;  // D
                case MAT_ROW_11: *scan_code = _ps2->TICK_MARK;   *is_special = 0; break;  // ' "
                case MAT_ROW_12: *scan_code = _ps2->NUMPAD_FIVE; *is_special = 0; break;  // 5 (KP)
                case MAT_ROW_13: *scan_code = _ps2->C;           *is_special = 0; break;  // C
                case MAT_ROW_14: *scan_code = _ps2->RIGHT_SHIFT; *is_special = 0; break;  // 'Blank'
            }
            break;

        case MAT_COL_E:
            switch(row)
            {
                case MAT_ROW_1: *scan_code = _ps2->F1;            *is_special = 0; break;  // F1
                case MAT_ROW_2: *scan_code = _ps2->F5;            *is_special = 0; break;  // F5
                case MAT_ROW_3: return;
                case MAT_ROW_4: *scan_code = _ps2->FOUR;          *is_special = 0; break;  // 4 $
                case MAT_ROW_5: *scan_code = _ps2->EQUAL;         *is_special = 0; break;  // = +
                case MAT_ROW_6: *scan_code = _ps2->NUMPAD_THREE;  *is_special = 0; break;  // 3 (KP)
                case MAT_ROW_7: return;
                case MAT_ROW_8: *scan_code = _ps2->R;             *is_special = 0; break;  // R
                case MAT_ROW_9: *scan_code = _ps2->CLOSE_BRACKET; *is_special = 0; break;  // ]
                case MAT_ROW_10: *scan_code = _ps2->F;            *is_special = 0; break;  // F
                case MAT_ROW_11: *scan_code = _ps2->BACKSLASH;    *is_special = 0; break;  // \ |
                case MAT_ROW_12: *scan_code = _ps2->NUMPAD_SIX;   *is_special = 0; break;  // 6 (KP)
                case MAT_ROW_13: *scan_code = _ps2->V;            *is_special = 0; break;  // V
                case MAT_ROW_14: return;
            }
            break;

        case MAT_COL_F:
            switch(row)
            {
                case MAT_ROW_1: *scan_code = _ps2->SPACE;        *is_special = 0; break;  // Space
                case MAT_ROW_2: *scan_code = _ps2->F6;           *is_special = 0; break;  // F6
                case MAT_ROW_3: *scan_code = _ps2->SCROLL_LOCK;  *is_special = 0; break;  // F14
                case MAT_ROW_4: *scan_code = _ps2->FIVE;         *is_special = 0; break;  // 5 %
                case MAT_ROW_5: *scan_code = _ps2->ACCENT;       *is_special = 0; break;  // ` ~
                case MAT_ROW_6: *scan_code = _ps2->NUMPAD_ENTER; *is_special = 1; break;  // Return (KP)
                case MAT_ROW_7: return;
                case MAT_ROW_8: *scan_code = _ps2->T;            *is_special = 0; break;  // T
                case MAT_ROW_9: *scan_code = _ps2->BACKSPACE;    *is_special = 0; break;  // Backspace
                case MAT_ROW_10: *scan_code = _ps2->G;           *is_special = 0; break;  // G
                case MAT_ROW_11: *scan_code = _ps2->ENTER;       *is_special = 0; break;  // Return
                case MAT_ROW_12: *scan_code = _ps2->COMMA;       *is_special = 0; break;  // , (KP)
                case MAT_ROW_13: *scan_code = _ps2->B;           *is_special = 0; break;  // B
                case MAT_ROW_14: return;
            }
            break;

        case MAT_COL_G:
            switch(row)
            {
                case MAT_ROW_1: *scan_code = _ps2->EQUAL;        *is_special = 0; break;  // = (KP)
                case MAT_ROW_2: *scan_code = _ps2->F7;           *is_special = 0; break;  // F7
                case MAT_ROW_3: return;
                case MAT_ROW_4: *scan_code = _ps2->SIX;          *is_special = 0; break;  // 6 ^
                case MAT_ROW_5: *scan_code = _ps2->INSERT;       *is_special = 1; break;  // INS
                case MAT_ROW_6: *scan_code = _ps2->NUMPAD_ZERO;  *is_special = 0; break;  // 0 (KP)
                case MAT_ROW_7: *scan_code = _ps2->NUMPAD_MINUS; *is_special = 0; break;  // - (KP)
                case MAT_ROW_8: *scan_code = _ps2->Y;            *is_special = 0; break;  // Y
                case MAT_ROW_9: *scan_code = _ps2->END;          *is_special = 1; break;  // EL
                case MAT_ROW_10: *scan_code = _ps2->H;           *is_special = 0; break;  // H
                case MAT_ROW_11: *scan_code = _ps2->DELETE;      *is_special = 1; break;  // DEL
                case MAT_ROW_12: *scan_code = _ps2->LEFT_ARROW;  *is_special = 1; break;  // Left Arrow
                case MAT_ROW_13: *scan_code = _ps2->N;           *is_special = 0; break;  // N
                case MAT_ROW_14: *scan_code = _ps2->UP_ARROW;    *is_special = 1; break;  // Up Arrow
            }
            break;

        case MAT_COL_H:
            switch(row)
            {
                case MAT_ROW_1: *scan_code = _ps2->NUMPAD_NINE;  *is_special = 0; break;  // 9 (KP)
                case MAT_ROW_2: *scan_code = _ps2->F8;           *is_special = 0; break;  // F8
                case MAT_ROW_3: *scan_code = _ps2->NUM_LOCK;     *is_special = 0; break;  // F16
                case MAT_ROW_4: *scan_code = _ps2->SEVEN;        *is_special = 0; break;  // 7 &
                case MAT_ROW_5: *scan_code = _ps2->PAGE_UP;      *is_special = 1; break;  // CLS
                case MAT_ROW_6: *scan_code = _ps2->DECIMAL;      *is_special = 0; break;  // . (KP)
                case MAT_ROW_7: *scan_code = _ps2->PLUS;         *is_special = 0; break;  // + (KP)
                case MAT_ROW_8: *scan_code = _ps2->U;            *is_special = 0; break;  // U
                case MAT_ROW_9: *scan_code = _ps2->PAGE_DOWN;    *is_special = 1; break;  // DUP
                case MAT_ROW_10: *scan_code = _ps2->J;           *is_special = 0; break;  // J
                case MAT_ROW_11: *scan_code = _ps2->HOME;        *is_special = 1; break;  // HOME
                case MAT_ROW_12: *scan_code = _ps2->RIGHT_ARROW; *is_special = 1; break;  // Right Arrow
                case MAT_ROW_13: *scan_code = _ps2->M;           *is_special = 0; break;  // M
                case MAT_ROW_14: *scan_code = _ps2->DOWN_ARROW;  *is_special = 1; break;  // Down Arrow
            }
            break;
    }
}
