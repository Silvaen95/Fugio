#include "portaudiooutputnode.h"

#include <QSettings>
#include <QDebug>
#include <QPushButton>
#include <QInputDialog>

#include <fugio/audio/audio_producer_interface.h>
#include <fugio/context_interface.h>
#include <fugio/pin_signals.h>
#include <fugio/core/uuid.h>

PortAudioOutputNode::PortAudioOutputNode( QSharedPointer<fugio::NodeInterface> pNode ) :
	NodeControlBase( pNode ), mInstance( nullptr ), mVolume( 1.0f )
{
	mPinInputAudio = pinInput( "Audio" );

	mPinInputVolume = pinInput( "Volume" );

	mPinInputVolume->registerPinInputType( PID_FLOAT );

	mPinInputVolume->setValue( 1.0f );

#if defined( PORTAUDIO_SUPPORTED )
	mDeviceName = DevicePortAudio::deviceOutputDefaultName();
#endif

	mNode->context()->registerPlayhead( this );
}

PortAudioOutputNode::~PortAudioOutputNode()
{
	mNode->context()->unregisterPlayhead( this );
}

bool PortAudioOutputNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

#if defined( PORTAUDIO_SUPPORTED )
	PaDeviceIndex		DevIdx = DevicePortAudio::deviceOutputNameIndex( mDeviceName );

	if( DevIdx == paNoDevice )
	{
		return( false );
	}

	if( ( mPortAudio = DevicePortAudio::newDevice( DevIdx ) ) )
	{
		mPortAudio->addOutput( this );
	}

	return( true );
#else
	mNode->setStatus( fugio::NodeInterface::Error );
	mNode->setStatusMessage( "No PortAudio Support" );

	return( false );
#endif
}

bool PortAudioOutputNode::deinitialise()
{
	mProducerMutex.lock();

	if( mInstance )
	{
		delete mInstance;

		mInstance = nullptr;
	}

	mProducerMutex.unlock();

	if( mPortAudio )
	{
		mPortAudio->remOutput( this );

		mPortAudio.clear();
	}

	return( NodeControlBase::deinitialise() );
}

void PortAudioOutputNode::audio( qint64 pSamplePosition, qint64 pSampleCount, int pChannelOffset, int pChannelCount, void **pBuffers, void *pInstanceData ) const
{
	Q_UNUSED( pInstanceData )

	const float		InputVolume = mVolume;

	mProducerMutex.lock();

	if( mInstance )
	{
		mInstance->audio( pSamplePosition, pSampleCount, pChannelOffset, pChannelCount, pBuffers );
	}

	mProducerMutex.unlock();

	// if volume is 1 (the default) then we don't need to do any further processing

	if( InputVolume == 1.0f )
	{
		return;
	}

	for( int c = pChannelOffset ; c < pChannelOffset + pChannelCount ; c++ )
	{
		float		*AudDat = (float *)pBuffers[ c ];

		// if the volume is 0 then we can just set everything to zero too
		// we could not call the producer instead, but this would interfere with
		// other processing further up the chain

		if( InputVolume == 0.0f )
		{
			memset( AudDat, 0, sizeof( float ) * pSampleCount );
		}
		else
		{
			for( qint64 i = 0 ; i < pSampleCount ; i++ )
			{
				*AudDat++ *= InputVolume;
			}
		}
	}
}

fugio::AudioInstanceBase *PortAudioOutputNode::audioAllocInstance( qreal pSampleRate, fugio::AudioSampleFormat pSampleFormat, int pChannels )
{
	if( mPinInputAudio->isConnectedToActiveNode() && !mPinInputAudio->connectedNode()->isInitialised() )
	{
		return( nullptr );
	}

	fugio::AudioProducerInterface	*IAP = input<fugio::AudioProducerInterface *>( mPinInputAudio );

	if( IAP )
	{
		QMutexLocker		Lock( &mProducerMutex );

		mInstance = IAP->audioAllocInstance( pSampleRate, pSampleFormat, pChannels );
	}

	return( nullptr );
}

