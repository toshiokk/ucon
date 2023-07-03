//
// Simple Japanese Input Method - Character-based User Interface
//

#ifndef imjcui_h
#define imjcui_h

#include "imj.h"

extern imj imj__;

int imcui_filter(char *key_seq, int input_len, int *output_len, int buf_len);
int imcui_get_converting_line_str(int mode, char *buffer, int buf_len);

#endif // imjcui_h

// End of imjcui.h
