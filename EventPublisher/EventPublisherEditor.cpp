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
    eventsLabel->setBounds(10,75,50,25);
    eventsLabel->setFont(Font("Small Text", 12, Font::plain));
    eventsLabel->setColour(Label::textColourId, Colours::darkgrey);
    addAndMakeVisible(eventsLabel);

    ttlButton = new UtilityButton("TTL", Font("Default", 10, Font::plain));
    ttlButton->addListener(this);
    ttlButton->setBounds(70, 80, 40, 18);
    ttlButton->setClickingTogglesState(true);
    ttlButton->setToggleState(p->getPublishTtl(), dontSendNotification);
    ttlButton->setTooltip("Send TTL events");
    addAndMakeVisible(ttlButton);

    textButton = new UtilityButton("Text", Font("Default", 10, Font::plain));
    textButton->addListener(this);
    textButton->setBounds(120, 80, 40, 18);
    textButton->setClickingTogglesState(true);
    textButton->setToggleState(p->getPublishText(), dontSendNotification);
    textButton->setTooltip("Send text events");
    addAndMakeVisible(textButton);

	// TTL channels
   	for (int i=0; i<8; i++)
    {
        UtilityButton* button = new UtilityButton(String(i+1), Font("Default", 10, Font::plain));
		button->setBounds(10+i*19, 105, 17, 18);
        button->addListener(this);
        button->setToggleState(p->getTtlChannelStatus(i), dontSendNotification);
        button->setClickingTogglesState(true);
		//button->setTooltip("Send TTL events for channel " + String(i+1));
		addAndMakeVisible(button);

        channelButtons.add(button);
	}


    setEnabledState(false);
}


void EventPublisherEditor::updateSettings()
{
	EventPublisher* p = (EventPublisher*)getProcessor();

	portLabel->setText(String(p->getListeningPort()), dontSendNotification);
	ttlButton->setToggleState(p->getPublishTtl(), dontSendNotification);
	textButton->setToggleState(p->getPublishText(), dontSendNotification);

	for (int i=0; i<channelButtons.size(); i++)
	{
		channelButtons[i]->setToggleState(p->getTtlChannelStatus(i), dontSendNotification);
	}
}


void EventPublisherEditor::buttonEvent(Button* button)
{
	EventPublisher* p = (EventPublisher*)getProcessor();

    if (button == restartConnection)
    {
        p->setListeningPort(p->getListeningPort(), true);
    }
	else if (button == ttlButton)
	{
		p->setPublishTtl(button->getToggleState());
	}
	else if (button == textButton)
	{
		p->setPublishText(button->getToggleState());
	}
	else
	{
		for (int i=0; i<channelButtons.size(); i++)
		{
			if (button == channelButtons[i])
			{
				p->setTtlChannelStatus(i, button->getToggleState());
			}
		}
	}
}


void EventPublisherEditor::labelTextChanged(juce::Label* label)
{
	EventPublisher* p = (EventPublisher*)getProcessor();

    if (label == portLabel)
    {
        p->setListeningPort(label->getTextValue().getValue());
    }
}

