#ifndef Cache_h_seen
#define Cache_h_seen


#include <string>

class Document;
class URI;
class Cache
{
  public:
    static const char * CACHE_DIR;

    /** Create the cache.
     * @param document needed for setCacheFile, so doc model persists itself.
     * @param useCache use a cache or not.
     * @param clearCache clear the cache at start or not.
     */
    Cache(Document & document, bool useCache, bool clearCache);

    /** Does the given URI live in the cache already?
     * @param uri the uri to check.
     * @return true if the file exists, false otherwise.
     */
    bool contains(const URI & uri) const;

    /** Load a file from the cache. Loads the original headers and the data.
     * @param uri the uri to load.
     * @return true if the file was loaded from the cache, false otherwise.
     */
    bool load(const URI & uri);

    std::string fileName(const URI & uri) const;

    /** Remove the given uri form the cache.
     * @param uri the uri to clean.
     */
    void clean(const URI & uri);

  private:
    Document & m_document;
    bool m_useCache;

    void feed(const std::string & filename);
};
#endif