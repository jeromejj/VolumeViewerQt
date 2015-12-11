#ifndef _VIEWER_HMESH_H_
#define _VIEWER_HMESH_H_

#include "..\VMeshLib\core\HexMesh\basehmesh.h"
#include "..\VMeshLib\core\HexMesh\hiterators.h"
#include "..\VMeshLib\core\Geometry\plane.h"
#include "..\VMeshLib\core\Parser\strutil.h"
#include "..\VMeshLib\core\Parser\parser.h"

namespace VMeshLib
{
	namespace HMeshLib
	{

		class CHViewerVertex : public CVertex
		{
		public:
			CHViewerVertex() { m_outside = false; m_selected = false; m_cut = false; };
			~CHViewerVertex(){};
			bool & outside() { return m_outside; };
			bool & selected() { return m_selected; };
			bool & cut() { return m_cut; };
			CPoint2 & uv() { return m_uv; };
			void _from_string()
			{
				CParser parser(m_string);
				for (std::list<CToken*>::iterator iter = parser.tokens().begin(); iter != parser.tokens().end(); ++iter)
				{
					CToken * token = *iter;
					if (token->m_key == "uv")
					{
						token->m_value >> m_uv;
					}
				}
			};

		protected:
			bool m_outside;
			bool m_selected;
			CPoint2 m_uv;
			bool m_cut;
		};

		/*!
		* \brief CViewerTVertex, Vertex for viewer purpose
		*/
		class CHViewerHVertex : public CHVertex
		{
		};

		/*!
		* \brief CViewerHalfEdge, HalfEdge for viewer purpose
		*/
		class CHViewerHalfEdge : public CHalfEdge
		{
		};

		/*!
		* \brief CViewerTEdge, TEdge for viewer purpose
		*/
		class CHViewerHEdge : public CHEdge
		{
		};

		/*!
		* \brief CViewerEdge, Edge for viewer purpose
		*/
		class CHViewerEdge : public CEdge
		{
		public:
			CHViewerEdge() { m_hw = 0.0; };
			~CHViewerEdge() {};

			double & harmonic_w() { return m_hw; };

			void _to_string() {};
			void _from_string()
			{
				CParser parser(m_string);
				for (std::list<CToken*>::iterator iter = parser.tokens().begin(); iter != parser.tokens().end(); iter++)
				{
					CToken * token = *iter;
					if (token->m_key == "hw")
					{
						std::string line = strutil::trim(token->m_value, "()");
						m_hw = strutil::parseString<double>(line);
					}
				}
			};

		protected:
			double m_hw;

		};

		/*!
		* \brief CViewerFace, Face for viewer purpose
		*/
		class CHViewerFace : public CFace
		{
		public:
			CHViewerFace() { m_selected = false; m_cut = false; };
			~CHViewerFace() {};

			bool & selected() { return m_selected; }
			bool & cut() { return m_cut; };

		protected:
			bool m_selected;
			bool m_cut;
		};

		/*!
		* \brief CViewerTet, Tet for viewer purpose
		*/
		class CHViewerHex : public CHex
		{
		public:
			CHViewerHex() { m_outside = false; };
			~CHViewerHex(){};
			bool & outside() { return m_outside; };

			CPoint & vector() { return m_vector; };

			void _from_string()
			{
				CParser parser(m_string);
				for (std::list<CToken*>::iterator iter = parser.tokens().begin(); iter != parser.tokens().end(); ++iter)
				{
					CToken * token = *iter;
					if (token->m_key == "vector")
					{
						token->m_value >> m_vector;
					}
				}
			};

		protected:
			bool m_outside;
			CPoint m_vector;
		};

		/*!
		* \brief CViewerHalfFace, HalfFace for viewer purpose
		*/
		class CHViewerHalfFace : public CHalfFace
		{
		public:
			CHViewerHalfFace(){};
			~CHViewerHalfFace(){};
			CPoint & normal() { return m_normal; };

		protected:
			CPoint m_normal;
		};


