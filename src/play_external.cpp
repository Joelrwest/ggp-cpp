/*
Using code from:
https://stackoverflow.com/questions/9528811/websocket-client-in-c
*/

#include "../player/include/player.hpp"
#include "../propnet/include/propnet.hpp"
#include "../propnet/include/vector_state.hpp"
#include "setup.hpp"

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <nlohmann/json.hpp>

namespace net = boost::asio;
namespace beast = boost::beast;

static constexpr auto PROGRAM_NAME{"gpp-cpp"};
static constexpr auto AGENT_COMMAND{"agent"};
static constexpr auto ROLE_COMMAND{"role"};
static constexpr auto MODEL_GAME_NUMBER_COMMAND{"model-number"};

static constexpr auto MAX_SAMPLE_SIZE_COMMAND{"sample-size"};
static constexpr auto DEFAULT_MAX_SAMPLE_SIZE{50};

static constexpr auto MAX_CFR_TIME_COMMAND{"cfr-time"};
static constexpr auto DEFAULT_MAX_CFR_TIME_S{3};

static constexpr auto MAX_TOTAL_SEARCH_TIME_COMMAND{"total-time"};
static constexpr auto DEFAULT_MAX_TOTAL_SEARCH_TIME_S{30};

static constexpr auto NUM_THREADS_COMMAND{"threads"};
static constexpr auto DEFAULT_NUM_THREADS{1};

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
    I hate my life this wasted so much time.
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

int main(int argc, char **argv)
{
    std::string game{};
    std::string agent_name{};
    std::string role_name{};
    std::size_t model_game_number{};
    std::size_t max_sample_size{};
    std::size_t max_cfr_time_s{};
    std::size_t max_total_search_time_s{};
    std::size_t num_threads{};
    auto options_description{setup::create_base_program_options(game)};
    options_description.add_options()(AGENT_COMMAND, po::value<std::string>(&agent_name)->required(),
                                      "agent name to play")(
        ROLE_COMMAND, po::value<std::string>(&role_name)->required(),
        "agent role to be given")(MODEL_GAME_NUMBER_COMMAND, po::value<std::size_t>(&model_game_number)->required(),
                                  "game number to load model from")(
        MAX_SAMPLE_SIZE_COMMAND, po::value<std::size_t>(&max_sample_size)->default_value(DEFAULT_MAX_SAMPLE_SIZE),
        "maximum sample size for ggp player (if player agent type is given)")(
        MAX_CFR_TIME_COMMAND, po::value<std::size_t>(&max_cfr_time_s)->default_value(DEFAULT_MAX_CFR_TIME_S),
        "maximum time that ggp player can CFR search each sampled state (in seconds if player agent type is given)")(
        MAX_TOTAL_SEARCH_TIME_COMMAND,
        po::value<std::size_t>(&max_total_search_time_s)->default_value(DEFAULT_MAX_TOTAL_SEARCH_TIME_S),
        "maximum time that the ggp player can search for in total (in seconds if player agent type is given)")(
        NUM_THREADS_COMMAND, po::value<std::size_t>(&num_threads)->default_value(DEFAULT_NUM_THREADS),
        "theads the ggp player are given (if player agent type is given)");
    setup::parse_program_options(options_description, argc, argv);

    const propnet::Propnet propnet{setup::load_propnet(game)};
    const auto &roles{propnet.get_player_roles()};
    const auto role_it{std::find_if(roles.begin(), roles.end(),
                                    [&role_name](const auto &role) { return role.get_name() == role_name; })};
    if (role_it == roles.end())
    {
        std::cout << "Role " << role_name << " wasn't found\n";
        return 1;
    }

    const auto role{*role_it};

    std::unique_ptr<agents::Agent> agent{nullptr};
    if (agent_name == player::Player<>::NAME)
    {
        auto model{player::Model::load_game_number(propnet, game, model_game_number)};

        const std::chrono::seconds max_cfr_time{max_cfr_time_s};
        const std::chrono::seconds max_total_search_time{max_total_search_time_s};
        const auto player_options{player::Player<>::Options{}
                                      .add_cfr_time_limit(max_cfr_time)
                                      .add_max_sample_size(max_sample_size)
                                      .add_total_time_limit(max_total_search_time)};

        agent = std::make_unique<player::Player<>>(role, propnet, model, player_options);
    }
    else
    {
        agent = setup::agent_factory(agent_name, role);
    }

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
