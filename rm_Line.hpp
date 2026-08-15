#pragma once

#include <QList>
#include <QRectF>
#include <span>

struct LinePoint {
	float x;
	float y;
	unsigned short speed;
	unsigned short width;
	unsigned char direction;
	unsigned char pressure;
} __attribute__((packed));
static_assert(sizeof(LinePoint) == 0xe, "LinePoint size mismatch");

struct Line {
	int tool;
	int color;
	unsigned int rgba;

	QList<LinePoint> points;

	double maskScale;
	float thickness;

	// these bounds are used for early hit testing on erase & select
	QRectF bounds;

	static void log(const Line& line);
	static Line fromPoints(QList<LinePoint> &&points, const QRectF& bounds);
	static Line selectionLineFromPoints(QList<LinePoint> &&points, const QRectF& bounds);
	static Line fromPoints(std::span<const LinePoint> points, const QRectF& bounds);
};
#ifdef __arm__
static_assert(sizeof(Line) == 0x48);
#elifdef __aarch64__
static_assert(sizeof(Line) == 0x58);
#else
#error "Unknown Arch"
#endif

