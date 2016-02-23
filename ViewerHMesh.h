#ifndef _VIEWER_HMESH_H_
#define _VIEWER_HMESH_H_

#include "..\MeshLib\core\HexMesh\basehmesh.h"
#include "..\MeshLib\core\HexMesh\hiterators.h"
#include "..\MeshLib\core\Geometry\plane.h"
#include "..\MeshLib\core\Parser\strutil.h"
#include "..\MeshLib\core\Parser\parser.h"

namespace MeshLib
{
	namespace HMeshLib
	{

		class CHViewerVertex : public CVertex
		{
		public:
			CHViewerVertex() { m_outside = false; m_selected = false; m_cut = false; m_boundary = false;};
			~CHViewerVertex(){};

			bool & boundary() { return m_boundary; };
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

			bool m_boundary;
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
			typedef F	CFace;
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

			std::vector<CVertex*> & selectedVertices() { return m_selectedVertices; };

			// normalize the hex mesh
			void _normalize();

			void _halfface_normal();

			void _cut(CPlane & p);

			void _labelBoundary();

			void _write_hm_samepoint(const char * output, std::map<int, int> vertexIdMap);

			void _write_cut_above_surface_qm(const char * output);
			void _write_cut_above_surface_obj(const char * output);

			void _write_cut_below_surface_qm(const char * output);
			void _write_cut_below_surface_obj(const char * output);

			void _write_surface_qm(const char * output);
			void _write_surface_obj(const char * output);

		public:
			std::vector<CHalfFace*> m_pHFaces_Above;
			std::vector<CHalfFace*> m_pHFaces_Below;
			std::vector<CFace *> m_cutFaces;
			std::vector<CFace *> m_selectedFacesList;
			std::vector<CVertex *> m_selectedVertices;
		};


		template<typename HXV, typename V, typename HE, typename HXE, typename E, typename HF, typename F, typename HX>
		void CViewerHMesh<HXV, V, HE, HXE, E, HF, F, HX>::_labelBoundary()
		{
			for (std::list<CVertex*>::iterator vIter = m_pVertices.begin(); vIter != m_pVertices.end(); vIter++)
			{
				CVertex * pV = *vIter;
				pV->boundary() = false;
			}

			for (std::list<HF*>::iterator hiter = m_pHalfFaces.begin(); hiter != m_pHalfFaces.end(); hiter++)
			{
				CHalfFace *pHF = *hiter;
				if (HalfFaceDual(pHF) != NULL)
				{
					continue;
				}

				CHalfEdge *pHE = HalfFaceHalfEdge(pHF);
				for (int k = 0; k < 4; k++)
				{
					CVertex * pV = HalfEdgeTarget(pHE);
					pV->boundary() = true;
					pHE = HalfEdgeNext(pHE);
				}
			}
		};


		template<typename HXV, typename V, typename HE, typename HXE, typename E, typename HF, typename F, typename HX>
		void CViewerHMesh<HXV, V, HE, HXE, E, HF, F, HX>::_write_hm_samepoint(const char * output, std::map<int, int> vertexIdMap)
		{
			//write traits to string, add by Wei Chen, 11/23/2015
			for (std::list<CVertex*>::iterator vIter = m_pVertices.begin(); vIter != m_pVertices.end(); vIter++)
			{
				CVertex * pV = *vIter;
				pV->_to_string();
			}

			for (std::list<CHex *>::iterator tIter = m_pHexs.begin(); tIter != m_pHexs.end(); tIter++)
			{
				CHex * pHex = *tIter;
				pHex->_to_string();
			}

			for (std::list<CEdge*>::iterator eIter = m_pEdges.begin(); eIter != m_pEdges.end(); eIter++)
			{
				CEdge * pE = *eIter;
				pE->_to_string();
			}
			//write traits end

			std::fstream _os(output, std::fstream::out);
			if (_os.fail())
			{
				fprintf(stderr, "Error while opening file %s\n", output);
				return;
			}

			for (std::list<CVertex*>::iterator vIter = m_pVertices.begin(); vIter != m_pVertices.end(); vIter++)
			{
				CVertex * pV = *vIter;
				CPoint p = pV->position();
				_os << "Vertex " << pV->id();
				for (int k = 0; k < 3; k++)
				{
					_os << " " << p[k];
				}
				if (pV->string().size() > 0)
				{
					_os << " " << "{" << pV->string() << "}";
				}
				_os << std::endl;
			}

			for (std::list<CHex *>::iterator tIter = m_pHexs.begin(); tIter != m_pHexs.end(); tIter++)
			{
				CHex * pHex = *tIter;
				_os << "Hex " << pHex->id();
				for (int k = 0; k < 8; k++)
				{
					CVertex * pV = HexVertex(pHex, k);
					if (vertexIdMap.find(pV->id()) != vertexIdMap.end())
					{
						_os << " " << vertexIdMap[pV->id()];
					}
					else
					{
						_os << " " << pV->id();
					}
				}
				if (pHex->string().size() > 0)
				{
					
					_os << " " << "{" << pHex->string() << "}";
				}
				_os << std::endl;
			}

			for (std::list<CEdge*>::iterator eIter = m_pEdges.begin(); eIter != m_pEdges.end(); eIter++)
			{
				CEdge * pE = *eIter;
				if (pE->string().size() > 0)
				{
					CVertex * pV1 = EdgeVertex1(pE);
					CVertex * pV2 = EdgeVertex2(pE);
					int id1 = (vertexIdMap.find(pV1->id()) != vertexIdMap.end()) ? vertexIdMap[pV1->id()] : pV1->id();
					int id2 = (vertexIdMap.find(pV2->id()) != vertexIdMap.end()) ? vertexIdMap[pV2->id()] : pV2->id();
					_os << "Edge " << id1 << " " << id2 << " ";
					_os << "{" << pE->string() << "}" << std::endl;
				}
			}

			_os.close();
		};
			
