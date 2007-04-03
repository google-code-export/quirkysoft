#include "ndspp.h"
#include "libnds.h"
#include "View.h"
#include "Document.h"
#include "TextArea.h"


View::View(Document & doc, ControllerI & c):m_document(doc), m_controller(c)
{
  m_document.registerView(this);
  m_textArea = new TextArea(/*"fonts/vera"*/);
}

void View::notify()
{
  //std::cout << "URI: " << m_document.uri() << std::endl;
  //std::cout << "Content: " << m_document.asText() << std::endl;
  Document::Status status(m_document.status());

  switch (status) {
    case Document::LOADED:
      {
        const char * text(m_document.asText());
        if (text != 0) {
          m_textArea->print(text, strlen(text), 0,0);
        }
        swiWaitForVBlank();
      }
      break;
    case Document::INPROGRESS:
      {
        const char * l = "Loading..";
        m_textArea->print(l, strlen(l), 0,0);
        swiWaitForVBlank();
      }
      break;
    default:
      break;
  }
}

void View::mainLoop()
{
  for(;;) {
    scanKeys();
    u16 keys = keysDownRepeat();
    if (keys & KEY_A)
    {
      // show the board thingy
    }

    swiWaitForVBlank();
  }
}
