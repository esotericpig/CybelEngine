/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2026 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#include "metric_man.h"

#include <cassert>
#include <format>
#include <iostream>

namespace cybel {

MetricMan& MetricMan::it() {
  static MetricMan it_{};
  return it_;
}

MetricMan::MetricMan() {
  print_timer_.restart();
}

std::size_t MetricMan::register_profiler(std::string_view name) {
  // Slow, but fine for now since only do once in static.
  for(std::size_t i = 0; i < profilers_.size(); ++i) {
    if(profilers_[i].name == name) { return i; }
  }

  if(name.length() > max_name_len_) { max_name_len_ = name.length(); }

  profilers_.emplace_back(std::string{name});
  return profilers_.size() - 1;
}

std::size_t MetricMan::register_counter(std::string_view name) {
  // Slow, but fine for now since only do once in static.
  for(std::size_t i = 0; i < counters_.size(); ++i) {
    if(counters_[i].name == name) { return i; }
  }

  if(name.length() > max_name_len_) { max_name_len_ = name.length(); }

  counters_.emplace_back(std::string{name});
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

  ++profiler.count;
  profiler.total += dur;

  if(profiler.em_avg <= Duration::kZero) {
    profiler.em_avg = dur;
  } else {
    profiler.em_avg = (profiler.em_avg * (1.0f - kSmoothingFactor)) + (dur * kSmoothingFactor);
  }
}

void MetricMan::inc_count(std::size_t id,std::uint32_t count) {
  assert(id < counters_.size());

  counters_[id].count += count;
  counters_[id].total += count;
}

void MetricMan::end_frame() {
  if(profilers_.empty() && counters_.empty()) { return; }

  for(auto& counter : counters_) {
    ++counter.frame_count;
  }

  if(print_timer_.peek() >= kPrintInterval) {
    print_timer_.restart();

    std::cout << "[METRICS]\n";
    std::cout << std::format(
      "  {:<7} , {:<{}} , {:<11} , {:<11} , {:<11}\n",
      "[Type]","[Name]",max_name_len_,"[Last]","[Avg]","[EMA]"
    );

    for(const auto& profiler : profilers_) {
      std::cout << std::format(
        "  {:<7} , {:<{}} , {:>8.4f} ms , {:>8.4f} ms , {:>8.4f} ms\n",
        "[PROF]",
        profiler.name,max_name_len_,
        profiler.timer.duration().millis(),
        (profiler.total / profiler.count).millis(),
        profiler.em_avg.millis()
      );
    }
    for(const auto& counter : counters_) {
      std::cout << std::format(
        "  {:<7} , {:<{}} , {:>8}    , {:>8.2f}    ,\n",
        "[COUNT]",
        counter.name,max_name_len_,
        counter.count,
        static_cast<double>(counter.total) / counter.frame_count
      );
    }

    std::cout << "[/METRICS]\n";
    std::cout.flush();
  }

  // Reset for next frame.
  for(auto& counter : counters_) {
    counter.count = 0;
  }
}

MetricMan::ScopeProfiler::ScopeProfiler(MetricMan& metric_man,std::size_t id)
  : metric_man_{&metric_man},id_{id} {
  metric_man_->begin_profile(id_);
}

MetricMan::ScopeProfiler::~ScopeProfiler() noexcept {
  metric_man_->end_profile(id_);
}

} // namespace cybel
