// 
// NetworkManager.h
// 
// Manage network connections to/from engine. 
// 

#ifndef NO_NETWORK

#ifndef __NETWORK_MANAGER_H__
#define __NETWORK_MANAGER_H__

// System includes.
#include <queue>
#include <string>
#include <vector>

// Engine includes.
#include "Manager.h"

// Two-letter acronym for easier access to manager.
#define NM df::NetworkManager::getInstance()

namespace df {

const std::string DRAGONFLY_PORT = "9876";

// Delayed messages: time (ticks), message (char *), length (bytes).
using delayedMessage = std::tuple<int, char *, size_t>;
using delayQueue = std::queue<delayedMessage>;
  
class NetworkManager : public Manager {

 private:
  NetworkManager();                       // Private since a singleton.
  NetworkManager(NetworkManager const&);  // Don't allow copy.
  void operator=(NetworkManager const&);  // Don't allow assignment.
  int m_accept_sock;	                  // Socket for accept connections.
  std::vector<int> m_sock;		  // Connected network sockets.
  int m_max_sock;                         // Limit of connected sockets.
  std::vector<int> m_delay;               // Delay for outgoing mgs per socket.
  std::vector<delayQueue> m_delay_q;      // Queue for outgoing delayed msgs per socket.

public:

  /// Get the one and only instance of the NetworkManager.
  static NetworkManager &getInstance();

  /// Start up NetworkManager.
  int startUp() override;

  /// Shut down NetworkManager.
  void shutDown() override;

  /// Return true only for network events.
  /// Return false for other engine events.
  bool isValid(std::string event_type) const override;

  /// Setup NetworkManager as server (if false, reset to client).
  /// Return 0 if ok, else -1.
  int setServer(bool server=true, std::string port = DRAGONFLY_PORT);

  /// Return true if successufully setup as server, else false.
  bool isServer() const;

  /// Accept network connection.
  /// If successful, generate EventNetwork (accept).
  /// Return sock index if new connection (note, 0 for first).
  /// Return -1 if no new connection, but no error.
  /// Return -2 if error.
  int accept();

  /// Make network connection to host at port.
  /// If successful, generate EventNetwork (connect).
  /// Return socket index if success, -1 if error.
  int connect(std::string host, std::string port = DRAGONFLY_PORT);

  /// Set maximum number of connections.
  void setMaxConnections(int new_max_sock);

  /// Get maximum number of connections.
  int getMaxConnections() const;

  /// Get number of connected sockets.
  int getNumConnections() const;

  /// Send bytes from buffer to connected network socket.
  /// Return 0 if success, -1 if error.
  int send(const void *buffer, int bytes, int sock_index=0);

  /// Receive from connected network socket (no more than bytes).
  /// If leave is true, don't remove data from socket (peek).
  /// Return number of bytes received, -1 if error.
  int receive(void *buffer, int bytes, bool leave, int sock_index=0);

  /// Close network connection on indicated socket.
  /// If successful, generate EventNetwork (close).
  /// Return 0 if success, else -1.
  int close(int sock_index=0);

  /// Close all network connections.
  /// If successful, generate EventNetwork (close), for each.
  /// Return 0 if success, else negative number.
  int closeAll();

  /// If complete network message on indicated socket, (first
  /// int is message size, in bytes) generate EventNetwork (data).
  /// Return 1 if generated event, 0 if no message, -1 if error.
  int genDataEvents(int sock_index=0) const;

  /// Return true if socket is connected, else false.
  bool isConnected(int sock_index=0) const;

  /// Check if network data on indicated socket.
  /// Return amount of data (0 if no data), -1 if not connected or error.
  int isData(int sock_index=0) const;

  /// Check if data on any connected socket.
  /// Return index of first socket with data.
  /// Return -1 if no socket has data.
  int isAnyData() const;

  /// Return system socket from socket index, -1 if error.
  int getSocket(int sock_index=0) const;
 
  /// Set delay amount added to outgoing messages (in ticks).
  /// Return 0 if ok, else -1.
  int setDelay(int new_delay, int sock_index);

  ////////////////////////////////////////////////////
  // Helper methods for adding delay to outgoing messages.

  // Send any delayed messages that have expired.
  // Return 0 if success, -1 if error.
  int sendDelayed();

  // Send bytes from buffer to connected network socket
  // immediately, without delay added via setDelay().
  // Return 0 if success, -1 if error.
  int sendNow(const void *buffer, int bytes, int sock_index=0);
  ////////////////////////////////////////////////////

};

} // end of namespace df

#endif // __NETWORK_MANAGER_H__

#endif // NO_NETWORK
