/*
  ==============================================================================

    EventPublisherEditor.cpp
    Created: 22 May 2015 3:34:30pm
    Author:  Christopher Stawarz

  ==============================================================================
*/

#include "EventPublisherEditor.h"
#include "EventPublisher.h"


EventPublisherEditor::EventPublisherEditor(GenericProcessor* parentNode, bool useDefaultParameterEditors)
    : GenericEditor(parentNode, useDefaultParameterEditors)

{
    desiredWidth = 180;

	EventPublisher* p = (EventPublisher*)getProcessor();

	// port
    urlLabel = new Label("Url", "Port");
    urlLabel->setBounds(10,30,140,18);
    urlLabel->setFont(Font("Small Text", 12, Font::plain));
    urlLabel->setColour(Label::textColourId, Colours::darkgrey);
    addAndMakeVisible(urlLabel);

    portLabel = new Label("Port", String(p->getListeningPort()));
    portLabel->setBounds(55,30,80,18);
    portLabel->setFont(Font("Default", 15, Font::plain));
    portLabel->setColour(Label::textColourId, Colours::white);
    portLabel->setColour(Label::backgroundColourId, Colours::grey);
    portLabel->setEditable(true);
    portLabel->addListener(this);
    addAndMakeVisible(portLabel);

    restartConnection = new UtilityButton("Restart Connection",Font("Default", 15, Font::plain));
    restartConnection->setBounds(10,55,150,18);
    restartConnection->addListener(this);
    addAndMakeVisible(restartConnection);

	// event types
    eventsLabel = new Label("Events", "Events");
    eventsLabel->setBounds(10,80,140,25);
    eventsLabel->setFont(Font("Small Text", 12, Font::plain));
    eventsLabel->setColour(Label::textColourId, Colours::darkgrey);
    addAndMakeVisible(eventsLabel);

    ttlButton = new UtilityButton("TTL", Font("Default", 10, Font::plain));
    ttlButton->addListener(this);
    ttlButton->setBounds(15, 105, 40, 18);
    ttlButton->setClickingTogglesState(true);
    ttlButton->setToggleState(p->getPublishTtl(), dontSendNotification);
    ttlButton->setTooltip("Send TTL events");
    addAndMakeVisible(ttlButton);

    textButton = new UtilityButton("Text", Font("Default", 10, Font::plain));
    textButton->addListener(this);
    textButton->setBounds(60, 105, 40, 18);
    textButton->setClickingTogglesState(true);
    textButton->setToggleState(p->getPublishText(), dontSendNotification);
    textButton->setTooltip("Send text events");
    addAndMakeVisible(textButton);

    setEnabledState(false);
}


void EventPublisherEditor::updateSettings()
{
	EventPublisher* p = (EventPublisher*)getProcessor();

	portLabel->setText(String(p->getListeningPort()), dontSendNotification);
	ttlButton->setToggleState(p->getPublishTtl(), dontSendNotification);
	textButton->setToggleState(p->getPublishText(), dontSendNotification);
}


void EventPublisherEditor::buttonEvent(Button* button)
{
    if (button == restartConnection)
    {
        EventPublisher* p = (EventPublisher*)getProcessor();
        p->setListeningPort(p->getListeningPort(), true);
    }
	else if (button == ttlButton)
	{
		EventPublisher* p = (EventPublisher*)getProcessor();
		p->setPublishTtl(button->getToggleState());
	}
	else if (button == textButton)
	{
		EventPublisher* p = (EventPublisher*)getProcessor();
		p->setPublishText(button->getToggleState());
	}
}


void EventPublisherEditor::labelTextChanged(juce::Label* label)
{
    if (label == portLabel)
    {
        Value val = label->getTextValue();

        EventPublisher* p = (EventPublisher*)getProcessor();
        p->setListeningPort(val.getValue());
    }
}

