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
                      public TableListBoxModel
{
public:
    CopedentTable()
    {
        // Create our table component and add it to this component..
        addAndMakeVisible (table);
        table.setModel (this);
        
        // give it a border
        table.setColour (ListBox::outlineColourId, Colours::grey);
        table.setOutlineThickness (1);
        
        // we could now change some initial settings..
        table.getHeader().setSortColumnId (1, true); // sort forwards by the ID column
        table.getHeader().setColumnVisible (7, false); // hide the "length" column until the user shows it
        
        // un-comment this line to have a go of stretch-to-fit mode
        // table.getHeader().setStretchToFitActive (true);
        
        table.setRowSelectedOnMouseDown(false);
    }
    
    //==============================================================================
    // this loads the embedded database XML file into memory
    void loadData(std::unique_ptr<InputStream>input)
    {
        data = parseXML (input->readString());
        
        dataList   = data->getChildByName ("DATA");
        columnList = data->getChildByName ("COLUMNS");
        
        //numRows = dataList->getNumChildElements();
        
        table.getHeader().removeAllColumns();
        
        // Add some columns to the table header, based on the column list in our database..
        for (auto* columnXml : columnList->getChildIterator())
        {
            table.getHeader().addColumn (columnXml->getStringAttribute ("name"),
                                         columnXml->getIntAttribute ("columnId"),
                                         columnXml->getIntAttribute ("width"),
                                         50, 400,
                                         TableHeaderComponent::defaultFlags);
        }
    }
    
    // This is overloaded from TableListBoxModel, and must return the total number of rows in our table
    int getNumRows() override
    {
        return numRows;
    }
    
    // This is overloaded from TableListBoxModel, and should fill in the background of the whole row
    void paintRowBackground (Graphics& g, int rowNumber, int /*width*/, int /*height*/, bool rowIsSelected) override
    {
        auto alternateColour = getLookAndFeel().findColour (ListBox::backgroundColourId)
        .interpolatedWith (getLookAndFeel().findColour (ListBox::textColourId), 0.03f);
//        if (rowIsSelected)
//            g.fillAll (Colours::lightblue);
        else if (rowNumber % 2)
            g.fillAll (alternateColour);
    }
    
    // This is overloaded from TableListBoxModel, and must paint any cells that aren't using custom
    // components.
    void paintCell (Graphics& g, int rowNumber, int columnId,
                    int width, int height, bool /*rowIsSelected*/) override
    {
        g.setColour (getLookAndFeel().findColour (ListBox::textColourId));
        g.setFont (font);
        
        if (auto* rowElement = dataList->getChildElement (rowNumber))
        {
            auto text = rowElement->getStringAttribute (getAttributeNameForColumnId (columnId));
            
            g.drawText (text, 2, 0, width - 4, height, Justification::centredLeft, true);
        }
        
        g.setColour (getLookAndFeel().findColour (ListBox::backgroundColourId));
        g.fillRect (width - 1, 0, 1, height);
    }
    
    // This is overloaded from TableListBoxModel, and tells us that the user has clicked a table header
    // to change the sort order.
    void sortOrderChanged (int newSortColumnId, bool isForwards) override
    {
        if (newSortColumnId != 0)
        {
            DemoDataSorter sorter (getAttributeNameForColumnId (newSortColumnId), isForwards);
            dataList->sortChildElements (sorter);
            
            table.updateContent();
        }
    }
    
    // This is overloaded from TableListBoxModel, and must update any custom components that we're using
    Component* refreshComponentForCell (int rowNumber, int columnId, bool /*isRowSelected*/,
                                        Component* existingComponentToUpdate) override
    {
        if (columnId == 1 || columnId == 7) // The ID and Length columns do not have a custom component
        {
            jassert (existingComponentToUpdate == nullptr);
            return nullptr;
        }
        
        if (columnId == 5) // For the ratings column, we return the custom combobox component
        {
            auto* ratingsBox = static_cast<RatingColumnCustomComponent*> (existingComponentToUpdate);
            
            // If an existing component is being passed-in for updating, we'll re-use it, but
            // if not, we'll have to create one.
            if (ratingsBox == nullptr)
                ratingsBox = new RatingColumnCustomComponent (*this);
            
            ratingsBox->setRowAndColumn (rowNumber, columnId);
            return ratingsBox;
        }
        
        // The other columns are editable text columns, for which we use the custom Label component
        auto* textLabel = static_cast<EditableTextCustomComponent*> (existingComponentToUpdate);
        
        // same as above...
        if (textLabel == nullptr)
            textLabel = new EditableTextCustomComponent (*this);
        
        textLabel->setRowAndColumn (rowNumber, columnId);
        return textLabel;
    }
    
    // This is overloaded from TableListBoxModel, and should choose the best width for the specified
    // column.
    int getColumnAutoSizeWidth (int columnId) override
    {
        if (columnId == 5)
            return 100; // (this is the ratings column, containing a custom combobox component)
        
        int widest = 32;
        
        // find the widest bit of text in this column..
        for (int i = getNumRows(); --i >= 0;)
        {
            if (auto* rowElement = dataList->getChildElement (i))
            {
                auto text = rowElement->getStringAttribute (getAttributeNameForColumnId (columnId));
                
                widest = jmax (widest, font.getStringWidth (text));
            }
        }
        
        return widest + 8;
    }
    
    // A couple of quick methods to set and get cell values when the user changes them
    int getRating (const int rowNumber) const
    {
        return dataList->getChildElement (rowNumber)->getIntAttribute ("Rating");
    }
    
    void setRating (const int rowNumber, const int newRating)
    {
        dataList->getChildElement (rowNumber)->setAttribute ("Rating", newRating);
    }
    
    String getText (const int columnNumber, const int rowNumber) const
    {
        return dataList->getChildElement (rowNumber)->getStringAttribute ( getAttributeNameForColumnId(columnNumber));
    }
    
    void setText (const int columnNumber, const int rowNumber, const String& newText)
    {
        auto columnName = table.getHeader().getColumnName (columnNumber);
        dataList->getChildElement (rowNumber)->setAttribute (columnName, newText);
    }
    
    //==============================================================================
    void resized() override
    {
        // position our table with a gap around its edge
        table.setBoundsInset (BorderSize<int> (8));
    }
    
private:
    TableListBox table;     // the table component itself
    Font font  { 14.0f };
    
    std::unique_ptr<XmlElement> data;  // This is the XML document loaded from the embedded file "demo table data.xml"
    XmlElement* columnList = nullptr;     // A pointer to the sub-node of demoData that contains the list of columns
    XmlElement* dataList   = nullptr;     // A pointer to the sub-node of demoData that contains the list of data rows
    static const int numRows = 12;        // Number of strings
    //==============================================================================
    // This is a custom Label component, which we use for the table's editable text columns.
    class EditableTextCustomComponent  : public Label
    {
    public:
        EditableTextCustomComponent (CopedentTable& td)  : owner (td)
        {
            // double click to edit the label text; single click handled below
            setEditable (false, true, false);
        }
        
        void mouseDown (const MouseEvent& event) override
        {
            // single click on the label should simply select the row
            owner.table.selectRowsBasedOnModifierKeys (row, event.mods, false);
            
            Label::mouseDown (event);
        }
        
        void textWasEdited() override
        {
            owner.setText (columnId, row, getText());
        }
        
        // Our demo code will call this when we may need to update our contents
        void setRowAndColumn (const int newRow, const int newColumn)
        {
            row = newRow;
            columnId = newColumn;
            setText (owner.getText(columnId, row), dontSendNotification);
        }
        
        void paint (Graphics& g) override
        {
            auto& lf = getLookAndFeel();
            if (! dynamic_cast<LookAndFeel_V4*> (&lf))
                lf.setColour (textColourId, Colours::black);
            
            Label::paint (g);
        }
        
    private:
        CopedentTable& owner;
        int row, columnId;
        Colour textColour;
    };
    
    //==============================================================================
    // This is a custom component containing a combo box, which we're going to put inside
    // our table's "rating" column.
    class RatingColumnCustomComponent    : public Component
    {
    public:
        RatingColumnCustomComponent (CopedentTable& td)  : owner (td)
        {
            // just put a combo box inside this component
            addAndMakeVisible (comboBox);
            comboBox.addItem ("fab",        1);
            comboBox.addItem ("groovy",     2);
            comboBox.addItem ("hep",        3);
            comboBox.addItem ("mad for it", 4);
            comboBox.addItem ("neat",       5);
            comboBox.addItem ("swingin",    6);
            comboBox.addItem ("wild",       7);
            
            comboBox.onChange = [this] { owner.setRating (row, comboBox.getSelectedId()); };
            comboBox.setWantsKeyboardFocus (false);
        }
        
        void resized() override
        {
            comboBox.setBoundsInset (BorderSize<int> (2));
        }
        
        // Our demo code will call this when we may need to update our contents
        void setRowAndColumn (int newRow, int newColumn)
        {
            row = newRow;
            columnId = newColumn;
            comboBox.setSelectedId (owner.getRating (row), dontSendNotification);
        }
        
    private:
        CopedentTable& owner;
        ComboBox comboBox;
        int row, columnId;
    };
    
    //==============================================================================
    // A comparator used to sort our data when the user clicks a column header
    class DemoDataSorter
    {
    public:
        DemoDataSorter (const String& attributeToSortBy, bool forwards)
        : attributeToSort (attributeToSortBy),
        direction (forwards ? 1 : -1)
        {
        }
        
        int compareElements (XmlElement* first, XmlElement* second) const
        {
            auto result = first->getStringAttribute (attributeToSort)
            .compareNatural (second->getStringAttribute (attributeToSort));
            
            if (result == 0)
                result = first->getStringAttribute ("ID")
                .compareNatural (second->getStringAttribute ("ID"));
            
            return direction * result;
        }
        
    private:
        String attributeToSort;
        int direction;
    };
    
    // (a utility method to search our XML for the attribute that matches a column ID)
    String getAttributeNameForColumnId (const int columnId) const
    {
        for (auto* columnXml : columnList->getChildIterator())
        {
            if (columnXml->getIntAttribute ("columnId") == columnId)
                return columnXml->getStringAttribute ("name");
        }
        
        return {};
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CopedentTable)
};
