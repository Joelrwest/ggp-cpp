#include "setup.hpp"
#include "../../player/include/player.hpp"
#include "../agents/include/human.hpp"
#include "../propnet/include/parser.hpp"

#include <iostream>
#include <stdexcept>

namespace setup
{
static constexpr auto HELP_COMMAND{"help"};
static constexpr auto GAME_COMMAND{"game"};

ProgramOptionsError::ProgramOptionsError(const char *message) : std::invalid_argument{message}
{
}

po::options_description create_base_program_options(std::string &game)
{
    po::options_description options_description{"Allowed options"};
    options_description.add_options()(HELP_COMMAND, "produce help message")(
        GAME_COMMAND, po::value<std::string>(&game)->required(), "game to play");

    return options_description;
}

po::variables_map parse_program_options(po::options_description &options_description, int argc, char **argv)
{
    po::variables_map variables_map;
    po::store(po::parse_command_line(argc, argv, options_description), variables_map);
    try
    {
        po::notify(variables_map);
    }
    catch (const boost::program_options::required_option &error)
    {
        if (variables_map.contains(HELP_COMMAND))
        {
            std::cout << options_description;
            throw ProgramOptionsError{"Help option given"};
        }
        else
        {
            std::cout << error.what();
            throw ProgramOptionsError{"Invalid options given"};
        }
    }

    if (variables_map.contains(HELP_COMMAND))
    {
        std::cout << options_description;
        throw ProgramOptionsError{"Help option given"};
    }

    return variables_map;
}

propnet::Propnet load_propnet(std::string_view game)
{
    std::cout << "Loading propnet\n";
    try
    {
        propnet::Parser parser{game};
        const auto propnet{parser.create_propnet()};
        std::cout << "Propnet loaded\n";

        return propnet;
    }
    catch (const propnet::ParsingError &error)
    {
        std::cout << "Error occurred whilst trying to parse " << game << ": " << error.what() << '\n';
        throw error;
    }
}

std::unique_ptr<agents::Agent> create_agent(const propnet::Propnet &propnet)
{
    const auto &roles{propnet.get_player_roles()};
    while (true)
    {
        std::string role_name{};
        std::cout << "Enter agent role: ";
        std::cin >> role_name; // TODO: Remove this and instead make it a command line argument

        const auto role_it{std::find_if(roles.begin(), roles.end(),
                                        [&role_name](const auto &role) { return role_name == role.get_name(); })};
        if (role_it != roles.end())
        {
            return create_agent(*role_it);
        }

        std::cout << "Error occurred whilst trying to find a role with name " << role_name << '\n';
        std::cout << "Valid roles are:\n";
        for (const auto &role : roles)
        {
            std::cout << role.get_name() << '\n';
        }
        std::cout << "Please try again.\n";
    }
}

std::unique_ptr<agents::Agent> create_agent(const propnet::Role &role)
{
    while (true)
    {
        std::string name{};
        try
        {
            std::cout << "Enter agent name for role '" << role.get_name() << "': ";
            std::cin >> name;

            return agent_factory(name, role);
        }
        catch (const std::invalid_argument &error)
        {
            std::cout << "Error occurred whilst trying to parse " << name << ": " << error.what() << '\n';
            std::cout << "Please try again.\n";
        }
    }
}

std::vector<std::unique_ptr<agents::Agent>> create_agents(const propnet::Propnet &propnet)
{
    std::vector<std::unique_ptr<agents::Agent>> agents{};
    for (const auto &player_role : propnet.get_player_roles())
    {
        agents.push_back(create_agent(player_role));
    }

    if (propnet.is_randomness())
    {
        const auto &random_role{propnet.get_random_role()};
        agents.push_back(std::make_unique<agents::RandomAgent>(*random_role));
    }

    return agents;
}

std::unique_ptr<agents::Agent> agent_factory(std::string_view name, const propnet::Role &role)
{
    if (name == agents::RandomAgent::NAME)
    {
        return std::make_unique<agents::RandomAgent>(role);
    }
    else if (name == agents::FirstAgent::NAME)
    {
        return std::make_unique<agents::FirstAgent>(role);
    }
    else if (name == agents::LastAgent::NAME)
    {
        return std::make_unique<agents::LastAgent>(role);
    }
    else if (name == agents::HumanAgent::NAME)
    {
        return std::make_unique<agents::HumanAgent>(role);
    }
    else
    {
        throw std::invalid_argument{"Given unkown agent name"};
    }
}

player::Model load_model(const propnet::Propnet &propnet, std::string_view game)
{
    try
    {
        std::cout << "Attempting to load most recent model\n";
        return player::Model::load_most_recent(propnet, game);
    }
    catch (std::runtime_error &error)
    {
        std::cout << error.what() << '\n';
        std::cout << "Creating new model\n";

        return player::Model{propnet, game};
    }
}
} // namespace setup
