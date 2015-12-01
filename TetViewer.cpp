#include "TetViewer.h"
#include "MainWindow.h"
#include <queue>
#include <random>

TetViewer::TetViewer(QWidget *_parent) : QGLWidget(_parent)
{
	mesh = new CVTMesh();
	isSelectionMode = false;
	isSelectionCutFaceMode = false;
	cutDistance = 0.0;
	cutPlane = CPlane(CPoint(0.0, 0.0, 1.0), cutDistance);
	init();
	isTextureLoaded = false;
	isLightOn = true;

	isRotationViewOn = true;

	isFiber = false;
	isVASP = false;
	fiberMinLength = 0;
}


TetViewer::~TetViewer()
{
}

void TetViewer::init()
{
	setAttribute(Qt::WA_NoSystemBackground, true);
	setFocusPolicy(Qt::StrongFocus);
	center = CPoint(0.0, 0.0, 0.0);
	radius = 0.0;
	trackballRadius = 0.6;
	isMeshLoaded = false;
	meshDrawMode = DRAW_MODE::NONE;
}

QSize TetViewer::sizeHint() const
{
	QRect rectangle = QApplication::desktop()->screenGeometry();
	return QSize(int(rectangle.width()*0.96), int(rectangle.height()));
	std::cout << "sizeHint" << std::endl;
}

void TetViewer::resizeGL(int width, int height)
{
	std::cout << "resizing...." << std::endl;
	std::cout << "width " << width << "height " << height << std::endl;
	glViewport(0, 0, width, height);
	glGetIntegerv(GL_VIEWPORT, viewPort);
	updateProjectionMatrix();
	updateGL();
}

void TetViewer::initializeGL()
{
	// get the projection and modelview matrix
	glGetDoublev(GL_PROJECTION_MATRIX, matProjection);
	glGetDoublev(GL_MODELVIEW_MATRIX, matModelView);
	glGetIntegerv(GL_VIEWPORT, viewPort);

	GLfloat lightOneColor[] = { 1, 1, 1, 1 };
	GLfloat globalAmb[] = { .1f, .1f, .1f, 1.0f };
	GLfloat lightOnePosition[] = { .0, .0, 1, 0.0 };

	GLfloat lightTwoColor[] = { 1, 1, 1, 1 };
	GLfloat lightTwoPosition[] = { .0, 1.0, .0, 0.0 };

	GLfloat lightThreeColor[] = { 1, 1, 1, 1 };
	GLfloat lightThreePosition[] = { 1.0, .0, .0, 0.0 };

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.3f, 0.3f, 0.8f, 0.3f);
	glShadeModel(GL_SMOOTH);

	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);
	glEnable(GL_LIGHT3);
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_POINT_SMOOTH);

	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightOneColor);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, lightTwoColor);
	glLightfv(GL_LIGHT3, GL_DIFFUSE, lightThreeColor);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmb);

	glLightfv(GL_LIGHT1, GL_POSITION, lightOnePosition);
	glLightfv(GL_LIGHT2, GL_POSITION, lightTwoPosition);
	glLightfv(GL_LIGHT3, GL_POSITION, lightThreePosition);

	setScene(center, 1.0);
}

void TetViewer::setScene(CPoint scenePosCenter, GLdouble sceneRadius)
{
	center = scenePosCenter;
	radius = sceneRadius;

	updateProjectionMatrix();
	makeWholeSceneVisible();
}

void TetViewer::setDrawMode(DRAW_MODE drawMode)
{
	meshDrawMode = drawMode;
	updateGL();
}

void TetViewer::updateProjectionMatrix()
{
	makeCurrent();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	std::cout << "width " << width() << " height " << height() << std::endl;
	gluPerspective(fovy(), (GLdouble)width() / (GLdouble)height(), 0.01 * radius, 100.0 * radius);
	glGetDoublev(GL_PROJECTION_MATRIX, matProjection);
	glGetDoublev(GL_MODELVIEW_MATRIX, matModelView);
}

void TetViewer::makeWholeSceneVisible()
{
	// update scene model view matrix
	GLdouble translation0 = matModelView[0] * center[0] + matModelView[4] * center[1] + matModelView[8] * center[2] + matModelView[12];
	GLdouble translation1 = matModelView[1] * center[0] + matModelView[5] * center[1] + matModelView[9] * center[2] + matModelView[13];
	GLdouble translation2 = matModelView[2] * center[0] + matModelView[6] * center[1] + matModelView[10] * center[2] + matModelView[14] + 3.0*radius;
	CPoint transVector = CPoint(-translation0, -translation1, -translation2);
	// change matrix and translation
	makeCurrent();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslated(transVector[0], transVector[1], transVector[2]);
	glMultMatrixd(matModelView);
	glGetDoublev(GL_MODELVIEW_MATRIX, matModelView);
}