		template<typename HXV, typename V, typename HE, typename HXE, typename E, typename HF, typename F, typename HX>
		class CViewerHMesh : public CHMesh < HXV, V, HE, HXE, E, HF, F, HX >
		{
			typedef HXV CHVertex;
			typedef V	CVertex;
			typedef HE	CHalfEdge;
			typedef HXE CHEdge;
			typedef E	CEdge;
			typedef HF	CHalfFace;
			typedef HX	CHex;
		public:

			typedef HMeshHexIterator<HXV, V, HE, HXE, E, HF, F, HX> MeshTetIterator;
			typedef HexHalfFaceIterator<HXV, V, HE, HXE, E, HF, F, HX> TetHFIterator;
			typedef HMeshEdgeIterator<HXV, V, HE, HXE, E, HF, F, HX> MeshEdgeIterator;
			typedef HMeshFaceIterator<HXV, V, HE, HXE, E, HF, F, HX> MeshFaceIterator;
			typedef HalfFaceVertexIterator<HXV, V, HE, HXE, E, HF, F, HX> HalfFaceVertexIterator;
			typedef FaceVertexIterator<HXV, V, HE, HXE, E, HF, F, HX> FaceVertexIterator;
			typedef HMeshVertexIterator<HXV, V, HE, HXE, E, HF, F, HX> MeshVertexIterator;
			typedef HVertexVertexIterator<HXV, V, HE, HXE, E, HF, F, HX> VertexVertexIterator;

			CViewerHMesh(){ ; };

			~CViewerHMesh(){ ; };

			// normalize the hex mesh
			void _normalize();

			void _halfface_normal();

			void _cut(CPlane & p);

		public:
			std::vector<HF*> m_pHFaces_Above;
			std::vector<HF*> m_pHFaces_Below;
			std::vector<F *> m_cutFaces;
			std::vector<F *> m_selectedFacesList;
		};

		template<typename HXV, typename V, typename HE, typename HXE, typename E, typename HF, typename F, typename HX>
		void CViewerHMesh<HXV, V, HE, HXE, E, HF, F, HX>::_normalize()
		{
			CPoint vmax(-1e+10, -1e+10, -1e+10);
			CPoint vmin(1e+10, 1e+10, 1e+10);

			for (std::list<V*>::iterator vIter = m_pVertices.begin(); vIter != m_pVertices.end(); vIter++)
			{
				V * pV = *vIter;
				CPoint p = pV->position();

				for (int k = 0; k < 3; k++)
				{
					vmax[k] = (vmax[k] > p[k]) ? vmax[k] : p[k];
					vmin[k] = (vmin[k] < p[k]) ? vmin[k] : p[k];
				}
			}

			CPoint center = (vmax + vmin) / 2.0;

			double d = 0;

			for (std::list<V*>::iterator vIter = m_pVertices.begin(); vIter != m_pVertices.end(); vIter++)
			{
				V * pV = *vIter;
				CPoint p = pV->position();
				p = p - center;
				pV->position() = p;

				for (int k = 0; k < 3; k++)
				{
					if (fabs(p[k]) > d) d = fabs(p[k]);
				}
			}

			for (std::list<V*>::iterator vIter = m_pVertices.begin(); vIter != m_pVertices.end(); vIter++)
			{
				V * pV = *vIter;
				CPoint p = pV->position();
				p /= d;
				pV->position() = p;
			}

		};

		template<typename HXV, typename V, typename HE, typename HXE, typename E, typename HF, typename F, typename HX>
		void CViewerHMesh<HXV, V, HE, HXE, E, HF, F, HX>::_halfface_normal()
		{
			for (std::list<HF*>::iterator hiter = m_pHalfFaces.begin(); hiter != m_pHalfFaces.end(); hiter++)
			{
				HF *pHF = *hiter;
				HE *pHE = HalfFaceHalfEdge(pHF);
				std::vector<CPoint> ps;
				for (int k = 0; k < 4; k++)
				{
					ps.push_back(HalfEdgeTarget(pHE)->position());
					pHE = HalfEdgeNext(pHE);
				}
				CPoint n0 = (ps[1] - ps[0]) ^ (ps[2] - ps[0]);
				CPoint n1 = (ps[2] - ps[0]) ^ (ps[3] - ps[0]);
				CPoint n = (n0 + n1) / 2;
				n = n / n.norm();
				pHF->normal() = n;
			}
		};

