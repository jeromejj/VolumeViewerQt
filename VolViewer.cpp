#include "VolViewer.h"
#include "MainWindow.h"
#include <queue>
#include <random>

VolViewer::VolViewer(QWidget *_parent) : QGLWidget(_parent)
{
	mesh = new TMeshLib::CVTMesh();
	hmesh = new HMeshLib::CVHMesh();

	isSelectionMode = false;
	isSelectionCutFaceMode = false;
	cutDistance = 0.0;
	cutPlane = CPlane(CPoint(0.0, 0.0, 1.0), cutDistance);
	init();
	isTextureLoaded = false;
	isLightOn = true;

	isRotationViewOn = true;

	fiberMinLength = 0;
}


VolViewer::~VolViewer()
{
}

void VolViewer::init()
{
	setAttribute(Qt::WA_NoSystemBackground, true);
	setFocusPolicy(Qt::StrongFocus);
	center = CPoint(0.0, 0.0, 0.0);
	radius = 0.0;
	trackballRadius = 0.6;
	isMeshLoaded = false;
	meshDrawMode = DRAW_MODE::NONE;
}

QSize VolViewer::sizeHint() const
{
	QRect rectangle = QApplication::desktop()->screenGeometry();
	return QSize(int(rectangle.width()*0.96), int(rectangle.height()));
}

void VolViewer::resizeGL(int width, int height)
{
	glViewport(0, 0, width, height);
	glGetIntegerv(GL_VIEWPORT, viewPort);
	updateProjectionMatrix();
	updateGL();
}

void VolViewer::initializeGL()
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

void VolViewer::setScene(CPoint scenePosCenter, GLdouble sceneRadius)
{
	center = scenePosCenter;
	radius = sceneRadius;

	updateProjectionMatrix();
	makeWholeSceneVisible();
}

void VolViewer::setDrawMode(DRAW_MODE drawMode)
{
	meshDrawMode = drawMode;
	updateGL();
}

void VolViewer::setVolType(VOLUME_TYPE volType)
{
	meshVolType = volType;
	updateGL();
}

void VolViewer::updateProjectionMatrix()
{
	makeCurrent();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	std::cout << "width " << width() << " height " << height() << std::endl;
	gluPerspective(fovy(), (GLdouble)width() / (GLdouble)height(), 0.01 * radius, 100.0 * radius);
	glGetDoublev(GL_PROJECTION_MATRIX, matProjection);
	glGetDoublev(GL_MODELVIEW_MATRIX, matModelView);
}

void VolViewer::makeWholeSceneVisible()
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

void VolViewer::paintGL()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();

	for (std::vector<TMeshLib::CVTMesh*>::iterator tIter = tmeshlist.begin(); tIter != tmeshlist.end(); tIter++)
	{
		TMeshLib::CVTMesh * currentMesh = *tIter;

		if (currentMesh->isFiber())
		{
			glDisable(GL_LIGHTING);
			drawFiber(currentMesh);
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
			drawMesh(currentMesh);
		}
	}

	for (std::vector<HMeshLib::CVHMesh*>::iterator tIter = hmeshlist.begin(); tIter != hmeshlist.end(); tIter++)
	{
		HMeshLib::CVHMesh * currenhmesh = *tIter;

		if (isLightOn)
		{
			glEnable(GL_LIGHTING);
		}
		else
		{
			glDisable(GL_LIGHTING);
		}
		drawMesh(currenhmesh);
	}

	glPopMatrix();
}

