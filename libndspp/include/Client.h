#ifndef Client_h_seen
#define Client_h_seen

namespace nds {
  class Client
  {
    public:
      /*!@brief Create a TCP client that connects to an IP address and port
       * @param ip IP address to connect to.
       * @param port port to connect to.
       */
      Client(const char * ip, int port);

      /*! @brief virtual destructor. Closes the socket (if open) */
      virtual ~Client();

      /*! @brief Connect to the given IP addres and port.*/
      void connect();
      /*! @brief Read bytes from the server address and port.*/
      void read();

      /*! @brief Write data to the target address and port. 
       * @param data pointer to the data to send
       * @param size size in bytes of each element of data
       * @returns number of bytes written
       * */
      unsigned int write(const void * data, unsigned int size);

      /*! @brief Fetch the current status of the connection.
       * @returns true if connected false otherwise.*/
      inline bool isConnected() const { return m_connected; }

      /*! @brief Pure virtual function to handle the bytes read from the server
       * @param buffer Data recieved from the other end.
       * @param amountRead Number of bytes read.
       */
      virtual void handle(void * buffer, int amountRead) = 0;
      /*! @brief Pure virtual function to handle the end of the reading. */
      virtual void finish() = 0;

      /*! @brief debug string 
       * @param c Characters to print (or whatever). */
      virtual void debug(const char * c)=0;

    private:
      //! The IP address
      const char * m_ip;
      //! The port on the server to connect to
      int m_port;
      //! The socket
      int m_tcp_socket;
      //! The status of the connection
      bool m_connected;
  };
};
#endif
