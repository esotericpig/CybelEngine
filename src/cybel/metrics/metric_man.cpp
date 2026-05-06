/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2026 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#include "metric_man.h"

#include <algorithm>
#include <cassert>
#include <format>
#include <iostream>

namespace cybel {

double MetricMan::calc_exp_mov_avg(double em_avg,double new_value) {
  if(em_avg <= 0.0) [[unlikely]] { return new_value; }

  return (em_avg * (1.0 - kEmAvgSmoothingFactor)) + (new_value * kEmAvgSmoothingFactor);
}

float MetricMan::calc_exp_mov_avg(float em_avg,float new_value) {
  if(em_avg <= 0.0f) [[unlikely]] { return new_value; }

  return (em_avg * (1.0f - kEmAvgSmoothingFactor)) + (new_value * kEmAvgSmoothingFactor);
}

Duration MetricMan::calc_exp_mov_avg(const Duration& em_avg,const Duration& new_value) {
  if(em_avg <= Duration::kZero) [[unlikely]] { return new_value; }

  return (em_avg * (1.0 - kEmAvgSmoothingFactor)) + (new_value * kEmAvgSmoothingFactor);
}

MetricMan& MetricMan::it() {
  static MetricMan it_{};
  return it_;
}

std::size_t MetricMan::register_profiler(std::string_view name) {
  // Slow, but fine for now since only do once in static.
  for(std::size_t i = 0; i < profilers_.size(); ++i) {
    if(profilers_[i].name == name) { return i; }
  }

  if(name.length() > max_name_len_) { max_name_len_ = name.length(); }

  profilers_.push_back({.name = std::string{name}});
  return profilers_.size() - 1;
}

std::size_t MetricMan::register_counter(std::string_view name) {
  // Slow, but fine for now since only do once in static.
  for(std::size_t i = 0; i < counters_.size(); ++i) {
    if(counters_[i].name == name) { return i; }
  }

  if(name.length() > max_name_len_) { max_name_len_ = name.length(); }

  counters_.push_back({.name = std::string{name}});
  return counters_.size() - 1;
}

MetricMan::ScopeProfiler MetricMan::profile_scope(std::size_t id) {
  return ScopeProfiler{*this,id};
}

void MetricMan::begin_profile(std::size_t id) {
  assert(id < profilers_.size());

  profilers_[id].timer.restart();
}

void MetricMan::end_profile(std::size_t id) {
  assert(id < profilers_.size());

  auto& profiler = profilers_[id];
  const auto& dur = profiler.timer.pause();

  profiler.total += dur;
  ++profiler.count;
  profiler.em_avg = calc_exp_mov_avg(profiler.em_avg,dur);

  profiler.life_total += dur;
  ++profiler.life_count;
  profiler.life_em_avg = calc_exp_mov_avg(profiler.life_em_avg,dur);
}

void MetricMan::inc_count(std::size_t id,std::uint32_t count) {
  assert(id < counters_.size());

  auto& counter = counters_[id];
  counter.count += count;

  counter.total += count;
  counter.life_total += count;
}

void MetricMan::end_frame() {
  if(profilers_.empty() && counters_.empty()) { return; }

  // Set frame data.
  for(auto& counter : counters_) {
    ++counter.frame_count;
    counter.em_avg = calc_exp_mov_avg(counter.em_avg,counter.count);

    ++counter.life_frame_count;
    counter.life_em_avg = calc_exp_mov_avg(counter.life_em_avg,counter.count);
  }

  if(print_and_reset_timer_.peek() >= kPrintAndResetInterval) {
    print_and_reset_timer_.restart();

    std::cout << "[METRICS]\n";
    std::cout << std::format(
      "  {:<7} , {:<{}} , {:<11} , {:<11} , {:<11} , {:<11} , {:<11}\n",
      "[Type]","[Name]",max_name_len_,"[Last]","[Avg]","[EMA]","[Life Avg]","[Life EMA]"
    );

    for(const auto& profiler : profilers_) {
      std::cout << std::format(
        "  {:<7} , {:<{}} , {:>8.4f} ms , {:>8.4f} ms , {:>8.4f} ms , {:>8.4f} ms , {:>8.4f} ms\n",
        "[PROF]",
        profiler.name,max_name_len_,
        profiler.timer.duration().millis(),
        (profiler.total / std::max(profiler.count,1u)).millis(),
        profiler.em_avg.millis(),
        (profiler.life_total / std::max(profiler.life_count,1u)).millis(),
        profiler.life_em_avg.millis()
      );
    }
    for(const auto& counter : counters_) {
      std::cout << std::format(
        "  {:<7} , {:<{}} , {:>11} , {:>11.2f} , {:>11.2f} , {:>11.2f} , {:>11.2f}\n",
        "[COUNT]",
        counter.name,max_name_len_,
        counter.count,
        static_cast<double>(counter.total) / std::max(counter.frame_count,1u),
        counter.em_avg,
        static_cast<double>(counter.life_total) / std::max(counter.life_frame_count,1u),
        counter.life_em_avg
      );
    }

    std::cout << "[/METRICS]\n";
    std::cout.flush();

    // Reset non-life (interval) data.
    for(auto& profiler : profilers_) {
      profiler.total.set_to_zero();
      profiler.count = 0;
      profiler.em_avg.set_to_zero();
    }
    for(auto& counter : counters_) {
      counter.count = 0; // Reset frame.

      counter.total = 0;
      counter.frame_count = 0;
      counter.em_avg = 0.0;
    }
  } else {
    // Reset data for next frame.
    for(auto& counter : counters_) {
      counter.count = 0;
    }
  }
}

MetricMan::ScopeProfiler::ScopeProfiler(MetricMan& metric_man,std::size_t id)
  : metric_man_{metric_man},id_{id} {
  metric_man_.get().begin_profile(id_);
}

MetricMan::ScopeProfiler::~ScopeProfiler() noexcept {
  metric_man_.get().end_profile(id_);
}

} // namespace cybel
