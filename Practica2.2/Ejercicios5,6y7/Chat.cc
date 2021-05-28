#include "Chat.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);

    //Serializar los campos type, nick y message en el buffer _data
    char* aux = _data;
    memcpy(aux, &type, sizeof(uint8_t));
    aux += sizeof(uint8_t);
    memcpy(aux, nick.c_str(), 8 * sizeof(char));
    aux += 8 * sizeof(char);
    memcpy(aux, message.c_str(), 80 * sizeof(char));
}

int ChatMessage::from_bin(char * bobj)
{
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    char auxNick [8];
    char auxMsg [80];

    //Reconstruir la clase usando el buffer _data
    char* aux = _data;
    memcpy(&type, aux, sizeof(uint8_t));
    aux += sizeof(uint8_t);
    memcpy(&auxNick, aux, 8 * sizeof(char));
    aux += 8 * sizeof(char);
    memcpy(&auxMsg, aux, 80 * sizeof(char));

    nick = auxNick;
    message = auxMsg;

    return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatServer::do_messages()
{
    while (true)
    {
        /*
         * NOTA: los clientes están definidos con "smart pointers", es necesario
         * crear un unique_ptr con el objeto socket recibido y usar std::move
         * para añadirlo al vector
         */

        Socket* clientSd;
        ChatMessage msg;
        //Recibir Mensajes en y en función del tipo de mensaje
        socket.recv(msg, clientSd);
        // - LOGIN: Añadir al vector clients
        if(msg.type == ChatMessage::MessageType::LOGIN){
            std::cout << msg.nick << " LOGIN\n";
            clients.push_back(std::unique_ptr<Socket>(std::move(clientSd)));
        }
        // - LOGOUT: Eliminar del vector clients
        else if(msg.type == ChatMessage::MessageType::LOGOUT){
            std::cout << msg.nick << " LOGOUT\n";
            auto it = clients.begin();
            while(it != clients.end() && !(*(*it).get() == *clientSd)) it++;
            clients.erase(it);
        }
        // - MESSAGE: Reenviar el mensaje a todos los clientes (menos el emisor)
        else if(msg.type == ChatMessage::MessageType::MESSAGE){
            std::cout << msg.nick << " MESSAGE\n";
            for(int i=0; i<clients.size(); ++i){
                if(!(*(clients[i].get()) == *clientSd))
                    socket.send(msg, (*clients[i].get()));
            }
        }
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatClient::login()
{
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGIN;

    socket.send(em, socket);
}

void ChatClient::logout()
{
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGOUT;

    socket.send(em, socket);
}

void ChatClient::input_thread()
{
    while (true)
    {
        // Leer stdin con std::getline
        std::string msg;
        std::getline(std::cin, msg);
        if(msg == "q"){
            logout();
            break;
        }
        ChatMessage chatMsg(nick, msg);
        chatMsg.type = ChatMessage::MessageType::MESSAGE;
        // Enviar al servidor usando socket
        socket.send(chatMsg, socket);
    }
}

void ChatClient::net_thread()
{
    while(true)
    {
        //Recibir Mensajes de red
        ChatMessage chatMsg;
        socket.recv(chatMsg);
        //Mostrar en pantalla el mensaje de la forma "nick: mensaje"
        std::cout << chatMsg.nick << ": " << chatMsg.message << "\n";
    }
}

