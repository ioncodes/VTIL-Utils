#include "vtil-utils.hpp"

using namespace vtil;
using namespace logger;

static args::Command dump(commands(), "dot", "Generate a .dot graph file", [](args::Subparser& parser) {
	// Argument handling
	args::Positional<std::string> input(parser, "input", "Input .vtil file", args::Options::Required);
	args::Positional<std::string> output(parser, "output", "Output .dot file", args::Options::Required);
	parser.Parse();

	// Command implementation
	auto rtn = load_routine(input.Get());

	std::stringstream dot;
	dot << "digraph X {" << std::endl;

	for (auto& [vip, block] : *rtn)
	{
		dot << "vip_" << std::to_string(vip) << " [shape=record label=\"";
		dot << "vip_" << std::to_string(vip) << "\\n";
		for (auto& instr : *block)
			dot << instr.to_string() << "\\n";

		dot << "\"]" << std::endl;
	}

	for (auto& [vip, block] : *rtn)
	{
		dot << "vip_" << std::to_string(vip) << ":s -> {";

		if (block->size() <= 0)
			continue;

		auto& instr = *(--block->end());

		if (instr.base->is_branching())
		{
			printf("%d: ", block->entry_vip);
			debug::dump(instr);

			for (auto it = instr.operands.begin(); it != instr.operands.end(); ++it)
			{
				auto& op = *it;

				if (!op.is_immediate())
					continue;

				dot << "vip_" << std::to_string(op.imm().u64);

				if (std::next(it) != instr.operands.end())
					dot << ", ";
			}

			dot << "}" << std::endl;
		}
	}

	dot << "}";

	std::cout << dot.str() << std::endl;

	//debug::dump(rtn);
});