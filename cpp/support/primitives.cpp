// Offline comparison executable. No JVM, networking, mining or wallet access.
#include "org/minima/objects/base/MiniByte.hpp"
#include "org/minima/objects/base/MiniData.hpp"
#include "org/minima/objects/base/MiniNumber.hpp"
#include "org/minima/objects/base/MiniString.hpp"
#include <iostream>
#include <sstream>
using namespace minima;
static std::string wire(const std::vector<std::uint8_t>& bytes) { return MiniData(bytes).to0xString(); }
int main() {
    std::string line;
    while (std::getline(std::cin, line)) {
        try {
            std::istringstream input(line);
            std::string op, a, b;
            input >> op >> a >> b;
            if (op == "byte") {
                MiniByte n(std::stoi(a));
                std::cout << n.toString() << '|' << int(n.getByteValue()) << '|' << wire(n.serialise()) << '\n';
            } else if (op == "data") {
                auto n = MiniData::fromHex(a);
                std::cout << n.to0xString() << '|' << wire(n.serialise()) << '\n';
            } else if (op == "less") {
                std::cout << (MiniData::fromHex(a).isLess(MiniData::fromHex(b)) ? "true" : "false") << '\n';
            } else if (op == "string") {
                auto bytes = MiniData::fromHex(a).bytes();
                MiniString n(std::string(bytes.begin(), bytes.end()));
                std::cout << wire(n.serialise()) << '\n';
            } else {
                MiniNumber n(a);
                if(op=="long" || op=="int") {std::cout<<(op=="long"?n.getAsLong():n.getAsInt())<<'\n';continue;}
                if (op == "add") n = n.add(MiniNumber(b));
                else if (op == "sub") n = n.sub(MiniNumber(b));
                else if (op == "mul") n = n.mult(MiniNumber(b));
                else if (op == "div") n = n.div(MiniNumber(b));
                else if (op == "pow") n = n.pow(std::stoi(b));
                else if (op == "sqrt") n = n.sqrt();
                else if (op == "mod") n = n.modulo(MiniNumber(b));
                else if (op == "floor") n = n.floor();
                else if (op == "ceil") n = n.ceil();
                else if (op == "sigdig") n = n.setSignificantDigits(std::stoi(b));
                else if (op != "number") throw std::invalid_argument("Unknown operation");
                std::cout << n.toString() << '|' << wire(n.serialise()) << '\n';
            }
        } catch (const std::exception&) { std::cout << "ERROR\n"; }
    }
}
