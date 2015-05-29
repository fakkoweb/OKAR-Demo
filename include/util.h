#ifndef UTIL_H
#define UTIL_H

//Template function to return 1 or -1 depending on input sign
template <typename T> int sgn(T val) {
	return (T(0) <= val) - (val < T(0));
}

template <typename T> int abs(T val) {
	return val*sgn(val);
}

#endif