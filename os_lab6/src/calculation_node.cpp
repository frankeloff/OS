#include "my_zmq.h"
#include <iostream>
#include <map>
#include <ctime>
#include <unistd.h>


int main(int argc, char **argv) {
    assert(argc == 2);
    int id = std::stoi(std::string(argv[1]));
    void *parent_context = zmq_ctx_new(); //создаем контекст
    void *parent_socket = zmq_socket(parent_context, ZMQ_PAIR); //создаем сокет
    if (zmq_connect(parent_socket, ("tcp://localhost:" + std::to_string(PORT_BASE + id)).c_str()) != 0) {
        std::cout << id << ": Can't connect to parent" << std::endl;
    };

    std::pair<void *, void *> brother, child; // <ctx, sock>
    int child_id = -1, brother_id = -1;
    std::cout << "OK: " << getpid() << std::endl;
    clock_t start = 0, now = 0;
    bool has_child = false, has_brother = false, awake = true, timer = false;
    while (awake) {
        msg_t token({fail, 0, 0});
        zmq::receive_msg(token, parent_socket);
        auto *reply = new msg_t({fail, id, id});
        if (token.action == create) {
            if (token.parent_id == id) {
                if (!has_child) {
                    zmq::init_ctx_socket(child.first, child.second);
                    if (zmq_bind(child.second, ("tcp://*:" + std::to_string(PORT_BASE + token.id)).c_str()) != 0) {
                        perror("Bind:");
                        exit(EXIT_FAILURE);
                    }
                    int fork_id = fork();
                    if (fork_id == 0) {
                        execl(NODE_EXECUTABLE_NAME, NODE_EXECUTABLE_NAME, std::to_string(token.id).c_str(), nullptr);
                        exit(EXIT_FAILURE);
                    } else if (fork_id > 0) {
                        auto msg_ping = new msg_t({ping, token.id, token.id});
                        msg_t reply_ping({fail, token.id, token.id});
                        bool ok = zmq::send_receive_wait(msg_ping, reply_ping, child.second);
                        ok = ok and reply_ping.action == success;
                        if (ok) {
                            reply->action = success;
                            child_id = token.id;
                            has_child = true;
                        } else {
                            if (zmq_close(child.second) != 0) {
                                exit(EXIT_FAILURE);
                            }
                            if (zmq_ctx_destroy(child.first) != 0) {
                                exit(EXIT_FAILURE);
                            }
                        }
                    } else {
                        perror("Fork error:");
                        exit(EXIT_FAILURE);
                    }
                } else {
                    auto *son_msg = new msg_t({create_brother, child_id, token.id});
                    zmq::send_receive_wait(son_msg, *reply, child.second);
                }
            } else {
                if (has_child) {
                    zmq::send_receive_wait(&token, *reply, child.second);
                    if (reply->action == success) {
                        zmq::send_msg_no_wait(reply, parent_socket);
                        continue;
                    }
                }
                if (has_brother) {
                    zmq::send_receive_wait(&token, *reply, brother.second);
                }
            }
        } else if (token.action == create_brother) {
            if (token.parent_id == id) {
                if (!has_brother) {
                    zmq::init_ctx_socket(brother.first, brother.second);
                    if (zmq_bind(brother.second, ("tcp://*:" + std::to_string(PORT_BASE + token.id)).c_str()) != 0) {
                        perror("Bind:");
                        exit(EXIT_FAILURE);
                    }
                    int fork_id = fork();
                    if (fork_id == 0) {
                        execl(NODE_EXECUTABLE_NAME, NODE_EXECUTABLE_NAME, std::to_string(token.id).c_str(), nullptr);
                        perror("Fork");
                        exit(EXIT_FAILURE);
                    } else if (fork_id > 0) {
                        auto msg_ping = new msg_t({ping, token.id, token.id});
                        msg_t reply_ping({fail, token.id, token.id});
                        bool ok = zmq::send_receive_wait(msg_ping, reply_ping, brother.second);
                        ok = ok and (reply_ping.action == success);
                        if (ok) {
                            reply->action = success;
                            brother_id = token.id;
                            has_brother = true;
                        } else {
                            if (zmq_close(child.second) != 0) {
                                exit(EXIT_FAILURE);
                            }
                            if (zmq_ctx_destroy(child.first) != 0) {
                                exit(EXIT_FAILURE);
                            }
                        }
                    } else {
                        perror("Fork");
                        exit(EXIT_FAILURE);
                    }
                } else {
                    auto *brother_msg = new msg_t({create_brother, brother_id, token.id});
                    zmq::send_receive_wait(brother_msg, *reply, brother.second);
                }
            } else {
                if (has_child) {
                    zmq::send_receive_wait(&token, *reply, child.second);
                    if (reply->action == success) {
                        zmq::send_msg_no_wait(reply, parent_socket);
                        continue;
                    }
                }
                if (has_brother) {
                    zmq::send_receive_wait(&token, *reply, brother.second);
                }
            }
        } else if (token.action == ping) {
            if (token.id == id) {
                reply->action = success;
            } else {
                if (has_child) {
                    zmq::send_receive_wait(&token, *reply, child.second);
                    if (reply->action == success) {
                        zmq::send_msg_no_wait(reply, parent_socket);
                        continue;
                    }
                }
                if (has_brother) {
                    zmq::send_receive_wait(&token, *reply, brother.second);
                }
            }
        } else if (token.action == exec) {
            if (token.id == id) {
                switch (token.parent_id) {
                    case 1: {
                        timer = true;
                        start = clock();
                        break;
                    }
                    case -1: {
                        timer = false;
                        break;
                    }
                    case 0: {
                        now = clock();
                        timer ? reply->parent_id = (int) (now - start) : reply->parent_id = -1;
                        break;
                    }
                }
                reply->action = success;
            } else{
                if (has_child) {
                    zmq::send_receive_wait(&token, *reply, child.second);
                    if (reply->action == success) {
                        zmq::send_msg_no_wait(reply, parent_socket);
                        continue;
                    }
                }
                if (has_brother) {
                    zmq::send_receive_wait(&token, *reply, brother.second);
                }
            }
        } else if(token.action == kill){
            reply->action = kill;
            if(has_child){
                zmq::send_msg_no_wait(reply, child.second);
                zmq_close(child.second);
                zmq_ctx_destroy(child.first);
            }
            if(has_brother){
                zmq::send_msg_no_wait(reply, brother.second);
                zmq_close(brother.second);
                zmq_ctx_destroy(child.first);
            }
            zmq_close(parent_socket);
            zmq_ctx_destroy(parent_context);
            exit(0);
        }
        zmq::send_msg_no_wait(reply, parent_socket);
    }
}