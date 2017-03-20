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
#ifndef RESULTS_QUEUE_INL_H_
#define RESULTS_QUEUE_INL_H_
namespace Norma {
template<typename R>
void ResultsQueue<R>::set_consumer(std::function<void(R)> consumer) {
    _consumer = consumer;
    output_done = std::async(std::launch::async,
                             &ResultsQueue::consume, this);
}

template<typename R>
void ResultsQueue<R>::add_producer(std::function<R(string_impl)> producer,
                                   const string_impl line) {
    // TODO(fpetran) set a timeout here maybe in case producer malfunctions?
    for (; ;) {
        if (num_threads.load() <= _max_threads)
            break;
    }
    std::unique_lock<std::mutex> consumer_lock(_mutex);
    results.push(std::async(_policy, producer, line));
    ++num_threads;
    if (!output_ready)
        output_ready = true;
    consumer_condition.notify_one();
}

template<typename R> bool ResultsQueue<R>::consume() {
    do {
        std::unique_lock<std::mutex> consumer_lock(_mutex);
        // this loop prevents spurious wakeup
        while (!output_ready)
            consumer_condition.wait(consumer_lock,
                                    [this]{ return output_ready.load(); });
        while (!results.empty()) {
            R result = results.front().get();
            _consumer(result);
            results.pop();
            --num_threads;
        }
    } while (!workers_done);
    return true;
}
}  // namespace Norma
#endif  // RESULTS_QUEUE_INL_H_
