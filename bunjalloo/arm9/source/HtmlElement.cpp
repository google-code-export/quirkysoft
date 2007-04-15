#include <vector>
#include "HtmlElement.h"
#include "ElementFactory.h"

using namespace std;

const std::string * HtmlElement::attributePtr(const std::string & name) const
{
  if (name == "id")
  {
    return &m_id;
  }
  if (name == "title")
  {
    return &m_title;
  }
  if (name == "lang")
  {
    return &m_lang;
  }
  if (name == "dir")
  {
    return &m_dir;
  }
  return 0;
}

std::string HtmlElement::attribute(const std::string & name) const
{
   const string * ptr = attributePtr(name);
   if (ptr)
   {
     return *ptr;
   }
   return "";
}

void HtmlElement::setAttribute(const std::string & name, const std::string & value)
{
   string * ptr = const_cast<string*>(attributePtr(name));
   if (ptr)
   {
     *ptr = value;
   }
}

void HtmlElement::append(HtmlElement * child)
{
  m_children.push_back(child);
  child->setParent(this);
}

void HtmlElement::remove(HtmlElement * child)
{
  ElementList::iterator forRemoval = find(m_children.begin(), m_children.end(), child);
  if (forRemoval != m_children.end())
  {
    m_children.erase(forRemoval);
  }
}

void HtmlElement::appendText(unsigned int value)
{
  if (m_children.size())
  {
    if (m_children.back()->isa("#text"))
    {
      m_children.back()->m_text += value;
      return;
    }
  }
  HtmlElement* textNode = new HtmlElement("#text");
  textNode->m_text = value;
  append(textNode);
}

HtmlElement::~HtmlElement()
{
  for_each(m_children.begin(), m_children.end(), ElementFactory::remove);
}

void HtmlElement::copyAttributes(HtmlElement * copyTo) const
{
  copyTo->m_id = m_id;
  copyTo->m_title = m_title;
  copyTo->m_lang = m_lang;
  copyTo->m_dir = m_dir;
}

HtmlElement * HtmlElement::clone() const
{
  HtmlElement * theClone = new HtmlElement(m_tagName);
  copyAttributes(theClone);
  return theClone;
}

#if 0
void HtmlElement::dump() const
{
  std::basic_string<wchar_t> dumper;
  UnicodeString::const_iterator it(m_text.begin());
  for (; it != m_text.end(); ++it)
  {
    dumper += *it;
  }
  
  wcout << dumper << endl;
}
#endif

void HtmlElement::removeAllChildren()
{
  m_children.clear();
}