		template<typename HXV, typename V, typename HE, typename HXE, typename E, typename HF, typename F, typename HX>
		void CViewerHMesh<HXV, V, HE, HXE, E, HF, F, HX>::_write_surface_qm(const char * output)
		{

			_labelBoundary();

			std::vector<CVertex*> vertices;

			for (std::list<CVertex*>::iterator vIter = m_pVertices.begin(); vIter != m_pVertices.end(); vIter++)
			{
				CVertex * pV = *vIter;
				if (pV->boundary())
				{
					vertices.push_back(pV);
				}
			}

			std::fstream _os(output, std::fstream::out);
			if (_os.fail())
			{
				fprintf(stderr, "Error is opening file %s\n", output);
				return;
			}

			for (std::vector<CVertex*>::iterator vIter = vertices.begin(); vIter != vertices.end(); vIter++)
			{
				CVertex * pV = *vIter;
				CPoint p = pV->position();
				_os << "Vertex " << pV->id() << " " << p << std::endl;
			}

			int faceid = 1;
			for (std::list<HF*>::iterator hiter = m_pHalfFaces.begin(); hiter != m_pHalfFaces.end(); hiter++, faceid++)
			{
				CHalfFace *pHF = *hiter;
				if (HalfFaceDual(pHF) != NULL)
				{
					continue;
				}

				_os << "Face " << faceid << " ";
				CHalfEdge *pHE = HalfFaceHalfEdge(pHF);
				for (int k = 0; k < 4; k++)
				{
					CVertex * pV = HalfEdgeTarget(pHE);
					_os << pV->id() << " ";
					pHE = HalfEdgeNext(pHE);
				}
				_os << std::endl;
			}

			_os.close();
		};

		template<typename HXV, typename V, typename HE, typename HXE, typename E, typename HF, typename F, typename HX>
		void CViewerHMesh<HXV, V, HE, HXE, E, HF, F, HX>::_write_surface_obj(const char * output)
		{

			_labelBoundary();

			std::fstream _os(output, std::fstream::out);
			if (_os.fail())
			{
				fprintf(stderr, "Error is opening file %s\n", output);
				return;
			}

			std::vector<CVertex*> vertices;

			for (std::list<CVertex*>::iterator vIter = m_pVertices.begin(); vIter != m_pVertices.end(); vIter++)
			{
				CVertex * pV = *vIter;
				if (pV->boundary())
				{
					vertices.push_back(pV);
				}
			}

			int vid = 1;

			for (std::vector<CVertex*>::iterator vIter = vertices.begin(); vIter != vertices.end(); vIter++)
			{
				CVertex * pV = *vIter;
				pV->id() = vid++;
			}

			_os << "# Generated by VolumeViewerQt" << std::endl;

			for (std::vector<CVertex*>::iterator vIter = vertices.begin(); vIter != vertices.end(); vIter++)
			{
				CVertex * pV = *vIter;
				CPoint p = pV->position();
				_os << "v " << p << std::endl;
			}

			for (std::list<HF*>::iterator hiter = m_pHalfFaces.begin(); hiter != m_pHalfFaces.end(); hiter++)
			{
				CHalfFace *pHF = *hiter;
				if (HalfFaceDual(pHF) != NULL)
				{
					continue;
				}

				_os << "f ";
				CHalfEdge *pHE = HalfFaceHalfEdge(pHF);
				for (int k = 0; k < 4; k++)
				{
					CVertex * pV = HalfEdgeTarget(pHE);
					_os << pV->id() << " ";
					pHE = HalfEdgeNext(pHE);
				}
				_os << std::endl;
			}

			_os.close();
		};

