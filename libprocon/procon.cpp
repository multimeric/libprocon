#include <linux/netlink.h>
#include <linux/connector.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/uio.h>

#include "procon.h"

/// @brief Creates a netlink socket
/// @return The socket's file descriptor
int proc_connect(){
    int sock = socket(
        AF_NETLINK,
        SOCK_DGRAM | SOCK_NONBLOCK | SOCK_CLOEXEC,
        // Kernel messages
        NETLINK_CONNECTOR
    );
    sockaddr_nl addr = {
        .nl_family = AF_NETLINK,
        .nl_pid = getpid(),
        .nl_groups = CN_IDX_PROC
    };
    bind(sock, (sockaddr*) &addr, sizeof(addr));
    proc_cn_mcast_op operation = PROC_CN_MCAST_LISTEN;
    
    iovec message[3] = {
        {
            // The message header
            .iov_base = new nlmsghdr {
                .nlmsg_len = NLMSG_LENGTH (sizeof(cn_msg) + sizeof(proc_cn_mcast_op)),
                .nlmsg_type = NLMSG_DONE,
                .nlmsg_flags = 0,
                .nlmsg_seq = 0,
                .nlmsg_pid = getpid()
            },
            .iov_len = NLMSG_LENGTH(0)
        },
        {
            .iov_base = new cn_msg {
                .id = {
                    .idx = CN_IDX_PROC,
                    .val = CN_VAL_PROC
                },
                .seq = 0,
                .ack = 0,
                .len = sizeof(proc_cn_mcast_op)
            },
            .iov_len = NLMSG_LENGTH(0)
        },
        {
            .iov_base = &operation,
            .iov_len = sizeof(PROC_CN_MCAST_LISTEN)
        }
    };
    writev(sock, message, 3);
    return sock;
}

/// @brief Yields events from a single socket read
/// @param socket File descriptor of the socket to read from
coro::generator<proc_event*> proc_receive(int socket){
    // Will be populated with message name
    sockaddr_nl address;
    // Will be populated with messages
    // Allocate space for as many messages as we can fit in one page
    nlmsghdr buffer[8192 / sizeof(nlmsghdr)];
    iovec payload[1] = {{
        .iov_base = &buffer,
        .iov_len = sizeof(buffer)
    }};
    msghdr message = {
        .msg_name = &address,
        .msg_namelen = sizeof(address),
        .msg_iov = payload,
        .msg_iovlen = 1,
        .msg_control = NULL,
        .msg_controllen = 0,
        .msg_flags = 0
    };
    int len = recvmsg(socket, &message, 0);

    // Ignore anything not sent by the kernel
    if (address.nl_pid != 0)
        co_return;

    // Iterate messages
    for (
        nlmsghdr *current_message = buffer;
        NLMSG_OK(current_message, len);
        current_message = NLMSG_NEXT(current_message, len)
    ){
        if (
        // current_message->nlmsg_type == NLMSG_DONE ||
        current_message->nlmsg_type == NLMSG_ERROR ||
        current_message->nlmsg_type == NLMSG_NOOP
        ) continue;

        cn_msg *data = (cn_msg*) NLMSG_DATA(current_message);
        proc_event *event = (proc_event*) data->data;
        co_yield event;
    }
}

coro::generator<proc_event*> proc_iter(){
    int socket = proc_connect();
    while (true){
        for (auto event : proc_receive(socket)){
            co_yield event;
        }
    }
}
