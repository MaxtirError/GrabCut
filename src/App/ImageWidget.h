#pragma once
#include <QWidget>
#include "Polygon.h"
#include "GrabCut.h"
QT_BEGIN_NAMESPACE
class QImage;
class QPainter;
QT_END_NAMESPACE

class ImageWidget :
	public QWidget
{
	Q_OBJECT

public:
	ImageWidget(void);
	~ImageWidget(void);

protected:
	void paintEvent(QPaintEvent *paintevent);

public slots:
	// File IO
	void Open();												// Open an image file, support ".bmp, .png, .jpg" format
	void Save();												// Save image to current file
	void SaveAs();												// Save image to another file

	// Image processing
	void Invert();												// Invert pixel value in image
	void Mirror(bool horizontal=false, bool vertical=true);		// Mirror image vertically or horizontally
	void TurnGray();											// Turn image to gray-scale map
	void Restore();												// Restore image to origin
	void ChoosePolygon();
	void ChooseRect();
	void Cut();

public:
	enum ChooseType {
		kNone,
		kRect,
		kPolygon
	};
	void mousePressEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
	void mouseDoubleClickEvent(QMouseEvent* event);

private:
	QImage		*ptr_image_;				// image 
	QImage		*ptr_image_backup_;
	bool draw_status;
	QPoint start_point_;
	QPoint end_point_;
	QPoint image_start_position;
	ChooseType ChooseStatus;
	CPolygon *m_Polygon;
	GrabCut *Cut_Tool;
	Vector3d** Image_RGB;
	int image_width, image_height;
	bool** mask;
};

