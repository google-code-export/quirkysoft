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
#ifndef Controller_h_seen
#define Controller_h_seen

class Document;
class View;
class Config;
class URI;
class Cache;

class HttpClient;
/** Controller maps user actions to model updates. For example, responds to setting the URI.
 */
class Controller
{
  friend class HttpClient;
  public:
    //!Constructor.
    Controller();
    //! Destructor
    ~Controller();

    /** Handle the user request for setting a URI.
     * @param uri the URI to use.
     */
    void doUri(const URI & uri);

    /** Refresh the page - reload from http not from cache.  */
    void reload();
    
    void previous();
    void next();
    void stop();

    const Config & config() const;

    /** Show the software licence.*/
    void showLicence();

    /** Loops forever.*/
    void mainLoop();

    bool wifiInitialised() const;

    Cache * cache() const;
  private:
    Document * m_document;
    View * m_view;
    Config * m_config;
    Cache * m_cache;
    bool m_wifiInit;
    bool m_stop;


    void localFile(const std::string &);
    void fetchHttp(const URI &);
    void configureUrl(const std::string & fileName);
    void loadError();
    // helper to avoid code dupe
    void handleUri(const URI & uri);
};
#endif