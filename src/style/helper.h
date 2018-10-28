#ifndef HELPER_H
#define HELPER_H
#include <algorithm>

constexpr unsigned int transparentize(unsigned int color, double percentage)
{
	unsigned int alpha = static_cast<unsigned int>(255.0 * percentage / 100)
	                     << 24;
	return (color & 0x00FFFFFF) | alpha;
}
constexpr unsigned int lighter(unsigned int color, double percentage)
{
	double f = 1.0 + percentage / 100;

	int r = std::min(
	    static_cast<int>((static_cast<double>((color >> 16) & 0xFF) * f)), 255);
	int g = std::min(
	    static_cast<int>((static_cast<double>((color >> 8) & 0xFF) * f)), 255);
	int b =
	    std::min(static_cast<int>(static_cast<double>(color & 0xFF) * f), 255);
	return static_cast<unsigned int>((r << 16) + (g << 8) + b);
}

constexpr unsigned int darker(unsigned int color, double percentage)
{
	double f = 1.0 - percentage / 100;
	int r = std::max(
	    static_cast<int>((static_cast<double>((color >> 16) & 0xFF) * f)), 0);
	int g = std::max(
	    static_cast<int>((static_cast<double>((color >> 8) & 0xFF) * f)), 0);
	int b =
	    std::max(static_cast<int>(static_cast<double>(color & 0xFF) * f), 0);
	return static_cast<unsigned int>((r << 16) + (g << 8) + b);
}
#endif  // HELPER_H