//void PortAudioOutputNode::audioFreeInstance( void *pInstanceData )
//{
//	Q_UNUSED( pInstanceData )

//	QMutexLocker		Lock( &mProducerMutex );

//	if( mProducer )
//	{
//		mProducer->audioFreeInstance( mInstance );

//		mProducer = nullptr;

//		mInstance = nullptr;
//	}
//}

void PortAudioOutputNode::audioDeviceSelected( const QString &pDeviceName )
{
	if( mPortAudio )
	{
		mPortAudio->remOutput( this );

		mPortAudio.clear();
	}

	mDeviceName = pDeviceName;

#if defined( PORTAUDIO_SUPPORTED )
	PaDeviceIndex		DevIdx = DevicePortAudio::deviceOutputNameIndex( mDeviceName );

	if( DevIdx == paNoDevice )
	{
		return;
	}

	if( ( mPortAudio = DevicePortAudio::newDevice( DevIdx ) ) )
	{
		mPortAudio->addOutput( this );
	}
#endif
}

void PortAudioOutputNode::clicked()
{
#if defined( PORTAUDIO_SUPPORTED )
	bool		OK;
	QString		NewDev = QInputDialog::getItem( nullptr, mNode->name(), tr( "Select Audio Device" ), DevicePortAudio::deviceOutputNameList(), DevicePortAudio::deviceOutputNameList().indexOf( mDeviceName ), false, &OK );

	if( OK && NewDev != mDeviceName )
	{
		audioDeviceSelected( NewDev );
	}
#endif
}

void PortAudioOutputNode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	mVolume = qMax( 0.0f, variant( mPinInputVolume ).toFloat() );

	QMutexLocker		Lock( &mProducerMutex );

	fugio::AudioProducerInterface	*IAP = input<fugio::AudioProducerInterface *>( mPinInputAudio );

	if( ( !IAP && mInstance ) || ( IAP && !IAP->isValid( mInstance ) ) )
	{
		delete mInstance;

		mInstance = nullptr;
	}

	if( !mPinInputAudio->isConnectedToActiveNode() || !mPinInputAudio->connectedNode()->isInitialised() )
	{
		return;
	}

	if( IAP && !mInstance )
	{
		qreal						SmpRte = mPortAudio ? mPortAudio->outputSampleRate() : 0;
		fugio::AudioSampleFormat	SmpFmt = fugio::AudioSampleFormat::Format32FS;
		int							ChnCnt = mPortAudio ? mPortAudio->outputChannelCount() : 0;

		mInstance = IAP->audioAllocInstance( SmpRte, SmpFmt, ChnCnt );
	}
}

QWidget *PortAudioOutputNode::gui()
{
	QPushButton		*GUI = new QPushButton( tr( "Choose Device" ) );

	if( GUI )
	{
		connect( GUI, SIGNAL(released()), this, SLOT(clicked()) );
	}

	return( GUI );
}

void PortAudioOutputNode::loadSettings( QSettings &pSettings )
{
	mDeviceName = pSettings.value( "device", mDeviceName ).toString();

	emit audioDeviceChanged( mDeviceName );
}

void PortAudioOutputNode::saveSettings( QSettings &pSettings ) const
{
	pSettings.setValue( "device", mDeviceName );
}


int PortAudioOutputNode::audioChannels() const
{
	return( mPortAudio ? mPortAudio->audioChannels() : 0 );
}

qreal PortAudioOutputNode::audioSampleRate() const
{
	return( mPortAudio ? mPortAudio->audioSampleRate() : 0 );
}

fugio::AudioSampleFormat PortAudioOutputNode::audioSampleFormat() const
{
	return( mPortAudio ? mPortAudio->audioSampleFormat() : fugio::AudioSampleFormat::FormatUnknown );
}

qint64 PortAudioOutputNode::audioLatency() const
{
	return( mPortAudio ? mPortAudio->audioLatency() : 0 );
}
