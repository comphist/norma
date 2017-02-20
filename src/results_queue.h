/* Copyright 2013-2015 Marcel Bollmann, Florian Petran
 *
 * This file is part of Norma.
 *
 * Norma is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * Norma is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License along
 * with Norma.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef RESULTS_QUEUE_H_
#define RESULTS_QUEUE_H_
#include<queue>
#include<thread>
#include<future>
#include<condition_variable>
#include<functional>
#include"string_impl.h"

namespace Norma {
/// a multi producer-single consumer queue
/**
 * policy only relates to the producer threads, the consumer is always async.
 * if max_threads is 0, it will be set to hardware_concurrency (the number of
 * threads that can physically be executed in parallel).
 **/
template<typename RESULT_TY> class ResultsQueue {
 public:
     ResultsQueue() { init(); }
     explicit ResultsQueue(unsigned max_threads) { init(max_threads); }
     explicit ResultsQueue(std::launch policy) { init(0, policy); }
     ResultsQueue(unsigned max_threads, std::launch policy) {
         init(max_threads, policy);
     }
     void set_consumer(std::function<void(RESULT_TY)> consumer);
     void add_producer(std::function<RESULT_TY(string_impl)> producer,
                       const string_impl line);
     /// consume remaining results and wait for the consumer to be done
     void finish() {
         output_ready = true;
         workers_done = true;
         consumer_condition.notify_all();
         output_done.wait();
     }

 private:
     std::launch _policy = std::launch::async|std::launch::deferred;
     unsigned _max_threads;
     std::atomic<unsigned> num_threads{0};
     std::mutex _mutex;
     std::function<void(RESULT_TY)> _consumer;
     std::queue<std::future<RESULT_TY>> results;
     std::future<bool> output_done;
     std::atomic<bool> output_ready{false}, workers_done{false};
     std::condition_variable consumer_condition;

     void init(unsigned max_threads = 0,
               std::launch policy = std::launch::async|std::launch::deferred) {
         if (max_threads == 0)
             _max_threads = std::thread::hardware_concurrency() * 2;
         _policy = policy;
     }
     bool consume();
};
}  // namespace Norma

#include"results_queue-inl.h"
#endif  // NORMA_RESULTS_QUEUE_H_

