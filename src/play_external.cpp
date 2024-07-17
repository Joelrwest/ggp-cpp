/*
Using code from:
https://stackoverflow.com/questions/9528811/websocket-client-in-c
*/

#include "../propnet/include/propnet.hpp"
#include "../propnet/include/vector_state.hpp"
#include "setup.hpp"

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <nlohmann/json.hpp>

namespace net = boost::asio;
namespace beast = boost::beast;

static constexpr auto PROGRAM_NAME{"gpp-cpp"};

class Client
{
  public:
    Client(std::string_view game_name, std::string_view role_name);

    enum class MessageType
    {
        Error,
        Update,
        Terminal,
        Finished,
    };

    class Error : public std::runtime_error
    {
      public:
        Error(const char *message);
    };

    void send_input(std::string_view role, propnet::PropId input);
    std::pair<MessageType, nlohmann::json> read_message();

  private:
    static constexpr auto HOST{"localhost"};
    static constexpr auto PORT{"8080"};

    static constexpr auto TYPE_KEY{"type"};
    static constexpr auto ROLE_KEY{"role"};
    static constexpr auto GAME_KEY{"game"};
    static constexpr auto INPUT_KEY{"input"};
    static constexpr auto PROGRAM_KEY{"program"};

    static constexpr auto NEW_CONNECTION_MESSAGE_TYPE{"connection"};
    static constexpr auto INPUT_MESSAGE_TYPE{"input"};
    static constexpr auto ERROR_MESSAGE_TYPE{"error"};
    static constexpr auto UPDATE_MESSAGE_TYPE{"update"};
    static constexpr auto TERMINAL_MESSAGE_TYPE{"terminal"};
    static constexpr auto FINISHED_MESSAGE_TYPE{"finished"};

    inline static const std::unordered_map<std::string, MessageType> message_type_mapping{
        {ERROR_MESSAGE_TYPE, MessageType::Error},
        {UPDATE_MESSAGE_TYPE, MessageType::Update},
        {TERMINAL_MESSAGE_TYPE, MessageType::Terminal},
        {FINISHED_MESSAGE_TYPE, MessageType::Finished}};

    void send(const nlohmann::json &json);

    net::io_service ios;
    net::ip::tcp::resolver resolver;
    net::ip::tcp::socket socket;
    beast::websocket::stream<decltype(socket) &> websocket;
};

void update(const propnet::Propnet &propnet, propnet::State &state, agents::Agent &agent,
            const nlohmann::json &message_json);
void terminal(const propnet::Propnet &propnet, propnet::State &state, agents::Agent &agent);

Client::Client(std::string_view game_name, std::string_view role_name)
    : ios{}, resolver{ios}, socket{ios}, websocket{socket}
{
    net::connect(socket, resolver.resolve(net::ip::tcp::resolver::query{HOST, PORT}));
    websocket.handshake(HOST, "/");

    const nlohmann::json json{{TYPE_KEY, NEW_CONNECTION_MESSAGE_TYPE},
                              {GAME_KEY, game_name},
                              {ROLE_KEY, role_name},
                              {PROGRAM_KEY, PROGRAM_NAME}};
    send(json);
}

Client::Error::Error(const char *message) : std::runtime_error{message}
{
}

void Client::send_input(std::string_view role, propnet::PropId input)
{
    const nlohmann::json json{{TYPE_KEY, INPUT_MESSAGE_TYPE}, {INPUT_KEY, input}, {ROLE_KEY, role}};
    send(json);
}

std::pair<Client::MessageType, nlohmann::json> Client::read_message()
{
    net::streambuf stream_buffer{};
    const auto read_size{websocket.read(stream_buffer)};
    stream_buffer.commit(read_size);

    std::istream input_stream{&stream_buffer};
    std::istreambuf_iterator<char> input_stream_it{input_stream};
    std::string input{input_stream_it, std::istreambuf_iterator<char>{}};

    /*
    TODO: I hate my life this wasted so much time.
    This only works if you do as below (with assignment)
    but not if you you use the constructor like this?? Why????

    const auto json{nlohmann::json::parse(input)};
    */
    const auto json = nlohmann::json::parse(input);

    if (const auto type_str_it{json.find(TYPE_KEY)}; type_str_it != json.end())
    {
        if (const auto type_it{message_type_mapping.find(*type_str_it)}; type_it != message_type_mapping.end())
        {
            const auto type{type_it->second};
            return std::make_pair(type, std::move(json));
        }

        throw Client::Error{"Invalid message type"};
    }

    throw Client::Error{"Missing type key"};
}

void Client::send(const nlohmann::json &json)
{
    websocket.write(net::buffer(json.dump()));
}

void update(const propnet::Propnet &propnet, propnet::State &state, agents::Agent &agent,
            const nlohmann::json &message_json)
{
    propnet::InputSet inputs{};
    if (const auto it{message_json.find("inputs")}; it != message_json.end())
    {
        for (const auto &input : *it)
        {
            inputs.add(input);
        }
    }
    else
    {
        /*
        Probs should be the responsibility of client to check this...
        */
        throw std::runtime_error{"Missing inputs key"};
    }

    propnet.take_sees_inputs(state, inputs);
    agent.take_observations(state);
    propnet.take_non_sees_inputs(state, inputs);
}

void terminal(const propnet::Propnet &propnet, propnet::State &state, agents::Agent &agent)
{
    state = propnet.create_initial_state();
    agent.prepare_new_game();
    agent.take_observations(state);
}

int main(void)
{
    const propnet::Propnet propnet{setup::load_propnet()};
    const auto agent{setup::create_agent(propnet)};
    Client client{propnet.get_game_name(), agent->get_role_name()};

    auto state{propnet.create_initial_state()};
    agent->prepare_new_game();
    agent->take_observations(state);

    client.send_input(agent->get_role_name(), agent->get_legal_input(state));
    while (true)
    {
        const auto [message_type, message_json]{client.read_message()};
        switch (message_type)
        {
        case Client::MessageType::Error:
            std::cout << "Error message received:\n";
            if (const auto it{message_json.find("message")}; it != message_json.end())
            {
                std::cout << *it;
            }
            else
            {
                /*
                Same here about client checking
                */
                throw std::runtime_error{"Missing message key"};
            }
            return 1;
        case Client::MessageType::Update:
            update(propnet, state, *agent, message_json);
            if (!propnet.is_game_over(state))
            {
                client.send_input(agent->get_role_name(), agent->get_legal_input(state));
            }
            break;
        case Client::MessageType::Terminal:
            terminal(propnet, state, *agent);
            client.send_input(agent->get_role_name(), agent->get_legal_input(state));
            break;
        case Client::MessageType::Finished:
            std::cout << "Games finished!\n";
            return 0;
        default:
            throw std::runtime_error{"Received unhandled message type"};
        }
    }
}
