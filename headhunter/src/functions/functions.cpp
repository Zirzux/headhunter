#include "functions/functions.h"
#include "addresses/addresses.hpp"
#include "console/console.h"
#include "api/api.h"
#include "execution/execution.h"

#include <intrin.h>

namespace custom_funcs
{
	int setreadonly(std::uintptr_t rl)
	{
		std::uintptr_t table_val = *reinterpret_cast<std::uintptr_t*>(*reinterpret_cast<std::uintptr_t*>(rl + offsets::luastate::top) - 32);
		*reinterpret_cast<bool*>(table_val + 7) = *reinterpret_cast<bool*>(*reinterpret_cast<std::uintptr_t*>(rl + offsets::luastate::top) - 16);
		return 0;
	}

	int getrawmetatable(std::uintptr_t rl)
	{
		std::uintptr_t& base = *reinterpret_cast<std::uintptr_t*>(rl + offsets::luastate::base);
		std::uintptr_t& top = *reinterpret_cast<std::uintptr_t*>(rl + offsets::luastate::top);

		std::uintptr_t enc_mt = 0;

		enc_mt = *reinterpret_cast<std::uintptr_t*>(base) + (*reinterpret_cast<std::uint32_t*>(base + 12) == 6 ? 0xC : 0x18);

		if (std::uintptr_t mt = *reinterpret_cast<std::uintptr_t*>(enc_mt) - enc_mt)
		{
			*reinterpret_cast<std::uintptr_t*>(top) = mt;
			*reinterpret_cast<std::uintptr_t*>(top + 12) = 7;
			top += 16;
			return 1;
		}
		return 0;
	}

	int setfpscap(std::uintptr_t rl)
	{
		__m128d a = _mm_load_sd(*reinterpret_cast<double**>(rl + offsets::luastate::base));
		__m128d b = _mm_load_pd(reinterpret_cast<double*>(addresses::xor_const));
		__m128d res = _mm_xor_pd(a, b);
		double done = _mm_cvtsd_f64(res);

		execution.set_fps(done);
		return 0;
	}

	int setidentity(std::uintptr_t rl) // DIY
	{
		output << console::color::red << "this ones for you to make!\n";
		return 0;
	}

	int loadstring(std::uintptr_t rl)
	{
		const char* script = **reinterpret_cast<const char***>(rl + offsets::luastate::base) + 0x14;

		roblox_encoder_t roblox_encoder{};
		const std::string compiled = Luau::compile(script, {}, {}, &roblox_encoder);
		rbx_deserialize(rl, "headhunter.exe", compiled.c_str(), compiled.size());
		return 1;
	}

	int getfuncaddy(std::uintptr_t rl) // if you know how to use this, everything else in this exploit will be hella easy to make
	{
		std::uintptr_t func = rbx_decryptfunc(**reinterpret_cast<std::uintptr_t**>(rl + offsets::luastate::base));
		printf_s("0x%08X\n", func);
		return 0;
	}

	int getnamecallmethod(std::uintptr_t rl)
	{
		if (std::uintptr_t method = *reinterpret_cast<std::uintptr_t*>(rl + 0x64))
		{
			rbx_pushvfstring(rl, "%s", reinterpret_cast<const char*>(method + 0x14));
			return 1;
		}
		return 0;
	}
}