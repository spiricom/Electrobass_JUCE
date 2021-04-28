/*
 ==============================================================================
 
 ESComponents.h
 Created: 19 Feb 2021 12:42:05pm
 Author:  Matthew Wang
 
 ==============================================================================
 */

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "ESLookAndFeel.h"

class ESAudioProcessorEditor;

//==============================================================================

class MappingSource : public Component
{
public:
    MappingSource(ESAudioProcessorEditor& editor, const String &name, float* sources, int n, Colour colour);
    ~MappingSource() override;
    
    Colour getColour() { return colour; }
    
    void resized() override;
    
    float* getValuePointer();
    int getNumSourcePointers();
    
    Label label;
    DrawableButton button;
    
private:

    std::unique_ptr<Drawable> image;
    float* source;
    int numSourcePointers;
    
    Colour colour;
    
    ESLookAndFeel laf;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MappingSource)
};

//==============================================================================

class MappingTarget : public Slider,
                      public DragAndDropTarget
{
public:
    
    MappingTarget(const String &name, OwnedArray<SmoothedParameter>& target, int index);
    ~MappingTarget() override;
    
    String getTextFromValue(double value) override { return text; }
    Colour getColour() { return colour; }
    
    bool isInterestedInDragSource(const SourceDetails &dragSourceDetails) override;
    void itemDropped(const SourceDetails &dragSourceDetails) override;
    
    void resized() override;
    
    void mouseDown(const MouseEvent& event) override;
    void mouseDrag(const MouseEvent& event) override;

    void setText(String s);
    void setTextColour(Colour colour);
    
    void setMapping(MappingSource* source, float start, float end, HookOperation op);
    void removeMapping();
    
    void setMappingRange(float start, float end);
    
    static void menuCallback(int result, MappingTarget* target);
    
private:
    
    String text;
    OwnedArray<SmoothedParameter>& target;
    int index;
    bool sliderEnabled;
    
    Colour colour;
    
    ESLookAndFeel laf;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MappingTarget)
};

//==============================================================================

class ESDial : public Component,
               public Slider::Listener
{
public:
    
    ESDial(ESAudioProcessorEditor& editor, const String& name);
    ESDial(ESAudioProcessorEditor& editor, const String& name, Colour colour, float* source, int n);
    ESDial(ESAudioProcessorEditor& editor, const String& name, OwnedArray<SmoothedParameter>& target);
    ~ESDial() override;
    
    void paint(Graphics& g) override;
    void resized() override;
    
    void mouseDown(const MouseEvent& event) override;
    
    void sliderValueChanged(Slider* slider) override;
    
    void setText (const String& newText, NotificationType notification);
    void setFont (const Font& newFont);
    
    MappingTarget* getTarget(int index);
    MappingSource* getSource();
    
    Slider& getSlider() { return slider; }
    
private:
    
    Slider slider;
    OwnedArray<MappingTarget> t;
    std::unique_ptr<MappingSource> s;
    Label label;
    
    ESLookAndFeel laf;
    
    static const int numTargets = 3;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ESDial)
};

//==============================================================================

class ConnectionsContainer : public Component,
                             public Timer
{
public:
    ConnectionsContainer()
    {
        cursor = Drawable::createFromImageData(BinaryData::mappingtargeticon_svg,
                                               BinaryData::mappingtargeticon_svgSize);
        addChildComponent(cursor.get());
        
        setInterceptsMouseClicks(false, false);
        
        startTimer(10);
    }
    ~ConnectionsContainer() override {};
    
    void paint(Graphics &g) override
    {
        Point<float> mousePos = getMouseXYRelative().toFloat();
        cursor->drawWithin(g, Rectangle<float>(10.f, 10.f).withCentre(mousePos),
                           RectanglePlacement::fillDestination, 1.0f);
        if (incompleteConnection) connections.getLast()->setEnd(mousePos);
        g.setColour(Colours::gold);
        for (auto line : connections) g.drawLine(*line);
    }
    
    void timerCallback() override
    {
        repaint();
    }
    
    void startConnection(float x, float y)
    {
        connections.add(new Line<float>(x, y, x, y));
        incompleteConnection = true;
    }

    std::unique_ptr<Drawable> cursor;
    OwnedArray<Line<float>> connections;
    
private:

    bool incompleteConnection;
};

class ESTabbedComponent : public TabbedComponent
{
public:
    using TabbedComponent::TabbedComponent;
    
