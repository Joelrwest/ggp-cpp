from propnet.propnet import load_propnet # type: ignore
from propnet.node import Proposition # type: ignore

from typing import Generator

RANDOM_ROLE = "random"

class PropnetWrapper:
    def __init__(self, game: str) -> None:
        self.initial_data, self.propnet = load_propnet(game)
        self.reset_data()

    def get_inputs_for(self, role: str) -> list[Proposition]:
        return self.propnet.actions_for[role]

    def get_legals_for(self, role: str) -> Generator[Proposition, None, None]:
        return self.propnet.get_legals_for(role, self.data)

    def get_sees_for(self, role: str) -> list[Proposition]:
        return self.propnet.sees_for[role]

    def get_roles(self) -> list[str]:
        return [
            role
            for role in self.propnet.roles
            if role != RANDOM_ROLE
        ]

    def num_roles(self) -> int:
        return len(self.get_roles())

    def is_terminal(self) -> bool:
        return self.propnet.is_terminal(self.data)

    def get_scores(self) -> dict[str, int]:
        return {
            role: score
            for role, score in self.propnet.scores(self.data).items()
            if role != RANDOM_ROLE
        }

    def is_randomness(self) -> bool:
        return RANDOM_ROLE in self.propnet.roles

    def reset_data(self) -> None:
        # Even though data isn't a list,
        # (it's actually a PersistentArray) treat it like one
        self.data: list[int] = self.initial_data.copy()

    def take_inputs(self, inputs: list[int]) -> None:
        self.propnet.do_step(self.data, inputs)
