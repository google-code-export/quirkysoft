#ifndef HeaderParser_h_seen
#define HeaderParser_h_seen
#include <string>


class HtmlParser;
class HtmlElement;
class CookieJar;
class URI;

/** Parse the headers and chunks from a HTTP GET request. 
 * The payload is then passed on to the HtmlParser.
 */
class HeaderParser
{
  public:
    /** Constructor for the parser.
     * @param html a pointer to the HtmlParser for parsing the rest of the message.
     * @param cookieJar a pointer to the class that manages the cookies.
     */
    HeaderParser(HtmlParser * html, CookieJar * cookieJar);

    /** Destructor.*/
    ~HeaderParser();

    /** Feed data to the parser. Call this with any amount of data, the state is kept between feeds.
     * To reset the state, call reset().
     * @param data data to be parsed.
     * @param length the amount of data in bytes.
     */
    void feed(const char * data, unsigned int length);

    void setDataState();

    /** Fetch how much HTML is expected. From Content-Length header or chunks.
     * @return the amount of HTML expected in bytes.
     */
    unsigned int expected() const;

    /** Get a redirect URL, if available. This only applies for 3XX HTTP responses.
     * @return The new location URL.
     */
    const std::string redirect() const;

    /** Get the HTTP status code. This is the numeric part of HTTP/1.1 NNN Blah blah.
     * @return the status code.
     */
    unsigned int httpStatusCode() const;

    /** Call this when you have a "meta" tag, it will check if any http-equiv attributes apply.
     * @param meta a meta tag element.
     */
    void checkMetaTagHttpEquiv(const HtmlElement* meta);

    //! Reset the internal parser state.
    void reset();

    void setUri(const std::string & uri);

  private:
    enum HeaderState
    {
      HTTP_RESPONSE,
      BEFORE_FIELD,
      FIELD,
      AFTER_FIELD,
      BEFORE_VALUE,
      VALUE,
      ENDING_HEADERS,
      CHUNK_LINE,
      DATA,
      PARSE_ERROR
    };

    HeaderState m_state;
    URI & m_uri;
    unsigned int m_value;
    const char * m_position;
    const char * m_end;
    const char * m_lastPosition;

    std::string m_field;
    std::string m_headerValue;
    std::string m_redirect;
    bool m_chunked;
    int m_chunkLength;
    std::string m_chunkLengthString;
    unsigned int m_httpStatusCode;
    unsigned int m_expected;

    HtmlParser* m_htmlParser;
    CookieJar * m_cookieJar;

    void parseError();
    void handleHeader(const std::string & field, const std::string & value);
    void rewind();
    void next();
    void httpResponse();
    void beforeField();
    bool isFieldValue() const;
    void field();
    void afterField();
    void beforeValue();
    void value();
    void endingHeaders();
    void chunkLine();
    void fireData();

};

#endif
