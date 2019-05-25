#pragma once

#include "Etaler/Core/Shape.hpp"
#include "Etaler/Core/Backend.hpp"
#include "Etaler/Core/Error.hpp"
#include "Etaler/Core/Tensor.hpp"
#include "Etaler/Core/Serialize.hpp"
#include "Etaler/Core/DefaultBackend.hpp"

namespace et
{

struct TemporalMemory
{
	TemporalMemory() = default;
	TemporalMemory(const Shape& input_shape, size_t cells_per_column, size_t max_synapses_per_cell=64, Backend* backend=defaultBackend());
	std::pair<Tensor, Tensor> compute(const Tensor& x, const Tensor& last_state);
	void learn(const Tensor& active_cells, const Tensor& last_active);

	void setPermanenceInc(float inc) { permanence_inc_ = inc; }
	float permanenceInc() const {return permanence_inc_;}

	void setPermanenceDec(float dec) { permanence_dec_ = dec; }
	float permanenceDec() const {return permanence_dec_;}

	void setConnectedPermanence(float thr) { connected_permanence_ = thr; }
	float connectedPermanence() const { return connected_permanence_; }

	void setActiveThreshold(size_t thr) { active_threshold_ = thr; }
	size_t activeThreshold() const { return active_threshold_; }

	size_t cellsPerColumn() const {return connections_.shape().back();}

	StateDict states() const
	{
		return {{"input_shape", input_shape_}, {"connections", connections_} , {"permanences", permanences_}
			, {"permanence_inc", permanence_inc_}, {"permanence_dec", permanence_dec_}
			, {"connected_permanence", connected_permanence_}, {"active_threshold", (int)active_threshold_}};
	}

	TemporalMemory to(Backend* b) const;

	void loadState(const StateDict& states);

	Shape input_shape_;
	float connected_permanence_ = 0.1;
	size_t active_threshold_ = 2;
	float permanence_inc_ = 0.1;
	float permanence_dec_ = 0.1;
	Tensor connections_;
	Tensor permanences_;
};

}
