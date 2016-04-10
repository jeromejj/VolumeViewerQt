#ifndef _TMESHLIB_VIEWER_MESH_H_
#define _TMESHLIB_VIEWER_MESH_H_

#include <stdio.h>
#include <map>

#include "..\MeshLib\core\TetMesh\BaseTMesh.h"
#include "..\MeshLib\core\TetMesh\titerators.h"
#include "..\MeshLib\core\Geometry\plane.h"
#include "..\MeshLib\core\Parser\strutil.h"
#include "..\MeshLib\core\Parser\parser.h"

namespace MeshLib
{
	namespace TMeshLib
	{

		class CViewerTVertex;
		class CViewerVertex;
		class CViewerHalfEdge;
		class CViewerTEdge;
		class CViewerEdge;
		class CViewerFace;
		class CViewerHalfFace;
		class CViewerTet;

		class CFiber
		{
		public:
			CFiber() { m_closed = false; };
			~CFiber() { m_points.clear(); };

			std::list<CPoint> & points() { return m_points; };

			void push_back(CPoint newp) { m_points.push_back(newp); };

			bool & closed() { return m_closed; };

		protected:
			std::list<CPoint> m_points;
			bool m_closed;
		};

		/*!
		* \brief CViewerVertex, Vertex for viewer purpose
		*/
		class CViewerVertex : public CVertex
		{
		public:
			CViewerVertex() { m_outside = false; m_selected = false; m_cut = false; m_boundary = false; };
			~CViewerVertex(){};

			bool & boundary() { return m_boundary; };
			bool & outside() { return m_outside; };
			bool & selected() { return m_selected; };
			bool & cut() { return m_cut; };
			CPoint2 & uv() { return m_uv; };

			int & group() { return m_group; };
			
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

			int m_group;
		};

		/*!
		* \brief CViewerTVertex, Vertex for viewer purpose
		*/
		class CViewerTVertex : public CTVertex
		{
		};

		/*!
		* \brief CViewerHalfEdge, HalfEdge for viewer purpose
		*/
		class CViewerHalfEdge : public CHalfEdge
		{
		};

		/*!
		* \brief CViewerTEdge, TEdge for viewer purpose
		*/
		class CViewerTEdge : public CTEdge
		{
		};

		/*!
		* \brief CViewerEdge, Edge for viewer purpose
		*/
		class CViewerEdge : public CEdge
		{
		public:
			CViewerEdge() { m_hw = 0.0; };
			~CViewerEdge() {};

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
		class CViewerFace : public CFace
		{
		public:
			CViewerFace() { m_selected = false; m_cut = false; };
			~CViewerFace() {};

			bool & selected() { return m_selected; }
			bool & cut() { return m_cut; };

		protected:
			bool m_selected;
			bool m_cut;
		};

		/*!
		* \brief CViewerTet, Tet for viewer purpose
		*/
		class CViewerTet : public CTet
		{
		public:
			CViewerTet() { m_outside = false; };
			~CViewerTet(){};
			bool & outside() { return m_outside; };

			CPoint & vector() { return m_vector; };

			int & group() { return m_group; };

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
					else if (token->m_key == "group")
					{
						std::string line = strutil::trim(token->m_value, "()");
						m_group = strutil::parseString<int>(line);
					}
				}
			};

		protected:
			bool m_outside;
			CPoint m_vector;
			int m_group = 0;
		};

		/*!
		* \brief CViewerHalfFace, HalfFace for viewer purpose
		*/
		class CViewerHalfFace : public CHalfFace
		{
		public:
			CViewerHalfFace(){};
			~CViewerHalfFace(){};
			CPoint & normal() { return m_normal; };

		protected:
			CPoint m_normal;
		};

