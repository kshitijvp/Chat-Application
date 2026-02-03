This is the basic chat application framework made using 
Boost.Asio in C++.

Multiple clients can connect to the server and communicate
with each other via text. Writing this from scratch helped
me understand a lot of C++ internals as well as multithreading,
sockets programming and much more.

TODO:
  Plan on adding utility to send formats other than text like
  PNG, PDF, etc. The idea is to serialize these files and send
  them just like text messages and deserialize them on the
  reciever side.
