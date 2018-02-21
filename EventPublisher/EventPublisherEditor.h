/*
  ==============================================================================

    EventPublisherEditor.h
    Created: 22 May 2015 3:34:30pm
    Author:  Christopher Stawarz

  ==============================================================================
*/

#ifndef EventPublisherEDITOR_H_INCLUDED
#define EventPublisherEDITOR_H_INCLUDED

#include <EditorHeaders.h>


/**

 User interface for the "EventPublisher" source node.

 @see EventPublisher

 */

class EventPublisherEditor : public GenericEditor, public Label::Listener
{
public:
    EventPublisherEditor(GenericProcessor* parentNode, bool useDefaultParameterEditors);

    void buttonEvent(Button* button) override;
    void labelTextChanged(juce::Label* label) override;
	void updateSettings() override;

private:
    ScopedPointer<UtilityButton> restartConnection;
    ScopedPointer<Label> urlLabel;
    ScopedPointer<Label> portLabel;

    ScopedPointer<Label> eventsLabel;
    ScopedPointer<UtilityButton> ttlButton;
    ScopedPointer<UtilityButton> textButton;

	OwnedArray<UtilityButton> channelButtons;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EventPublisherEditor);
};


#endif  // EventPublisherEDITOR_H_INCLUDED
