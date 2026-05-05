/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2026 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_METRICS_METRIC_MAN_H_
#define CYBEL_METRICS_METRIC_MAN_H_

#include "cybel/common.h"

#include "cybel/types/duration.h"
#include "cybel/util/timer.h"

#include <vector>

#if defined(CYBEL_METRICS)

  #define CYBEL_CONCAT_IMP(a,b) a##b
  #define CYBEL_CONCAT(a,b) CYBEL_CONCAT_IMP(a,b)

  #define CYBEL_METRICS_PROFILE_SCOPE(name) \
    [[maybe_unused]] const auto CYBEL_CONCAT(cybel_scope_profiler_,__LINE__) = []() { \
      static const auto cybel_scope_profiler_id = cybel::MetricMan::it().register_profiler(name); \
      return cybel::MetricMan::it().profile_scope(cybel_scope_profiler_id); \
    }()

  #define CYBEL_METRICS_COUNT(name,count) \
    do { \
      static const auto cybel_counter_id = cybel::MetricMan::it().register_counter(name); \
      cybel::MetricMan::it().inc_count(cybel_counter_id,count); \
    } while(false)

  #define CYBEL_METRICS_END_FRAME() \
    do { \
      cybel::MetricMan::it().end_frame(); \
    } while(false)

#else // !CYBEL_METRICS

  #define CYBEL_METRICS_PROFILE_SCOPE(name)
  #define CYBEL_METRICS_COUNT(name,count)
  #define CYBEL_METRICS_END_FRAME()

#endif // CYBEL_METRICS

namespace cybel {

class MetricMan final {
public:
  class ScopeProfiler final {
  public:
    explicit ScopeProfiler(MetricMan& metric_man,std::size_t id);
    ScopeProfiler(const ScopeProfiler& other) = default;
    ScopeProfiler(ScopeProfiler&& other) noexcept = default;
    ~ScopeProfiler() noexcept;

    ScopeProfiler& operator=(const ScopeProfiler& other) = default;
    ScopeProfiler& operator=(ScopeProfiler&& other) noexcept = default;

  private:
    MetricMan* metric_man_{};
    std::size_t id_{};
  };

  static MetricMan& it();

  MetricMan(const MetricMan& other) = delete;
  MetricMan(MetricMan&& other) noexcept = delete;
  ~MetricMan() noexcept = default;

  MetricMan& operator=(const MetricMan& other) = delete;
  MetricMan& operator=(MetricMan&& other) noexcept = delete;

  [[nodiscard]] std::size_t register_profiler(std::string_view name);
  [[nodiscard]] std::size_t register_counter(std::string_view name);

  [[nodiscard]] ScopeProfiler profile_scope(std::size_t id);
  void inc_count(std::size_t id,std::uint32_t count);

  void end_frame();

private:
  struct Profiler final {
    std::string name{};

    Timer timer{};
    std::uint32_t count{};
    Duration total{};
    Duration em_avg{}; // Exponential Moving Average.
  };

  struct Counter final {
    std::string name{};

    std::uint32_t count{};
    std::uint32_t total{};
    // Store inside of Counter (instead of in MetricMan), in case add a Counter later after several frames.
    std::uint32_t frame_count{};
  };

  static constexpr double kSmoothingFactor = 0.3; // For Exponential Moving Average.
  static constexpr Duration kPrintInterval = Duration::from_secs(3.0);

  std::vector<Profiler> profilers_{};
  std::vector<Counter> counters_{};

  Timer print_timer_{};
  std::size_t max_name_len_ = 1;

  explicit MetricMan();

  void begin_profile(std::size_t id);
  void end_profile(std::size_t id);
};

} // namespace cybel
#endif
