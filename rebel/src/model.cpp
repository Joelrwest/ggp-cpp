#include <model.hpp>

#include <torch/torch.h>

namespace rebel
{
    /*
    def __init__(self, num_inputs, num_actions, roles, head_depth=3):
        super().__init__()
        self.dropout = nn.Dropout(p=0.2)
        self.activ = nn.ELU()
        self.sigmoid = nn.Sigmoid()
        self.softmax = nn.Softmax(-1)
        self.head = nn.Sequential(*[
            nn.Sequential(nn.Linear(num_inputs, num_inputs), self.activ, self.dropout)
        for _ in range(head_depth)])
        self.val = nn.Linear(num_inputs, len(roles))
        self.policy_head = nn.Sequential(nn.Linear(num_inputs, num_inputs), self.activ, self.dropout)
        self.policy = nn.Sequential(*[nn.Linear(num_inputs, num_actions[role]) for role in roles])
        self.max_actions = max(num_actions[role] for role in roles)

    def forward(self, x):
        x = self.head(x)

        val = self.sigmoid(self.val(x))
        x = self.policy_head(x)
        if len(x.shape) > 1:
            policies = torch.zeros((x.shape[0], val.shape[-1], self.max_actions))
            for i, policy_layer in enumerate(self.policy):
                out = self.softmax(policy_layer(x))
                policies[:,i, :out.shape[-1]] = out
        else:
            policies = [self.softmax(policy_layer(x)) for policy_layer in self.policy]

        return policies, val
    */
    struct Network : torch::nn::Module
    {
        Network() {}

        torch::Tensor forward(torch::Tensor x)
        {
            // // Use one of many tensor manipulation functions.
            // x = torch::relu(fc1->forward(x.reshape({x.size(0), 784})));
            // x = torch::dropout(x, /*p=*/0.5, /*train=*/is_training());
            // x = torch::relu(fc2->forward(x));
            // x = torch::log_softmax(fc3->forward(x), /*dim=*/1);
            return x;
        }
    };
}
