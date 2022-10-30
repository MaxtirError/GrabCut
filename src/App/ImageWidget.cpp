#include "ImageWidget.h"
#include <QImage>
#include <QPainter>
#include <QtWidgets> 
#include <iostream>

using std::cout;
using std::endl;

ImageWidget::ImageWidget(void)
{
	draw_status = false;
	ptr_image_ = new QImage();
	ptr_image_backup_ = new QImage();
	ChooseStatus = kNone;
	m_Polygon = NULL;
}


ImageWidget::~ImageWidget(void)
{
}

void ImageWidget::paintEvent(QPaintEvent *paintevent)
{
	QPainter painter;
	painter.begin(this);

	// Draw background
	painter.setBrush(Qt::lightGray);

	QRect back_rect(0, 0, width(), height());
	painter.drawRect(back_rect);

	// Draw image
	image_start_position.setX((width() - ptr_image_->width()) / 2);
	image_start_position.setY((height() - ptr_image_->height()) / 2);
	QRect rect = QRect( (width()-ptr_image_->width())/2, (height()-ptr_image_->height())/2, ptr_image_->width(), ptr_image_->height());
	painter.drawImage(rect, *ptr_image_);


	// Draw choose region
	painter.setBrush(Qt::NoBrush);
	painter.setPen(Qt::red);
	if (m_Polygon != NULL)
		m_Polygon->Draw(painter);
	else {
		painter.drawRect(start_point_.x(), start_point_.y(),
			end_point_.x() - start_point_.x(), end_point_.y() - start_point_.y());
	}

	painter.end();
}


void ImageWidget::mousePressEvent(QMouseEvent* event)
{
	if (Qt::LeftButton == event->button() && ChooseStatus != kNone)
	{
		start_point_ = end_point_ = event->pos();
		if (ChooseStatus == kPolygon) 
		{
			if (draw_status) 
			{
				assert(m_Polygon != NULL);
				m_Polygon->addPoint();
			}
			else {
				m_Polygon = new CPolygon(event->pos());
				setMouseTracking(true);
			}
		}
		draw_status = true;
	}
	update();
}

void ImageWidget::mouseMoveEvent(QMouseEvent* event)
{
	if (draw_status && ChooseStatus != kNone)
	{
		end_point_ = event->pos();
		if (ChooseStatus == kPolygon && m_Polygon != NULL)
			m_Polygon->set_end(end_point_);
	}
	update();
}

void ImageWidget::mouseReleaseEvent(QMouseEvent* event)
{
	if (ChooseStatus == kRect) {
		ChooseStatus = kNone;
		draw_status = false;
	}
	update();
}


void ImageWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
	if (ChooseStatus == kPolygon)
	{
		ChooseStatus = kNone;
		draw_status = false;
		setMouseTracking(false);
	}
	update();
}

void ImageWidget::Open()
{
	// Open file
	QString fileName = QFileDialog::getOpenFileName(this, tr("Read Image"), ".", tr("Images(*.bmp *.png *.jpg)"));

	// Load file
	if (!fileName.isEmpty())
	{
		ptr_image_->load(fileName);
		*(ptr_image_backup_) = *(ptr_image_);
	}

	//ptr_image_->invertPixels(QImage::InvertRgb);
	//*(ptr_image_) = ptr_image_->mirrored(true, true);
	//*(ptr_image_) = ptr_image_->rgbSwapped();
	cout<<"image size: "<<ptr_image_->width()<<' '<<ptr_image_->height()<<endl;
	update();
}

void ImageWidget::Save()
{
	SaveAs();
}

void ImageWidget::SaveAs()
{
	QString filename = QFileDialog::getSaveFileName(this, tr("Save Image"), ".", tr("Images(*.bmp *.png *.jpg)"));
	if (filename.isNull())
	{
		return;
	}	

	ptr_image_->save(filename);
}

