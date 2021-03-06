//-----------------------------------------------------------------------------
// File: VerilogInstanceParser.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 03.04.2019
//
// Description:
// Parser for Verilog instances.
//-----------------------------------------------------------------------------

#ifndef VERILOGINSTANCEPARSER_H
#define VERILOGINSTANCEPARSER_H

#include <Plugins/PluginSystem/ImportPlugin/HighlightSource.h>

#include <QSharedPointer>

class Component;
class Kactus2Group;

//-----------------------------------------------------------------------------
//! Parser for Verilog instances.
//-----------------------------------------------------------------------------
class VerilogInstanceParser : public HighlightSource
{
public:

    /*!
     *  The constructor.
     */
    VerilogInstanceParser();

    /*!
     *  The destructor.
     */
    virtual ~VerilogInstanceParser() = default;

    // Disable copying.
    VerilogInstanceParser(VerilogInstanceParser const& rhs) = delete;
    VerilogInstanceParser& operator=(VerilogInstanceParser const& rhs) = delete;

    /*!
     *  Runs the port parsing for the given input and adds the parsed ports to the given component.
     *
     *      @param [in] input                           The input text to parse.
     *      @param [in] targetComponent                 The component to add all the imported parameters to.
     */
    virtual void import(QString const& input, QSharedPointer<Component> targetComponent);

    /*!
     *  Sets the given highlighter to be used by the generic parser.
     *
     *      @param [in] highlighter   The highlighter to use.          
     */
    virtual void setHighlighter(Highlighter* highlighter);

private:

    /*!
     *  Find matches for the component instances in the selected verilog file.
     *
     *      @param [in] input   The selected verilog file.
     *
     *      @return List of component instance matches.
     */
    QVector<QRegularExpressionMatch> findInstances(QString const& input);

    /*!
     *  Get a list of separated sub items and their values from the selected sub item declarations.
     *
     *      @param [in] subItemDeclarations     The selected sub item declarations.
     *
     *      @return List of sub item name value pairs.
     */
    QVector<QPair<QString, QString> > getSeparatedSubItems(QString& subItemDeclarations) const;

    /*!
     *  Create a vendor extension group for the contained sub items.
     *
     *      @param [in] groupName   Name of the sub items.
     *      @param [in] itemType    Type of the sub items.
     *      @param [in] subItems    List of sub item name value pairs.
     *
     *      @return The created sub item vendor extension group.
     */
    QSharedPointer<Kactus2Group> createSubItemGroup(QString const& groupName, QString const& itemType,
        QVector<QPair<QString, QString> > const& subItems) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The highlighter to use.
    Highlighter* highlighter_;
};

#endif // VERILOGINSTANCEPARSER_H