void VolViewer::drawHalfFaces(std::vector<TMeshLib::CViewerHalfFace*> & HalfFaces)
{
	glBindTexture(GL_TEXTURE_2D, texName);
	glBegin(GL_TRIANGLES);
	for (std::vector<TMeshLib::CViewerHalfFace*>::iterator hfIter = HalfFaces.begin(); hfIter != HalfFaces.end(); hfIter++)
	{

		TMeshLib::CViewerHalfFace * pHF = *hfIter;
		TMeshLib::CViewerFace * pF = mesh->HalfFaceFace(pHF);

		if (pF->selected())
		{
			glColor3f(0.0, 0.5, 1.0);
		}
		else
		{
			glColor3f(1.0, 0.5, 0.0);
		}

		for (TMeshLib::CVTMesh::HalfFaceVertexIterator fvIter(mesh, pHF); !fvIter.end(); ++fvIter)
		{
			TMeshLib::CViewerVertex * v = *fvIter;
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

void VolViewer::drawHalfFaces(std::vector<HMeshLib::CHViewerHalfFace*> & HalfFaces)
{
	glBindTexture(GL_TEXTURE_2D, texName);

	for (std::vector<HMeshLib::CHViewerHalfFace*>::iterator hfIter = HalfFaces.begin(); hfIter != HalfFaces.end(); hfIter++)
	{
		glBegin(GL_POLYGON);
		HMeshLib::CHViewerHalfFace * pHF = *hfIter;
		HMeshLib::CHViewerFace * pF = hmesh->HalfFaceFace(pHF);

		if (pF->selected())
		{
			glColor3f(0.0, 0.5, 1.0);
		}
		else
		{
			glColor3f(1.0, 0.5, 0.0);
		}

		for (HMeshLib::CVHMesh::HalfFaceVertexIterator fvIter(hmesh, pHF); !fvIter.end(); ++fvIter)
		{
			HMeshLib::CHViewerVertex * v = *fvIter;
			CPoint pt = v->position();
			CPoint n = pHF->normal();
			CPoint2 uv = v->uv();
			glNormal3d(n[0], n[1], n[2]);
			glTexCoord2d(uv[0], uv[1]);
			glVertex3d(pt[0], pt[1], pt[2]);
		}
		glEnd();
	}

}

void VolViewer::drawMeshPoints(TMeshLib::CVTMesh * mesh)
{
	for (TMeshLib::CVTMesh::MeshVertexIterator vIter(mesh); !vIter.end(); vIter++)
	{
		TMeshLib::CViewerVertex * pV = *vIter;
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

void VolViewer::drawMeshPoints(HMeshLib::CVHMesh * hmesh)
{
	for (HMeshLib::CVHMesh::MeshVertexIterator vIter(hmesh); !vIter.end(); vIter++)
	{
		HMeshLib::CHViewerVertex * pV = *vIter;
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

void VolViewer::drawSphere(CPoint center, double radius)
{
	GLUquadric *quad;
	quad = gluNewQuadric();
	glTranslated(center[0], center[1], center[2]);
	gluSphere(quad, radius, 20, 10);
	glTranslated(-center[0], -center[1], -center[2]);
}

void VolViewer::drawFiber(TMeshLib::CVTMesh * mesh)
{
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(matProjection);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(matModelView);

	std::list<TMeshLib::CFiber*> fibers = mesh->fibers();

	std::uniform_real_distribution<double> unif(0.0, 1.0);
	std::default_random_engine dre;

	for (std::list<TMeshLib::CFiber*>::iterator fIter = fibers.begin(); fIter != fibers.end(); fIter++)
	{
		TMeshLib::CFiber * pFiber = *fIter;
		std::list<CPoint> pointList = pFiber->points();
		if (pointList.size() < fiberMinLength)
		{
			continue;
		}

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

void VolViewer::drawVector()
{

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(matProjection);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(matModelView);

	glBegin(GL_LINES);

	for (TMeshLib::CVTMesh::MeshTetIterator tIter(mesh); !tIter.end(); tIter++)
	{
		TMeshLib::CViewerTet * pT = *tIter;

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
			TMeshLib::CViewerVertex * pV = mesh->TetVertex(pT, j);
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

void VolViewer::drawMesh(HMeshLib::CVHMesh * mesh)
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
		drawMeshPoints(mesh);
		break;

	case DRAW_MODE::WIREFRAME:
		glDisable(GL_TEXTURE_2D);
		glPolygonMode(GL_FRONT, GL_LINE);
		drawHalfFaces(mesh->m_pHFaces_Below);
		drawSelectedVertex(mesh);
		break;

	case DRAW_MODE::FLATLINES:
		glDisable(GL_TEXTURE_2D);
		glPolygonMode(GL_FRONT, GL_LINE);
		drawHalfFaces(mesh->m_pHFaces_Above);
		glPolygonMode(GL_FRONT, GL_FILL);
		drawHalfFaces(mesh->m_pHFaces_Below);
		drawSelectedVertex(mesh);
		break;

	case DRAW_MODE::FLAT:
		glDisable(GL_TEXTURE_2D);
		glPolygonMode(GL_FRONT, GL_FILL);
		drawHalfFaces(mesh->m_pHFaces_Below);
		break;
		drawSelectedVertex(mesh);
		break;

	case DRAW_MODE::BOUNDARY:
		glDisable(GL_TEXTURE_2D);
		glPolygonMode(GL_FRONT, GL_FILL);
		drawBoundaryHalfFaces(mesh);
		break;

	case DRAW_MODE::TEXTURE:
		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		drawHalfFaces(hmesh->m_pHFaces_Below);
		break;

	case DRAW_MODE::TEXTUREMODULATE:
		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		drawHalfFaces(mesh->m_pHFaces_Below);
		break;

	case DRAW_MODE::VECTOR:
		glDisable(GL_TEXTURE_2D);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		drawVector();
		drawMeshPoints(mesh);
		break;

	default:
		break;
	}
}


void VolViewer::drawMesh(TMeshLib::CVTMesh * mesh)
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
		drawMeshPoints(mesh);
		break;

	case DRAW_MODE::WIREFRAME:
		glDisable(GL_TEXTURE_2D);
		glPolygonMode(GL_FRONT, GL_LINE);
		drawHalfFaces(mesh->m_pHFaces_Below);
		drawSelectedVertex(mesh);
		break;

	case DRAW_MODE::FLATLINES:
		glDisable(GL_TEXTURE_2D);
		glPolygonMode(GL_FRONT, GL_LINE);
		drawHalfFaces(mesh->m_pHFaces_Above);
		glPolygonMode(GL_FRONT, GL_FILL);
		drawHalfFaces(mesh->m_pHFaces_Below);
		drawSelectedVertex(mesh);
		break;

	case DRAW_MODE::FLAT:
		glDisable(GL_TEXTURE_2D);
		glPolygonMode(GL_FRONT, GL_FILL);
		drawHalfFaces(mesh->m_pHFaces_Below);
		break;
		drawSelectedVertex(mesh);
		break;

	case DRAW_MODE::BOUNDARY:
		glDisable(GL_TEXTURE_2D);
		glPolygonMode(GL_FRONT, GL_FILL);
		drawBoundaryHalfFaces(mesh);
		break;

	case DRAW_MODE::TEXTURE:
		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		drawHalfFaces(hmesh->m_pHFaces_Below);
		break;

	case DRAW_MODE::TEXTUREMODULATE:
		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		drawHalfFaces(mesh->m_pHFaces_Below);
		break;

	case DRAW_MODE::VECTOR:
		glDisable(GL_TEXTURE_2D);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		drawVector();
		drawMeshPoints(mesh);
		break;

	default:
		break;
	}
}

void VolViewer::loadTextMesh()
{
	if (!isTextureLoaded)
	{
		openTexture();
	}

	initTexture();
}

void VolViewer::initTexture()
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

void VolViewer::drawSelectedVertex(TMeshLib::CVTMesh * mesh)
{
	for (TMeshLib::CVTMesh::MeshVertexIterator vIter(mesh); !vIter.end(); vIter++)
	{
		TMeshLib::CViewerVertex * pV = *vIter;
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

void VolViewer::drawSelectedVertex(HMeshLib::CVHMesh * mesh)
{
	for (HMeshLib::CVHMesh::MeshVertexIterator vIter(mesh); !vIter.end(); vIter++)
	{
		HMeshLib::CHViewerVertex * pV = *vIter;
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

void VolViewer::drawBoundaryHalfFaces(TMeshLib::CVTMesh * mesh)
{
	std::vector<TMeshLib::CViewerHalfFace *> halffaceList = mesh->m_pHFaces_Below;
	glBegin(GL_TRIANGLES);
	for (std::vector<TMeshLib::CViewerHalfFace*>::iterator iter = halffaceList.begin(); iter != halffaceList.end(); iter++)
	{
		TMeshLib::CViewerHalfFace * pHF = *iter;
		TMeshLib::CViewerHalfFace * pHD = mesh->HalfFaceDual(pHF);
		if (pHD == NULL)
		{
			glColor3f(1.0, 0.0, 0.0);
			for (TMeshLib::CVTMesh::HalfFaceVertexIterator fvIter(mesh, pHF); !fvIter.end(); ++fvIter)
			{
				TMeshLib::CViewerVertex * v = *fvIter;
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

void VolViewer::drawBoundaryHalfFaces(HMeshLib::CVHMesh * mesh)
{
	std::vector<HMeshLib::CHViewerHalfFace *> halffaceList = mesh->m_pHFaces_Below;
	glBegin(GL_TRIANGLES);
	for (std::vector<HMeshLib::CHViewerHalfFace*>::iterator iter = halffaceList.begin(); iter != halffaceList.end(); iter++)
	{
		HMeshLib::CHViewerHalfFace * pHF = *iter;
		HMeshLib::CHViewerHalfFace * pHD = mesh->HalfFaceDual(pHF);
		if (pHD == NULL)
		{
			glColor3f(1.0, 0.0, 0.0);
			for (HMeshLib::CVHMesh::HalfFaceVertexIterator fvIter(mesh, pHF); !fvIter.end(); ++fvIter)
			{
				HMeshLib::CHViewerVertex * v = *fvIter;
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

void VolViewer::mousePressEvent(QMouseEvent * mouseEvent)
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

void VolViewer::selectCutFace(QPoint newPos)
{

	CPoint newNear, newFar;
	CPoint newRay = getRayVector(newPos, newNear, newFar);

	double minAngle = std::numeric_limits<double>::max();
	TMeshLib::CViewerVertex * minVertex;

	std::vector<TMeshLib::CViewerFace*> cutFacesList = mesh->_getCutFaces();

	for (std::vector<TMeshLib::CViewerFace*>::iterator cFIter = cutFacesList.begin(); cFIter != cutFacesList.end(); cFIter++)
	{
		TMeshLib::CViewerFace * currCF = *cFIter;
		for (TMeshLib::CVTMesh::FaceVertexIterator fvIter(mesh, currCF); !fvIter.end(); fvIter++)
		{
			TMeshLib::CViewerVertex * pV = *fvIter;
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

void VolViewer::selectAllCutFaces(QPoint newPos)
{
	CPoint newNear, newFar;
	CPoint newRay = getRayVector(newPos, newNear, newFar);

	double minAngle = std::numeric_limits<double>::max();
	TMeshLib::CViewerVertex * minVertex;

	std::vector<TMeshLib::CViewerFace*> cutFacesList = mesh->_getCutFaces();

	for (std::vector<TMeshLib::CViewerFace*>::iterator cFIter = cutFacesList.begin(); cFIter != cutFacesList.end(); cFIter++)
	{
		TMeshLib::CViewerFace * currCF = *cFIter;
		for (TMeshLib::CVTMesh::FaceVertexIterator fvIter(mesh, currCF); !fvIter.end(); fvIter++)
		{
			TMeshLib::CViewerVertex * pV = *fvIter;
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

	std::queue<TMeshLib::CViewerVertex*> queue;
	queue.push(minVertex);

	while (!queue.empty())
	{
		TMeshLib::CViewerVertex * currV = queue.front();
		queue.pop();

		for (TMeshLib::CVTMesh::VertexVertexIterator vvIter(mesh, currV); !vvIter.end(); vvIter++)
		{
			TMeshLib::CViewerVertex * pV = *vvIter;
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

CPoint VolViewer::getRayVector(QPoint point, CPoint & nearPt, CPoint & farPt)
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

void VolViewer::mouseMoveEvent(QMouseEvent * mouseEvent)
{
	QPoint newMousePos = mouseEvent->pos();
	// enable OpenGL Context
	makeCurrent();
	if (isLatestMouseOK)
	{
		switch (mouseButton)
		{
		case Qt::LeftButton:
			// rotate the view
			rotationView(newMousePos);
			break;
		case Qt::RightButton:
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

void VolViewer::mouseReleaseEvent(QMouseEvent * /*mouseEvent*/)
{
	mouseButton = Qt::NoButton;
	isLatestMouseOK = false;
	std::cout << "Mouse Release..." << std::endl;
}

void VolViewer::wheelEvent(QWheelEvent * mouseEvent)
{
	// scroll the wheel to scale the view port
	double moveAmount = -(double)mouseEvent->delta() / (120.0*8.0);
	translate(CPoint(0.0, 0.0, moveAmount));
	updateGL();
	mouseEvent->accept();
}

bool VolViewer::arcball(QPoint screenPos, CPoint &new3Dpos)
{
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

void VolViewer::rotationView(QPoint newPos)
{
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
	}
}

void VolViewer::rotate(CPoint axis, double angle)
{
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
void VolViewer::translateView(QPoint newPos)
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

	translate(transVector);

}

void VolViewer::translate(CPoint transVector)
{

	makeCurrent();
	glLoadIdentity();
	glTranslated(-transVector[0], -transVector[1], -transVector[2]);
	glMultMatrixd(matModelView);
	glGetDoublev(GL_MODELVIEW_MATRIX, matModelView);
}

void VolViewer::loadFile(const char * meshfile, std::string fileExt)
{

	VOLUME_TYPE currentVolType;

	if (isMeshLoaded)
	{
		mesh = new TMeshLib::CVTMesh();
		hmesh = new HMeshLib::CVHMesh();
	}

	if (fileExt == "tet")
	{
		mesh->_load(meshfile);
		tmeshlist.push_back(mesh);
		currentVolType = VOLUME_TYPE::TET;
	}

	if (fileExt == "hm")
	{
		hmesh->_load_hm(meshfile);
		hmeshlist.push_back(hmesh);
		currentVolType = VOLUME_TYPE::HEX;
	}

	if (fileExt == "t")
	{
		mesh->_load_t(meshfile);
		tmeshlist.push_back(mesh);
		currentVolType = VOLUME_TYPE::TET;
	}

	if (fileExt == "f")
	{
		bool ok;
		fiberMinLength = QInputDialog::getInt(this, tr("Input the minimal length of fibers to draw"), tr("Minimal Fiber Length"), 0, 0, INT_MAX, 1, &ok);
		{
			mesh->_load_f(meshfile);
			tmeshlist.push_back(mesh);
			currentVolType = VOLUME_TYPE::FIBER;
		}
	}

	// read in traits
	for (TMeshLib::CVTMesh::MeshVertexIterator vIter(mesh); !vIter.end(); vIter++)
	{
		TMeshLib::CViewerVertex * pV = *vIter;
		pV->_from_string();
	}

	for (TMeshLib::CVTMesh::MeshEdgeIterator eIter(mesh); !eIter.end(); eIter++)
	{
		TMeshLib::CViewerEdge * pE = *eIter;
		pE->_from_string();
	}

	for (TMeshLib::CVTMesh::MeshTetIterator tIter(mesh); !tIter.end(); tIter++)
	{
		TMeshLib::CViewerTet * pT = *tIter;
		pT->_from_string();
	}

	meshDrawMode = DRAW_MODE::FLAT;

	CPlane p(CPoint(0.0, 0.0, 1.0), 0.0);

	for (size_t t = 0; t < tmeshlist.size(); t++)
	{
		TMeshLib::CVTMesh * tmesh = tmeshlist[t];
		tmesh->_normalize();
		tmesh->_halfface_normal();
		tmesh->_cut(p);
	}
	
	for (size_t h = 0; h < hmeshlist.size(); h++)
	{
		HMeshLib::CVHMesh * hmesh = hmeshlist[h];
		hmesh->_normalize();
		hmesh->_halfface_normal();
		hmesh->_cut(p);
	}

	isMeshLoaded = true;

	updateGL();
}

void VolViewer::loadFromMainWin(std::string outFilename, std::string outExt)
{
	sFilename = outFilename;
	filename = QString::fromStdString(outFilename);
	const char * _filename = outFilename.c_str();
	loadFile(_filename, outExt);
}

void VolViewer::newScene()
{
	tmeshlist.clear();
	hmeshlist.clear();

	QString windowTitle = "VolumeViewerQt";

	foreach(QWidget *widget, qApp->topLevelWidgets())
	{
		MainWindow * mainWin = qobject_cast<MainWindow*>(widget);
		mainWin->setWindowTitle(windowTitle);
	}

	updateGL();
}

void VolViewer::openMesh()
{

	filenames = QFileDialog::getOpenFileNames(this,
		tr("Open volume meshes"),
		tr("./"),
		tr("Hex Mesh (*.hm);;"
		"Tet Mesh (*.tet *t);;"
		"Fiber (*.f)"));


	QString windowTitle = "VolumeViewerQt - ";

	for (QStringList::iterator sIter = filenames.begin(); sIter != filenames.end(); sIter++)
	{
		filename = *sIter;
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

			windowTitle += canonicalFilePath + "; ";
		}
	}

	foreach(QWidget *widget, qApp->topLevelWidgets())
	{
		MainWindow * mainWin = qobject_cast<MainWindow*>(widget);
		mainWin->setWindowTitle(windowTitle);
	}

}

void VolViewer::openTexture()
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

void VolViewer::screenshot()
{
	GLfloat * buffer = new GLfloat[width() * height() * 3];
	glReadBuffer(GL_FRONT_LEFT);
	glReadPixels(0, 0, width(), height(), GL_RGB, GL_FLOAT, buffer);

	RgbImage image(height(), width());

	for (int i = 0; i < height(); i++)
	{
		for (int j = 0; j < width(); j++)
		{
			float r = buffer[(i * width() + j) * 3 + 0];
			float g = buffer[(i * width() + j) * 3 + 1];
			float b = buffer[(i * width() + j) * 3 + 2];

			image.SetRgbPixelf(i, j, r, g, b);
		}
	}

	delete[]buffer;

	QFileInfo * fileInfo = new QFileInfo(filename);
	QString path = fileInfo->canonicalPath();

	QTime time = QTime::currentTime();
	QString currTime = time.toString(QString("hh_mm_ss_zzz"));
	QString currTime2 = time.toString(QString("hh:mm:ss.zzz"));
	std::string imageName = path.toStdString() + "/ScreenShot_" + currTime.toStdString() + ".bmp";
	image.WriteBmpFile(imageName.c_str());

	std::cout << "Screenshot taken on " + currTime2.toStdString() << std::endl;
}

void VolViewer::saveMesh()
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

void VolViewer::saveFile(const char * meshfile, std::string sExt)
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

void VolViewer::exportVisibleMesh()
{
	QString exportFilename = QFileDialog::getSaveFileName(this,
		tr("Save Mesh File"),
		tr("../models/"),
		tr("Mesh Files (*.m)"));
	QFileInfo * exportFileInfo = new QFileInfo(exportFilename);
	std::string exportFileExt = exportFileInfo->suffix().toStdString();
	if (!exportFilename.isEmpty())
	{
		QByteArray byteArray = exportFilename.toUtf8();
		const char * _exportFilename = byteArray.constData();
		exportVisibleSurface(_exportFilename, exportFileExt);
	}
}

void VolViewer::exportVisibleSurface(const char * surface_file, std::string sExt)
{
	assert(sExt == "m");
	mesh->_write_visible_surface(surface_file);
}

void VolViewer::enterSelectionMode()
{
	if (!isSelectionMode)
	{
		isSelectionMode = true;
	}
}

void VolViewer::quitSelectionMode()
{
	if (isSelectionMode)
	{
		isSelectionMode = false;
	}
}

void VolViewer::enterSelectionCutFaceMode()
{
	if (!isSelectionCutFaceMode)
	{
		isSelectionCutFaceMode = true;
	}
}

void VolViewer::quitSelectionCutFaceMode()
{
	if (isSelectionCutFaceMode)
	{
		isSelectionCutFaceMode = false;
	}
}

void VolViewer::xCut()
{
	cutPlane = CPlane(CPoint(1, 0, 0), cutDistance);
	CPoint pNormal = cutPlane.normal();
	double distance = cutPlane.d();
	std::cout << "CutPlane " << "Normal=(" << pNormal[0] << " " << pNormal[1] << " " << pNormal[2] << ") ";
	std::cout << "d=" << distance << std::endl;
	
	for (std::vector<TMeshLib::CVTMesh*>::iterator tIter = tmeshlist.begin(); tIter != tmeshlist.end(); tIter++)
	{
		TMeshLib::CVTMesh * tmesh = *tIter;
		tmesh->_cut(cutPlane);
	}

	for (std::vector<HMeshLib::CVHMesh*>::iterator hIter = hmeshlist.begin(); hIter != hmeshlist.end(); hIter++)
	{
		HMeshLib::CVHMesh * hmesh = *hIter;
		hmesh->_cut(cutPlane);
	}
	updateGL();
}

void VolViewer::yCut()
{
	cutPlane = CPlane(CPoint(0, 1, 0), cutDistance);
	CPoint pNormal = cutPlane.normal();
	double distance = cutPlane.d();
	std::cout << "CutPlane " << "Normal=(" << pNormal[0] << " " << pNormal[1] << " " << pNormal[2] << ") ";
	std::cout << "d=" << distance << std::endl;
	
	for (std::vector<TMeshLib::CVTMesh*>::iterator tIter = tmeshlist.begin(); tIter != tmeshlist.end(); tIter++)
	{
		TMeshLib::CVTMesh * tmesh = *tIter;
		tmesh->_cut(cutPlane);
	}

	for (std::vector<HMeshLib::CVHMesh*>::iterator hIter = hmeshlist.begin(); hIter != hmeshlist.end(); hIter++)
	{
		HMeshLib::CVHMesh * hmesh = *hIter;
		hmesh->_cut(cutPlane);
	}

	updateGL();
}

void VolViewer::zCut()
{
	cutPlane = CPlane(CPoint(0, 0, 1), cutDistance);
	CPoint pNormal = cutPlane.normal();
	double distance = cutPlane.d();
	std::cout << "CutPlane " << "Normal=(" << pNormal[0] << " " << pNormal[1] << " " << pNormal[2] << ") ";
	std::cout << "d=" << distance << std::endl;
	
	for (std::vector<TMeshLib::CVTMesh*>::iterator tIter = tmeshlist.begin(); tIter != tmeshlist.end(); tIter++)
	{
		TMeshLib::CVTMesh * tmesh = *tIter;
		tmesh->_cut(cutPlane);
	}

	for (std::vector<HMeshLib::CVHMesh*>::iterator hIter = hmeshlist.begin(); hIter != hmeshlist.end(); hIter++)
	{
		HMeshLib::CVHMesh * hmesh = *hIter;
		hmesh->_cut(cutPlane);
	}

	updateGL();
}

void VolViewer::plusMove()
{
	cutDistance += 0.05;
	cutPlane.d() = cutDistance;
	CPoint pNormal = cutPlane.normal();
	double distance = cutPlane.d();
	std::cout << "CutPlane " << "Normal=(" << pNormal[0] << " " << pNormal[1] << " " << pNormal[2] << ") ";
	std::cout << "d=" << distance << std::endl;

	for (std::vector<TMeshLib::CVTMesh*>::iterator tIter = tmeshlist.begin(); tIter != tmeshlist.end(); tIter++)
	{
		TMeshLib::CVTMesh * tmesh = *tIter;
		tmesh->_cut(cutPlane);
	}

	for (std::vector<HMeshLib::CVHMesh*>::iterator hIter = hmeshlist.begin(); hIter != hmeshlist.end(); hIter++)
	{
		HMeshLib::CVHMesh * hmesh = *hIter;
		hmesh->_cut(cutPlane);
	}

	updateGL();
}

void VolViewer::minusMove()
{
	cutDistance -= 0.05;
	cutPlane.d() = cutDistance;
	CPoint pNormal = cutPlane.normal();
	double distance = cutPlane.d();
	std::cout << "CutPlane " << "Normal=(" << pNormal[0] << " " << pNormal[1] << " " << pNormal[2] << ") ";
	std::cout << "d=" << distance << std::endl;
	
	for (std::vector<TMeshLib::CVTMesh*>::iterator tIter = tmeshlist.begin(); tIter != tmeshlist.end(); tIter++)
	{
		TMeshLib::CVTMesh * tmesh = *tIter;
		tmesh->_cut(cutPlane);
	}

	for (std::vector<HMeshLib::CVHMesh*>::iterator hIter = hmeshlist.begin(); hIter != hmeshlist.end(); hIter++)
	{
		HMeshLib::CVHMesh * hmesh = *hIter;
		hmesh->_cut(cutPlane);
	}

	updateGL();
}

void VolViewer::cutVolume()
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

void VolViewer::clearSelectedVF()
{
	for (TMeshLib::CVTMesh::MeshVertexIterator vIter(mesh); !vIter.end(); vIter++)
	{
		TMeshLib::CViewerVertex * pV = *vIter;
		pV->selected() = false;
	}

	mesh->_updateSelectedFaces();

	updateGL();
}

void VolViewer::lightOn()
{
	isLightOn = true;
	updateGL();
}

void VolViewer::lightOff()
{
	isLightOn = false;
	updateGL();
}

void VolViewer::rotationViewOn()
{
	isRotationViewOn = true;
}

void VolViewer::rotationViewOff()
{
	isRotationViewOn = false;
}