#include "../propnet/include/propnet.hpp"
#include "setup.hpp"

int main(void)
{
    const propnet::Propnet propnet{setup::load_propnet("blindtictactoe")};
    auto model{player::Model::load_game_number(propnet, "blindtictactoe", 2000)};

    auto state{propnet.create_initial_state()};
    std::cout << model.eval_evs(state) << '\n';

    propnet::InputSet inputs_1{324, 62, 525};
    propnet.take_sees_inputs(state, inputs_1);
    propnet.take_non_sees_inputs(state, inputs_1);
    std::cout << model.eval_evs(state) << '\n';

    propnet::InputSet inputs_2{73, 192, 518};
    propnet.take_sees_inputs(state, inputs_2);
    propnet.take_non_sees_inputs(state, inputs_2);
    std::cout << model.eval_evs(state) << '\n';

    propnet::InputSet inputs_3{153, 57, 518};
    propnet.take_sees_inputs(state, inputs_3);
    propnet.take_non_sees_inputs(state, inputs_3);
    std::cout << model.eval_evs(state) << '\n';
}