		/*!
		 *	\brief CViewerTMesh class
		 *
		 *	TMesh class for viewing purpose
		 */
		template<typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
		class CViewerTMesh : public CTMesh < TV, V, HE, TE, E, HF, F, T >
		{
		public:
			typedef TV  CTVertex;
			typedef V	CVertex;
			typedef HE	CHalfEdge;
			typedef TE  CTEdge;
			typedef E	CEdge;
			typedef HF	CHalfFace;
			typedef F	CFace;
			typedef T	CTet;
		public:
			typedef TMeshTetIterator<TV, V, HE, TE, E, HF, F, T> MeshTetIterator;
			typedef TetHalfFaceIterator<TV, V, HE, TE, E, HF, F, T> TetHFIterator;
			typedef TMeshEdgeIterator<TV, V, HE, TE, E, HF, F, T> MeshEdgeIterator;
			typedef TMeshFaceIterator<TV, V, HE, TE, E, HF, F, T> MeshFaceIterator;
			typedef HalfFaceVertexIterator<TV, V, HE, TE, E, HF, F, T> HalfFaceVertexIterator;
			typedef FaceVertexIterator<TV, V, HE, TE, E, HF, F, T> FaceVertexIterator;
			typedef TMeshVertexIterator<TV, V, HE, TE, E, HF, F, T> MeshVertexIterator;
			typedef TVertexVertexIterator<TV, V, HE, TE, E, HF, F, T> VertexVertexIterator;
			typedef VertexTVertexIterator<TV, V, HE, TE, E, HF, F, T> VertexTVertexIterator;

		public:
			void _normalize();
			void _halfface_normal();
			void _computeGroupNumber();
			void _cut(CPlane &);
			void _updateSelectedFaces();

			void _labelVertexGroup();

			void _labelBoundary();

			/*! get cut faces vector */
			std::vector<F *> _getCutFaces() { return m_cutFaces; };

			std::vector<CVertex*> & selectedVertices() { return m_selectedVertices; };

			// cut the volume along selected faces
			void _cutVolumeWrite(const char * filename, std::string ext);

			// write the cut vertices to file
			void _write_cut_vertices(const char * vFilename);

			// get the fibers of current tet mesh
			std::list<CFiber *> & fibers() { return m_fibers; };

			// get the number of fibers
			int numFibers() { return (int)m_fibers.size(); };

			// load fiber file
			void _load_f(const char * input);

			void _write_cut_above_surface_m(const char * output);
			void _write_cut_above_surface_obj(const char * output);

			void _write_cut_above_surface_group_obj(const char * output, int groupid);
			void _write_cut_above_surface_only_group_obj(const char * output, int groupid);

			void _write_cut_below_surface_m(const char * output);
			void _write_cut_below_surface_obj(const char * output);

			void _write_cut_below_surface_group_obj(const char * output, int groupid);
			void _write_cut_below_surface_only_group_obj(const char * output, int groupid);

			// write the visible surface(m_pHFaces_Below) as a mesh file
			void _write_surface_m(const char * output);

			void _write_surface_obj(const char * output);

			bool & isFiber() { return m_isFiber; };

			int & maxGroup() { return m_maxGroup; };

			int & minGroup() { return m_minGroup; };

		private:
			std::map<int, int> mapSelectedNewVertex;

		public:
			std::vector<HF*> m_pHFaces_Above;
			std::vector<HF*> m_pHFaces_Below;
			std::vector<F *> m_cutFaces;
			std::vector<F *> m_selectedFacesList;

			std::vector<CVertex*> m_selectedVertices;

		protected:

			int m_maxGroup = 0;
			int m_minGroup = 0;

			std::list<CFiber*> m_fibers;
			int m_nFibers;
			bool m_isFiber = false;
		};