void TetViewer::paintGL()
{
	std::cout << "paintGL" << std::endl;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();

	if (isVASP)
	{
		//glDisable(GL_LIGHTING);
		drawAtom();
	}
	else if (isFiber)
	{
		glDisable(GL_LIGHTING);
		drawFiber();
	}
	else
	{
		if (isLightOn)
		{
			glEnable(GL_LIGHTING);
		}
		else
		{
			glDisable(GL_LIGHTING);
		}
		drawMesh();
	}

	glPopMatrix();
}

void TetViewer::drawHalfFaces(std::vector<CViewerHalfFace*> & HalfFaces)
{
	glBindTexture(GL_TEXTURE_2D, texName);
	glBegin(GL_TRIANGLES);
	for (std::vector<CViewerHalfFace*>::iterator hfIter = HalfFaces.begin(); hfIter != HalfFaces.end(); hfIter++)
	{

		CViewerHalfFace * pHF = *hfIter;
		CViewerFace * pF = mesh->HalfFaceFace(pHF);

		if (pF->selected())
		{
			glColor3f(0.0, 0.5, 1.0);
		}
		else
		{
			glColor3f(1.0, 0.5, 0.0);
		}

		for (CVTMesh::HalfFaceVertexIterator fvIter(mesh, pHF); !fvIter.end(); ++fvIter)
		{
			CViewerVertex * v = *fvIter;
			CPoint pt = v->position();
			CPoint n = pHF->normal();
			CPoint2 uv = v->uv();
			glNormal3d(n[0], n[1], n[2]);
			glTexCoord2d(uv[0], uv[1]);
			glVertex3d(pt[0], pt[1], pt[2]);
		}
	}
	glEnd();
}

void TetViewer::drawMeshPoints()
{
	for (CVTMesh::MeshVertexIterator vIter(mesh); !vIter.end(); vIter++)
	{
		CViewerVertex * pV = *vIter;
		CPoint pos = pV->position();
		glPointSize(6);
		if (pV->selected())
		{
			glColor3d(1.0, 0.2, 0.1);
		}
		else
		{
			glColor3d(1.0, 0.5, 0.0);
		}
		glBegin(GL_POINTS);
		glVertex3d(pos[0], pos[1], pos[2]);
		glEnd();
	}
}

void TetViewer::drawAllFaces()
{
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(matProjection);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(matModelView);

	glBindTexture(GL_TEXTURE_2D, texName);
	glBegin(GL_TRIANGLES);

	for (CVTMesh::MeshFaceIterator fIter(mesh); !fIter.end(); fIter++)
	{
		CViewerFace * pF = *fIter;

		if (pF->selected())
		{
			glColor3f(0.0, 0.5, 1.0);
		}
		else
		{
			glColor3f(1.0, 0.5, 0.0);
		}

		for (CVTMesh::FaceVertexIterator fvIter(mesh, pF); !fvIter.end(); ++fvIter)
		{
			CViewerVertex * v = *fvIter;
			CPoint pt = v->position();
			//CPoint n = pHF->normal();
			CPoint2 uv = v->uv();
			//glNormal3d(n[0], n[1], n[2]);
			glTexCoord2d(uv[0], uv[1]);
			glVertex3d(pt[0], pt[1], pt[2]);
		}
	}

	glEnd();
}

void TetViewer::drawAtom()
{
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(matProjection);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(matModelView);

	double radius = 0.4;

	glBegin(GL_POINTS);
	glColor3d(1.0, 0.5, 0.0);
	for (std::vector<CPoint>::iterator pIter = vaspVertexList.begin(); pIter != vaspVertexList.end(); pIter++)
	{
		CPoint p = *pIter;
		drawSphere(p, radius);
		//glVertex3f(p[0], p[1], p[2]);
	}

	glEnd();
}

void TetViewer::drawSphere(CPoint center, double radius)
{
	GLUquadric *quad;
	quad = gluNewQuadric();
	glTranslated(center[0], center[1], center[2]);
	gluSphere(quad, radius, 20, 10);
	glTranslated(-center[0], -center[1], -center[2]);
}

