#include <cstring>
#include <cerrno>
#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

static const int kBufSize = 512;
static const int kBackLog = 2;

int main(int argc, char** argv) {
	if (argc < 3) {
		std::cerr << "Usage: " << argv[0] <<
			     " <socket_path> <command>" << std::endl;
		return 1;
	}

	std::string socket_path = argv[1];
	std::string cmd = argv[2];

	int cfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (cfd < 0) {
		std::cerr << "Failed to create socket" << std::endl;
		return 1;
	}

	struct sockaddr_un addr;
	memset(&addr, 0, sizeof(struct sockaddr_un));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, socket_path.c_str(), sizeof(addr.sun_path) - 1);

	if (connect(cfd, (struct sockaddr*) &addr,
		    sizeof(struct sockaddr_un)) < 0) {
		std::cerr << "Connect failed" << std::endl;
		return 1;
	}

	ssize_t sent = 0;
	while (sent < cmd.size()) {
		ssize_t num_wrote = write(cfd, cmd.c_str() + sent,
					  cmd.size() - sent);
		if (num_wrote < 0) {
			std::cerr << "Write failed" << std::endl;
			return 1;
		}

		sent += num_wrote;
	}


	ssize_t num_read;
	char tmp[kBufSize];
	num_read = read(cfd, tmp, kBufSize);

	if (num_read < 0) {
		std::cerr << "Read failed" << std::endl;
		return 1;
	}

	std::string response(tmp, num_read);
	std::cout << response << std::endl;

	return 0;
}
