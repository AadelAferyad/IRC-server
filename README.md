*This project has been created as part of the 42 curriculum by imellali, aaferyad.*

# ft_irc — IRC Server

## Description

`ft_irc` is an IRC (Internet Relay Chat) server written from scratch in C++98.
The goal of the project is to understand how a real network server works by
implementing one: TCP sockets, non-blocking I/O, a single-threaded event loop
built on `poll()`, and a subset of the IRC protocol described in RFC 1459 /
RFC 2812.

The server accepts multiple simultaneous clients on a single process, without
ever forking or threading. All sockets (the listening socket and every client
socket) are non-blocking and are multiplexed through one `poll()` call: no
`read`, `recv`, `send` or `accept` is ever performed outside of an event
reported by `poll()`.

Because it speaks the standard protocol, it works with real IRC clients such as
**irssi**, **HexChat** or **WeeChat**.

What the server does:

- Authenticates clients with a connection password (`PASS`) before registration.
- Registers users with `NICK` / `USER` and enforces unique nicknames.
- Handles channels: joining, parting, topics, invitations, kicks and operator
  privileges.
- Relays private messages between users and broadcasts channel messages.
- Implements the required channel modes: `i`, `t`, `k`, `o`, `l`.

### Architecture

| File / directory | Role |
| --- | --- |
| `srcs/main.cpp` | Entry point, argument checking (`./ircserv <port> <password>`). |
| `srcs/Server.cpp` | Socket setup, `poll()` loop, accept / read / send, message framing on `\r\n`, command parsing, numeric replies. |
| `srcs/Client.cpp` | Per-client state: fd, registration flags, nickname/username/realname, input and output buffers. |
| `srcs/Channel.cpp` | Per-channel state: members, operators, invite list, topic, modes (`+i`, `+t`, `+k`, `+l`). |
| `srcs/Dispatcher.cpp` | Maps a command name to its handler (`std::map<std::string, void (server::*)(...)>`). |
| `srcs/handlers/` | One file per implemented command. |
| `includes/Numeric.hpp` | Numeric reply codes used by the server. |

Data flow of a single message:

```
poll() -> POLLIN -> recv() -> client input buffer
       -> split on "\r\n" -> parse() -> Command{name, params}
       -> Dispatcher::dispatchCmd() -> handler
       -> queueMsg() -> client output buffer + POLLOUT
       -> poll() -> POLLOUT -> send()
```

Reads are buffered per client, so a command split across several TCP packets is
only executed once its terminating `\r\n` has been received. Writes are also
buffered: handlers never call `send()` directly, they append to the recipient's
output buffer and `POLLOUT` is enabled for that socket.

## Instructions

### Requirements

- A C++ compiler supporting C++98 (`c++` / `g++` / `clang++`)
- `make`
- A UNIX-like system (Linux or macOS)

### Compilation

```bash
git clone https://github.com/AadelAferyad/IRC-server.git
cd IRC-server
make
```

This produces the `ircserv` binary at the root of the repository.
The project compiles with `-Wall -Wextra -Werror -std=c++98`.

Other rules:

```bash
make clean    # remove object files
make fclean   # remove object files and the binary
make re       # fclean + all
```

### Execution

```bash
./ircserv <port> <password>
```

- `port` — TCP port the server listens on (1–65535).
- `password` — connection password every client must send with `PASS`.

Example:

```bash
./ircserv 6667 secretpass
```

### Connecting

With HexChat: *Network list → Add → Server `localhost/6667`, set the server
password to `secretpass`, then connect.*

Manually, with `nc` (useful for testing, remember that IRC lines end with CRLF):

```bash
nc -C localhost 6667
PASS secretpass
NICK alice
USER alice 0 * :Alice Liddell
JOIN #general
PRIVMSG #general :hello world
```

## Command Manual

A client must complete registration before using any channel or messaging
command. Registration requires the three commands `PASS`, `NICK` and `USER`;
the server sends `001 RPL_WELCOME` once all three are accepted. Any other
command sent before that is answered with `451 ERR_NOTREGISTERED`.

Notation: `<required>`, `[optional]`. Every line sent to the server must end
with `\r\n`.

### Registration

#### PASS

```
PASS <password>
```

Supplies the server password given on the command line. Must match, otherwise
the server replies `464 ERR_PASSWDMISMATCH`. Ignored once the client is fully
registered.

Errors: `461 ERR_NEEDMOREPARAMS`, `464 ERR_PASSWDMISMATCH`.

#### NICK

```
NICK <nickname>
```

Sets the client's nickname. Nicknames are unique across the server: if the
nickname is already taken the server answers `433 ERR_NICKNAMEINUSE` and the
current nickname is kept.

Errors: `431 ERR_NONICKNAMEGIVEN`, `433 ERR_NICKNAMEINUSE`.

#### USER