void TetViewer::drawFiber()
{
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(matProjection);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(matModelView);

	std::list<CFiber*> fibers = mesh->fibers();

	std::uniform_real_distribution<double> unif(0.0, 1.0);
	std::default_random_engine dre;

	for (std::list<CFiber*>::iterator fIter = fibers.begin(); fIter != fibers.end(); fIter++)
	{
		CFiber * pFiber = *fIter;
		std::list<CPoint> pointList = pFiber->points();
		if (pointList.size() < fiberMinLength)
		{
			continue;
		}

		//if (pFiber->closed())
		//{
		//	continue;
		//}

		double r = unif(dre);
		double g = unif(dre);
		double b = unif(dre);
		glColor3f(r, g, b);
		glBegin(GL_LINE_STRIP);
		for (std::list<CPoint>::iterator pIter = pointList.begin(); pIter != pointList.end(); pIter++)
		{
			CPoint p = *pIter;
			glVertex3f(p[0], p[1], p[2]);
		}
		glEnd();
	}
}

void TetViewer::drawTet(int id)
{
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(matProjection);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(matModelView);

	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_TRIANGLES);

	for (CVTMesh::MeshTetIterator tIter(mesh); !tIter.end(); tIter++)
	{
		CViewerTet * pT = *tIter;
		if (pT->id() == id)
		{
			for (CVTMesh::TetHFIterator thfIter(mesh, pT); !thfIter.end(); thfIter++)
			{
				CViewerHalfFace *pHF = *thfIter;
				for (CVTMesh::HalfFaceVertexIterator hfvIter(mesh, pHF); !hfvIter.end(); hfvIter++)
				{
					CViewerVertex * pV = *hfvIter;
					CPoint p = pV->position();
					glVertex3d(p[0], p[1], p[2]);
				}
			}
		}
	}

	glEnd();
}

void TetViewer::drawVector()
{

	drawTet(1983);

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(matProjection);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(matModelView);


	glBegin(GL_LINES);

	for (CVTMesh::MeshTetIterator tIter(mesh); !tIter.end(); tIter++)
	{
		CViewerTet * pT = *tIter;

		CPoint vectorTemp = pT->vector();
		if (vectorTemp.norm() < 1.0)
		{
			continue;
		}
		std::cout << pT->id() << std::endl;
		glColor3f(1.0, 0, 0);
		CPoint tetCentral(0.0, 0.0, 0.0);
		for (int j = 0; j < 4; j++)
		{
			CViewerVertex * pV = mesh->TetVertex(pT, j);
			tetCentral = tetCentral + 0.25 * pV->position();
		}
		//tetCentral = mesh->idVertex(5)->position();
		glVertex3f(tetCentral[0], tetCentral[1], tetCentral[2]);

		CPoint vector = pT->vector();
		CPoint endPoint = tetCentral + 0.3 * vector;
		glVertex3f(endPoint[0], endPoint[1], endPoint[2]);
	}

	glEnd();
}

void TetViewer::drawMesh()
{
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(matProjection);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(matModelView);

	switch (meshDrawMode)
	{
	case DRAW_MODE::NONE:
		break;

	case DRAW_MODE::POINTS:
		drawMeshPoints();
		break;

	case DRAW_MODE::TOTALWIRE:
		glDisable(GL_TEXTURE_2D);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		drawAllFaces();
		drawSelectedVertex();
		break;

	case DRAW_MODE::WIREFRAME:
		glDisable(GL_TEXTURE_2D);
		glPolygonMode(GL_FRONT, GL_LINE);
		drawHalfFaces(mesh->m_pHFaces_Below);
		drawSelectedVertex();
		break;

	case DRAW_MODE::FLATLINES:
		glDisable(GL_TEXTURE_2D);
		glPolygonMode(GL_FRONT, GL_LINE);
		drawHalfFaces(mesh->m_pHFaces_Above);
		glPolygonMode(GL_FRONT, GL_FILL);
		drawHalfFaces(mesh->m_pHFaces_Below);
		drawSelectedVertex();
		break;

	case DRAW_MODE::FLAT:
		glDisable(GL_TEXTURE_2D);
		glPolygonMode(GL_FRONT, GL_FILL);
		drawHalfFaces(mesh->m_pHFaces_Below);
		//drawBoundaryHalfFaces();
		drawSelectedVertex();
		break;

	case DRAW_MODE::BOUNDARY:
		glDisable(GL_TEXTURE_2D);
		glPolygonMode(GL_FRONT, GL_FILL);
		drawBoundaryHalfFaces();
		break;

	case DRAW_MODE::TEXTURE:
		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		drawHalfFaces(mesh->m_pHFaces_Below);
		break;

	case DRAW_MODE::TEXTUREMODULATE:
		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		drawHalfFaces(mesh->m_pHFaces_Below);
		break;

	case DRAW_MODE::VECTOR:
		glDisable(GL_TEXTURE_2D);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//drawAllFaces();
		drawVector();
		drawMeshPoints();
		break;

	default:
		break;
	}
}

void TetViewer::loadTextMesh()
{
	if (!isTextureLoaded)
	{
		openTexture();
	}

	initTexture();
}

