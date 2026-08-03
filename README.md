# IRC 


What is socket : is an endpoint for communication between two programs running on a network a way of IPC (inter process communication) between to process on a network. And it's a file descriptor
Core Concepts:
	IP Address: Identifies the specific machine on the network.
	Port Number: Identifies the specific application or process on that machine.
	Socket Pair: A unique combination of source IP/port and destination IP/port.
Lifecycle:


    Server: socket() -> bind() -> listen() -> accept() ----+
                                                       | (Data Exchange)
    Client: socket() ------------> connect() ------------->+ -> send/recv() -> close()

--

	1.Creation: Both server and client create a socket using socket().
	2.Binding: The server binds the socket to an IP and port using bind().
	3.Listening: The server waits for incoming connections using listen().
	4.Connecting: The client initiates a connection to the server via connect().
	5.Transfer: Both sides exchange data using send() and recv().
	6.Closing: Both sides close the connection using close()
 
-sockaddr_in : a data structure used to store ipv4 network add and port numbers for socket conections, containing feilds like :
       	sin_family : address family wish must be set to AF_INET for ipv4 (type of ip)
       	sin_port   : the transport layer port number in net byte order using (htons)
       	sin_addr   : a in_addr struct holidng the 4 byte ipv4 add (s_addr)
	sin_zero   : padding bytes set to 0 to make struct size match the generic sockaddr
it is cast to generic scokadrr pointer when passed to system function like bind() or connect ().

-pollfd      : a data structure used to watch file discriptors for input/output events i/o events containing three key fields :

	fd	   : the file discriptor number you want to check if the fd is set to a negative number the struct is ignored
	events	   : a bitmask of input flags telling the system what events to look for (such as readable or writable data)
	revnets	   : a bitmask of output flags filled by the system to show whishc events actually happend
		common flags : 
		POLLIN       : Data is ready to read
		POLLOUT      : Data is ready to write
		POLLERR      : An error happend on the file discriptor
		POLLHUP	     : The connection or channel was hung up.
-poll() : system call in cpp allowes an application to efficiently monitor multiple network sockets simultaneously without blocking on a single connection it monitors an array of pollfd structures blocking the calling thread until at least one socket become ready for I/O operations or a timeout expires.
FUNCTION SIGN : 

		int poll(struct pollfd fds[], nfds_t nfds, int timeout);
		fds[]: An array of pollfd structures representing your active sockets.
		nfds: The total number of items inside your fds array.
		timeout: Time to wait in milliseconds. Use -1 to block indefinitely, or 0 to return immediately.
		Return Value: Returns > 0 indicating the number of sockets with active events, 0 on timeout, and -1 if an internal failure occurs.

