#pragma once
#include "netcommon.h"

template <typename T>
class tsqueue
{
public:
	tsqueue() = default;
	tsqueue(const tsqueue<T>&) = delete;
	virtual ~tsqueue() { clear(); }
public:
	const T& front() {
		std::scoped_lock lock(muxQueue);
		return deqQueue.front();
	}
	const T& back() {
		std::scoped_lock lock(muxQueue);
		return deqQueue.back();
	}
	void push_back(const T& elem) {
		std::scoped_lock lock(muxQueue);
		deqQueue.emplace_back(std::move(elem));
	}
	void push_front(const T& elem) {
		std::scoped_lock lock(muxQueue);
		deqQueue.emplace_front(std::move(elem));
	}
	bool empty() {
		std::scoped_lock lock(muxQueue);
		return deqQueue.empty();
	}
	size_t count() {
		std::scoped_lock lock(muxQueue);
		return deqQueue.size();
	}
	void clear() {
		std::scoped_lock lock(muxQueue);
		deqQueue.clear();
	}
	T pop_front() {
		std::scoped_lock lock(muxQueue);
		auto t = std::move(deqQueue.front());
        deqQueue.pop_front();
        return t;
	}
	void resize(size_t n) {
		std::scoped_lock lock(muxQueue);
		deqQueue.resize(n);
	}
protected:
	std::mutex muxQueue;
	std::deque<T> deqQueue;
};
