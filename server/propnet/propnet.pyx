# cython: profile=True
from propnet.node import node_types, transition_split, Node
from constants import (AND, OR, PROPOSITION, TRANSITION, NOT, CONSTANT,
                        UNKNOWN, init, base, input, legal, goal, sees, terminal,
                        other)
import os
import importlib
from collections import defaultdict
from propnet.persistent_array import PersistentArray


def _split_gdl(gdl):
    gdl = gdl[1:-1]
    start = 0
    c = 0
    for i, e in enumerate(gdl):
        if e == '(':
            # if c == 0:
            c += 1
        if e == ')':
            c -= 1
        if c == 0 and e == ' ':
            yield gdl[start:i]
            start = i+1
    assert(c == 0)
    yield gdl[start:]


def split_gdl(gdl):
    return filter(None, _split_gdl(gdl))


def make_propnet(gdl, name):
    fn = os.path.join('rulesheets', name + '.kif')
    with open(fn, 'w') as f:
        f.write(gdl)
    return convert_to_propnet(fn)


def load_propnet(base):
    current_file_path = os.path.dirname(__file__)
    module_path = os.path.abspath(os.path.join(current_file_path, '..', '..', 'games', 'python', f"{base}.py"))

    spec = importlib.util.spec_from_file_location(base, module_path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)

    return Propnet.Create(module.roles, module.entries)


