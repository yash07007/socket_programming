
// PORT ALLOCATION
// Backend Server A -> UDP: 30504
// Backend Server B -> UDP: 31504
// Main Server -> UDP(with servers): 32504

// Display mainserver bootup message
// Main server is up and running.

// Main server contacts both the backend servers for metadata

// Receive metadata from the respective server
// Main server has received the state list from server A using UDP over port <Main server UDP port number>

// Store the metadata from backend servers into a map (state, server)
// List the results of which states serverA/B is responsible for
// Server A
// <State Name 1>
// <State Name 2>
// 
// Server B
// <State Name 3>

// Ask for user input for a state name
// Enter state name:

// If input state is not found in map print message 
// <State Name> does not show up in server A&B
// If input state is found in map retrive server id
// <State Name> shows up in server <A or B>

// Send statename to server whose server id has been found
// The Main Server has sent request for <State Name> to server <A or B> using UDP over port <Main server UDP port number>

// Receive the city names from the server where request has been posted
// print in following format
// The Main server has received searching result(s) of <State Name> from server<A or B>
// There are <num> distinct cities in <State Name>: <city name1>, <city name2>...

// Ask for a new query
// -----Start a new query-----