		template<typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
		void CViewerTMesh<TV, V, HE, TE, E, HF, F, T>::_load_f(const char * input)
		{
			std::fstream is(input, std::fstream::in);

			if (is.fail())
			{
				fprintf(stderr, "Error in opening file %s\n", input);
				return;
			}
			
			m_isFiber = true;

			std::string line;
			while (!is.eof())
			{
				std::getline(is, line);
				line = strutil::trim(line);
				strutil::Tokenizer stokenizer(line, " \r\n");
				stokenizer.nextToken();
				std::string token = stokenizer.getToken();

				if (token == "fiber")
				{
					CFiber * fiber = new CFiber();

					stokenizer.nextToken();
					std::string closed = stokenizer.getToken();
					int closed_i = strutil::parseString<int>(closed);
					if (closed_i == 1)
					{
						fiber->closed() = true;
					}
					else if (closed_i == 0)
					{
						fiber->closed() = false;
					}
					else
					{
						fprintf(stderr, "Error in opening file %s\n", input);
						return;
					}

					stokenizer.nextToken();
					std::string vnumber = stokenizer.getToken();
					int vnum = strutil::parseString<int>(vnumber);

					for (int i = 0; i < vnum && !is.eof(); i++)
					{
						std::string pointLine;
						std::getline(is, pointLine);
						pointLine = strutil::trim(pointLine);
						strutil::Tokenizer vertokenizer(pointLine, " \r\n");
						CPoint p;
						for (int k = 0; k < 3; k++)
						{
							vertokenizer.nextToken();
							std::string position = vertokenizer.getToken();
							p[k] = strutil::parseString<double>(position);
						}

						fiber->push_back(p);
					}

					m_fibers.push_back(fiber);
				}
				else
				{
					fprintf(stderr, "File Format Error\r\n");
					return;
				}
			}

		};