		template<typename HXV, typename V, typename HE, typename HXE, typename E, typename HF, typename F, typename HX>
		void CViewerHMesh<HXV, V, HE, HXE, E, HF, F, HX>::_write_cut_above_surface_obj(const char * output)
		{

			std::vector<CVertex*> vertices;

			for (std::vector<CHalfFace*>::iterator hiter = m_pHFaces_Above.begin(); hiter != m_pHFaces_Above.end(); hiter++)
			{
				CHalfFace * pHF = *hiter;
				CHalfEdge *pHE = HalfFaceHalfEdge(pHF);
				for (int k = 0; k < 4; k++)
				{
					CVertex * pV = HalfEdgeTarget(pHE);
					if (std::find(vertices.begin(), vertices.end(), pV) != vertices.end())
					{
						vertices.push_back(pV);
					}
					pHE = HalfEdgeNext(pHE);
				}
			}

			int vid = 1;
			for (std::vector<CVertex*>::iterator vIter = vertices.begin(); vIter != vertices.end(); vIter++)
			{
				CVertex * pV = *vIter;
				pV->id() = vid++;
			}

			std::fstream _os(output, std::fstream::out);
			if (_os.fail())
			{
				fprintf(stderr, "Error is opening file %s\n", output);
				return;
			}

			_os << "# Generated by VolumeViewerQt" << std::endl;

			for (std::vector<CVertex*>::iterator vIter = vertices.begin(); vIter != vertices.end(); vIter++)
			{
				CVertex * pV = *vIter;
				CPoint p = pV->position();
				_os << "v " << p << std::endl;
			}

			for (std::vector<HF*>::iterator hiter = m_pHFaces_Above.begin(); hiter != m_pHFaces_Above.end(); hiter++)
			{
				CHalfFace *pHF = *hiter;
				_os << "f ";
				CHalfEdge *pHE = HalfFaceHalfEdge(pHF);
				for (int k = 0; k < 4; k++)
				{
					CVertex * pV = HalfEdgeTarget(pHE);
					_os << pV->id() << " ";
					pHE = HalfEdgeNext(pHE);
				}
				_os << std::endl;
			}

			_os.close();

		};

		template<typename HXV, typename V, typename HE, typename HXE, typename E, typename HF, typename F, typename HX>
		void CViewerHMesh<HXV, V, HE, HXE, E, HF, F, HX>::_write_cut_above_surface_qm(const char * output)
		{

			std::vector<CVertex*> vertices;

			for (std::vector<CHalfFace*>::iterator fIter = m_pHFaces_Above.begin(); fIter != m_pHFaces_Above.end(); fIter++)
			{
				CHalfFace * pHF = *fIter;

				CHalfEdge *pHE = HalfFaceHalfEdge(pHF);
				for (int k = 0; k < 4; k++)
				{
					CVertex * pV = HalfEdgeTarget(pHE);
					if (std::find(vertices.begin(), vertices.end(), pV) != vertices.end())
					{
						vertices.push_back(pV);
					}
					pHE = HalfEdgeNext(pHE);
				}
			}


			std::fstream _os(output, std::fstream::out);
			if (_os.fail())
			{
				fprintf(stderr, "Error is opening file %s\n", output);
				return;
			}

			for (std::vector<CVertex*>::iterator vIter = vertices.begin(); vIter != vertices.end(); vIter++)
			{
				CVertex * pV = *vIter;
				CPoint p = pV->position();
				_os << "Vertex " << pV->id() << " " << p << std::endl;
			}

			int faceid = 1;
			for (std::vector<CHalfFace*>::iterator hiter = m_pHFaces_Above.begin(); hiter != m_pHFaces_Above.end(); hiter++, faceid++)
			{
				CHalfFace *pHF = *hiter;
				_os << "Face " << faceid << " ";
				CHalfEdge *pHE = HalfFaceHalfEdge(pHF);
				for (int k = 0; k < 4; k++)
				{
					CVertex * pV = HalfEdgeTarget(pHE);
					_os << pV->id() << " ";
					pHE = HalfEdgeNext(pHE);
				}
				_os << std::endl;
			}

			_os.close();

		};
		
