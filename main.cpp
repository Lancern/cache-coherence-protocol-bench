#include <atomic>
#include <chrono>
#include <cstddef>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

namespace {

constexpr const int Iterations = 500'000'000;

class Benchmark {
public:
  virtual ~Benchmark() noexcept = default;

  void Run(size_t maxNumThreads) noexcept {
    for (size_t threads = 1; threads <= maxNumThreads; ++threads) {
      RunWithThreads(threads);
    }
  }

  void RunWithThreads(size_t numThreads) noexcept {
    std::cout << GetName() << ": numThreads = " << numThreads << " ... ";
    std::cout.flush();

    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    auto startTime = std::chrono::high_resolution_clock::now();

    for (size_t i = 0; i < numThreads; ++i) {
      threads.emplace_back([this]() { RunThread(); });
    }

    for (auto& t : threads) {
      t.join();
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto dur = endTime - startTime;

    std::cout << dur / std::chrono::milliseconds(1) << " ms" << std::endl;
  }

protected:
  [[nodiscard]]
  virtual std::string GetName() const noexcept = 0;

  virtual void RunThread() noexcept = 0;
};

class NonAtomicBaseline : public Benchmark {
protected:
  [[nodiscard]]
  std::string GetName() const noexcept override {
    return "Non-atomic Baseline";
  }

  void RunThread() noexcept override {
    volatile int64_t counter = 0;
    for (auto i = 0; i < Iterations; ++i) {
      ++counter;
    }
  }
};

class NonAtomicBenchmark : public Benchmark {
public:
  explicit NonAtomicBenchmark() noexcept
    : _counter(0)
  { }

protected:
  [[nodiscard]]
  std::string GetName() const noexcept override {
    return "Non-atomic Benchmark";
  }

  void RunThread() noexcept override {
    for (auto i = 0; i < Iterations; ++i) {
      ++_counter;
    }
  }

private:
  volatile int64_t _counter;
};

[[nodiscard]]
const char* GetMemoryOrderRepr(std::memory_order order) noexcept {
  switch (order) {
    case std::memory_order_relaxed:
      return "relaxed";
    case std::memory_order_consume:
      return "consume";
    case std::memory_order_acquire:
      return "acquire";
    case std::memory_order_release:
      return "release";
    case std::memory_order_acq_rel:
      return "acq_rel";
    case std::memory_order_seq_cst:
      return "seq_cst";
    default:
      __builtin_unreachable();
  }
}

template <std::memory_order Order>
class AtomicBaseline : public Benchmark {
protected:
  [[nodiscard]]
  std::string GetName() const noexcept override {
    std::string name = "Atomic Baseline (";
    name.append(GetMemoryOrderRepr(Order));
    name.append(")");
    return name;
  }

  void RunThread() noexcept override {
    std::atomic_int64_t counter = 0;
    for (auto i = 0; i < Iterations; ++i) {
      counter.fetch_add(1, Order);
    }
  }
};

template <std::memory_order Order>
class AtomicBenchmark : public Benchmark {
public:
  explicit AtomicBenchmark() noexcept
    : _counter(0)
  { }

protected:
  [[nodiscard]]
  std::string GetName() const noexcept override {
    std::string name = "Atomic Benchmark (";
    name.append(GetMemoryOrderRepr(Order));
    name.append(")");
    return name;
  }

  void RunThread() noexcept override {
    for (auto i = 0; i < Iterations; ++i) {
      _counter.fetch_add(1, Order);
    }
  }

private:
  std::atomic_int64_t _counter;
};

} // namespace <anonymous>

int main() {
  std::vector<std::unique_ptr<Benchmark>> benchmarks;

#define ADD_BENCHMARK(t) \
    benchmarks.push_back(std::make_unique<t>())

  ADD_BENCHMARK(NonAtomicBaseline);
  ADD_BENCHMARK(NonAtomicBenchmark);

#define MEMORY_ORDER_LIST(h) \
  h(std::memory_order_relaxed) \
  h(std::memory_order_consume) \
  h(std::memory_order_acquire) \
  h(std::memory_order_release) \
  h(std::memory_order_acq_rel) \
  h(std::memory_order_seq_cst)

#define ADD_ATOMIC_BASELINE(order) \
  ADD_BENCHMARK(AtomicBaseline<order>);

#define ADD_ATOMIC_BENCHMARK(order) \
  ADD_BENCHMARK(AtomicBenchmark<order>);

  MEMORY_ORDER_LIST(ADD_ATOMIC_BASELINE)
  MEMORY_ORDER_LIST(ADD_ATOMIC_BENCHMARK)

#undef ADD_ATOMIC_BENCHMARK
#undef ADD_ATOMIC_BASELINE
#undef ADD_BENCHMARK

  for (const auto& b : benchmarks) {
    b->Run(10);
  }

  return 0;
}
