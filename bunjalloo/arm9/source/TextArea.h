#ifndef TextArea_h_seen
#define TextArea_h_seen

#include "Font.h"
#include "UnicodeString.h"
#include <list>

class HtmlElement;
class Link;
/** A widget for displaying text.*/
class TextArea
{
  public:
    /** Constructor.*/
    TextArea();
    /** Destructor.*/
    ~TextArea();

    /** Init the font to use.
     * @param fontBase the base name of the font.
     */
    void init(const std::string & fontBase);

    /** Set the palette.
     * @param fileName the name of the font palette.
     */
    void setPalette(const std::string & fileName);

    /** Set the cursor position. This is where the text will be "drawn" the
     * next time a print routine is called.
     * @param x the cursor x position in pixels.
     * @param y the cursor y position in pixels.
     */
    void setCursor(int x, int y);

    /** Print encoded text using the current encoding.
     * @param data the encoded text data.
     * @param amount the number of bytes of encoded data.
     */
    void print(const char * data, int amount);

    /** Print a unicode string starting at the current cursor position. The
     * values are not decoded from the string.
     * @param unicodeString a string containing unicode values.
     */
    void printu(const UnicodeString & unicodeString);

    /** Get the current font.
     * @return the current font.
     */
    inline const Font & font() const;

    /** Set the current encoding. @see print().
     * @param encoding the encoding to use for all encoded text.
     */
    void setEncoding(const std::string & encoding="utf-8");

    /** Set the text colour. Since the text is antialiased, this creates a coloured
     * scale palette based on the original grey-scale palette.
     * @param color the new text colour in 15 bit RGB format.
     */
    void setColor(unsigned short color);

    /** Set the start line to draw the text from. All text before the line is
     * reached is skipped. This can be used to "scroll" text up and down.
     * @param line the line of text at which to start the printing.
     */
    void setStartLine(int line);

    /** Get the current start line value.
     * @return the line to start the text printing at.
     */
    int startLine() const;

    inline bool parseNewline() const;
    inline void setParseNewline(bool parse=true);
    inline void setLink(bool isLink=true);

    void addLink(const HtmlElement * anchor);
    Link * clickLink(int x, int y) const;

  private:
    Font * m_font;
    unsigned short * m_palette;
    unsigned short * m_basePalette;
    int m_paletteLength;
    std::string m_encoding;
    int m_startLine;
    bool m_foundPosition;
    bool m_parseNewline;
    bool m_isLink;

    int m_cursorx;
    int m_cursory;
    int m_initialCursorx;
    int m_initialCursory;
    typedef std::list<Link*> LinkList;
    LinkList m_links;

    void printAt(Font::Glyph & g, int xPosition, int yPosition);
    void incrLine();
    void checkLetter(Font::Glyph & g);
    void printuImpl(const UnicodeString & unicodeString);
    bool doSingleChar(unsigned int value);
    int textSize(const UnicodeString & unicodeString) const;
    void printuWord(const UnicodeString & word);
    void removeLinks();

};

// inline implementations

const Font & TextArea::font() const {
  return *m_font;
}
bool TextArea::parseNewline() const
{
  return m_parseNewline;
}
void TextArea::setParseNewline(bool parse)
{
  m_parseNewline = parse;
}
void TextArea::setLink(bool isLink)
{
  m_isLink = isLink;
}
#endif