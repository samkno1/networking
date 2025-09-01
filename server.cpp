#include <cstring>
#include <cerrno>
#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

static const int kBufSize = 512;
static const int kBackLog = 2;

std::string get_head_commit_hash() {
	std::string head_hash;
//
//	git_repository* repo = nullptr;
//	if (git_repository_open(&repo, ".") < 0) {
//		std::cerr << "Failed to open repository" << std::endl;
//		return NULL;
//	}
//
//	git_reference *head = nullptr;
//	if (git_repository_head(&head, repo) < 0) {
//		std::cerr << "Failed to get HEAD" << std::endl;
//		git_repository_free(repo);
//		return NULL;
//	}
//
//	// get commit ID
//	const git_oid* oid = git_reference_target(head);
//	char hash[GIT_OID_HEXSZ + 1];
//	git_oid_tostr(hash, sizeof(hash), oid);
//	head_hash = hash;
	char tmp[kBufSize];
	FILE* pipe = popen("git rev-parse HEAD", "r");
	if (!pipe) {
		std::cerr << "Failed to get hash of latest commit" << std::endl;
		return NULL;
	}

	while (fgets(tmp, kBufSize, pipe) != nullptr)
		head_hash.append(tmp);

	// Remove new line from end of string
	if (head_hash.back() == '\n')
		head_hash.pop_back();

	return head_hash;
}

int write_socket(int fd, std::string s) {
	ssize_t sent = 0;
	while (sent < s.size()) {
		ssize_t num_wrote = write(fd, s.c_str() + sent,
					  s.size() - sent);
		if (num_wrote < 0) {
			std::cerr << "Write failed" << std::endl;
			return 1;
		}

		sent += num_wrote;
	}

	return 0;
}

int main(int argc, char** argv) {
	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] <<
				" <socket-path>" << std::endl;
		return 1;
	}

	// Initialize the git2 library
	git_libgit2_init();

	std::string socket_path = argv[1];

	int sfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sfd < 0) {
		std::cerr << "Server failed" << std::endl;
		return 1;
	}

	if (remove(socket_path.c_str()) < 0 && errno != ENOENT) {
		std::cerr << "Unable to remove " << socket_path << std::endl;
		return 1;
	}

	struct sockaddr_un addr;
	memset(&addr, 0, sizeof(struct sockaddr_un));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, socket_path.c_str(), sizeof(addr.sun_path) - 1);

	if (bind(sfd, (struct sockaddr*) &addr, sizeof(struct sockaddr_un)) < 0) {
		std::cerr << "Bind failed" << std::endl;
		return 1;
	}

	if (listen(sfd, kBackLog) < 0) {
		std::cerr << "Listen failed" << std::endl;
		return 1;
	}

	for (;;) {
		int cfd = accept(sfd, NULL, NULL);
		if (cfd < 0) {
			std::cerr << "Accept failed" << std::endl;
			return 1;
		}

		ssize_t num_read;
		char tmp[kBufSize];
		std::string cmd;
		num_read = read(cfd, tmp, kBufSize);

		if (num_read < 0) {
			std::cerr << "Read failed" << std::endl;
			return 1;
		}

		cmd = tmp;
		if (cmd == "VERSION") {
			std::string hash = get_head_commit_hash();
			if (hash.empty())
				return 1;

			if (write_socket(cfd, hash)) {
				return 1;
			}
		} else {
			if (write_socket(cfd, "REJECTED")) {
				return 1;
			}
		}

		if (close(cfd) < 0) {
			std::cerr << "Close failed" << std::endl;
			return 1;
		}
	}

	return 0;
}