void TetViewer::initTexture()
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &texName);
	glBindTexture(GL_TEXTURE_2D, texName);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	int imageWidth = texture->GetNumCols();
	int imageHeight = texture->GetNumRows();

	GLubyte * ptr = (GLubyte *)texture->ImageData();

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
		imageWidth,
		imageHeight,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		ptr);

	// may be improved later to specify different mode GL_MODULATE
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glEnable(GL_TEXTURE_2D);
}

void TetViewer::drawSelectedVertex()
{
	for (CVTMesh::MeshVertexIterator vIter(mesh); !vIter.end(); vIter++)
	{
		CViewerVertex * pV = *vIter;
		if (pV->selected())
		{
			CPoint pt = pV->position();
			glPointSize(6);
			glColor3f(0.0, 0.5, 1.0);
			glBegin(GL_POINTS);
			glVertex3d(pt[0], pt[1], pt[2]);
			glEnd();
		}
	}
}

void TetViewer::drawBoundaryHalfFaces()
{
	std::vector<CViewerHalfFace *> halffaceList = mesh->m_pHFaces_Below;
	glBegin(GL_TRIANGLES);
	for (std::vector<CViewerHalfFace*>::iterator iter = halffaceList.begin(); iter != halffaceList.end(); iter++)
	{
		CViewerHalfFace * pHF = *iter;
		CViewerHalfFace * pHD = mesh->HalfFaceDual(pHF);
		if (pHD == NULL)
		{
			glColor3f(1.0, 0.0, 0.0);
			for (CVTMesh::HalfFaceVertexIterator fvIter(mesh, pHF); !fvIter.end(); ++fvIter)
			{
				CViewerVertex * v = *fvIter;
				CPoint pt = v->position();
				CPoint n = pHF->normal();
				CPoint2 uv = v->uv();
				glNormal3d(n[0], n[1], n[2]);
				glVertex3d(pt[0], pt[1], pt[2]);
			}
		}
	}
	glEnd();
}

void TetViewer::mousePressEvent(QMouseEvent * mouseEvent)
{
	mouseButton = mouseEvent->button();
	latestMousePos = mouseEvent->pos();
	isLatestMouseOK = arcball(latestMousePos, latestMouse3DPos);

	// debug output
	if (mouseButton == Qt::LeftButton)
	{
		std::cout << "Mouse Left Press..." << std::endl;
		if (isSelectionMode)
		{
			selectCutFace(latestMousePos);
		}

		if (isSelectionCutFaceMode)
		{
			selectAllCutFaces(latestMousePos);
		}

	}
	else if (mouseButton == Qt::RightButton)
	{
		std::cout << "Mouse Right Press..." << std::endl;
	}
}

void TetViewer::selectCutFace(QPoint newPos)
{

	CPoint newNear, newFar;
	CPoint newRay = getRayVector(newPos, newNear, newFar);

	double minAngle = std::numeric_limits<double>::max();
	CViewerVertex * minVertex;

	std::vector<CViewerFace*> cutFacesList = mesh->_getCutFaces();

	for (std::vector<CViewerFace*>::iterator cFIter = cutFacesList.begin(); cFIter != cutFacesList.end(); cFIter++)
	{
		CViewerFace * currCF = *cFIter;
		for (CVTMesh::FaceVertexIterator fvIter(mesh, currCF); !fvIter.end(); fvIter++)
		{
			CViewerVertex * pV = *fvIter;
			CPoint pT = pV->position();
			CPoint vertVecFromStart = pT - newNear;

			double dotProduct = vertVecFromStart * newRay;
			double angle = dotProduct / (vertVecFromStart.norm() * newRay.norm());
			// std::cout << angle << std::endl;
			// find the minimum
			if (abs(acos(angle)) - 0.0 < minAngle)
			{
				minAngle = abs(acos(angle));
				minVertex = pV;
			}
		}
	}
	if (minVertex->selected())
	{
		minVertex->selected() = false;
	}
	else
	{
		minVertex->selected() = true;
	}

	mesh->_updateSelectedFaces();

	updateGL();
}

