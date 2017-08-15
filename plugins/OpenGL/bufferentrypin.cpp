#include "bufferentrypin.h"

#include <QOpenGLFunctions_3_0>
#include <QOpenGLFunctions_4_1_Core>

BufferEntryPin::BufferEntryPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), mSize( 0 ), mType( 0 ), mNormalised( GL_FALSE ), mTypeSize( 0 )
{

}

void BufferEntryPin::setType( GLenum pType )
{
	mType = pType;

	switch( mType )
	{
		case GL_BYTE:
		case GL_UNSIGNED_BYTE:
			mTypeSize = 1;
			break;

		case GL_SHORT:
		case GL_UNSIGNED_SHORT:
		case GL_HALF_FLOAT:
			mTypeSize = 2;
			break;

		case GL_INT:
		case GL_UNSIGNED_INT:
		case GL_FLOAT:
	case GL_FIXED:
		case GL_INT_2_10_10_10_REV:
		case GL_UNSIGNED_INT_2_10_10_10_REV:
		case GL_UNSIGNED_INT_10F_11F_11F_REV:
			mTypeSize = 4;
			break;

		case GL_DOUBLE:
			mTypeSize = 8;
			break;

		default:
			mTypeSize = 0;
			break;
	}
}

const GLvoid *BufferEntryPin::bind( GLuint pIndex, GLsizei pStride, const GLvoid *pPointer )
{
	initializeOpenGLFunctions();

	const GLvoid	*NextBuff = reinterpret_cast<const GLvoid *>( reinterpret_cast<const GLubyte *>( pPointer ) + entrySize() );

	glEnableVertexAttribArray( pIndex );

	QOpenGLFunctions_4_1_Core	*GL41 = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_1_Core>();

	if( GL41 && GL41->initializeOpenGLFunctions() )
	{
		switch( mType )
		{
			case GL_DOUBLE:
				GL41->glVertexAttribLPointer( pIndex, mSize, mType, pStride, pPointer );
				return( NextBuff );
		}
	}

	QOpenGLFunctions_3_0		*GL30 = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_0>();

	if( GL30 && GL30->initializeOpenGLFunctions() )
	{
		switch( mType )
		{
			case GL_BYTE:
			case GL_UNSIGNED_BYTE:
			case GL_SHORT:
			case GL_UNSIGNED_SHORT:
			case GL_INT:
			case GL_UNSIGNED_INT:
				GL30->glVertexAttribIPointer( pIndex, mSize, mType, pStride, pPointer );
				return( NextBuff );
		}
	}

	glVertexAttribPointer( pIndex, mSize, mType, mNormalised, pStride, pPointer );

	return( NextBuff );
}

void BufferEntryPin::bind( GLuint pIndex )
{
	Q_UNUSED( pIndex )
}

void BufferEntryPin::release( GLuint pIndex )
{
	initializeOpenGLFunctions();

	glDisableVertexAttribArray( pIndex );
}
