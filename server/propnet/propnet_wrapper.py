from propnet.propnet import load_propnet
from propnet.node import Proposition

from typing import Generator

class PropnetWrapper:
    def __init__(self, game: str) -> None:
        self.data, self.propnet = load_propnet(game)

        # Even though data isn't a list,
        # (it's actually a PersistentArray) treat it like one
        self.data: list[int] = self.data

    def get_actions_for(self, role: str) -> list[Proposition]:
        return self.propnet.actions_for[role]

    def get_legals_for(self, role: str) -> Generator[Proposition, None, None]:
        return self.propnet.get_legals_for(role, self.data)

    def get_sees_for(self, role: str) -> list[Proposition]:
        return self.propnet.sees_for[role]

    def get_roles(self) -> list[str]:
        return self.propnet.roles

    def take_actions(self, actions: Generator[Proposition, None, None]) -> None:
        actions = (
            action.input_id
            for action in actions
        )
        self.propnet.do_step(self.data, actions)
