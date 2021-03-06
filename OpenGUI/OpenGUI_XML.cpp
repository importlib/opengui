// OpenGUI (http://opengui.sourceforge.net)
// This source code is released under the BSD License
// See LICENSE.TXT for details

#include "tinyxml.h"
#include "OpenGUI_XML.h"
#include "OpenGUI_Exception.h"
#include "OpenGUI_Resource.h"
#include "OpenGUI_ResourceProvider.h"
#include "OpenGUI_System.h"
namespace OpenGUI {
	//############################################################################
	void XMLDoc::clear() {
		while ( mChildren.size() > 0 ) {
			delete mChildren.front();
		}
	}
	//############################################################################
	void XMLDoc::loadFile( const String& filename ) {
		mFileName = filename;
		TiXmlDocument doc;
		Resource_CStr res;
		ResourceProvider* resProvider = System::getSingleton()._getResourceProvider();
		resProvider->loadResource( filename, res );
		doc.Parse( res.getString() );
		TiXmlElement* root = doc.RootElement();
		if ( root ) {
			do {
				XMLNode* node = new XMLNode( root, this );
			} while (( root = root->NextSiblingElement() ) );
		}
	}
	//############################################################################
	void XMLDoc::saveFile( const String& filename ) {
		mFileName = filename;
		TiXmlDocument doc;
		TiXmlDeclaration decl( "1.0", "UTF-8", "" );
		doc.InsertEndChild( decl );
		for ( XMLNodeList::iterator iter = mChildren.begin(); iter != mChildren.end(); iter++ ) {
			XMLNode* node = ( *iter );
			void* out = node->_totxml();
			TiXmlElement* element = ( TiXmlElement* )out;
			doc.LinkEndChild( element );
		}
		doc.SaveFile( filename );
	}
	//############################################################################
	//############################################################################
	//############################################################################
	XMLNodeContainer::~XMLNodeContainer() {
		while ( mChildren.size() > 0 ) {
			delete mChildren.front();
		}
	}
	//############################################################################
	XMLNodeList XMLNodeContainer::getChildren()const {
		return mChildren;
	}
	//############################################################################
	XMLNodeList XMLNodeContainer::getChildren( const String& tagName )const {
		XMLNodeList ret;
		for ( XMLNodeList::const_iterator iter = mChildren.begin(); iter != mChildren.end(); iter++ ) {
			XMLNode* child = ( *iter );
			if ( tagName == child->getTagName() ) {
				ret.push_back( child );
			}
		}
		return ret;
	}
	//############################################################################
	void XMLNodeContainer::_doNotifyChildDetach( XMLNode* childPtr ) {
		mChildren.remove( childPtr );
	}
	//############################################################################
	void XMLNodeContainer::_doNotifyChildAttach( XMLNode* childPtr ) {
		mChildren.push_back( childPtr );
	}
	//############################################################################
	void XMLNodeContainer::_notifyChildDetach( XMLNodeContainer* parent, XMLNode* childPtr ) {
		parent->_doNotifyChildDetach( childPtr );
	}
	//############################################################################
	void XMLNodeContainer::_notifyChildAttach( XMLNodeContainer* parent, XMLNode* childPtr ) {
		parent->_doNotifyChildAttach( childPtr );
	}
	//############################################################################
	//############################################################################
	//############################################################################
	XMLNode::XMLNode( const String& tagName, XMLNodeContainer* parentPtr )
			: mTagName( tagName ), mParent( parentPtr ) {
		if ( mParent )
			_notifyChildAttach( mParent, this );
	}
	//############################################################################
	XMLNode::~XMLNode() {
		if ( mParent )
			_notifyChildDetach( mParent, this );
	}
	//############################################################################
	XMLNodeContainer* XMLNode::getParent()const {
		return mParent;
	}
	//############################################################################
	void XMLNode::setParent( XMLNodeContainer* newParentPtr ) {
		if ( mParent )
			_notifyChildDetach( mParent, this );
		mParent = newParentPtr;
		if ( mParent )
			_notifyChildAttach( mParent, this );
	}
	//############################################################################
	const String& XMLNode::getAttribute( const String& name ) const {
		XMLAttributeMap::const_iterator iter = mAttributes.find( name );
		if ( iter == mAttributes.end() )
			OG_THROW( Exception::ERR_ITEM_NOT_FOUND,
					  "Attribute \"" + name + "\" not found in tag <" + getTagName() + ">",
					  __FUNCTION__ );
		return iter->second;
	}
	//############################################################################
	XMLAttributeMap XMLNode::getAttributes() const {
		XMLAttributeMap attribs = mAttributes;
		return attribs;
	}
	//############################################################################
	void XMLNode::setAttribute( const String& name, const String& value ) {
		mAttributes[name] = value;
	}
	//############################################################################
	void XMLNode::removeAttribute( const String& name ) {
		mAttributes.erase( name );
	}
	//############################################################################
	bool XMLNode::hasAttribute( const String& name ) const {
		XMLAttributeMap::const_iterator iter = mAttributes.find( name );
		if ( iter != mAttributes.end() )
			return true;
		return false;
	}
	//############################################################################
	const String& XMLNode::getText() const {
		return mText;
	}
	//############################################################################
	void XMLNode::setText( const String& text ) {
		mText = text;
	}
	//############################################################################
	XMLDoc* XMLNode::getDoc() const {
		if ( mParent ) {
			XMLDoc* doc = dynamic_cast<XMLDoc*>( mParent );
			if ( doc )
				return doc;
			else {
				XMLNode* pnode = dynamic_cast<XMLNode*>( mParent );
				if ( pnode )
					return pnode->getDoc();
				else
					return 0;
			}
		}
		return 0;
	}
	//############################################################################
	XMLNode::XMLNode( void* txmle, XMLNodeContainer* parentPtr ): mParent( parentPtr ) {
		TiXmlElement* mytxml = ( TiXmlElement* ) txmle;
		mTagName = mytxml->Value();
		if ( mParent )
			_notifyChildAttach( mParent, this );
		TiXmlAttribute* attrib = mytxml->FirstAttribute();
		if ( attrib ) {
			do {
				setAttribute( attrib->Name(), attrib->Value() );
			} while (( attrib = attrib->Next() ) );
		}
		const TiXmlNode* child_node = mytxml->FirstChild();
		if ( child_node ) {
			bool hasText = false;
			do {
				const TiXmlElement* child_element = child_node->ToElement();
				if ( child_element ) {
					if ( child_element->Value() == "TEXT" && !hasText ) {
						hasText = true;
						setText( child_element->GetText() );
					} else {
						void* c = ( void* ) child_element;
						new XMLNode( c, this );
					}
				}
			} while ( child_node = mytxml->IterateChildren( child_node ) );
		}
	}
	//############################################################################
	void* XMLNode::_totxml() {
		TiXmlElement* mytxml = new TiXmlElement( getTagName() );
		XMLAttributeMap::iterator iter = mAttributes.begin();
		while ( iter != mAttributes.end() ) {
			const String& aName = iter->first;
			const String& aValue = iter->second;
			mytxml->SetAttribute( aName, aValue );
			iter++;
		}
		if ( getText() != "" ) {
			TiXmlElement* texttxml = new TiXmlElement( "TEXT" );
			TiXmlText* thetext = new TiXmlText( getText() );
			texttxml->LinkEndChild( thetext );
			mytxml->LinkEndChild( texttxml );
		}

		XMLNodeList::iterator child_iter = mChildren.begin();
		while ( child_iter != mChildren.end() ) {
			XMLNode* child = ( *child_iter );
			TiXmlElement* childtxml = ( TiXmlElement* )child->_totxml();
			mytxml->LinkEndChild( childtxml );
			child_iter++;
		}
		return ( void* )mytxml;
	}
	//############################################################################
	/*! For example, if the tag were \c thisTag in the following code:
	\code
	<level1>
	<level2>
		<thisTag />
	</level2>
	</level1>
	\endcode
	Then the returned path would be "/level1/level2/".
	The path is always comprised of a leading "/", following by \c tagName + "/" for each proceeding
	node before the current node.
	*/
	String XMLNode::getPath() const {
		String path = "/";
		XMLNode* parentNode = dynamic_cast<XMLNode*>( mParent );
		if ( parentNode ) {
			parentNode->_buildPath( path );
		}
		return path;
	}
	//############################################################################
	void XMLNode::_buildPath( String& path ) {
		XMLNode* parentNode = dynamic_cast<XMLNode*>( mParent );
		if ( parentNode ) {
			parentNode->_buildPath( path );
		}
		path = path + mTagName + "/";
	}
	//############################################################################
	String XMLNode::dump() const {
		std::stringstream out;
		out << "<" << mTagName;
		for ( XMLAttributeMap::const_iterator iter = mAttributes.begin(); iter != mAttributes.end(); iter++ ) {
			out << " " << iter->first << "=\"" << iter->second << "\"";
		}
		out << ">";
		if ( mText.length() > 0 ) {
			out << "#- " << mText;
		}
		return out.str();
	}
	//############################################################################
}//namespace OpenGUI{
