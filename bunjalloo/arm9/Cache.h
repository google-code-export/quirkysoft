/*
  Copyright (C) 2007 Richard Quirk

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef Cache_h_seen
#define Cache_h_seen


#include <string>
#include <map>

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

    void remove(const URI & uri);

    std::string fileName(const URI & uri) const;

    /** Remove the given uri form the cache.
     * @param uri the uri to clean.
     */
    void clean(const URI & uri);

  private:
    Document & m_document;
    bool m_useCache;
    typedef std::map<int, int> CachedMap;
    CachedMap m_fileIds;

    void feed(const std::string & filename);
};
#endif