		template<typename HXV, typename V, typename HE, typename HXE, typename E, typename HF, typename F, typename HX>
		void CViewerHMesh<HXV, V, HE, HXE, E, HF, F, HX>::_write_cut_below_surface_obj(const char * output)
		{
		
			std::vector<CVertex*> vertices;
			for (std::vector<CHalfFace*>::iterator hfIter = m_pHFaces_Below.begin(); hfIter != m_pHFaces_Below.end(); hfIter++)
			{
				CHalfFace * pHF = *hfIter;
	
				CHalfEdge *pHE = HalfFaceHalfEdge(pHF);
				for (int k = 0; k < 4; k++)
				{
					CVertex * pV = HalfEdgeTarget(pHE);
					if (std::find(vertices.begin(), vertices.end(), pV) != vertices.end())
					{
						vertices.push_back(pV);
					}

					pHE = HalfEdgeNext(pHE);
				}
			}

			int vid = 1;
			for (std::vector<CVertex*>::iterator vIter = vertices.begin(); vIter != vertices.end(); vIter++)
			{
				CVertex * pV = *vIter;
				pV->id() = vid++;
			}

			std::fstream _os(output, std::fstream::out);
			if (_os.fail())
			{
				fprintf(stderr, "Error is opening file %s\n", output);
				return;
			}

			_os << "# Generated by VolumeViewerQt" << std::endl;

			for (std::vector<CVertex*>::iterator vIter = vertices.begin(); vIter != vertices.end(); vIter++)
			{
				CVertex * pV = *vIter;
				CPoint p = pV->position();
				_os << "v " << p << std::endl;
			}

			for (std::vector<HF*>::iterator hiter = m_pHFaces_Below.begin(); hiter != m_pHFaces_Below.end(); hiter++)
			{
				CHalfFace *pHF = *hiter;
				_os << "f ";
				CHalfEdge *pHE = HalfFaceHalfEdge(pHF);
				for (int k = 0; k < 4; k++)
				{
					CVertex * pV = HalfEdgeTarget(pHE);
					_os << pV->id() << " ";
					pHE = HalfEdgeNext(pHE);
				}
				_os << std::endl;
			}

			_os.close();
		};

		template<typename HXV, typename V, typename HE, typename HXE, typename E, typename HF, typename F, typename HX>
		void CViewerHMesh<HXV, V, HE, HXE, E, HF, F, HX>::_write_cut_below_surface_qm(const char * output)
		{
			
			std::vector<CVertex*> vertices;

			for (std::vector<CHalfFace*>::iterator hfIter = m_pHFaces_Below.begin(); hfIter != m_pHFaces_Below.end(); hfIter++)
			{
				CHalfFace * pHF = *hfIter;

				CHalfEdge *pHE = HalfFaceHalfEdge(pHF);
				for (int k = 0; k < 4; k++)
				{
					CVertex * pV = HalfEdgeTarget(pHE);
					if (std::find(vertices.begin(), vertices.end(), pV) != vertices.end())
					{
						vertices.push_back(pV);
					}

					pHE = HalfEdgeNext(pHE);
				}
			}

			std::fstream _os(output, std::fstream::out);
			if (_os.fail())
			{
				fprintf(stderr, "Error is opening file %s\n", output);
				return;
			}

			for (std::vector<CVertex*>::iterator vIter = vertices.begin(); vIter != vertices.end(); vIter++)
			{
				CVertex * pV = *vIter;
				CPoint p = pV->position();
				_os << "Vertex " << pV->id() << " " << p << std::endl;
			}

			int faceid = 1;
			for (std::vector<CHalfFace*>::iterator hiter = m_pHFaces_Below.begin(); hiter != m_pHFaces_Below.end(); hiter++, faceid++)
			{
				CHalfFace *pHF = *hiter;
				_os << "Face " << faceid << " ";
				CHalfEdge *pHE = HalfFaceHalfEdge(pHF);
				for (int k = 0; k < 4; k++)
				{
					CVertex * pV = HalfEdgeTarget(pHE);
					_os << pV->id() << " ";
					pHE = HalfEdgeNext(pHE);
				}
				_os << std::endl;
			}

			_os.close();
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
					if (pT_l->outside() != pT_r->outside() && !pF->cut())
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