		template<typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
		void CViewerTMesh<TV, V, HE, TE, E, HF, F, T>::_cutVolumeWrite(const char * filename, std::string ext)
		{

			int k = 1;

			std::list<V*> newVertices;

			for (std::list<V*>::iterator vIter = m_pVertices.begin(); vIter != m_pVertices.end(); vIter++)
			{
				V * pV = *vIter;
				if (pV->selected())
				{
					int newId = m_maxVertexId + k;
					mapSelectedNewVertex.insert(std::pair<int, int>(pV->id(), newId));
					V * newpV = new V();
					newpV->id() = newId;
					newpV->position() = pV->position();
					newVertices.push_back(newpV);
					m_map_Vertices.insert(std::pair<int, V *>(newId, newpV));
					k++;
				}
			}

			for (std::list<V*>::iterator vIter = newVertices.begin(); vIter != newVertices.end(); vIter++)
			{
				V * pV = *vIter;
				m_pVertices.push_back(pV);
			}

			m_maxVertexId += k - 1;
			m_nVertices += k - 1;

			std::fstream _os(filename, std::fstream::out);
			if (_os.fail())
			{
				fprintf(stderr, "Error while opening file %s\n", filename);
				return;
			}

			if (ext == "t")
			{
				for (std::list<V*>::iterator vIter = m_pVertices.begin(); vIter != m_pVertices.end(); vIter++)
				{
					V * pV = *vIter;
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

				for (std::list<T*>::iterator tIter = m_pTets.begin(); tIter != m_pTets.end(); tIter++)
				{
					T * pT = *tIter;
					_os << "Tet " << pT->id();
					if (pT->outside())
					{
						for (int j = 0; j < 4; j++)
						{
							V * pV = TetVertex(pT, j);
							if (pV->selected())
							{
								int newId = mapSelectedNewVertex[pV->id()];
								V * newpV = m_map_Vertices[newId];
								_os << " " << newpV->id();
							}
							else
							{
								_os << " " << pV->id();
							}
						}
					}
					else
					{
						for (int k = 0; k < 4; k++)
						{
							_os << " " << pT->tvertex(k)->vert()->id();
						}
					}
					_os << std::endl;
				}
			}
			else if (ext == "tet")
			{
				_os << m_nVertices << " vertices" << std::endl;
				_os << m_nTets << " tets" << std::endl;

				for (std::list<V*>::iterator vIter = m_pVertices.begin(); vIter != m_pVertices.end(); vIter++)
				{
					V * pV = *vIter;
					CPoint  p = pV->position();
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

				for (std::list<T*>::iterator tIter = m_pTets.begin(); tIter != m_pTets.end(); tIter++)
				{
					T * pT = *tIter;
					_os << 4;
					if (pT->outside())
					{
						for (int j = 0; j < 4; j++)
						{
							V * pV = TetVertex(pT, j);
							if (pV->selected())
							{
								int newId = mapSelectedNewVertex[pV->id()];
								V * newpV = m_map_Vertices[newId];
								_os << " " << newpV->id();
							}
							else
							{
								_os << " " << pV->id();
							}
						}
					}
					else
					{
						for (int k = 0; k < 4; k++)
						{
							_os << " " << pT->tvertex(k)->vert()->id();
						}
					}
					_os << std::endl;
				}
			}

			_os.close();
		}

		template<typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
		void CViewerTMesh<TV, V, HE, TE, E, HF, F, T>::_write_cut_vertices(const char * vFilename)
		{
			std::ofstream outputv;
			outputv.open(vFilename);
			for (std::map<int, int>::iterator mIter = mapSelectedNewVertex.begin(); mIter != mapSelectedNewVertex.end(); mIter++)
			{
				outputv << mIter->first << " " << mIter->second << std::endl;
			}
			outputv.close();
		}

		template<typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
		void CViewerTMesh<TV, V, HE, TE, E, HF, F, T>::_updateSelectedFaces()
		{
			m_selectedFacesList.clear();

			for (std::vector<F*>::iterator fIter = m_cutFaces.begin(); fIter != m_cutFaces.end(); fIter++)
			{
				F * pF = *fIter;
				pF->selected() = false;

				bool faceSelected = true;
				for (FaceVertexIterator fvIter(this, pF); !fvIter.end(); fvIter++)
				{
					V * pvF = *fvIter;
					if (pvF->selected() == false)
					{
						faceSelected = false;
					}
				}

				if (faceSelected)
				{
					pF->selected() = true;
					m_selectedFacesList.push_back(pF);
				}
			}
		}

		template<typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
		void CViewerTMesh<TV, V, HE, TE, E, HF, F, T>::_normalize()
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

		template<typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
		void CViewerTMesh<TV, V, HE, TE, E, HF, F, T>::_labelVertexGroup()
		{
			for (std::list<CTet*>::iterator tIter = m_pTets.begin(); tIter != m_pTets.end(); tIter++)
			{
				CTet * pT = *tIter;
				for (int k = 0; k < 4; k++)
				{
					CVertex * pV = TetVertex(pT, k);
					pV->group() = pT->group();
				}
			}

			for (std::list<HF*>::iterator hiter = m_pHalfFaces.begin(); hiter != m_pHalfFaces.end(); hiter++)
			{
				HF * pHF = *hiter;
				HF * pDHF = HalfFaceDual(pHF);
				if (pDHF != nullptr)
				{
					CTet * pT = HalfFaceTet(pHF);
					CTet * pDT = HalfFaceTet(pDHF);
					if (pT->group() != pDT->group())
					{
						for (int k = 0; k < 4; k++)
						{
							CVertex * pV = TetVertex(pT, k);
							pV->group() = 0;
						}
					}
				}
			}

		};

		template<typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
		void CViewerTMesh<TV, V, HE, TE, E, HF, F, T>::_computeGroupNumber()
		{
			int maxg = INT_MIN;
			int ming = INT_MAX;

			for (std::list<V*>::iterator vIter = m_pVertices.begin(); vIter != m_pVertices.end(); vIter++)
			{
				V * pV = *vIter;
				maxg = (maxg < pV->group()) ? pV->group() : maxg;
				ming = (ming > pV->group()) ? pV->group() : ming;
			}

			m_maxGroup = maxg;
			m_minGroup = ming;
		};

		template<typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
		void CViewerTMesh<TV, V, HE, TE, E, HF, F, T>::_halfface_normal()
		{
			for (std::list<HF*>::iterator hiter = m_pHalfFaces.begin(); hiter != m_pHalfFaces.end(); hiter++)
			{
				HF *pHF = *hiter;
				HE *pHE = HalfFaceHalfEdge(pHF);
				std::vector<CPoint> ps;
				for (int k = 0; k < 3; k++)
				{
					ps.push_back(HalfEdgeTarget(pHE)->position());
					pHE = HalfEdgeNext(pHE);
				}
				CPoint n = (ps[1] - ps[0]) ^ (ps[2] - ps[0]);
				n = n / n.norm();
				pHF->normal() = n;
			}
		};

		template<typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
		void CViewerTMesh<TV, V, HE, TE, E, HF, F, T>::_cut(CPlane & p)
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

			for (std::list<T*>::iterator tIter = m_pTets.begin(); tIter != m_pTets.end(); tIter++)
			{
				T * pT = *tIter;

				V * pV[4];

				for (int k = 0; k < 4; k++)
					pV[k] = TetVertex(pT, k);


				pT->outside() = true;
				for (int j = 0; j < 4; j++)
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
					T * pT_l = HalfFaceTet(pHF);
					T * pT_r = HalfFaceTet(pHFD);
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
				T  * pT = HalfFaceTet(pF);

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

				T * pDT = HalfFaceTet(pD);

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
		};

		template<typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
		void CViewerTMesh<TV, V, HE, TE, E, HF, F, T>::_labelBoundary()
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

		template<typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
		void CViewerTMesh<TV, V, HE, TE, E, HF, F, T>::_write_surface_obj(const char * output)
		{

			std::fstream _os(output, std::fstream::out);
			if (_os.fail())
			{
				fprintf(stderr, "Error is opening file %s\n", output);
				return;
			}

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
				for (int k = 0; k < 3; k++)
				{
					CVertex * pV = HalfEdgeTarget(pHE);
					_os << pV->id() << " ";
					pHE = HalfEdgeNext(pHE);
				}
				_os << std::endl;
			}

			_os.close();
		};

		template<typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
		void CViewerTMesh<TV, V, HE, TE, E, HF, F, T>::_write_surface_m(const char * output)
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
				for (int k = 0; k < 3; k++)
				{
					CVertex * pV = HalfEdgeTarget(pHE);
					_os << pV->id() << " ";
					pHE = HalfEdgeNext(pHE);
				}
				_os << std::endl;
			}