void TetViewer::selectAllCutFaces(QPoint newPos)
{
	CPoint newNear, newFar;
	CPoint newRay = getRayVector(newPos, newNear, newFar);

	double minAngle = std::numeric_limits<double>::max();
	CViewerVertex * minVertex;

	std::vector<CViewerFace*> cutFacesList = mesh->_getCutFaces();

	for (std::vector<CViewerFace*>::iterator cFIter = cutFacesList.begin(); cFIter != cutFacesList.end(); cFIter++)
	{
		CViewerFace * currCF = *cFIter;
		for (CVTMesh::FaceVertexIterator fvIter(mesh, currCF); !fvIter.end(); fvIter++)
		{
			CViewerVertex * pV = *fvIter;
			CPoint pT = pV->position();
			CPoint vertVecFromStart = pT - newNear;

			double dotProduct = vertVecFromStart * newRay;
			double angle = dotProduct / (vertVecFromStart.norm() * newRay.norm());
			// std::cout << angle << std::endl;
			// find the minimum
			if (abs(acos(angle)) - 0.0 < minAngle)
			{
				minAngle = abs(acos(angle));
				minVertex = pV;
			}
		}
	}

	minVertex->selected() = true;

	std::queue<CViewerVertex*> queue;
	queue.push(minVertex);

	while (!queue.empty())
	{
		CViewerVertex * currV = queue.front();
		queue.pop();

		for (CVTMesh::VertexVertexIterator vvIter(mesh, currV); !vvIter.end(); vvIter++)
		{
			CViewerVertex * pV = *vvIter;
			if (pV->cut())
			{
				if (pV->selected())
				{
					continue;
				}
				pV->selected() = true;
				queue.push(pV);
			}
		}
	}

	mesh->_updateSelectedFaces();

	updateGL();
}

CPoint TetViewer::getRayVector(QPoint point, CPoint & nearPt, CPoint & farPt)
{
	GLdouble mousePosX = point.x();
	GLdouble mousePosY = height() - point.y();

	GLdouble objNearX, objNearY, objNearZ;
	GLdouble objFarX, objFarY, objFarZ;

	glGetDoublev(GL_MODELVIEW_MATRIX, matModelView);
	glGetDoublev(GL_PROJECTION_MATRIX, matProjection);
	glGetIntegerv(GL_VIEWPORT, viewPort);

	gluUnProject(mousePosX, mousePosY, 0.0, matModelView, matProjection, viewPort, &objNearX, &objNearY, &objNearZ);
	gluUnProject(mousePosX, mousePosY, 1.0, matModelView, matProjection, viewPort, &objFarX, &objFarY, &objFarZ);

	nearPt[0] = objNearX; nearPt[1] = objNearY; nearPt[2] = objNearZ;
	farPt[0] = objFarX; farPt[1] = objFarY; farPt[2] = objFarZ;

	CPoint rayVector = farPt - nearPt;

	return rayVector;
}

void TetViewer::mouseMoveEvent(QMouseEvent * mouseEvent)
{
	QPoint newMousePos = mouseEvent->pos();
	// enable OpenGL Context
	makeCurrent();
	if (isLatestMouseOK)
	{
		switch (mouseButton)
		{
		case Qt::LeftButton:
			// debug output
			std::cout << "Mouse Left Moving..." << std::endl;
			// rotate the view
			rotationView(newMousePos);
			break;
		case Qt::RightButton:
			std::cout << "Mouse Right Moving..." << std::endl;
			// translate the view
			translateView(newMousePos);
			break;
		default:
			break;
		}
	}

	latestMousePos = newMousePos;
	//std::cout << "starting debugging ..." << std::endl;
	isLatestMouseOK = arcball(latestMousePos, latestMouse3DPos);
	// update OpenGL, trigger re-draw
	updateGL();
}

void TetViewer::mouseReleaseEvent(QMouseEvent * /*mouseEvent*/)
{
	mouseButton = Qt::NoButton;
	isLatestMouseOK = false;
	std::cout << "Mouse Release..." << std::endl;
}

void TetViewer::wheelEvent(QWheelEvent * mouseEvent)
{
	// scroll the wheel to scale the view port
	double moveAmount = -(double)mouseEvent->delta() / (120.0*8.0);
	translate(CPoint(0.0, 0.0, moveAmount));
	updateGL();
	mouseEvent->accept();
}

bool TetViewer::arcball(QPoint screenPos, CPoint &new3Dpos)
{
	std::cout << "arcball" << std::endl;
	double x = (2.0 * screenPos.x() - width()) / (double)width();
	double y = -(2.0 * screenPos.y() - height()) / (double)height();
	double norm = x * x + y * y;

	double trackballRSqr = trackballRadius * trackballRadius;
	CPoint modelPostion;
	modelPostion[0] = x;
	modelPostion[1] = y;
	if (norm < 0.5 * trackballRSqr)
	{
		modelPostion[2] = sqrt(trackballRSqr - norm);
	}
	else
	{
		modelPostion[2] = 0.5 * trackballRSqr / sqrt(norm);
	}
	new3Dpos[0] = modelPostion[0];
	new3Dpos[1] = modelPostion[1];
	new3Dpos[2] = modelPostion[2];

	return true;
}