    void currentTabChanged (int newCurrentTabIndex, const String &newCurrentTabName) override
    {
        TabbedButtonBar& bar = getTabbedButtonBar();
        for (int i = 0; i < bar.getNumTabs(); ++i)
        {
            bar.getTabButton(i)->setAlpha(i == getCurrentTabIndex() ? 1.0f : 0.7f);
        }
    }
};

//==============================================================================
class CopedentTable : public Component,
                      public TableListBoxModel,
                      public ComboBox::Listener
{
public:
    CopedentTable(Array<Array<float>>& array) :
    copedentArray(array),
    exportChooser("Export copedent to file...",
                  File::getSpecialLocation(File::userDocumentsDirectory),
                  "*.xml"),
    importChooser("Import copedent file...",
                  File::getSpecialLocation(File::userDocumentsDirectory),
                  "*.xml")
    {
        for (int i = 0; i < CopedentColumnNil; ++i)
        {
            columnList.add(cCopedentColumnNames[i]);
        }
        
        stringTable.setModel (this);
        stringTable.setLookAndFeel(&laf);
        stringTable.setColour (ListBox::outlineColourId, Colours::grey);
        stringTable.setOutlineThickness (1);
        
        leftTable.setModel (this);
        leftTable.setLookAndFeel(&laf);
        leftTable.setColour (ListBox::outlineColourId, Colours::grey);
        leftTable.setOutlineThickness (1);
        
        pedalTable.setModel (this);
        pedalTable.setLookAndFeel(&laf);
        pedalTable.setColour (ListBox::outlineColourId, Colours::grey);
        pedalTable.setOutlineThickness (1);
        
        rightTable.setModel (this);
        rightTable.setLookAndFeel(&laf);
        rightTable.setColour (ListBox::outlineColourId, Colours::grey);
        rightTable.setOutlineThickness (1);
        
        int i = 0;
        int columnId = 1;
        TableHeaderComponent::ColumnPropertyFlags flags =
        TableHeaderComponent::ColumnPropertyFlags::notResizableOrSortable;
        stringTable.getHeader().addColumn(columnList[i++], columnId++, 20, 1, 1000, flags);
        
        leftTable.getHeader().addColumn(columnList[i++], columnId++, 20, 1, 1000, flags);
        leftTable.getHeader().addColumn(columnList[i++], columnId++, 20, 1, 1000, flags);
        leftTable.getHeader().addColumn(columnList[i++], columnId++, 20, 1, 1000, flags);
        
        pedalTable.getHeader().addColumn(columnList[i++], columnId++, 20, 1, 1000, flags);
        pedalTable.getHeader().addColumn(columnList[i++], columnId++, 20, 1, 1000, flags);
        pedalTable.getHeader().addColumn(columnList[i++], columnId++, 20, 1, 1000, flags);
        pedalTable.getHeader().addColumn(columnList[i++], columnId++, 20, 1, 1000, flags);
        pedalTable.getHeader().addColumn(columnList[i++], columnId++, 20, 1, 1000, flags);
        
        rightTable.getHeader().addColumn(columnList[i++], columnId++, 20, 1, 1000, flags);
        rightTable.getHeader().addColumn(columnList[i++], columnId++, 20, 1, 1000, flags);

        addAndMakeVisible (stringTable);
        addAndMakeVisible (leftTable);
        addAndMakeVisible (pedalTable);
        addAndMakeVisible (rightTable);
    }
    
    ~CopedentTable()
    {
        stringTable.setLookAndFeel(nullptr);
        leftTable.setLookAndFeel(nullptr);
        pedalTable.setLookAndFeel(nullptr);
        rightTable.setLookAndFeel(nullptr);
    }
    
    //==============================================================================
    // this loads the embedded database XML file into memory
    void exportXml()
    {
        exportChooser.launchAsync (FileBrowserComponent::saveMode |
                                   FileBrowserComponent::canSelectFiles |
                                   FileBrowserComponent::warnAboutOverwriting,
                                   [this] (const FileChooser& chooser)
                                   {
            String path = chooser.getResult().getFullPathName();
            if (path.isEmpty()) return;
            
            File file(path);
            
            ValueTree copedentVT("Copedent");
            
            for (int c = 0; c < numColumns; ++c)
            {
                String name = String(cCopedentColumnNames[c]);
                ValueTree child(name);
                for (int r = 0; r < numRows; ++r)
                {
                    child.setProperty("r" + String(r), copedentArray[c][r], nullptr);
                }
                copedentVT.addChild(child, -1, nullptr);
            }
            
            std::unique_ptr<XmlElement> xml = copedentVT.createXml();
            
            xml->writeTo(file, XmlElement::TextFormat());
        });
    }
    
    void importXml()
    {
        importChooser.launchAsync (FileBrowserComponent::openMode |
                                   FileBrowserComponent::canSelectFiles,
                                   [this] (const FileChooser& chooser)
                                   {
            String path = chooser.getResult().getFullPathName();
            if (path.isEmpty()) return;
            File file(path);
            importXml(XmlDocument::parse(file).get());
        });
    }
    
    void importXml(XmlElement* xml)
    {
        for (int c = 0; c < numColumns; ++c)
        {
            String name = String(cCopedentColumnNames[c]);
            XmlElement* columnXml = xml->getChildByName(name);
            if (columnXml == nullptr) continue;
            for (int r = 0; r < numRows; ++r)
            {
                float value = columnXml->getDoubleAttribute("r" + String(r));
                copedentArray.getReference(c).set(r, value);
            }
        }
        resized();
    }
    
    String getTextFromData (const int columnNumber, const int rowNumber, bool asDestination) const
    {
        float value = copedentArray[columnNumber-1][rowNumber];
        
        if (value == 0.0f) return String();
        
        String text = String();
        bool displayAsDestination = columnNumber == 1 || asDestination;
        if (displayAsDestination)
        {
            if (columnNumber != 1)
            {
                if (value > 0.0f) for (int i = 0; i < value; i+=12) text += String("+");
                else for (int i = 0; i < -value; i+=12) text += String("-");
                value += copedentArray[0][rowNumber];
            }
            int n = roundf(value);
            float f = value - n;
            // Use sharp except for E and B
            bool useSharps = (n % 12 != 4) && (n % 12 != 11);
            
            text += MidiMessage::getMidiNoteName(n, useSharps, false, 4);
            if (f > 0.0f) text += "+" + String(f);
            else if (f < 0.0f) text += String(f);
        }
        else
        {
            if (value > 0.0f) text += "+";
            text += String(value);
        }
        return text;
    }
    
    void setDataFromText (const int columnNumber, const int rowNumber, const String& newText)
    {
        float value = newText.getFloatValue();
        copedentArray.getReference(columnNumber-1).set(rowNumber, value);
    }
    
    // This is overloaded from TableListBoxModel, and must return the total number of rows in our table
    int getNumRows() override
    {
        return numRows;
    }
    
    // This is overloaded from TableListBoxModel, and should fill in the background of the whole row
    void paintRowBackground (Graphics& g, int rowNumber, int /*width*/, int /*height*/, bool rowIsSelected) override
    {
        if (rowNumber % 2) g.fillAll (Colours::darkgrey.withBrightness(0.1f));
        else g.fillAll (Colours::darkgrey.withBrightness(0.1f));
    }
    
    // This is overloaded from TableListBoxModel, and must paint any cells that aren't using custom
    // components.
    void paintCell (Graphics& g, int rowNumber, int columnId,
                    int width, int height, bool /*rowIsSelected*/) override
    {
        g.setColour (getLookAndFeel().findColour (ListBox::textColourId));
        g.setFont (Font(14.));
        
        for (int c = 0; c < numColumns; ++c)
        {
            for (int r = 0; r < numRows; ++r)
            {
                g.drawText (getTextFromData(c, r, true), 2, 0, width - 4, height,
                            Justification::centredLeft, true);
            }
        }
        
        g.setColour (getLookAndFeel().findColour (ListBox::backgroundColourId));
        g.fillRect (width - 1, 0, 1, height);
    }
    
    // This is overloaded from TableListBoxModel, and must update any custom components that we're using
    Component* refreshComponentForCell (int rowNumber, int columnId, bool /*isRowSelected*/,
                                        Component* existingComponentToUpdate) override
    {
        // The other columns are editable text columns, for which we use the custom Label component
        auto* textLabel = static_cast<EditableTextCustomComponent*> (existingComponentToUpdate);
        
        // same as above...
        if (textLabel == nullptr)
            textLabel = new EditableTextCustomComponent (*this);
        
        textLabel->setRowAndColumn (rowNumber, columnId);
        return textLabel;
    }
    
    void comboBoxChanged(ComboBox *comboBoxThatHasChanged) override
    {
        // Display behavior setting
    }
    
    //==============================================================================
    void resized() override
    {
        Rectangle<int> area = getLocalBounds();
        
        int n = (numColumns*2)+3;
        int w = area.getWidth() / n;
        int r = area.getWidth() - (w*n) - 2;

        stringTable.setBounds(area.removeFromLeft(w*2+r));
        area.removeFromLeft(w);
        leftTable.setBounds(area.removeFromLeft(w*6));
        area.removeFromLeft(w);
        pedalTable.setBounds(area.removeFromLeft(w*10));
        area.removeFromLeft(w);
        rightTable.setBounds(area.removeFromLeft(w*4));
        
        int columnId = 1;
        stringTable.getHeader().setColumnWidth(columnId++, w*2+r-2);
        
        leftTable.getHeader().setColumnWidth(columnId++, w*2);
        leftTable.getHeader().setColumnWidth(columnId++, w*2-1);
        leftTable.getHeader().setColumnWidth(columnId++, w*2-1);
        
        pedalTable.getHeader().setColumnWidth(columnId++, w*2);
        pedalTable.getHeader().setColumnWidth(columnId++, w*2);
        pedalTable.getHeader().setColumnWidth(columnId++, w*2);
        pedalTable.getHeader().setColumnWidth(columnId++, w*2-1);
        pedalTable.getHeader().setColumnWidth(columnId++, w*2-1);
        
        rightTable.getHeader().setColumnWidth(columnId++, w*2-1);
        rightTable.getHeader().setColumnWidth(columnId++, w*2-1);
        
        int h = stringTable.getHeight() / (numRows+1);
        r = stringTable.getHeight() - (h*(numRows+1)) - 2;
        stringTable.setHeaderHeight(h+r);
        stringTable.setRowHeight(h);
        leftTable.setHeaderHeight(h+r);
        leftTable.setRowHeight(h);
        pedalTable.setHeaderHeight(h+r);
        pedalTable.setRowHeight(h);
        rightTable.setHeaderHeight(h+r);
        rightTable.setRowHeight(h);
    }
    
    
private:
    
    TableListBox stringTable;
    TableListBox leftTable;
    TableListBox pedalTable;
    TableListBox rightTable;
    
    static const int numColumns = CopedentColumnNil;
    static const int numRows = NUM_VOICES;        // Number of strings
    
    StringArray columnList;
    Array<Array<float>>& copedentArray;
    
    FileChooser exportChooser;
    FileChooser importChooser;
    
    ESLookAndFeel laf;
    
    //==============================================================================
    // This is a custom Label component, which we use for the table's editable text columns.
    class EditableTextCustomComponent : public Label
    {
    public:
        EditableTextCustomComponent (CopedentTable& td)  : owner (td)
        {
            // double click to edit the label text; single click handled below
            setEditable (true, true, false);
            setJustificationType(Justification::centred);
        }
        
        void mouseDown (const MouseEvent& event) override
        {
//            // single click on the label should simply select the row
//            owner.table.selectRowsBasedOnModifierKeys (row, event.mods, false);
//
            Label::mouseDown (event);
        }
        
        void textWasEdited() override
        {
            owner.setDataFromText (columnId, row, getText());
            setText (owner.getTextFromData(columnId, row, true), dontSendNotification);
        }
        
        // Our demo code will call this when we may need to update our contents
        void setRowAndColumn (const int newRow, const int newColumn)
        {
            row = newRow;
            columnId = newColumn;
            setText (owner.getTextFromData(columnId, row, true), dontSendNotification);
        }
        
        void paint (Graphics& g) override
        {
            auto& lf = getLookAndFeel();
            if (! dynamic_cast<LookAndFeel_V4*> (&lf))
                lf.setColour (textColourId, Colours::black);
            
            Label::paint (g);
            g.setColour(Colours::lightgrey);
            g.fillRect(0, getHeight()-1, getWidth(), 1);
            if (columnId != 1 && columnId != 4 && columnId != 9 && columnId != 11)
                g.fillRect(getWidth()-1, 0, 1, getHeight());
            
            if (TextEditor* editor = getCurrentTextEditor())
                editor->setJustification(Justification::centredLeft);
        }
        
    private:
        CopedentTable& owner;
        int row, columnId;
        Colour textColour;
    };
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CopedentTable)
};
