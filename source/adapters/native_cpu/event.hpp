#include "common.hpp"
#include "queue.hpp"
#include "ur_api.h"
#include <cstdint>
#include <future>
#include <mutex>
#include <vector>

struct ur_event_handle_t_ : RefCounted {

  ur_event_handle_t_(ur_queue_handle_t queue, ur_command_t command_type,
                     std::vector<std::future<void>> &futures)
      : queue(queue), context(queue->getContext()), command_type(command_type),
        done(false), futures(std::move(futures)) {}

  ur_event_handle_t_(ur_queue_handle_t queue, ur_command_t command_type)
      : queue(queue), context(queue->getContext()), command_type(command_type),
        done(false) {}

  void set_callback(const std::function<void()>& cb) {
    has_callback = true;
    callback = std::move(cb);
  }

  void wait() {
    std::lock_guard<std::mutex> lock(mutex);
    if(done) {
      return;
    }
    for(auto& f : futures) {
      f.wait();
    }
    if (has_callback)
      callback();
    done = true;
  }

  uint32_t getExecutionStatus() {
    // TODO: add support for UR_EVENT_STATUS_RUNNING
    std::lock_guard<std::mutex> lock(mutex);
    if(done) {
      return UR_EVENT_STATUS_COMPLETE;
    }
    return UR_EVENT_STATUS_SUBMITTED;
  }

  ur_queue_handle_t getQueue() {
    return queue;
  }

  ur_context_handle_t getContext() {
    return context;
  }

  ur_command_t getCommandType() { return command_type; }

  void add_futures(std::vector<std::future<void>> &fs) {
    for (auto &f : fs) {
      futures.emplace_back(std::move(f));
    }
  }

  void tick_start() {
    timestamp_start = get_timestamp();
  }

  void tick_end() {
    timestamp_end = get_timestamp();
  }

  uint64_t get_start_timestamp() const {
    return timestamp_start;
  }

  uint64_t get_end_timestamp() const {
    return timestamp_end;
  }

private:
  ur_queue_handle_t queue;
  ur_context_handle_t context;
  ur_command_t command_type;
  bool done;
  std::mutex mutex;
  std::vector<std::future<void>> futures;
  bool has_callback = false;
  std::function<void()> callback;
  uint64_t timestamp_start = 0;
  uint64_t timestamp_end = 0;
};
