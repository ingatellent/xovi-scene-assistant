#include "rm_Line.hpp"

void Line::log(const Line& line) {
	printf("Line log - tool: %d, color: %d, rgba: %08X, pointCount: %zu, maskScale: %f, thickness: %f\n",
			line.tool, line.color, line.rgba, (size_t)line.points.size(), line.maskScale, line.thickness);
	printf("Line log - bounds: (%.2f, %.2f, %.2f, %.2f)\n", line.bounds.x(), line.bounds.y(), line.bounds.width(), line.bounds.height());

	printf("Line points:\n");
	for (qsizetype i = 0; i < line.points.size(); i++) {
		const LinePoint &point = line.points[i];
		printf("  Point %zd: (x: %f, y: %f, speed: %u, width: %u, direction: %u, pressure: %u)\n",
				(size_t)i, point.x, point.y, point.speed, point.width, point.direction, point.pressure);
	}
}

Line Line::selectionLineFromPoints(QList<LinePoint> &&points, const QRectF& bounds) {
	Line line = {};
	// Selection tool i 0x0b, but 0x13 works fine
	// line.tool = 0x0b; 
	line.tool = 0x13; 
	line.color = 0; // Black
	line.rgba = 0xff000000;
	line.points = QList(points.begin(), points.end());
	line.maskScale = 1.0;
	line.thickness = 0.0f;
	line.bounds = bounds;
	return line;
}

Line Line::fromPoints(QList<LinePoint> &&points, const QRectF& bounds) {
	Line line = {};
	line.tool = 0x13; // SolidPen
	line.color = 0; // Black
	line.rgba = 0xff000000;
	line.points = QList(points.begin(), points.end());
	line.maskScale = 1.0;
	line.thickness = 0.0f;
	line.bounds = bounds;
	return line;
}


Line Line::fromPoints(std::span<const LinePoint> _points, const QRectF& bounds) {
	auto points = QList<LinePoint>(_points.begin(), _points.end());
	return fromPoints(std::move(points), bounds);
}

