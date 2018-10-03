/*

License

Menge
Copyright © and trademark ™ 2012-14 University of North Carolina at Chapel Hill.
All rights reserved.

Permission to use, copy, modify, and distribute this software and its documentation
for educational, research, and non-profit purposes, without fee, and without a
written agreement is hereby granted, provided that the above copyright notice,
this paragraph, and the following four paragraphs appear in all copies.

This software program and documentation are copyrighted by the University of North
Carolina at Chapel Hill. The software program and documentation are supplied "as is,"
without any accompanying services from the University of North Carolina at Chapel
Hill or the authors. The University of North Carolina at Chapel Hill and the
authors do not warrant that the operation of the program will be uninterrupted
or error-free. The end-user understands that the program was developed for research
purposes and is advised not to rely exclusively on the program for any reason.

IN NO EVENT SHALL THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL OR THE AUTHORS
BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS
DOCUMENTATION, EVEN IF THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL OR THE
AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL AND THE AUTHORS SPECIFICALLY
DISCLAIM ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE AND ANY STATUTORY WARRANTY
OF NON-INFRINGEMENT. THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND
THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL AND THE AUTHORS HAVE NO OBLIGATIONS
TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

Any questions or comments should be sent to the authors {menge,geom}@cs.unc.edu

*/

#include "MengeVis/SceneGraph/Select.h"
#include "MengeVis/SceneGraph/graphCommon.h"
#include <sstream>
#include <algorithm>

namespace MengeVis {

	namespace SceneGraph {

		///////////////////////////////////////////////////////////////////////////
		//				IMPLEMENTATION FOR Selectable
		///////////////////////////////////////////////////////////////////////////

		const unsigned int Selectable::BUFFER_SIZE = 16384;
		unsigned int * Selectable::_buffer = new unsigned int[ Selectable::BUFFER_SIZE ];
		unsigned int Selectable::ID = 0; // should start at zero I think - paul.
		// unsigned int Selectable::ID = 1;
		Selectable *	Selectable::_selectedObject = 0x0;
		unsigned int Selectable::_selectedName = 0;
		std::vector< Selectable * >	Selectable::_selectables;

		///////////////////////////////////////////////////////////////////////////

		Selectable::Selectable() :_id(ID), _selected(false) {
			++ID;
			_selectables.push_back( this );
			std::cout << "CREATE SELECTABLE " << _selectables.size() << std::endl;
		}

		Selectable::~Selectable() {
			std::cout << "DELETE SELECTABLE " << _selectables.size() << std::endl;
		  // 	std::vector< Selectable * >::iterator iter = std::find(_selectables.begin(), _selectables.end(), this);
		  //   if (iter != _selectables.end())
		  //   {
		  //   	delete *iter;
		  //       _selectables.erase(iter);
		  //   }
		}

		///////////////////////////////////////////////////////////////////////////

		void Selectable::loadSelectName() const {
			glLoadName( _id );
		}

		///////////////////////////////////////////////////////////////////////////

		void Selectable::selectStart() {
			glSelectBuffer( BUFFER_SIZE, _buffer );
			glRenderMode( GL_SELECT );
			glInitNames();
			glPushName( 0 );	// if the hit is zero, it means nothing is selected
		}

		///////////////////////////////////////////////////////////////////////////

		bool Selectable::selectEnd() {
			int hitCount = glRenderMode( GL_RENDER );

			unsigned int oldSelName = _selectedName;
			Selectable * oldSelection = _selectedObject;
			// deselect the currently selected node
			if ( _selectedObject ) {
				_selectedObject->_selected = false;
			}

			_selectedObject = 0x0;
			_selectedName = 0;
			if ( hitCount ) {
				_selectedName = _buffer[3];
				unsigned int selDepth = _buffer[1];
				int baseIndex = 4;
				for ( int i = 1; i < hitCount; ++i ) {
					if ( _buffer[ baseIndex + 1 ] < selDepth ) {
						_selectedName = _buffer[ baseIndex + 3 ];
						selDepth = _buffer[ baseIndex + 1 ];
					}
					baseIndex += 4;
				}
				if ( _selectedName > 0 && _selectedName <= (unsigned int)_selectables.size() ) {
					_selectedObject = _selectables[ _selectedName - 1 ];
					_selectedObject->_selected = true;
				}
			}

			return _selectedObject != oldSelection || oldSelName != _selectedName;
		}

		///////////////////////////////////////////////////////////////////////////

	}	// namespace SceneGraph
}	// namespace MengeVis
