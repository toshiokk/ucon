// myminmax.c

#define MIN__(aa, bb)				((aa) < (bb) ? (aa) : (bb))
#define MAX__(aa, bb)				((aa) > (bb) ? (aa) : (bb))
#define MIN_MAX_(min, val, max)		((val) < (min) ? (min) : ((max) < (val) ? (max) : (val)))

int int_min(int aa, int bb)
{
	return MIN__(aa, bb);
}
int int_max(int aa, int bb)
{
	return MAX__(aa, bb);
}
int int_min_max(int min, int val, int max)
{
	return MIN_MAX_(min, val, max);
}

// End of myminmax.c