void TetViewer::rotationView(QPoint newPos)
{
	std::cout << "rotationView" << std::endl;
	CPoint new3DPos;
	bool isNewPosHitArcball = arcball(newPos, new3DPos);
	if (isNewPosHitArcball)
	{
		// because we always rotate centered by the zero point
		// so the axis is simply the cross product of the new mouse 3D postion and the old mouse 3D position
		CPoint rotationAxis = latestMouse3DPos ^ new3DPos;

		if (rotationAxis.norm() < 1e-7) // too small move
		{
			rotationAxis = CPoint(1.0, 0.0, 0.0);
		}
		else
		{
			// normalize
			rotationAxis = rotationAxis / rotationAxis.norm();
		}

		// compute the rotation angle
		// sin law
		CPoint diff = latestMouse3DPos - new3DPos;
		double t = 0.5 * diff.norm() / trackballRadius;
		if (t < -1.0)
		{
			t = -1.0;
		}
		else if (t > 1.0)
		{
			t = 1.0;
		}
		// rotation angle
		double rotatioinAngle = 2.0 * asin(t) * 180 / PI;
		if (isRotationViewOn)
		{
			rotate(rotationAxis, rotatioinAngle);
		}
		else if (!isRotationViewOn && isVASP)
		{
			double R[3][3];
			R[0][0] = cos(rotatioinAngle) + rotationAxis[0] * rotationAxis[0] * (1 - cos(rotatioinAngle));
			R[0][1] = rotationAxis[0] * rotationAxis[1] * (1 - cos(rotatioinAngle)) - rotationAxis[2] * sin(rotatioinAngle);
			R[0][2] = rotationAxis[0] * rotationAxis[2] * (1 - cos(rotatioinAngle)) + rotationAxis[1] * sin(rotatioinAngle);

			R[1][0] = rotationAxis[1] * rotationAxis[0] * (1 - cos(rotatioinAngle)) + rotationAxis[2] * sin(rotatioinAngle);
			R[1][1] = cos(rotatioinAngle) + rotationAxis[1] * rotationAxis[1] * (1 - cos(rotatioinAngle));
			R[1][2] = rotationAxis[1] * rotationAxis[2] * (1 - cos(rotatioinAngle)) - rotationAxis[0] * sin(rotatioinAngle);

			R[2][0] = rotationAxis[2] * rotationAxis[0] * (1 - cos(rotatioinAngle)) - rotationAxis[1] * sin(rotatioinAngle);
			R[2][1] = rotationAxis[2] * rotationAxis[1] * (1 - cos(rotatioinAngle)) + rotationAxis[0] * sin(rotatioinAngle);
			R[2][2] = cos(rotatioinAngle) + rotationAxis[2] * rotationAxis[2] * (1 - cos(rotatioinAngle));

			for (size_t i = 0; i < vaspVertexList.size() / 2; i++)
			{
				CPoint p = vaspVertexList[i];
				double px = R[0][0] * p[0] + R[0][1] * p[1] + R[0][2] * p[2];
				double py = R[1][0] * p[0] + R[1][1] * p[1] + R[1][2] * p[2];
				double pz = R[2][0] * p[0] + R[2][1] * p[1] + R[2][2] * p[2];
				CPoint newP(px, py, pz);
				vaspVertexList[i] = newP;
			}
		}
	}
}

void TetViewer::rotate(CPoint axis, double angle)
{
	std::cout << "rotate" << std::endl;
	GLdouble translation0 = matModelView[0] * center[0] + matModelView[4] * center[1] + matModelView[8] * center[2] + matModelView[12];
	GLdouble translation1 = matModelView[1] * center[0] + matModelView[5] * center[1] + matModelView[9] * center[2] + matModelView[13];
	GLdouble translation2 = matModelView[2] * center[0] + matModelView[6] * center[1] + matModelView[10] * center[2] + matModelView[14];
	CPoint transVector = CPoint(translation0, translation1, translation2);

	// modify the modelview matrix
	// first make it at the center of the screen
	makeCurrent();
	glLoadIdentity();
	glTranslated(transVector[0], transVector[1], transVector[2]);
	// then rotate it
	glRotated(angle, axis[0], axis[1], axis[2]);
	// move back to the center
	glTranslated(-transVector[0], -transVector[1], -transVector[2]);
	// update the matrix
	glMultMatrixd(matModelView);
	glGetDoublev(GL_MODELVIEW_MATRIX, matModelView);
}

