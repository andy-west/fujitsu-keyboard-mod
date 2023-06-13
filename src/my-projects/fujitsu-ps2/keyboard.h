#include "pico/stdlib.h"
#include "ps2dev.h"

const uint DEBOUNCE_SCANS = 3;

const uint REPEAT_DELAY_MS = 500;
const uint REPEAT_MS = 92;

const uint MAT_COL_A = 20;
const uint MAT_COL_B = 16;
const uint MAT_COL_C = 17;
const uint MAT_COL_D = 18;
const uint MAT_COL_E = 19;
const uint MAT_COL_F = 15;
const uint MAT_COL_G = 14;
const uint MAT_COL_H = 21;

const uint MAT_ROW_1 = 3;
const uint MAT_ROW_2 = 13;
const uint MAT_ROW_3 = 12;
const uint MAT_ROW_4 = 11;
const uint MAT_ROW_5 = 10;
const uint MAT_ROW_6 = 0;
const uint MAT_ROW_7 = 1;
const uint MAT_ROW_8 = 9;
const uint MAT_ROW_9 = 8;
const uint MAT_ROW_10 = 7;
const uint MAT_ROW_11 = 6;
const uint MAT_ROW_12 = 2;
const uint MAT_ROW_13 = 5;
const uint MAT_ROW_14 = 4;

class Keyboard
{
    public:
        Keyboard(PS2dev& ps2);
        void process_keys();
    private:
        PS2dev *_ps2;

        int _cols[8] = { MAT_COL_A, MAT_COL_B, MAT_COL_C, MAT_COL_D, MAT_COL_E,
            MAT_COL_F, MAT_COL_G, MAT_COL_H };

        int _rows[14] = { MAT_ROW_1, MAT_ROW_2, MAT_ROW_3, MAT_ROW_4, MAT_ROW_5,
            MAT_ROW_6, MAT_ROW_7, MAT_ROW_8, MAT_ROW_9, MAT_ROW_10, MAT_ROW_11,
            MAT_ROW_12, MAT_ROW_13, MAT_ROW_14 };

        bool _key_states[8][14] = {};
        uint _debounce_counts[8][14] = {};
        uint32_t _key_repeat_time;

        void ps2_repeat_key(int col, int row);
        void ps2_send_key(int col, int row, bool key_state);
        void get_ps2_scan_code(int col, int row, unsigned char *scan_code, bool *is_special);
};
