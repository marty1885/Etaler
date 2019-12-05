#pragma once

#include "SmallVector.hpp"
#include "Shape.hpp"
#include "TensorImpl.hpp"

#include <variant>
#include <memory>
#include <optional>

namespace et
{

struct Range
{
	Range() = default;
	Range(intmax_t start)
		: start_(start), stop_(start+1)
	{}

	Range(intmax_t start, intmax_t stop)
		: start_(start), stop_(stop)
	{}

	Range(intmax_t start, intmax_t stop, intmax_t step)
		: start_(start), stop_(stop), step_(step)
	{}

	std::optional<intmax_t> start() const {return start_;}
	std::optional<intmax_t> stop() const {return stop_;}
	std::optional<intmax_t> step() const {return step_;}

protected:
	std::optional<intmax_t> start_;
	std::optional<intmax_t> stop_;
	std::optional<intmax_t> step_;
};

inline Range all()
{
	return Range();
}

inline Range range(intmax_t start, intmax_t end)
{
	return Range(start, end);
}

inline Range range(intmax_t end)
{
	return Range(0, end);
}

inline Range range(intmax_t start, intmax_t stop, intmax_t step)
{
	return Range(start, stop, step);
}


}
