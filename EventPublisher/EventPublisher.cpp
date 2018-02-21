/*
  ==============================================================================

    EventPublisher.cpp
    Created: 22 May 2015 3:31:50pm
    Author:  Christopher Stawarz

  ==============================================================================
*/

#include <sstream>
#include <iomanip>

#include "EventPublisher.h"
#include "EventPublisherEditor.h"


std::shared_ptr<void> EventPublisher::getZMQContext() {
    // Note: C++11 guarantees that initialization of static local variables occurs exactly once, even
    // if multiple threads attempt to initialize the same static local variable concurrently.
#ifdef ZEROMQ
    static const std::shared_ptr<void> ctx(zmq_ctx_new(), zmq_ctx_destroy);
#else
    static const std::shared_ptr<void> ctx;
#endif
    return ctx;
}


void EventPublisher::closeZMQSocket(void* socket)
{
#ifdef ZEROMQ
    zmq_close(socket);
#endif
}


EventPublisher::EventPublisher()
    : GenericProcessor("Event Publisher"),
    zmqContext(getZMQContext()),
    zmqSocket(nullptr, &closeZMQSocket),
    listeningPort(0),
	publishTtl(true),
	publishText(true)
{
    setProcessorType (PROCESSOR_TYPE_SINK);

    setListeningPort(5557);

	for (int i=0; i<8; i++)
	{
		ttlChannelStatus.add(true);
	}
}


AudioProcessorEditor* EventPublisher::createEditor()
{
    editor = new EventPublisherEditor(this, true);
    return editor;
}


int EventPublisher::getListeningPort() const
{
    return listeningPort;
}


void EventPublisher::setListeningPort(int port, bool forceRestart)
{
    if ((listeningPort != port) || forceRestart)
    {
#ifdef ZEROMQ
        zmqSocket.reset(zmq_socket(zmqContext.get(), ZMQ_PUB));
        if (!zmqSocket)
        {
            std::cout << "Failed to create socket: " << zmq_strerror(zmq_errno()) << std::endl;
            return;
        }

        String url = String("tcp://*:") + String(port);
        if (0 != zmq_bind(zmqSocket.get(), url.toRawUTF8()))
        {
            std::cout << "Failed to open socket: " << zmq_strerror(zmq_errno()) << std::endl;
            return;
        }
#endif

        listeningPort = port;
    }
}


void EventPublisher::setTtlChannelStatus(int channel, bool enabled)
{
	if (channel >= 0 && channel < 8)
	{
		ttlChannelStatus.set(channel, enabled);
	}
}


bool EventPublisher::getTtlChannelStatus(int channel)
{
	bool status = false;
	if (channel >= 0 && channel < 8)
	{
		status = ttlChannelStatus[channel];
	}

	return status;
}


void EventPublisher::process(AudioSampleBuffer& continuousBuffer)
{
    checkForEvents(true);
}


void EventPublisher::updateSettings()
{
	if (editor != nullptr)
	{
		editor->updateSettings();
	}
}



void EventPublisher::sendEvent(const MidiMessage& event, const EventChannel* eventInfo) const
{
	float eventSampleRate = eventInfo->getSampleRate();
	double timestampSeconds = double(Event::getTimestamp(event)) / eventSampleRate;
	uint16 type = Event::getBaseType(event);

#ifdef ZEROMQ
    if ((Event::getEventType(event) == EventChannel::TTL) && publishTtl)
    {
		TTLEventPtr ttl = TTLEvent::deserializeFromMessage(event, eventInfo);

        const int eventId = ttl->getState() ? 1 : 0;
        const int eventChannel = ttl->getChannel();

		if (ttlChannelStatus[eventChannel])
		{
			// not particularly efficient but should work on different platforms
			// without any complicated parsing.
			// format: TTL time ttl_channel state
			// note that ttl_channel is 1-based to make it consistent what is
			// shown in the GUI
	 		std::ostringstream s;
	  		s << "TTL " << std::setprecision(12) << timestampSeconds;
			s << " " << eventChannel+1;
			s << " " << eventId;
			String msg = String(s.str());

			if(zmq_send(zmqSocket.get(), msg.getCharPointer(), msg.length(), ZMQ_DONTWAIT) == -1)
			{
				std::cout << "Could not publish TTL message: " << msg.toRawUTF8() << "\n";
			}
		}
	}
	else if ((Event::getEventType(event) == EventChannel::TEXT) && publishText)
	{

		TextEventPtr text = TextEvent::deserializeFromMessage(event, eventInfo);

		// format: TEXT time message_text
 		std::ostringstream s;
  		s << "TEXT " << std::setprecision(12) << timestampSeconds << " ";
		String msg = String(s.str()) + text->getText();

		if(zmq_send(zmqSocket.get(), msg.getCharPointer(), msg.length(), ZMQ_DONTWAIT) == -1)
		{
			std::cout << "Could not publish text message: " << msg.toRawUTF8() << "\n";
		}
	}

#endif
}

void EventPublisher::handleEvent(const EventChannel* channelInfo, const MidiMessage& event, int samplePosition)
{
	sendEvent(event, channelInfo);
}

void EventPublisher::handleSpike(const SpikeChannel* channelInfo, const MidiMessage& event, int samplePosition)
{
//	sendEvent(event, channelInfo);
}

void EventPublisher::saveCustomParametersToXml(XmlElement* parentElement)
{
    XmlElement* mainNode = parentElement->createNewChildElement("EventPublisher");
    mainNode->setAttribute("Port", listeningPort);
	mainNode->setAttribute("PublishTTL", publishTtl);
	mainNode->setAttribute("PublishText", publishText);

	String s = "";
	for (int i=0; i<ttlChannelStatus.size(); i++)
	{
		if (i > 0)
			s += ",";
		s += String(ttlChannelStatus[i]);
	}
	mainNode->setAttribute("TTLChannelStatus", s);
}


void EventPublisher::loadCustomParametersFromXml()
{
    if (parametersAsXml)
    {
        forEachXmlChildElement(*parametersAsXml, node)
        {
            if (node->hasTagName("EventPublisher"))
            {
                setListeningPort(node->getIntAttribute("Port"));
				setPublishTtl(node->getBoolAttribute("PublishTTL"));
				setPublishText(node->getBoolAttribute("PublishText"));

				String s = node->getStringAttribute("TTLChannelStatus");

				StringArray tokens;
				tokens.addTokens(s, ",", "\"");
				for (int i=0; i<tokens.size(); i++)
				{
				    setTtlChannelStatus(i, tokens[i].getIntValue() > 0); // holds next token
				}

				updateSettings();
            }
        }
    }
}