void ImageWidget::Invert()
{
	for (int i=0; i<ptr_image_->width(); i++)
	{
		for (int j=0; j<ptr_image_->height(); j++)
		{
			QRgb color = ptr_image_->pixel(i, j);
			ptr_image_->setPixel(i, j, qRgb(255-qRed(color), 255-qGreen(color), 255-qBlue(color)) );
		}
	}

	// equivalent member function of class QImage
	// ptr_image_->invertPixels(QImage::InvertRgb);
	ChooseStatus = kNone;
	update();

}

void ImageWidget::Mirror(bool ishorizontal, bool isvertical)
{
	QImage image_tmp(*(ptr_image_));
	int width = ptr_image_->width();
	int height = ptr_image_->height();

	if (ishorizontal)
	{
		if (isvertical)
		{
			for (int i=0; i<width; i++)
			{
				for (int j=0; j<height; j++)
				{
					ptr_image_->setPixel(i, j, image_tmp.pixel(width-1-i, height-1-j));
				}
			}
		} 
		else			//仅水平翻转			
		{
			for (int i=0; i<width; i++)
			{
				for (int j=0; j<height; j++)
				{
					ptr_image_->setPixel(i, j, image_tmp.pixel(width-1-i, j));
				}
			}
		}
		
	}
	else
	{
		if (isvertical)		//仅垂直翻转
		{
			for (int i=0; i<width; i++)
			{
				for (int j=0; j<height; j++)
				{
					ptr_image_->setPixel(i, j, image_tmp.pixel(i, height-1-j));
				}
			}
		}
	}

	// equivalent member function of class QImage
	//*(ptr_image_) = ptr_image_->mirrored(true, true);
	ChooseStatus = kNone;
	update();
}

void ImageWidget::TurnGray()
{
	for (int i=0; i<ptr_image_->width(); i++)
	{
		for (int j=0; j<ptr_image_->height(); j++)
		{
			QRgb color = ptr_image_->pixel(i, j);
			int gray_value = (qRed(color)+qGreen(color)+qBlue(color))/3;
			ptr_image_->setPixel(i, j, qRgb(gray_value, gray_value, gray_value) );
		}
	}

	ChooseStatus = kNone;
	update();
}

void ImageWidget::Restore()
{
	*(ptr_image_) = *(ptr_image_backup_);
	ChooseStatus = kNone;
	update();
}

void ImageWidget::ChoosePolygon()
{
	ChooseStatus = kPolygon;
}

void ImageWidget::ChooseRect()
{
	if (m_Polygon != NULL) {
		delete m_Polygon;
		m_Polygon = NULL;
	}
	ChooseStatus = kRect;
}


void ImageWidget::Cut()
{
	if (m_Polygon != NULL)
	{
		auto Tu = m_Polygon->Get_Inside();
		image_width = ptr_image_->width();
		image_height = ptr_image_->height();
		Image_RGB = new Vector3d * [image_width];
		mask = new bool* [image_width];
		for (int i = 0; i < image_width; ++i) {
			Image_RGB[i] = new Vector3d[image_height];
			mask[i] = new bool[image_height] {0};
			for (int j = 0; j < image_height; ++j) {
				auto z = ptr_image_->pixel(i, j);
				Image_RGB[i][j] = Vector3d(GetRValue(z), GetGValue(z), GetBValue(z));
			}
		}
		for (auto x : Tu) {
			x -= image_start_position;
			//printf("%d %d\n", x.x(), x.y());
			if (x.x() >= image_width || x.y() >= image_height)
				continue;
			mask[x.x()][x.y()] = true;
		}
		Cut_Tool = new GrabCut(image_width, image_height, Image_RGB, mask);
		Cut_Tool->Work();
		for (int i = 0; i < image_width; ++i)
			for (int j = 0; j < image_height; ++j)
				if (!Cut_Tool->alpha[i][j])
					ptr_image_->setPixel(i, j, qRgb(0, 0, 0));
	}
}