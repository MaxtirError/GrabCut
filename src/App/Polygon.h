#pragma once
#include <QtGui>
#include<vector>
#include<list>


typedef std::vector<QPoint> ScannerRegion;
typedef class XET {
public:
	XET();
	XET(QPoint s, QPoint e);
	void Move();
	void Insert(XET Edge);
	bool CheckNext(int current_y);
	XET* next;
	double x;
	bool operator < (XET b);
private:
	int ymax;
	double dx;
}AET, NET;

class LineScanner
{
public:
	LineScanner();
	~LineScanner();
	void add_point(QPoint point);
	ScannerRegion Scanning();
private:
	void Get_NET();
	void Insert_Edge(QPoint s, QPoint e);
	int m_miny, m_maxy;
	QPolygon m_Polygon;
	NET* pnet[1024];
};

class CPolygon {
public:
	CPolygon(QPoint start);
	~CPolygon();

	void set_start(QPoint s);
	void set_end(QPoint e);
	void Draw(QPainter& painter);
	ScannerRegion Get_Inside();
	void addPoint();

private:
	QPolygon *m_Polygon;
	QPoint start;
	QPoint end;
};

