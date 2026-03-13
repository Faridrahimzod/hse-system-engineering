import operator
import re


class Argument:
    def __init__(self, viewtype, begin, end=None):
        self.type = viewtype
        self.begin = int(begin)
        self.end = None if end is None else int(end)


class Lram(bytearray):
    pass


class Pram(bytearray):
    def __init__(self):
        super().__init__()
        self.instruction = re.compile(
            r"\[(?P<out>[^\]]+)\](?P<op>\w+)\((?P<in>[^\)]+)\)"
        )
        self.operations = {
            "add": operator.add,
            "sub": operator.sub,
            "mul": operator.mul,
            "mod": operator.mod,
            "div": operator.truediv,
        }
        self.types = {
            "i8": "b",
            "u8": "B",
            "i16": "h",
            "u16": "H",
            "i32": "i",
            "u32": "I",
            "f32": "f",
        }

    def _parse_arguments(self, expression, lram):
        arguments = [Argument(*chunk.split(":")) for chunk in expression.split(",")]
        parsed = []

        for arg in arguments:
            end = arg.end if arg.end is not None else len(lram)
            view = memoryview(lram)[arg.begin:end].cast(self.types[arg.type])
            parsed.append(view)

        return parsed

    def _vectorize(self, op, output, inputs):
        for i, values in enumerate(zip(*inputs)):
            output[i] = op(*values)

    def run(self, lram):
        program = bytes(self).decode("utf-8", errors="ignore").replace(" ", "")
        operations = self.instruction.findall(program)

        for out_expr, op_name, in_expr in operations:
            outputs = self._parse_arguments(out_expr, lram)
            inputs = self._parse_arguments(in_expr, lram)

            if op_name not in self.operations:
                raise ValueError(f"Unsupported operation: {op_name}")

            self._vectorize(self.operations[op_name], outputs[0], inputs)


class Unit:
    def __init__(self):
        self.lram = Lram()
        self.pram = Pram()


class Ctrl(list):
    def __init__(self, units):
        super().__init__()
        self.units = units

    def wait(self):
        if len(self) == 0:
            return []

        number = self.pop(0)
        unit = self.units[number]
        unit.pram.run(unit.lram)
        return [number]


class Device:
    def __init__(self, units):
        self.units = [Unit() for _ in range(units)]
        self.ctrl = Ctrl(self.units)