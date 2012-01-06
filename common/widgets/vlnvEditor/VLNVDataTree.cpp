//-----------------------------------------------------------------------------
// File: VLNVDataTree.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 5.1.2012
//
// Description:
// Optimized VLNV data tree for VLNV editor data searches.
//-----------------------------------------------------------------------------

#include "VLNVDataTree.h"

#include <LibraryManager/libraryinterface.h>
#include <LibraryManager/libraryitem.h>

#include <models/component.h>

//-----------------------------------------------------------------------------
// Function: VLNVDataNode()
//-----------------------------------------------------------------------------
VLNVDataNode::VLNVDataNode(QString const& name, unsigned int level) : name_(name), level_(level), vlnv_()
{
}

//-----------------------------------------------------------------------------
// Function: ~VLNVDataNode()
//-----------------------------------------------------------------------------
VLNVDataNode::~VLNVDataNode()
{
}

//-----------------------------------------------------------------------------
// Function: getName()
//-----------------------------------------------------------------------------
QString const& VLNVDataNode::getName() const
{
    return name_;
}

//-----------------------------------------------------------------------------
// Function: getVLNV()
//-----------------------------------------------------------------------------
VLNV const& VLNVDataNode::getVLNV() const
{
    return vlnv_;
}

//-----------------------------------------------------------------------------
// Function: getLevel()
//-----------------------------------------------------------------------------
unsigned int VLNVDataNode::getLevel() const
{
    return level_;
}

