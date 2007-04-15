#ifndef DocumentTest_h_seen
#define DocumentTest_h_seen

#include <cppunit/extensions/HelperMacros.h>

class Document;
class DocumentTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( DocumentTest );
  CPPUNIT_TEST( test0 );
  CPPUNIT_TEST( test1 );
  CPPUNIT_TEST( testHtmlAttributes );
  CPPUNIT_TEST( testHead );
  CPPUNIT_TEST( testEmpty );
  CPPUNIT_TEST( testHead2 );
  CPPUNIT_TEST( testTitle );
  CPPUNIT_TEST( testHead3 );
  CPPUNIT_TEST( testGoogle );
  CPPUNIT_TEST( testAnchor );
  CPPUNIT_TEST( testBrokenAnchor );
  CPPUNIT_TEST( testCharacterStart );
  CPPUNIT_TEST( testEndTagStart );
  CPPUNIT_TEST( testMismatchFormat );
  CPPUNIT_TEST( testSimpleBodyA );
  CPPUNIT_TEST( testLi );
  CPPUNIT_TEST( testDD );
  CPPUNIT_TEST( testPlaintext );
  CPPUNIT_TEST( testFont );
  CPPUNIT_TEST( testFont2);
  CPPUNIT_TEST( testPin8);
  CPPUNIT_TEST( testClarin);
  CPPUNIT_TEST( testAdoption);
  CPPUNIT_TEST( testAdoption2);
  CPPUNIT_TEST_SUITE_END();

  public:
  void setUp();
  void tearDown();
  void test0();
  void test1();
  void testHtmlAttributes();
  void testHead();
  void testEmpty();
  void testHead2();
  void testTitle();
  void testHead3();
  void testGoogle();
  void testAnchor();
  void testBrokenAnchor();
  void testCharacterStart();
  void testEndTagStart();
  void testMismatchFormat();
  void testSimpleBodyA();
  void testLi();
  void testDD();
  void testPlaintext();
  void testFont();
  void testFont2();
  void testPin8();
  void testClarin();
  void testAdoption();
  void testAdoption2();
  private:
  Document * m_document;
  char * m_data;
  unsigned int m_length;
  void readFile(const char * fileName);
};

#endif