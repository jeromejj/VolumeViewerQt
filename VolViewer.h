#ifndef VolViewer_H
#define VolViewer_H

#include <QGLWidget>
#include <QApplication>
#include <QDesktopWidget>
#include <QMouseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>

#include <string>

#include "OpenGLHeader.h"
#include "..\MeshLib\core\bmp\RgbImage.h"
#include "ViewerTFiberMesh.h"
#include "ViewerHMesh.h"

#ifndef PI
#define PI 3.14159265
#endif

#ifndef W_EPSILON
#define W_EPSILON 2e-5
#endif

using namespace MeshLib;

enum class DRAW_MODE { NONE, POINTS, WIREFRAME, FLATLINES, FLAT, SMOOTH, TEXTURE, TEXTUREMODULATE, BOUNDARY, VECTOR };

enum class VOLUME_TYPE {TET, HEX, FIBER};

class VolViewer : public QGLWidget
{
	Q_OBJECT 
public:
	VolViewer(QWidget *_parent = 0);
	~VolViewer();

	void loadFile(const char *, std::string sExt);
	void saveFile(const char *, std::string sExt);
	void exportVisibleSurface(const char *, std::string sExt);	//!< export the visible surface of the tet mesh

	void setDrawMode(DRAW_MODE drawMode);

	void setVolType(VOLUME_TYPE volType);

	void loadTextMesh();
	void loadFromMainWin(std::string, std::string);

public slots:

	void newScene();
	void openMesh();
	void openTexture();
	void saveMesh();
	void exportVisibleMesh();	//!< export the visible surface of the tet mesh as a mesh file(such as .m file)
	void screenshot();
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
	void drawMesh(TMeshLib::CVTMesh * tmesh);
	void drawMesh(HMeshLib::CVHMesh * hmesh);

	void drawSphere(CPoint p, double radius);

	void drawFiber(TMeshLib::CVTMesh * fiber);
	
	void drawMeshPoints(TMeshLib::CVTMesh * tmesh);
	void drawMeshPoints(HMeshLib::CVHMesh * hmesh);

	void drawMeshWireframe();
	void drawMeshFlatline();
	void drawMeshFlat();

	void drawSelectedVertex(TMeshLib::CVTMesh * tmesh);
	void drawSelectedVertex(HMeshLib::CVHMesh * hmesh);
	void drawBoundaryHalfFaces(TMeshLib::CVTMesh * tmesh);
	void drawBoundaryHalfFaces(HMeshLib::CVHMesh * hmesh);
	void drawVector();

	void drawHalfFaces(std::vector<TMeshLib::CViewerHalfFace*> & HalfFaces);

	void drawHalfFaces(std::vector<HMeshLib::CHViewerHalfFace*> & HalfFaces);

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

	bool isLightOn;

	bool isRotationViewOn;

	VOLUME_TYPE meshVolType;

	TMeshLib::CVTMesh * mesh;
	HMeshLib::CVHMesh * hmesh;

	std::vector<TMeshLib::CVTMesh*> tmeshlist;
	std::vector<HMeshLib::CVHMesh*> hmeshlist;

	CPoint center;
	GLdouble radius;
	double trackballRadius;
	bool isMeshLoaded;
	DRAW_MODE meshDrawMode;

	QString filename;
	QStringList filenames;
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
};

#endif