```
USER <username> <hostname> <servername> :<realname>
```

Provides the user identity. All four parameters are required; the server stores
`<username>` and `<realname>` (`<hostname>` and `<servername>` are accepted and
ignored, as clients send them anyway). Ignored once the client is registered.

Example: `USER alice 0 * :Alice Liddell`

### Channels

#### JOIN

```
JOIN <#channel> [key]
```

Joins a channel, creating it if it does not exist. Channel names must start
with `#`. **The first user to join a newly created channel becomes its
operator.** On success the server broadcasts the `JOIN` to every member and
sends the name list (`353 RPL_NAMREPLY`, `366 RPL_ENDOFNAMES`) to the joining
client; operators are prefixed with `@`.

The `key` parameter is required when the channel is key-protected (`+k`).
Joining is refused when the channel is invite-only and the client was not
invited (`+i`), or when the user limit is reached (`+l`).

Examples:

```
JOIN #general
JOIN #private secretkey
```

Errors: `403 ERR_NOSUCHCHANNEL`, `475 ERR_BADCHANNELKEY`,
`473 ERR_INVITEONLYCHAN`, `471 ERR_CHANNELISFULL`.

#### PART

```
PART <#channel>
```

Leaves a channel. The departure is broadcast to all members. If the leaving
user was the channel operator, another member is automatically promoted to
operator; when the last member leaves, the channel is destroyed.

Errors: `461 ERR_NEEDMOREPARAMS`, `403 ERR_NOSUCHCHANNEL`,
`442 ERR_NOTONCHANNEL`.

#### TOPIC

```
TOPIC <#channel>            # query the current topic
TOPIC <#channel> :<topic>   # set a new topic
```

With one parameter the topic is returned (`332 RPL_TOPIC`, or `331 RPL_NOTOPIC`
when none is set). With a topic argument the topic is changed and broadcast to
all members. When the channel has mode `+t`, only operators may change it.

Examples:

```
TOPIC #general
TOPIC #general :Welcome to the general channel
```

Errors: `461 ERR_NEEDMOREPARAMS`, `403 ERR_NOSUCHCHANNEL`,
`442 ERR_NOTONCHANNEL`, `482 ERR_CHANOPRIVSNEEDED`.

#### INVITE

```
INVITE <nickname> <#channel>
```

Invites a user to a channel. **Operator only.** The inviter must be on the
channel. The target receives an `INVITE` message and is added to the channel's
invite list, which lets them bypass mode `+i` once. The inviter gets
`341 RPL_INVITING`.

Example: `INVITE bob #private`

Errors: `461 ERR_NEEDMOREPARAMS`, `403 ERR_NOSUCHCHANNEL`,
`442 ERR_NOTONCHANNEL`, `482 ERR_CHANOPRIVSNEEDED`, `401 ERR_NOSUCHNICK`.

#### KICK

```
KICK <#channel> <nickname>
```

Ejects a user from a channel. **Operator only.** The kick is broadcast to all
members, including the target. If the kicked user was an operator, a new
operator is promoted; if the channel becomes empty, it is destroyed.

Example: `KICK #general bob`

Errors: `461 ERR_NEEDMOREPARAMS`, `403 ERR_NOSUCHCHANNEL`,
`442 ERR_NOTONCHANNEL`, `482 ERR_CHANOPRIVSNEEDED`, `401 ERR_NOSUCHNICK`.

#### MODE

```
MODE <#channel> <flag> [argument]
```

Changes a channel mode. **Operator only.** One flag per command; the resulting
mode change is broadcast to all channel members.

| Mode | Argument | Description |
| --- | --- | --- |
| `+i` / `-i` | — | Invite-only. When set, only users invited with `INVITE` can `JOIN`. |
| `+t` / `-t` | — | Topic restricted. When set, only operators can change the topic. |
| `+k` | `<key>` | Sets a channel key (password) required to `JOIN`. |
| `-k` | — | Removes the channel key. |
| `+o` | `<nickname>` | Gives operator privileges to a member of the channel. |
| `-o` | `<nickname>` | Takes operator privileges away from a member. |
| `+l` | `<limit>` | Sets the maximum number of members (must be a positive number). |
| `-l` | — | Removes the user limit. |

Examples:

```
MODE #general +i
MODE #general +k secretkey
MODE #general +o bob
MODE #general +l 10
MODE #general -t
```

Errors: `461 ERR_NEEDMOREPARAMS`, `403 ERR_NOSUCHCHANNEL`,
`442 ERR_NOTONCHANNEL`, `482 ERR_CHANOPRIVSNEEDED`, `472 ERR_UNKNOWNMODE`,
`401 ERR_NOSUCHNICK`.

### Messaging

#### PRIVMSG

```
PRIVMSG <nickname> :<message>
PRIVMSG <#channel> :<message>
```