		template<typename HXV, typename V, typename HE, typename HXE, typename E, typename HF, typename F, typename HX>
		void CViewerHMesh<HXV, V, HE, HXE, E, HF, F, HX>::_cut(CPlane & p)
		{

			m_pHFaces_Above.clear();
			m_pHFaces_Below.clear();
			m_cutFaces.clear();

			for (std::list<V*>::iterator vIter = m_pVertices.begin(); vIter != m_pVertices.end(); vIter++)
			{
				V * pV = *vIter;
				pV->cut() = false;
				CPoint pos = pV->position();
				pV->outside() = (p.side(pos) >= 0);
			}

			for (std::list<HX*>::iterator tIter = m_pHexs.begin(); tIter != m_pHexs.end(); tIter++)
			{
				HX * pT = *tIter;

				V * pV[8];

				for (int k = 0; k < 8; k++)
					pV[k] = HexVertex(pT, k);


				pT->outside() = true;
				for (int j = 0; j < 8; j++)
				{
					if (!pV[j]->outside())
					{
						pT->outside() = false;
						break;
					}
				}
			}

			for (std::list<F*>::iterator fIter = m_pFaces.begin(); fIter != m_pFaces.end(); fIter++)
			{
				F * pF = *fIter;
				pF->cut() = false;
			}

			for (std::list<HF *>::iterator fIter = m_pHalfFaces.begin(); fIter != m_pHalfFaces.end(); fIter++)
			{
				HF * pHF = *fIter;

				HF * pHFD = HalfFaceDual(pHF);

				F * pF = HalfFaceFace(pHF);

				if (pHF != NULL && pHFD != NULL)
				{
					HX * pT_l = HalfFaceHex(pHF);
					HX * pT_r = HalfFaceHex(pHFD);
					if (pT_l->outside() != pT_r->outside())
					{
						m_cutFaces.push_back(pF);
						pF->cut() = true;
					}
				}
			}

			for (std::vector<F*>::iterator fIter = m_cutFaces.begin(); fIter != m_cutFaces.end(); fIter++)
			{
				F * pF = *fIter;
				HF * pHF = FaceLeftHalfFace(pF);
				HE * pHE = HalfFaceHalfEdge(pHF);
				HE * pHE2 = HalfEdgeNext(pHE);
				HE * pHE3 = HalfEdgeNext(pHE2);

				V * pV = HalfEdgeTarget(pHE);
				V * pV2 = HalfEdgeTarget(pHE2);
				V * pV3 = HalfEdgeTarget(pHE3);
				pV->cut() = true;
				pV2->cut() = true;
				pV3->cut() = true;
			}

			for (std::list<HF*>::iterator it = m_pHalfFaces.begin(); it != m_pHalfFaces.end(); it++)
			{
				HF * pF = *it;
				HF * pD = HalfFaceDual(pF);
				HX  * pT = HalfFaceHex(pF);

				if (pD == NULL)
				{
					if (pT->outside())
					{
						m_pHFaces_Above.push_back(pF);
					}
					else
					{
						m_pHFaces_Below.push_back(pF);
					}
					continue;
				}

				HX * pDT = HalfFaceHex(pD);

				if (pT->outside() && !pDT->outside())
				{
					m_pHFaces_Above.push_back(pF);
					continue;
				}
				if (!pT->outside() && pDT->outside())
				{
					m_pHFaces_Below.push_back(pF);
					continue;
				}
			}
		}


		typedef CViewerHMesh<CHViewerHVertex, CHViewerVertex, CHViewerHalfEdge, CHViewerHEdge, CHViewerEdge, CHViewerHalfFace, CHViewerFace, CHViewerHex> CVHMesh;
	};
}
#endif