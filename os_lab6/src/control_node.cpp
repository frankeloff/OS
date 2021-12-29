#include <unistd.h>
#include <vector>
#include <zmq.h>

#include "my_zmq.h"
#include "N_Tree.h"

using node_id_type = int;


int main() {
    auto *topology = new N_Tree<node_id_type>(-1);
    void *socket, *ctx;// [context, socket]
    std::string s;
    node_id_type id;
    std::cout << "\t\tUsage" << std::endl;
    std::cout << "Create id parent: create calculation node (use parent = -1 if parent is control node)" << std::endl;
    std::cout << "Ping id: ping calculation node with id $id" << std::endl;
    std::cout << "Exec id n k1,k2,...,kn: sum k1,k2,...,kn" << std::endl;
    std::cout << "Print end 1: exit" << std::endl;
    std::cout << "Print print 1: print topology" << std::endl;
    while (std::cin >> s >> id) {
        if (s == "create") {
            node_id_type parent_id;
            std::cin >> parent_id;
            if (parent_id == -1) {
                if (topology->get_data() != -1) {
                    std::cout << "Root already exists" << std::endl;
                    continue;
                }
                zmq::init_ctx_socket(ctx, socket);
                zmq_bind(socket, ("tcp://*:" + std::to_string(PORT_BASE + id)).c_str()); //связываем сокет с локальной конечной точкой
                int fork_id = fork();
                if (fork_id == 0) {
                    execl(NODE_EXECUTABLE_NAME, NODE_EXECUTABLE_NAME, std::to_string(id).c_str(), nullptr);
                    return 0;
                } else {
                    auto *token = new msg_t({ping, id, id});
                    msg_t reply({fail, id, id});
                    if (zmq::send_receive_wait(token, reply, socket) and reply.action == success) {
                        topology->set_data(id);
                    } else {
                        if (zmq_close(socket) != 0) {
                            exit(EXIT_FAILURE);
                        }
                        if (zmq_ctx_destroy(ctx) != 0) {
                            exit(EXIT_FAILURE);
                        }
                    }
                }
            } else if (search_tree(parent_id, topology) == nullptr) {
                std::cout << "Error: Parent not found" << std::endl;
                continue;
            } else {
                if (search_tree(id, topology) != nullptr) {
                    std::cout << "Error: Already exists" << std::endl;
                    continue;
                }
                auto *msg = new msg_t({create, parent_id, id});
                msg_t reply({fail, id, id});
                if (zmq::send_receive_wait(msg, reply, socket) and reply.action == success) {
                    topology->insert(id, parent_id);
                } else {
                    std::cout << "Error: Parent is unavailable" << std::endl;
                }
            }
        } else if (s == "ping") {
            if (search_tree(id, topology) == nullptr) {
                std::cout << "Error: Not found" << std::endl;
                continue;
            }
            auto *token = new msg_t({ping, id, id});
            msg_t reply({fail, id, id});
            if (zmq::send_receive_wait(token, reply, socket) and reply.action == success) {
                std::cout << "OK: 1" << std::endl;
            } else {
                std::cout << "OK: 0" << std::endl;
            }
        } else if (s == "exec") {
            std::string cmd;
            std::cin >> cmd;
            if (cmd == "start" or cmd == "stop") {
                auto *token = new msg_t({exec, 2, id});
                (cmd == "start") ? token->parent_id = 1 : token->parent_id = -1;
                msg_t reply({fail, id, id});
                if (zmq::send_receive_wait(token, reply, socket) and reply.action == success) {
                    std::cout << "OK: " << id << std::endl;
                } else {
                    std::cout << "Node is unavailable" << std::endl;
                }
            } else if (cmd == "time"){
                auto *token = new msg_t({exec, 0, id});
                msg_t reply({fail, 0, 0});
                if (zmq::send_receive_wait(token, reply, socket) and reply.action == success){
                    if (reply.parent_id != -1) {
                        std::cout << "OK: " << id << ": " << reply.parent_id << std::endl;
                    } else{
                        std::cout << "Error: Timer isn't started" << std::endl;
                    }
                }
            }
        } else if(s == "end"){
            if(id == 1){
                auto *token = new msg_t({kill, 0, id});
                zmq::send_msg_no_wait(token, socket);
                delete_topology(topology);
                break;
            }
        }else if (s == "print") {
            if (id == 1) traverse_inorder(topology);
        }
    }
    zmq_close(socket);
    zmq_ctx_destroy(ctx);
    return 0;
}