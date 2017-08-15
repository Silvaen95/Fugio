#ifndef SYPHONPIN_H
#define SYPHONPIN_H

#include "opengl_includes.h"

#include <QObject>

#include <fugio/pin_control_interface.h>
#include <fugio/opengl/texture_interface.h>

#include <fugio/pincontrolbase.h>

#include "syphonreceiver.h"

using namespace fugio;

class SyphonPin : public fugio::PinControlBase, public fugio::OpenGLTextureInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::OpenGLTextureInterface )

public:
	Q_INVOKABLE explicit SyphonPin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~SyphonPin( void );

#if defined( SYPHON_SUPPORTED )
	SyphonReceiver &receiver( void )
	{
		return( mReceiver );
	}

	void updateFromReceiver( void );

#endif

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE
	{
		return( QString( "%1: %2x%3" ).arg( mName ).arg( mSize.x() ).arg( mSize.y() ) );
	}

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "Syphon" );
	}

	//-------------------------------------------------------------------------
	// InterfaceTexture

	virtual QVector3D textureSize( void ) const Q_DECL_OVERRIDE;

	virtual QVector3D size( void ) const Q_DECL_OVERRIDE;

	virtual quint32 srcTexId( void ) const Q_DECL_OVERRIDE;
	virtual quint32 dstTexId( void ) const Q_DECL_OVERRIDE;

	virtual QOpenGLTexture::Target target( void ) const Q_DECL_OVERRIDE;

	virtual QOpenGLTexture::PixelFormat format( void ) const Q_DECL_OVERRIDE;

	virtual QOpenGLTexture::TextureFormat internalFormat( void ) const Q_DECL_OVERRIDE;

	virtual QOpenGLTexture::PixelType type( void ) const Q_DECL_OVERRIDE;

	virtual int filterMin( void ) const Q_DECL_OVERRIDE;
	virtual int filterMag( void ) const Q_DECL_OVERRIDE;
	virtual int wrapS( void ) const Q_DECL_OVERRIDE;
	virtual int wrapT( void ) const Q_DECL_OVERRIDE;
	virtual int wrapR( void ) const Q_DECL_OVERRIDE;
	virtual bool genMipMaps( void ) const Q_DECL_OVERRIDE;

	virtual void setSize( qint32 pWidth, qint32 pHeight = 0, qint32 pDepth = 0 ) Q_DECL_OVERRIDE;

	virtual void setSize( const QVector3D &pSize ) Q_DECL_OVERRIDE;

	virtual void setTarget( QOpenGLTexture::Target pTarget ) Q_DECL_OVERRIDE;

	virtual void setFormat( QOpenGLTexture::PixelFormat pFormat ) Q_DECL_OVERRIDE;

	virtual void setType( QOpenGLTexture::PixelType pType ) Q_DECL_OVERRIDE;

	virtual void setInternalFormat( QOpenGLTexture::TextureFormat pInternalFormat ) Q_DECL_OVERRIDE;

	virtual void update( void ) Q_DECL_OVERRIDE;

	virtual void update( const unsigned char *pData, int pDataSize, int pLineSize, int pCubeFaceIndex = 0 ) Q_DECL_OVERRIDE;

	virtual void setFilter( QOpenGLTexture::Filter pMin, QOpenGLTexture::Filter pMag ) Q_DECL_OVERRIDE;

	virtual void setWrap( QOpenGLTexture::WrapMode pX, QOpenGLTexture::WrapMode pY, QOpenGLTexture::WrapMode pZ ) Q_DECL_OVERRIDE;

	virtual void setGenMipMaps( bool pGenMipMaps ) Q_DECL_OVERRIDE;

	virtual void free( void ) Q_DECL_OVERRIDE;

	virtual QImage image( void ) Q_DECL_OVERRIDE;

	virtual void srcBind( void ) Q_DECL_OVERRIDE;
	virtual void dstBind( void ) Q_DECL_OVERRIDE;

	virtual void release( void ) Q_DECL_OVERRIDE;

	virtual quint32 fbo( bool ) Q_DECL_OVERRIDE
	{
		return( 0 );
	}

	virtual void freeFbo( void ) Q_DECL_OVERRIDE
	{

	}

	virtual quint32 fboMultiSample( int, bool ) Q_DECL_OVERRIDE
	{
		return( 0 );
	}

	virtual void swapTexture( void ) Q_DECL_OVERRIDE
	{

	}

	virtual void setDoubleBuffered( bool ) Q_DECL_OVERRIDE
	{
	}

	virtual bool isDoubleBuffered( void ) const Q_DECL_OVERRIDE
	{
		return( false );
	}

	virtual bool isDepthTexture( void ) const Q_DECL_OVERRIDE
	{
		return( false );
	}

	virtual QOpenGLTexture::ComparisonFunction compare() const Q_DECL_OVERRIDE
	{
		return( QOpenGLTexture::CompareAlways );
	}

	virtual void setCompare( QOpenGLTexture::ComparisonFunction pCompare ) Q_DECL_OVERRIDE
	{
		Q_UNUSED( pCompare )
	}

	virtual OpenGLTextureDescription textureDescription() const Q_DECL_OVERRIDE
	{
		return( OpenGLTextureDescription() );
	}

	virtual void setTextureDescription( const OpenGLTextureDescription &pDescription ) Q_DECL_OVERRIDE
	{
		Q_UNUSED( pDescription )
	}

	//-------------------------------------------------------------------------

private:
#if defined( SYPHON_SUPPORTED )
	SyphonReceiver			 mReceiver;
#endif

	QString					 mName;
	QVector3D				 mSize;
};

#endif // SYPHONPIN_H
