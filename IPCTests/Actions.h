
/// The different actions a worker thread can do
#define ACTION int
#define ACTION_STOP -2
#define ACTION_NONE -1
#define ACTION_WRITE 0
#define ACTION_READ 1
#define ACTION_DISCONNECT 2
#define ACTION_CLOSE_SERVER 3
#define ACTION_DECONSTRUCT 4
#define ACTION_CONNECT 5