// translate the view
void TetViewer::translateView(QPoint newPos)
{

	double zVal = -(matModelView[2] * center[0] + matModelView[6] * center[1] + matModelView[10] * center[2] + matModelView[14]) /
		(matModelView[3] * center[0] + matModelView[7] * center[1] + matModelView[11] * center[2] + matModelView[15]);
	double screenAspect = width() / height();
	double top = tan(fovy() / 2.0f * PI / 180.0f) * zNear();
	double right = screenAspect * top;
	QPoint posDiff = latestMousePos - newPos;
	CPoint transVector = CPoint(2.0*posDiff.x() / width() * right / zNear() * zVal,
		-2.0*posDiff.y() / height() * top / zNear() * zVal,
		0.0f);
	if (!isVASP)
	{
		translate(transVector);
	}
	else
	{
		for (size_t i = 0; i < vaspVertexList.size() / 2; i++)
		{
			CPoint p = vaspVertexList[i];
			p = p - transVector;
			vaspVertexList[i] = p;
		}
	}

}

void TetViewer::translate(CPoint transVector)
{

	makeCurrent();
	glLoadIdentity();
	glTranslated(-transVector[0], -transVector[1], -transVector[2]);
	glMultMatrixd(matModelView);
	glGetDoublev(GL_MODELVIEW_MATRIX, matModelView);
}

void TetViewer::loadFile(const char * meshfile, std::string fileExt)
{
	if (isMeshLoaded)
	{
		free(mesh);
		mesh = new CVTMesh();
	}

	if (fileExt == "tet")
	{
		mesh->_load(meshfile);
	}

	if (fileExt == "t")
	{
		mesh->_load_t(meshfile);
	}

	if (fileExt == "f")
	{

		bool ok;
		fiberMinLength = QInputDialog::getInt(this, tr("Input the minimal length of fibers to draw"), tr("Minimal Fiber Length"), 0, 0, INT_MAX, 1, &ok);
		{
			mesh->_load_f(meshfile);
			isFiber = true;
			updateGL();
		}
		return;
	}

	if (fileExt == "vasp")
	{
		std::fstream is(meshfile, std::fstream::in);
		if (is.fail())
		{
			fprintf(stderr, "Error in opening file %s\n", meshfile);
			return;
		}

		isVASP = true;

		std::string line;

		bool flag = false;
		while (!is.eof())
		{
			if (!flag)
			{
				std::getline(is, line);
				line = strutil::trim(line);
				strutil::Tokenizer stokenizer(line, " \r\n");
				stokenizer.nextToken();
				std::string token = stokenizer.getToken();

				if (token != "Cartesian")
				{
					continue;
				}
				flag = true;
			}

			std::getline(is, line);
			line = strutil::trim(line);
			strutil::Tokenizer stokenizer(line, " \r\n\t");
			stokenizer.nextToken();
			CPoint p;
			for (int k = 0; k < 3; k++)
			{
				std::string token = stokenizer.getToken();
				p[k] = strutil::parseString<double>(token);
				stokenizer.nextToken();
			}
			vaspVertexList.push_back(p);
		}

		updateGL();

		return;
	}

	// read in traits
	for (CVTMesh::MeshVertexIterator vIter(mesh); !vIter.end(); vIter++)
	{
		CViewerVertex * pV = *vIter;
		pV->_from_string();
	}

	for (CVTMesh::MeshEdgeIterator eIter(mesh); !eIter.end(); eIter++)
	{
		CViewerEdge * pE = *eIter;
		pE->_from_string();
	}

	for (CVTMesh::MeshTetIterator tIter(mesh); !tIter.end(); tIter++)
	{
		CViewerTet * pT = *tIter;
		pT->_from_string();
	}

	mesh->_normalize();
	mesh->_halfface_normal();

	meshDrawMode = DRAW_MODE::FLAT;

	CPlane p(CPoint(0.0, 0.0, 1.0), 0.0);
	mesh->_cut(p);

	isMeshLoaded = true;

	updateGL();
}

void TetViewer::loadFromMainWin(std::string outFilename, std::string outExt)
{
	sFilename = outFilename;
	filename = QString::fromStdString(outFilename);
	const char * _filename = outFilename.c_str();
	loadFile(_filename, outExt);
}

void TetViewer::openMesh()
{
	filename = QFileDialog::getOpenFileName(this,
		tr("Open Tetrahedra meshes"),
		tr("../models/"),
		tr("TET Files (*.tet);;"
		"T Files (*.t);;"
		"F Files(*.f);;"
		"VASP Files(*.vasp)"));
	QFileInfo * fileInfo = new QFileInfo(filename);
	QString fileExt = fileInfo->suffix();
	QString canonicalFilePath = fileInfo->canonicalFilePath();
	std::string sFileExt = fileExt.toStdString();
	if (!filename.isEmpty())
	{
		QByteArray byteArray = filename.toUtf8();
		const char * _filename = byteArray.constData();
		sFilename = filename.toStdString();
		loadFile(_filename, sFileExt);

		foreach(QWidget *widget, qApp->topLevelWidgets())
		{
			MainWindow * mainWin = qobject_cast<MainWindow*>(widget);
			QString title = "TetViewerQt - " + canonicalFilePath;
			mainWin->setWindowTitle(title);
		}
	}
}