//-----------------------------------------------------------------------------
// Function: findChild()
//-----------------------------------------------------------------------------
VLNVDataNode* VLNVDataNode::findChild(QString const& name)
{
    foreach (QSharedPointer<VLNVDataNode> node, children_)
    {
        if (node->getName() == name)
        {
            return node.data();
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: findChild()
//-----------------------------------------------------------------------------
VLNVDataNode const* VLNVDataNode::findChild(QString const& name) const
{
    foreach (QSharedPointer<VLNVDataNode> node, children_)
    {
        if (node->getName() == name)
        {
            return node.data();
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: getChildren()
//-----------------------------------------------------------------------------
QList< QSharedPointer<VLNVDataNode> > const& VLNVDataNode::getChildren() const
{
    return children_;
}

//-----------------------------------------------------------------------------
// Function: addChild()
//-----------------------------------------------------------------------------
VLNVDataNode* VLNVDataNode::addChild(QString const& name)
{
    QSharedPointer<VLNVDataNode> child(new VLNVDataNode(name, level_ + 1));
    children_.append(child);
    return child.data();
}

//-----------------------------------------------------------------------------
// Function: removeChild()
//-----------------------------------------------------------------------------
void VLNVDataNode::removeChild(QString const& name)
{
    foreach (QSharedPointer<VLNVDataNode> node, children_)
    {
        if (node->getName() == name)
        {
            children_.removeOne(node);
            return;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: removeChildren()
//-----------------------------------------------------------------------------
void VLNVDataNode::removeChildren()
{
    children_.clear();
}

//-----------------------------------------------------------------------------
// Function: setVLNV()
//-----------------------------------------------------------------------------
void VLNVDataNode::setVLNV(VLNV const& vlnv)
{
    vlnv_ = vlnv;
}

//-----------------------------------------------------------------------------
// Function: VLNVDataTree()
//-----------------------------------------------------------------------------
VLNVDataTree::VLNVDataTree() : firmnessFilterEnabled_(false), firmnessFilter_(KactusAttribute::KTS_TEMPLATE),
                               hierarchyFilterEnabled_(false), hierarchyFilter_(KactusAttribute::KTS_IP),
                               implementationFilterEnabled_(false), implementationFilter_(KactusAttribute::KTS_HW),
                               swTypeFilterEnabled_(false), swTypeFilter_(KactusAttribute::KTS_SW_MAPPING)
{
}

//-----------------------------------------------------------------------------
// Function: ~VLNVDataTree()
//-----------------------------------------------------------------------------
VLNVDataTree::~VLNVDataTree()
{
}

//-----------------------------------------------------------------------------
// Function: parse()
//-----------------------------------------------------------------------------
void VLNVDataTree::parse(LibraryInterface* lh, VLNV::IPXactType type)
{
    // Retrieve the type root library item.
    LibraryItem const* root = lh->getTreeRoot();
    LibraryItem const* typeRoot = 0;

    for (int i = 0; i < root->getNumberOfChildren(); ++i)
    {
        if (root->child(i)->getName() == VLNV::type2Print(type))
        {
            typeRoot = root->child(i);
            break;
        }
    }

    // Parse all library objects.
    if (typeRoot != 0)
    {
        parseSubtree(lh, typeRoot, *this);
    }
}

//-----------------------------------------------------------------------------
// Function: clear()
//-----------------------------------------------------------------------------
void VLNVDataTree::clear()
{
    removeChildren();
}

//-----------------------------------------------------------------------------
// Function: setFirmnessFilter()
//-----------------------------------------------------------------------------
void VLNVDataTree::setFirmnessFilter(bool on, KactusAttribute::Firmness firmness /*= KactusAttribute::KTS_TEMPLATE*/)
{
    firmnessFilterEnabled_ = on;
    firmnessFilter_ = firmness;
}

//-----------------------------------------------------------------------------
// Function: setHierarchyFilter()
//-----------------------------------------------------------------------------
void VLNVDataTree::setHierarchyFilter(bool on, KactusAttribute::ProductHierarchy productHier /*= KactusAttribute::KTS_IP*/)
{
    hierarchyFilterEnabled_ = on;
    hierarchyFilter_ = productHier;
}

//-----------------------------------------------------------------------------
// Function: setImplementationFilter()
//-----------------------------------------------------------------------------
void VLNVDataTree::setImplementationFilter(bool on, KactusAttribute::Implementation implementation /*= KactusAttribute::KTS_HW*/)
{
    implementationFilterEnabled_ = on;
    implementationFilter_ = implementation;
}

//-----------------------------------------------------------------------------
// Function: setSWTypeFilter()
//-----------------------------------------------------------------------------
void VLNVDataTree::setSWTypeFilter(bool on, KactusAttribute::SWType swType /*= KactusAttribute::KTS_SW_MAPPING*/)
{
    swTypeFilterEnabled_ = on;
    swTypeFilter_ = swType;
}

//-----------------------------------------------------------------------------
// Function: parseVendor()
//-----------------------------------------------------------------------------
void VLNVDataTree::parseSubtree(LibraryInterface* lh, LibraryItem const* libItem, VLNVDataNode& node)
{
    for (int i = 0; i < libItem->getNumberOfChildren(); ++i)
    {
        LibraryItem const* item = libItem->child(i);

        // Check if this is a leaf item.
        if (item->getLevel() == LibraryItem::VERSION)
        {
            VLNV const* vlnv = item->getVLNV();
            Q_ASSERT(vlnv != 0);

            // If filtering is off, just accept the item.
            if (!firmnessFilterEnabled_ && !hierarchyFilterEnabled_ &&
                !implementationFilterEnabled_ && !swTypeFilterEnabled_)
            {
                VLNVDataNode* childNode = node.addChild(item->getName());
                childNode->setVLNV(*vlnv);
                return;
            }

            // Retrieve the library component for filtering. Filtering is possible only if the
            // library component is an IP-XACT component.
            QSharedPointer<LibraryComponent const> libComp = lh->getModelReadOnly(*vlnv);
            QSharedPointer<Component const> component = libComp.dynamicCast<Component const>();

            if (component == 0 ||
                ((!firmnessFilterEnabled_ || firmnessFilter_ == component->getComponentFirmness()) &&
                (!hierarchyFilterEnabled_ || hierarchyFilter_ == component->getComponentHierarchy()) &&
                (!implementationFilterEnabled_ || implementationFilter_ == component->getComponentImplementation()) &&
                (!swTypeFilterEnabled_|| swTypeFilter_ == component->getComponentSWType())))
            {
                VLNVDataNode* childNode = node.addChild(item->getName());
                childNode->setVLNV(*vlnv);
            }
        }
        else
        {
            // Otherwise parse the subtree recursively.
            VLNVDataNode* childNode = node.findChild(item->getName());

            if (childNode == 0)
            {
                childNode = node.addChild(item->getName());
            }

            parseSubtree(lh, item, *childNode);

            if (childNode->getChildren().size() == 0)
            {
                node.removeChild(item->getName());
            }
        }
    }
}