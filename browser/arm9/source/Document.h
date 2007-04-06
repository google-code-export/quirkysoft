#ifndef Document_h_seen
#define Document_h_seen
#include <string>
#include <vector>
#include "ViewI.h"
#include "HtmlParser.h"

class HeaderParser;
class Document : public HtmlParser
{
  public:

    enum Status {
      NOTHING,
      LOADED,
      INPROGRESS
    };
    Document();

    void setUri(const std::string & uriString);
    void appendData(const char * data, int size); 
    const std::string & uri() const;
    const std::basic_string<unsigned int> & asText() const;
    Status status() const;
    void setStatus(Status newStatus);
    void setLoading(int amount);
    void reset();

    void registerView(ViewI * v);
    void unregisterView(ViewI * v);

    // Html callbacks
    virtual void handleStartEndTag(const std::string & tag, const std::vector<Attribute*> & attrs);
    virtual void handleStartTag(const std::string & tag, const std::vector<Attribute*> & attrs);
    virtual void handleEndTag(const std::string & tag);
    // virtual void handleData(const std::string & data);
    virtual void handleData(unsigned int ucodeChar);

  private:
    std::string m_uri;
    std::basic_string<unsigned int> m_data;
    Status m_status;
    int m_amount;
    std::vector<ViewI*> m_views;
    HeaderParser * m_headerParser;

    void notifyAll() const;
};
#endif