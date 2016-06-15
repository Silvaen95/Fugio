#ifndef PORTAUDIOINPUTNODE_H
#define PORTAUDIOINPUTNODE_H

#include <QObject>
#include <QVector>

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/nodecontrolbase.h>

#include <fugio/portaudio/uuid.h>
#include <fugio/audio/audio_producer_interface.h>
#include <fugio/audio/audio_generator_interface.h>

#include "deviceportaudio.h"

class PortAudioInputNode : public fugio::NodeControlBase, public fugio::AudioProducerInterface, public fugio::AudioGeneratorInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::AudioProducerInterface fugio::AudioGeneratorInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Inputs audio data from PortAudio" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/PortAudio_Input" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit PortAudioInputNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~PortAudioInputNode( void );

	// NodeControlInterface interface
public:
	virtual bool initialise() Q_DECL_OVERRIDE;
	virtual bool deinitialise() Q_DECL_OVERRIDE;

	virtual void loadSettings(QSettings &pSettings) Q_DECL_OVERRIDE;
	virtual void saveSettings(QSettings &pSettings) Q_DECL_OVERRIDE;

	virtual QWidget *gui() Q_DECL_OVERRIDE;

	// InterfaceAudioProducer interface
public:
	virtual void audio( qint64 pSamplePosition, qint64 pSampleCount, int pChannelOffset, int pChannelCount, float **pBuffers, qint64 pLatency, void *pInstanceData ) const Q_DECL_OVERRIDE;
	virtual void *allocAudioInstance( qreal pSampleRate, fugio::AudioSampleFormat pSampleFormat, int pChannels ) Q_DECL_OVERRIDE;
	virtual void freeAudioInstance(void *pInstanceData) Q_DECL_OVERRIDE;

	// AudioGeneratorInterface interface
public:
	virtual int audioChannels() const Q_DECL_OVERRIDE;
	virtual qreal audioSampleRate() const Q_DECL_OVERRIDE;
	virtual fugio::AudioSampleFormat audioSampleFormat() const Q_DECL_OVERRIDE;

signals:
	//void audioUpdated( void );

	void audioDeviceChanged( const QString &pDeviceName );

protected slots:
	void audioDeviceSelected( const QString &pDeviceName );

	void audioInput( const float **pData, quint64 pFrames, int pChannelCount, qint64 pTimeStamp );

	void clicked( void );

protected:
	typedef struct
	{
		float		**mData;
		quint64		  mSamples;
		int			  mChannels;
		qint64		  mPosition;
	} AudioBuffer;

	void audioInput( AudioBuffer &AB, const float **pData, quint64 pSampleCount, int pChannelCount, qint64 pTimeStamp );

	typedef struct
	{
		qreal						mSampleRate;
		fugio::AudioSampleFormat	mSampleFormat;
		int							mChannels;
	} AudioInstanceData;

private:
	QSharedPointer<fugio::PinInterface>		 mPinAudio;
	fugio::AudioProducerInterface			*mValAudio;

	QSharedPointer<DevicePortAudio>			 mPortAudio;

	QList<AudioBuffer>						 mAudioBuffers;

	QString									 mDeviceName;
};

#endif // PORTAUDIOINPUTNODE_H