Sends a message. When the target starts with `#` the message is relayed to
every member of the channel except the sender, and the sender must be a member
of that channel. Otherwise the target is a nickname and the message is
delivered to that user only.

Examples:

```
PRIVMSG bob :hey, are you there?
PRIVMSG #general :hello everyone
```

Errors: `461 ERR_NEEDMOREPARAMS`, `403 ERR_NOSUCHCHANNEL`,
`404 ERR_CANNOTSENDTOCHAN`, `401 ERR_NOSUCHNICK`.

### Session

#### QUIT

```
QUIT [:<message>]
```

Disconnects the client. The user is removed from every channel they were on;
operator privileges are transferred and empty channels are destroyed.

### Numeric replies

| Code | Name | Meaning |
| --- | --- | --- |
| 001 | `RPL_WELCOME` | Registration complete |
| 331 | `RPL_NOTOPIC` | No topic is set |
| 332 | `RPL_TOPIC` | Current channel topic |
| 341 | `RPL_INVITING` | Invitation was sent |
| 353 | `RPL_NAMREPLY` | List of channel members |
| 366 | `RPL_ENDOFNAMES` | End of the members list |
| 401 | `ERR_NOSUCHNICK` | Unknown nickname |
| 403 | `ERR_NOSUCHCHANNEL` | Unknown channel |
| 404 | `ERR_CANNOTSENDTOCHAN` | Not a member of the channel |
| 431 | `ERR_NONICKNAMEGIVEN` | `NICK` sent without a nickname |
| 432 | `ERR_ERRONEUSNICKNAME` | Invalid nickname |
| 433 | `ERR_NICKNAMEINUSE` | Nickname already taken |
| 442 | `ERR_NOTONCHANNEL` | You are not on that channel |
| 451 | `ERR_NOTREGISTERED` | Command used before registration |
| 461 | `ERR_NEEDMOREPARAMS` | Missing parameters |
| 464 | `ERR_PASSWDMISMATCH` | Wrong server password |
| 471 | `ERR_CHANNELISFULL` | Channel is full (`+l`) |
| 472 | `ERR_UNKNOWNMODE` | Unknown mode flag |
| 473 | `ERR_INVITEONLYCHAN` | Channel is invite-only (`+i`) |
| 475 | `ERR_BADCHANNELKEY` | Wrong or missing channel key (`+k`) |
| 482 | `ERR_CHANOPRIVSNEEDED` | Operator privileges required |

## Technical choices

- **Single `poll()` loop, one process, no threads.** `poll()` was chosen over
  `select()` (no `FD_SETSIZE` limit, clearer per-fd event flags) and over
  `epoll`/`kqueue` (not portable between Linux and macOS).
- **Everything non-blocking.** The listening socket and every accepted socket
  are set to `O_NONBLOCK` with `fcntl()`, so a slow or malicious client can
  never stall the server.
- **Input and output buffers per client.** TCP is a stream, not a sequence of
  messages: input is accumulated until a `\r\n` delimiter is found, and output
  is accumulated and flushed on `POLLOUT`, handling partial `send()`.
- **Table-driven dispatch.** `Dispatcher` maps a command name to a pointer to
  member function, so adding a command means adding one file in
  `srcs/handlers/` and one line in the map, instead of an `if/else` chain.
- **`SO_REUSEADDR`** on the listening socket so the server can be restarted
  immediately on the same port without waiting for `TIME_WAIT` to expire.

## Resources

Protocol and specifications:

- [RFC 1459 — Internet Relay Chat Protocol](https://datatracker.ietf.org/doc/html/rfc1459)
- [RFC 2812 — IRC Client Protocol](https://datatracker.ietf.org/doc/html/rfc2812)
- [Modern IRC Client Protocol (ircdocs)](https://modern.ircdocs.horse/)
- [IRC numeric replies list (ircdocs)](https://defs.ircdocs.horse/defs/numerics.html)

Network programming:

- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)
- `man 2 socket`, `man 2 bind`, `man 2 listen`, `man 2 accept`, `man 2 poll`,
  `man 2 recv`, `man 2 send`, `man 2 fcntl`, `man 7 tcp`

Clients used for testing:

- [HexChat](https://hexchat.github.io/)

### Use of AI

AI assistants were used as a documentation and review aid, never as a code
generator for the project's core logic:

- **Understanding the subject and the protocol**: clarifying RFC 1459 / RFC 2812
  wording, the exact registration sequence (`PASS` → `NICK` → `USER`), and which
  numeric reply belongs to which error case.
- **Explaining system calls**: the semantics of non-blocking sockets, `poll()`
  event flags (`POLLIN`, `POLLOUT`, `POLLHUP`, `POLLERR`) and partial
  `send()` / `recv()` behaviour.
- **Debugging help**: reasoning about why real clients (HexChat etc..) rejected
  some of our replies, which led to fixing the CRLF framing and the numeric
  reply format.
