#ifndef TETVIEWER_H
#define TETVIEWER_H

#include <QGLWidget>
#include <QApplication>
#include <QDesktopWidget>
#include <QMouseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>

#include <string>

#include "OpenGLHeader.h"
#include "..\TMeshLib\core\bmp\RgbImage.h"
#include "..\TMeshLib\algorithm\viewer\ViewerTMesh.h"

#ifndef PI
#define PI 3.14159265
#endif

#ifndef W_EPSILON
#define W_EPSILON 2e-5
#endif

using namespace TMeshLib;

enum class DRAW_MODE { NONE, POINTS, WIREFRAME, FLATLINES, FLAT, SMOOTH, TEXTURE, TEXTUREMODULATE, BOUNDARY, VECTOR, TOTALWIRE };

class TetViewer : public QGLWidget
{
	Q_OBJECT 
public:
	TetViewer(QWidget *_parent = 0);
	~TetViewer();

	void loadFile(const char *, std::string sExt);
	void saveFile(const char *, std::string sExt);

	void setDrawMode(DRAW_MODE drawMode);

	void loadTextMesh();
	void loadFromMainWin(std::string, std::string);

public slots:
	void openMesh();
	void openTexture();
	void saveMesh();
	void enterSelectionMode();
	void quitSelectionMode();

	void enterSelectionCutFaceMode();
	void quitSelectionCutFaceMode();

	void lightOn();
	void lightOff();

	void rotationViewOn();
	void rotationViewOff();

	void xCut();
	void yCut();
	void zCut();

	void plusMove();
	void minusMove();

	void cutVolume();
	void clearSelectedVF();

private:

	void init();
	void initializeGL();
	void initTexture();

	QSize sizeHint() const;
	void resizeGL(int, int);

	void setScene(CPoint scenePosCenter, GLdouble sceneRadius);
	void updateProjectionMatrix();
	void makeWholeSceneVisible();

	void paintGL();
	void drawMesh();

	void drawAtom();
	void drawSphere(CPoint p, double radius);

	void drawFiber();
	void drawAllFaces();
	void drawMeshPoints();
	void drawMeshWireframe();
	void drawMeshFlatline();
	void drawMeshFlat();
	void drawSelectedVertex();
	void drawBoundaryHalfFaces();
	void drawVector();
	void drawTet(int id);

	void drawHalfFaces(std::vector<CViewerHalfFace*> & HalfFaces);

	float zNear() { return 0.01 * radius; }
	float zFar() { return 100 * radius; }
	float fovy() const { return 45.0f; }

	void mousePressEvent(QMouseEvent * mouseEvent);
	void mouseMoveEvent(QMouseEvent * mouseEvent);
	void mouseReleaseEvent(QMouseEvent * mouseEvent);
	void wheelEvent(QWheelEvent * mouseEvent);

	bool arcball(QPoint screenPos, CPoint &new3Dpos);

	void translateView(QPoint newPos);
	void translate(CPoint transVector);
	void rotationView(QPoint newPos);
	void rotate(CPoint axis, double angle);

	void selectCutFace(QPoint newPos);
	void selectAllCutFaces(QPoint newPos);

	CPoint getRayVector(QPoint point, CPoint & nearPt, CPoint & farPt);

private:

	bool isSelectionMode;
	bool isSelectionCutFaceMode;

	bool isFiber;

	bool isLightOn;

	bool isRotationViewOn;

	CVTMesh * mesh;
	CPoint center;
	GLdouble radius;
	double trackballRadius;
	bool isMeshLoaded;
	DRAW_MODE meshDrawMode;

	QString filename;
	std::string sFilename;

	// texture
	QString texFilename;
	std::string sTexFilename;
	RgbImage * texture;
	GLuint texName;

	GLdouble matModelView[16], matProjection[16];
	GLint viewPort[4];

	int mouseButton;
	QPoint latestMousePos;
	CPoint latestMouse3DPos;
	bool isLatestMouseOK;

	bool isTextureLoaded;

	CPlane cutPlane;
	double cutDistance;

	// fibers with length less than fiberMinLength will not be drawn
	int fiberMinLength;

	std::vector<CPoint> vaspVertexList;

	bool isVASP;
};

#endif
