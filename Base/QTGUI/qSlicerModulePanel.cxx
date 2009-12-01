#include "qSlicerModulePanel.h"

// SlicerQT includes
#include "qSlicerAbstractModule.h"

// CTK includes
#include "qCTKCollapsibleButton.h"
#include "qCTKFittedTextBrowser.h"

// QT includes
#include <QLabel>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QSpacerItem>
#include <QStackedWidget>
#include <QResizeEvent>
#include <QTextBrowser>
#include <QWebView>

//---------------------------------------------------------------------------
struct qSlicerModulePanel::qInternal
{
  void setupUi(QWidget * widget);

  QTextBrowser*          HelpLabel;
  //QWebView*              HelpLabel;
  QBoxLayout*            Layout;
  QScrollArea*           ScrollArea;
  qCTKCollapsibleButton* HelpCollapsibleButton;
};

//---------------------------------------------------------------------------
qSlicerModulePanel::qSlicerModulePanel(QWidget* parent, Qt::WindowFlags f)
  :qSlicerAbstractModulePanel(parent, f)
{
  this->Internal = new qInternal;
  this->Internal->setupUi(this);
}

//---------------------------------------------------------------------------
qSlicerModulePanel::~qSlicerModulePanel()
{
  delete this->Internal;
}

//---------------------------------------------------------------------------
void qSlicerModulePanel::setModule(qSlicerAbstractModule* module)
{
  // Retrieve current module associated with the module panel
  QLayoutItem* item = this->Internal->Layout->itemAt(1);
  qSlicerAbstractModule* currentModule =
    item ? qobject_cast<qSlicerAbstractModule*>(item->widget()) : 0;

  // If module is already set, return.
  if (module == currentModule)
    {
    return;
    }

  if (currentModule)
    {
    // Remove the current module
    this->removeModule(currentModule);
    }

  if (module)
    {
    // Add the new module
    this->addModule(module);
    }
  else
    {
    //this->Internal->HelpLabel->setHtml("");
    }
}

//---------------------------------------------------------------------------
void qSlicerModulePanel::clear()
{
  this->setModule(0);
}

//---------------------------------------------------------------------------
void qSlicerModulePanel::addModule(qSlicerAbstractModule* module)
{
  Q_ASSERT(module);

  // Update module layout
  module->layout()->setContentsMargins(0, 0, 0, 0);

  // Insert module in the panel
  this->Internal->Layout->insertWidget(1, module);

  module->setSizePolicy(QSizePolicy::Ignored, module->sizePolicy().verticalPolicy());
  module->setVisible(true);

  this->Internal->HelpCollapsibleButton->setVisible(!module->helpText().isEmpty());
  this->Internal->HelpLabel->setHtml(module->helpText());
  //this->Internal->HelpLabel->load(QString("http://www.slicer.org/slicerWiki/index.php?title=Modules:Transforms-Documentation-3.4&useskin=chick"));

  emit moduleAdded(module);
}

//---------------------------------------------------------------------------
void qSlicerModulePanel::removeModule(qSlicerAbstractModule* module)
{
  Q_ASSERT(module);

  int index = this->Internal->Layout->indexOf(module);
  if (index == -1)
    {
    return;
    }

  //emit moduleAboutToBeRemoved(module);

  // Remove widget from layout
  //this->Internal->Layout->removeWidget(module);
  this->Internal->Layout->takeAt(index);

  module->setVisible(false);
  module->setParent(0);

//   // if nobody took ownership of the module, make sure it both lost its parent and is hidden
//   if (module->parent() == this->Internal->Layout->parentWidget())
//     {
//     module->setVisible(false);
//     module->setParent(0);
//     }

  emit moduleRemoved(module);
}

//---------------------------------------------------------------------------
void qSlicerModulePanel::removeAllModule()
{
  this->clear();
}

//---------------------------------------------------------------------------
// Internal methods

//---------------------------------------------------------------------------
void qSlicerModulePanel::qInternal::setupUi(QWidget * widget)
{
  QWidget* panel = new QWidget;
  this->HelpCollapsibleButton = new qCTKCollapsibleButton("Help");
  this->HelpCollapsibleButton->setCollapsed(true);
  this->HelpCollapsibleButton->setSizePolicy(
    QSizePolicy::Ignored, QSizePolicy::Minimum);
  // QTextBrowser instead of QLabel because QLabel doesn't word wrap links 
  // correctly
  //this->HelpLabel = new QWebView;
  this->HelpLabel = static_cast<QTextBrowser*>(new qCTKFittedTextBrowser);
  this->HelpLabel->setOpenExternalLinks(true);
  this->HelpLabel->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  this->HelpLabel->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  this->HelpLabel->setFrameShape(QFrame::NoFrame);
  
  QPalette p = this->HelpLabel->palette();
  p.setBrush(QPalette::Window, QBrush ());
  this->HelpLabel->setPalette(p);

  QGridLayout* helpLayout = new QGridLayout(this->HelpCollapsibleButton);
  helpLayout->addWidget(this->HelpLabel);

  this->Layout = new QVBoxLayout(panel);
  this->Layout->addWidget(this->HelpCollapsibleButton);
  this->Layout->addItem(
    new QSpacerItem(0, 0, QSizePolicy::Minimum, 
                    QSizePolicy::MinimumExpanding));

  this->ScrollArea = new QScrollArea;
  this->ScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  this->ScrollArea->setWidget(panel);
  this->ScrollArea->setWidgetResizable(true);
  
  QGridLayout* gridLayout = new QGridLayout;
  gridLayout->addWidget(this->ScrollArea);
  gridLayout->setContentsMargins(0,0,0,0);
  widget->setLayout(gridLayout);

}