cdef class Propnet:
    cdef public list roles
    cdef public list nodes
    cdef public list transitions
    cdef public list propositions
    cdef public list legal
    cdef public list base
    cdef public list input
    cdef public list init
    cdef public list goal
    cdef public list sees
    cdef public terminal

    cdef public dict actions_for
    cdef public dict sees_for
    cdef public dict id_to_move
    cdef public dict actions
    cdef public set posts
    cdef public list legal_to_input

    cdef public list topsorted

    @classmethod
    def Create(cls, roles, entries):
        max_id = max(e[0] for e in entries)
        trans = sum(e[2] == TRANSITION for e in entries)
        data = PersistentArray(maxlen=max_id+trans+1)
        propnet = Propnet(roles, entries, data)
        return data, propnet

    def __init__(self, roles, entries, data):
        self.roles = roles

        max_id = max(e[0] for e in entries)
        trans = sum(e[2] == TRANSITION for e in entries)
        self.nodes = [None] * (max_id+trans+1)
        pres = []
        posts = []
        for e in entries:
            if e[2] == TRANSITION:
                max_id += 1
                pre, post = transition_split(e, max_id)
                pres.append(pre)
                posts.append(post)
                self.nodes[e[0]] = pre
                self.nodes[max_id] = post
            else:
                self.nodes[e[0]] = node_types[e[2]](*e)
            data[e[0]] = False

        for pre in pres:
            for o in pre._outputs:
                self.nodes[o].update_inputs(pre.id, pre.post_id)

        self.transitions  = [node for node in self.nodes if node.node_type == TRANSITION]
        self.propositions = [node for node in self.nodes if node.node_type == PROPOSITION]

        self.legal    = [node for node in self.propositions if node.prop_type == legal]
        self.base     = [node for node in self.propositions if node.prop_type == base]
        self.input    = [node for node in self.propositions if node.prop_type == input]
        self.init     = [node for node in self.propositions if node.prop_type == init]
        self.goal     = [node for node in self.propositions if node.prop_type == goal]
        self.sees     = [node for node in self.propositions if node.prop_type == sees]
        self.terminal = [node for node in self.propositions if node.prop_type == terminal]

        for x in [self.legal, self.propositions, self.base, self.input, self.init, self.goal, self.sees, self.terminal]:
            x.sort(key=lambda n: n.gdl)

        self.actions_for = {}
        for leg in self.legal:
            if leg.move_role not in self.actions_for:
                self.actions_for[leg.move_role] = []
            self.actions_for[leg.move_role].append(leg)

        self.sees_for = {}
        for see in self.sees:
            if see.move_role not in self.sees_for:
                self.sees_for[see.move_role] = []
            self.sees_for[see.move_role].append(see)

        self.id_to_move = {}
        for move in self.legal:
            self.id_to_move[move.id] = move

        self.actions = {inp.normalised_gdl: inp.id for inp in self.input}

        self.legal_to_input = [-1] * (max_id + 1)
        for i in self.input:
            self.legal_to_input[i.id] = i.id
        for l in self.legal:
            inp = self.actions[f'(does {l.move_role} {l.move_gdl})'.replace(' ', '')]
            l.input_id = inp
            self.legal_to_input[l.id] = inp

        assert(len(self.terminal) == 1)
        self.terminal = self.terminal[0]

        self.posts = {p.id for p in posts}
        self.make_topsorted()

        self.do_step(data, init=True)

    def make_topsorted(self):
        self.topsorted = list(self.posts)
        seen = set(self.posts)
        stack = [(node.id, False) for node in self.nodes if node.id not in self.posts]
        while stack:
            # import pdb; pdb.set_trace()
            cur, done = stack.pop()
            if not done:
                if cur in seen:
                    continue
                seen.add(cur)
                stack.append((cur, True))
                for child in self.nodes[cur].inputs:
                    if child not in seen:
                        stack.append((child, False))
            else:
                self.topsorted.append(cur)

    # def do_step_parse(self, data, action_strs, init=False):
    #     if isinstance(action_strs, str):
    #         action_strs = split_gdl(action_strs)
    #     action_set = {f'(does{role}{action})'.replace(' ', '')
    #                   for role, action in zip(self.roles, action_strs)}
    #     actions = [self.actions[a] for a in action_set]
    #     self.do_step(data, actions, init)

    cpdef do_step(self, data, actions=set(), init=False):
        actions = {self.legal_to_input[x] for x in actions}

        if isinstance(data, PersistentArray):
            datacopy = list(data.values())
            copy2 = list(datacopy)
        else:
            datacopy = data

        for id in self.topsorted:
            if id not in self.posts:
                datacopy[id] = self.nodes[id].eval(datacopy, init, actions)

        for id in self.topsorted:
            datacopy[id] = self.nodes[id].eval(datacopy, init, actions)

        if isinstance(data, PersistentArray):
            for i, (a, b) in enumerate(zip(datacopy, copy2)):
                if a != b:
                    data[i] = a
        else:
            data = datacopy

    cpdef do_sees_step(self, data, actions=set(), init=False):
        actions = {self.legal_to_input[x] for x in actions}

        if isinstance(data, PersistentArray):
            datacopy = list(data.values())
            copy2 = list(datacopy)
        else:
            datacopy = data

        for id in self.topsorted:
            if id not in self.posts:
                datacopy[id] = self.nodes[id].eval(datacopy, init, actions)

        if isinstance(data, PersistentArray):
            for i, (a, b) in enumerate(zip(datacopy, copy2)):
                if a != b:
                    data[i] = a
        else:
            data = datacopy

    cpdef do_non_sees_step(self, data, actions=set(), init=False):
        actions = {self.legal_to_input[x] for x in actions}

        if isinstance(data, PersistentArray):
            datacopy = list(data.values())
            copy2 = list(datacopy)
        else:
            datacopy = data

        for id in self.topsorted:
            datacopy[id] = self.nodes[id].eval(datacopy, init, actions)

        if isinstance(data, PersistentArray):
            for i, (a, b) in enumerate(zip(datacopy, copy2)):
                if a != b:
                    data[i] = a
        else:
            data = datacopy

    def visible(self, data):
        return (
            see
            for see in self.sees
            if data[see.id]
        )

    def visible_dict(self, data):
        sees = defaultdict(list)
        for move in self.visible(data):
            sees[move.move_role].append(move)
        return sees


    # def visible(self, data, sees):
    #     return (see for see in self.sees if data[see.id].eval())

    # def visible_ids(self, data):
    #     return tuple(see.id for see in self.sees if data[see.id])

    # def sees_moves_for(self, role, data):
    #     for move in self.visible(data):
    #         if move.move_role == role:
    #             yield move

    def sees_ids_for(self, role, data):
        return tuple(move.id for move in self.visible(data) if move.move_role == role)
        # for move in self.sees_moves(data):
        #     if move.move_role == role:
        #         yield move

    def legal_moves(self, data):
        return (legal for legal in self.legal if data[legal.id])

    def get_legals_for(self, role, data):
        return (
            action
            for action in self.actions_for[role]
            if data[action.id]
        )

    def legal_moves_dict(self, data):
        actions = defaultdict(list)
        for move in self.legal_moves(data):
            actions[move.move_role].append(move)
        return actions

    def is_terminal(self, data):
        return data[self.terminal.id]

    def data2num(self, data):
        return int(
            "".join(
                str(int(x))
                for x in data
            ), 2
        )

    def scores(self, data):
        scores = {}
        if self.is_terminal(data):
            for g in self.goal:
                if data[g.id]:
                    scores[g.role] = g.score
        return scores