void TetViewer::openTexture()
{
	texture = new RgbImage();
	texFilename = QFileDialog::getOpenFileName(this,
		tr("Open Texture"),
		tr("../models/"),
		tr("BMP Files (*.bmp)"));
	QFileInfo * fileInfo = new QFileInfo(texFilename);
	QString fileExt = fileInfo->suffix();
	std::string sFileExt = fileExt.toStdString();
	if (!texFilename.isEmpty())
	{
		QByteArray byteArray = texFilename.toUtf8();
		const char * _filename = byteArray.constData();
		sFilename = filename.toStdString();
		//loadFile(_filename, sFileExt);
		texture->LoadBmpFile(_filename);
		isTextureLoaded = true;
	}
}

void TetViewer::saveMesh()
{
	QString saveFilename = QFileDialog::getSaveFileName(this,
		tr("Save Mesh File"),
		tr("../models/"),
		tr("TET Files (*.tet);;"
		"T Files (*.t);;"
		"All Files (*.*)"));
	QFileInfo * saveFileInfo = new QFileInfo(saveFilename);
	std::string saveFileExt = saveFileInfo->suffix().toStdString();
	if (!saveFilename.isEmpty())
	{
		QByteArray byteArray = saveFilename.toUtf8();
		const char * _saveFilename = byteArray.constData();
		saveFile(_saveFilename, saveFileExt);
	}
}

void TetViewer::saveFile(const char * meshfile, std::string sExt)
{
	if (sExt == "tet")
	{
		mesh->_write(meshfile);
	}
	else if (sExt == "t")
	{
		mesh->_write_t(meshfile);
	}
}

void TetViewer::enterSelectionMode()
{
	if (!isSelectionMode)
	{
		isSelectionMode = true;
	}
}

void TetViewer::quitSelectionMode()
{
	if (isSelectionMode)
	{
		isSelectionMode = false;
	}
}

void TetViewer::enterSelectionCutFaceMode()
{
	if (!isSelectionCutFaceMode)
	{
		isSelectionCutFaceMode = true;
	}
}

void TetViewer::quitSelectionCutFaceMode()
{
	if (isSelectionCutFaceMode)
	{
		isSelectionCutFaceMode = false;
	}
}

void TetViewer::xCut()
{
	cutPlane = CPlane(CPoint(1, 0, 0), cutDistance);
	mesh->_cut(cutPlane);
	updateGL();
}

void TetViewer::yCut()
{
	cutPlane = CPlane(CPoint(0, 1, 0), cutDistance);
	mesh->_cut(cutPlane);
	updateGL();
}

void TetViewer::zCut()
{
	cutPlane = CPlane(CPoint(0, 0, 1), cutDistance);
	mesh->_cut(cutPlane);
	updateGL();
}

void TetViewer::plusMove()
{
	cutDistance += 0.05;
	cutPlane.d() = cutDistance;
	mesh->_cut(cutPlane);
	updateGL();
}

void TetViewer::minusMove()
{
	cutDistance -= 0.05;
	cutPlane.d() = cutDistance;
	mesh->_cut(cutPlane);
	updateGL();
}

void TetViewer::cutVolume()
{


	std::string cutName = sFilename + "_cut.";
	QFileInfo * fileInfo = new QFileInfo(filename);
	QString fileExt = fileInfo->suffix();
	std::string sFileExt = fileExt.toStdString();
	std::string cutVertexName = cutName + "txt";
	cutName = cutName + sFileExt;
	const char * charFilename = cutName.c_str();
	mesh->_cutVolumeWrite(charFilename, sFileExt);
	//saveFile(charFilename, sFileExt);

	mesh->_write_cut_vertices(cutVertexName.c_str());
}

void TetViewer::clearSelectedVF()
{
	for (CVTMesh::MeshVertexIterator vIter(mesh); !vIter.end(); vIter++)
	{
		CViewerVertex * pV = *vIter;
		pV->selected() = false;
	}

	mesh->_updateSelectedFaces();

	updateGL();
}

void TetViewer::lightOn()
{
	isLightOn = true;
	updateGL();
}

void TetViewer::lightOff()
{
	isLightOn = false;
	updateGL();
}

void TetViewer::rotationViewOn()
{
	isRotationViewOn = true;
}

void TetViewer::rotationViewOff()
{
	isRotationViewOn = false;
}