			_os.close();
		}

		template<typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
		void CViewerTMesh<TV, V, HE, TE, E, HF, F, T>::_write_cut_above_surface_m(const char * output)
		{

			_labelBoundary();

			std::vector<CHalfFace*> outputHalfFaces;

			for (std::vector<CFace*>::iterator fIter = m_cutFaces.begin(); fIter != m_cutFaces.end(); fIter++)
			{
				CFace * pF = *fIter;
				CHalfFace * pHF = FaceLeftHalfFace(pF);

				outputHalfFaces.push_back(pHF);

				CHalfEdge *pHE = HalfFaceHalfEdge(pHF);
				for (int k = 0; k < 4; k++)
				{
					CVertex * pV = HalfEdgeTarget(pHE);
					pV->boundary() = true;
					pHE = HalfEdgeNext(pHE);
				}
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
			for (std::vector<CHalfFace*>::iterator hiter = outputHalfFaces.begin(); hiter != outputHalfFaces.end(); hiter++, faceid++)
			{
				CHalfFace *pHF = *hiter;
				_os << "Face " << faceid << " ";
				CHalfEdge *pHE = HalfFaceHalfEdge(pHF);
				for (int k = 0; k < 3; k++)
				{
					CVertex * pV = HalfEdgeTarget(pHE);
					_os << pV->id() << " ";
					pHE = HalfEdgeNext(pHE);
				}
				_os << std::endl;
			}

			_os.close();
		};

		template<typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
		void CViewerTMesh<TV, V, HE, TE, E, HF, F, T>::_write_cut_above_surface_obj(const char * output)
		{
			
			_labelBoundary();

			std::vector<CHalfFace*> outputHalfFaces;

			for (std::vector<CFace*>::iterator fIter = m_cutFaces.begin(); fIter != m_cutFaces.end(); fIter++)
			{
				CFace * pF = *fIter;
				CHalfFace * pHF = FaceLeftHalfFace(pF);

				outputHalfFaces.push_back(pHF);

				CHalfEdge *pHE = HalfFaceHalfEdge(pHF);
				for (int k = 0; k < 4; k++)
				{
					CVertex * pV = HalfEdgeTarget(pHE);
					pV->boundary() = true;
					pHE = HalfEdgeNext(pHE);
				}
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

			for (std::vector<HF*>::iterator hiter = outputHalfFaces.begin(); hiter != outputHalfFaces.end(); hiter++)
			{
				CHalfFace *pHF = *hiter;
				_os << "f ";
				CHalfEdge *pHE = HalfFaceHalfEdge(pHF);
				for (int k = 0; k < 3; k++)
				{
					CVertex * pV = HalfEdgeTarget(pHE);
					_os << pV->id() << " ";
					pHE = HalfEdgeNext(pHE);
				}
				_os << std::endl;
			}

			_os.close();
		};

		template<typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
		void CViewerTMesh<TV, V, HE, TE, E, HF, F, T>::_write_cut_above_surface_group_obj(const char * output, int groupid)
		{

			_labelBoundary();

			std::vector<CHalfFace*> outputHalfFaces;

			for (std::vector<CHalfFace*>::iterator hfIter = m_pHFaces_Above.begin(); hfIter != m_pHFaces_Above.end(); hfIter++)
			{
				CHalfFace * pHF = *hfIter;

				if (HalfFaceDual(pHF) == NULL)
				{
					continue;
				}

				CTet * pT = HalfFaceTet(pHF);
				if (pT->group() == groupid)
				{
					outputHalfFaces.push_back(pHF);
				}

			}

			std::vector<CVertex*> vertices;
			for (std::list<CVertex*>::iterator vIter = m_pVertices.begin(); vIter != m_pVertices.end(); vIter++)
			{
				CVertex * pV = *vIter;
				if (pV->boundary() && (pV->group() == groupid || pV->group() == 0))
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

			std::string outputName = output;
			size_t idx = outputName.find_last_of(".");
			std::string ext = outputName.substr(idx, outputName.length() - idx);

			std::string outputfile;
			if (m_maxGroup - m_minGroup == 0)
			{
				outputfile = outputName.substr(0, idx) + ext;
			}
			else
			{
				outputfile = outputName.substr(0, idx) + "." + std::to_string(groupid) + ext;
			}

			std::fstream _os(outputfile.c_str(), std::fstream::out);
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

			for (std::vector<HF*>::iterator hiter = outputHalfFaces.begin(); hiter != outputHalfFaces.end(); hiter++)
			{
				CHalfFace *pHF = *hiter;
				_os << "f ";
				CHalfEdge *pHE = HalfFaceHalfEdge(pHF);
				for (int k = 0; k < 3; k++)
				{
					CVertex * pV = HalfEdgeTarget(pHE);
					_os << pV->id() << " ";
					pHE = HalfEdgeNext(pHE);
				}
				_os << std::endl;
			}

			_os.close();

		};

		template<typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
		void CViewerTMesh<TV, V, HE, TE, E, HF, F, T>::_write_cut_above_surface_only_group_obj(const char * output, int groupid)
		{

			_labelBoundary();
			std::vector<CHalfFace*> outputHalfFaces;

			for (std::vector<CHalfFace*>::iterator hfIter = m_pHFaces_Above.begin(); hfIter != m_pHFaces_Above.end(); hfIter++)
			{
				CHalfFace * pHF = *hfIter;

				if (HalfFaceDual(pHF) != NULL)
				{
					continue;
				}

				CTet * pT = HalfFaceTet(pHF);
				if (pT->group() == groupid)
				{
					outputHalfFaces.push_back(pHF);
				}

			}

			std::vector<CVertex*> vertices;
			for (std::list<CVertex*>::iterator vIter = m_pVertices.begin(); vIter != m_pVertices.end(); vIter++)
			{
				CVertex * pV = *vIter;
				if (pV->boundary() && (pV->group() == groupid || pV->group() == 0))
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

			std::string outputName = output;
			size_t idx = outputName.find_last_of(".");
			std::string ext = outputName.substr(idx, outputName.length() - idx);

			std::string outputfile;
			if (m_maxGroup - m_minGroup == 0)
			{
				outputfile = outputName.substr(0, idx) + ".surface" + ext;
			}
			else
			{
				outputfile = outputName.substr(0, idx) + ".surface." + std::to_string(groupid) + ext;
			}

			std::fstream _os(outputfile.c_str(), std::fstream::out);
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

			for (std::vector<HF*>::iterator hiter = outputHalfFaces.begin(); hiter != outputHalfFaces.end(); hiter++)
			{
				CHalfFace *pHF = *hiter;
				_os << "f ";
				CHalfEdge *pHE = HalfFaceHalfEdge(pHF);
				for (int k = 0; k < 3; k++)
				{
					CVertex * pV = HalfEdgeTarget(pHE);
					_os << pV->id() << " ";
					pHE = HalfEdgeNext(pHE);
				}
				_os << std::endl;
			}

			_os.close();

		};

		template<typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
		void CViewerTMesh<TV, V, HE, TE, E, HF, F, T>::_write_cut_below_surface_m(const char * output)
		{ 
			
			_labelBoundary();
			std::vector<CHalfFace*> outputHalfFaces;

			for (std::vector<CHalfFace*>::iterator hfIter = m_pHFaces_Below.begin(); hfIter != m_pHFaces_Below.end(); hfIter++)
			{
				CHalfFace * pHF = *hfIter;
				if (HalfFaceDual(pHF) != NULL)
				{
					continue;
				}

				outputHalfFaces.push_back(pHF);

			}

			for (std::vector<CFace*>::iterator fIter = m_cutFaces.begin(); fIter != m_cutFaces.end(); fIter++)
			{
				CFace * pF = *fIter;
				CHalfFace * pHF = FaceLeftHalfFace(pF);

				outputHalfFaces.push_back(pHF);

				CHalfEdge *pHE = HalfFaceHalfEdge(pHF);
				for (int k = 0; k < 4; k++)
				{
					CVertex * pV = HalfEdgeTarget(pHE);
					pV->boundary() = true;
					pHE = HalfEdgeNext(pHE);
				}
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
			for (std::vector<CHalfFace*>::iterator hiter = outputHalfFaces.begin(); hiter != outputHalfFaces.end(); hiter++, faceid++)
			{
				CHalfFace *pHF = *hiter;
				_os << "Face " << faceid << " ";
				CHalfEdge *pHE = HalfFaceHalfEdge(pHF);
				for (int k = 0; k < 3; k++)
				{
					CVertex * pV = HalfEdgeTarget(pHE);
					_os << pV->id() << " ";
					pHE = HalfEdgeNext(pHE);
				}
				_os << std::endl;
			}

			_os.close();
		};

		template<typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
		void CViewerTMesh<TV, V, HE, TE, E, HF, F, T>::_write_cut_below_surface_obj(const char * output)
		{ 

			std::vector<CVertex*> vertices;
			for (std::vector<CHalfFace*>::iterator hfIter = m_pHFaces_Below.begin(); hfIter != m_pHFaces_Below.end(); hfIter++)
			{
				CHalfFace * pHF = *hfIter;
				
				CHalfEdge *pHE = HalfFaceHalfEdge(pHF);
				for (int k = 0; k < 4; k++)
				{
					CVertex * pV = HalfEdgeTarget(pHE);
					if (std::find(vertices.begin(), vertices.end(), pV) == vertices.end())
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

			for (std::vector<CHalfFace*>::iterator hiter = m_pHFaces_Below.begin(); hiter != m_pHFaces_Below.end(); hiter++)
			{
				CHalfFace *pHF = *hiter;
				_os << "f ";
				CHalfEdge *pHE = HalfFaceHalfEdge(pHF);
				for (int k = 0; k < 3; k++)
				{
					CVertex * pV = HalfEdgeTarget(pHE);
					_os << pV->id() << " ";
					pHE = HalfEdgeNext(pHE);
				}
				_os << std::endl;
			}

			_os.close();
		};

		template<typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
		void CViewerTMesh<TV, V, HE, TE, E, HF, F, T>::_write_cut_below_surface_group_obj(const char * output, int groupid)
		{
			
			_labelBoundary();

			std::vector<CHalfFace*> outputHalfFaces;

			for (std::vector<CHalfFace*>::iterator hfIter = m_pHFaces_Below.begin(); hfIter != m_pHFaces_Below.end(); hfIter++)
			{
				CHalfFace * pHF = *hfIter;

				if (HalfFaceDual(pHF) == NULL)
				{
					continue;
				}

				CTet * pT = HalfFaceTet(pHF);
				if (pT->group() == groupid)
				{
					outputHalfFaces.push_back(pHF);
				}

			}

			std::vector<CVertex*> vertices;
			
			for (std::vector<HF*>::iterator hiter = outputHalfFaces.begin(); hiter != outputHalfFaces.end(); hiter++)
			{
				CHalfFace *pHF = *hiter;
				CHalfEdge *pHE = HalfFaceHalfEdge(pHF);
				for (int k = 0; k < 3; k++)
				{
					CVertex * pV = HalfEdgeTarget(pHE);
					if (std::find(vertices.begin(), vertices.end(), pV) == vertices.end())
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

			std::string outputName = output;
			size_t idx = outputName.find_last_of(".");
			std::string ext = outputName.substr(idx, outputName.length() - idx);

			std::string outputfile;
			if (m_maxGroup - m_minGroup == 0)
			{
				outputfile = outputName.substr(0, idx) + ext;
			}
			else
			{
				outputfile = outputName.substr(0, idx) + "." + std::to_string(groupid) + ext;
			}

			std::fstream _os(outputfile.c_str(), std::fstream::out);
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

			for (std::vector<HF*>::iterator hiter = outputHalfFaces.begin(); hiter != outputHalfFaces.end(); hiter++)
			{
				CHalfFace *pHF = *hiter;
				_os << "f ";
				CHalfEdge *pHE = HalfFaceHalfEdge(pHF);
				for (int k = 0; k < 3; k++)
				{
					CVertex * pV = HalfEdgeTarget(pHE);
					_os << pV->id() << " ";
					pHE = HalfEdgeNext(pHE);
				}
				_os << std::endl;
			}

			_os.close();

		};

		template<typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
		void CViewerTMesh<TV, V, HE, TE, E, HF, F, T>::_write_cut_below_surface_only_group_obj(const char * output, int groupid)
		{

			_labelBoundary();

			std::vector<CHalfFace*> outputHalfFaces;

			for (std::vector<CHalfFace*>::iterator hfIter = m_pHFaces_Below.begin(); hfIter != m_pHFaces_Below.end(); hfIter++)
			{
				CHalfFace * pHF = *hfIter;

				if (HalfFaceDual(pHF) != NULL)
				{
					continue;
				}

				CTet * pT = HalfFaceTet(pHF);
				if (pT->group() == groupid)
				{
					outputHalfFaces.push_back(pHF);
				}

			}

			std::vector<CVertex*> vertices;
			for (std::list<CVertex*>::iterator vIter = m_pVertices.begin(); vIter != m_pVertices.end(); vIter++)
			{
				CVertex * pV = *vIter;
				if (pV->boundary() && (pV->group() == groupid || pV->group() == 0))
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

			std::string outputName = output;
			size_t idx = outputName.find_last_of(".");
			std::string ext = outputName.substr(idx, outputName.length() - idx);

			std::string outputfile;
			if (m_maxGroup - m_minGroup == 0)
			{
				outputfile = outputName.substr(0, idx) + ".surface" + ext;
			}
			else
			{
				outputfile = outputName.substr(0, idx) + ".surface." + std::to_string(groupid) + ext;
			}

			std::fstream _os(outputfile.c_str(), std::fstream::out);
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

			for (std::vector<HF*>::iterator hiter = outputHalfFaces.begin(); hiter != outputHalfFaces.end(); hiter++)
			{
				CHalfFace *pHF = *hiter;
				_os << "f ";
				CHalfEdge *pHE = HalfFaceHalfEdge(pHF);
				for (int k = 0; k < 3; k++)
				{
					CVertex * pV = HalfEdgeTarget(pHE);
					_os << pV->id() << " ";
					pHE = HalfEdgeNext(pHE);
				}
				_os << std::endl;
			}

			_os.close();

		};

		typedef CViewerTMesh<CViewerTVertex, CViewerVertex, CViewerHalfEdge, CViewerTEdge, CViewerEdge, CViewerHalfFace, CViewerFace, CViewerTet> CVTMesh;

	};
};
#endif