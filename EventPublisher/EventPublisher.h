/*
  ==============================================================================

    EventPublisher.h
    Created: 22 May 2015 3:31:50pm
    Author:  Christopher Stawarz

  ==============================================================================
*/

#ifndef EventPublisher_H_INCLUDED
#define EventPublisher_H_INCLUDED

#include <ProcessorHeaders.h>

#ifdef ZEROMQ
    #ifdef WIN32
        #include <zmq.h>
        #include <zmq_utils.h>
    #else
        #include <zmq.h>
    #endif
#endif

#include <memory>


class EventPublisher : public GenericProcessor
{
public:
    EventPublisher();

    AudioProcessorEditor* createEditor() override;

    int getListeningPort() const;
    void setListeningPort (int port, bool forceRestart = false);

    void process (AudioSampleBuffer& continuousBuffer) override;
    void handleEvent (const EventChannel* channelInfo, const MidiMessage& event, int samplePosition = 0) override;
	void handleSpike(const SpikeChannel* channelInfo, const MidiMessage& event, int samplePosition = 0) override;
	void updateSettings() override;

    void saveCustomParametersToXml (XmlElement* parentElement) override;
    void loadCustomParametersFromXml() override;

	void setPublishTtl(bool status) { publishTtl = status; }
	bool getPublishTtl() { return publishTtl; }

	void setPublishText(bool status) { publishText = status; }
	bool getPublishText() { return publishText; }

	void setTtlChannelStatus(int channel, bool enabled);
	bool getTtlChannelStatus(int channel);

private:
	void sendEvent(const MidiMessage& event, const EventChannel* eventInfo) const;
    static std::shared_ptr<void> getZMQContext();
    static void closeZMQSocket (void* socket);

    const std::shared_ptr<void> zmqContext;
    std::unique_ptr<void, decltype (&closeZMQSocket)> zmqSocket;
    int listeningPort;

	bool publishTtl;
	bool publishText;
	Array<bool> ttlChannelStatus;
};


#endif  // EventPublisher_H_INCLUDED
