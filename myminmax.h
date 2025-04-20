// myminmax.h

#ifndef myminmax_h
#define myminmax_h

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

#define MIN_(aa, bb)			int_min((aa), (bb))
#define MAX_(aa, bb)			int_max((aa), (bb))
#define LIM_MIN_(aa, bb)		MAX_((aa), (bb))
#define LIM_MAX_(aa, bb)		MIN_((aa), (bb))
#define MIN_MAX(min, val, max)	int_min_max((min), (val), (max))
#define MK_IN_RANGE(min, val, max)	MIN_MAX((min), (val), (max)-1)
#define IS_OVERLAP(x1, x2, y1, y2)	(MAX_((x1), (y1)) < MIN_((x2), (y2)))
#define IS_IN_RANGE(min, val, max)		((min) <= (val) && (val) < (max))

int int_min(int aa, int bb);
int int_max(int aa, int bb);
int int_min_max(int min, int val, int max);

#if defined(__cplusplus)
}
#endif // __cplusplus

#endif // myminmax_h

// End of myminmax.h
