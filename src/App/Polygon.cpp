#include "Polygon.h"

CPolygon::CPolygon(QPoint start)
{
	m_Polygon = new QPolygon();
	m_Polygon->append(start);
}

CPolygon::~CPolygon()
{
}

void CPolygon::Draw(QPainter& painter)
{
	QPolygon t_Polygon = *m_Polygon;
	t_Polygon.append(end);
	painter.drawPolygon(t_Polygon);
}

ScannerRegion CPolygon::Get_Inside() 
{
	LineScanner Scanner;
	for (auto p : *m_Polygon)
	{
		Scanner.add_point(p);
	}
	return Scanner.Scanning();
}

void CPolygon::addPoint()
{
	m_Polygon->append(end);
	//printf("%d %d\n", end.rx(), end.ry());
}

void CPolygon::set_start(QPoint s)
{
	start = s;
}

void CPolygon::set_end(QPoint e)
{
	end = e;
}

XET::XET()
{
	next = NULL;
}
XET::XET(QPoint s, QPoint e)
{
	next = NULL;
	dx = 1.0 * (s.rx() - e.rx()) / (s.ry() - e.ry());
	ymax = e.ry();
	x = s.rx();
}
bool XET::operator<(XET b) {
	return fabs(x - b.x) < 1e-9 ? dx < b.dx : x < b.x;
}
void XET::Move()
{
	x += dx;
}
void XET::Insert(XET Edge)
{
	XET* p = this;
	for (; p->next != NULL; p = p->next) {
		if (Edge < *p->next)
			break;
	}
	XET* nx = p->next;
	p->next = new XET();
	*p->next = Edge;
	p->next->next = nx;
	//printf("EDGEX:%.2lf\n", Edge.x);
	//printf("EDGELX:%.2lf\n", p->x);
}
bool XET::CheckNext(int current_y) {
	while (next != NULL && next->ymax <= current_y)
	{
		next = next->next;
	}
	return next != NULL;
}
LineScanner::LineScanner()
{
	m_miny = 1024; m_maxy = 0;
	m_Polygon.clear();
}
LineScanner::~LineScanner()
{

}
void LineScanner::add_point(QPoint point)
{
	m_Polygon.append(point);
}

void LineScanner::Insert_Edge(QPoint s, QPoint e)
{
	if (s.ry() == e.ry())
		return;
	if (s.ry() > e.ry()) std::swap(s, e);
	NET*& head = pnet[s.ry()];
	NET* nx = head->next;
	head->next = new NET(s, e);
	head->next->next = nx;
}
void LineScanner::Get_NET()
{
	for (auto p : m_Polygon) {
		m_miny = std::min(m_miny, p.ry());
		m_maxy = std::max(m_maxy, p.ry());
	}
	for (int i = m_miny; i <= m_maxy; ++i)
		pnet[i] = new NET();
	int n = m_Polygon.size();
	assert(n >= 2);
	for (int i = 0; i < n - 1; ++i)
	{
		Insert_Edge(m_Polygon[i], m_Polygon[i + 1]);
	}
	Insert_Edge(m_Polygon[0], m_Polygon[n - 1]);
}
ScannerRegion LineScanner::Scanning() {
	Get_NET();
	ScannerRegion pointlist;
	AET* paet = new AET();
	for (int current_y = m_miny; current_y <= m_maxy; ++current_y) {
		for (AET* p = paet; p->next != NULL; p = p->next)
		{
			if (!p->CheckNext(current_y)) break;
			p->next->Move();
		}

		for (NET* p = pnet[current_y]; p->next != NULL; p = p->next)
		{
			paet->Insert(*p->next);
		}
		/*for (AET* p = paet; p->next != NULL; p = p->next)
		{
			printf("%d %.6lf ", current_y, p->next->x);
		}
		puts("");*/
		for (AET* p = paet->next; p != NULL; p = p->next->next) {
			assert(p->next != NULL);
			assert(p->x <= p->next->x);
			for (int i = p->x; i <= p->next->x; ++i)
				pointlist.push_back(QPoint(i, current_y));
		}
	}
	return pointlist;
}
