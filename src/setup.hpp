#pragma once

#include "../agents/include/non_seeing.hpp"
#include "../player/include/model.hpp"
#include "../propnet/include/propnet.hpp"
#include "../propnet/include/role.hpp"

#include <boost/program_options.hpp>

namespace po = boost::program_options;

namespace setup
{
class ProgramOptionsError : public std::invalid_argument
{
  public:
    ProgramOptionsError(const char *message);
};

po::options_description create_base_program_options(std::string &game);
po::variables_map parse_program_options(po::options_description &options_description, int argc, char **argv);
propnet::Propnet load_propnet(std::string_view game);
std::unique_ptr<agents::Agent> create_agent(const propnet::Propnet &propnet);
std::unique_ptr<agents::Agent> create_agent(const propnet::Role &role);
std::vector<std::unique_ptr<agents::Agent>> create_agents(const propnet::Propnet &propnet);
std::unique_ptr<agents::Agent> agent_factory(std::string_view name, const propnet::Role &role);
player::Model load_model(const propnet::Propnet &propnet, std::string_view game);
} // namespace setup
