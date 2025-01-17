/* Copyright 2017-2021 Institute for Automation of Complex Power Systems,
 *                     EONERC, RWTH Aachen University
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *********************************************************************************/

#include <dpsim/ThreadScheduler.h>

#include <iostream>

using namespace CPS;
using namespace DPsim;

ThreadScheduler::ThreadScheduler(Int threads, String outMeasurementFile, Bool useConditionVariable) :
	mNumThreads(threads), mOutMeasurementFile(outMeasurementFile), mStartBarrier(threads, useConditionVariable) {
	if (threads < 1)
		throw SchedulingException();
	mTempSchedules.resize(threads);
	mSchedules.resize(threads, nullptr);
}

ThreadScheduler::~ThreadScheduler() {
	for (int i = 0; i < mNumThreads; i++)
		delete[] mSchedules[i];
}

void ThreadScheduler::scheduleTask(int thread, CPS::Task::Ptr task) {
	mTempSchedules[thread].push_back(task);
}

void ThreadScheduler::finishSchedule(const Edges& inEdges) {
	std::map<CPS::Task::Ptr, Counter*> counters;
	for (int thread = 0; thread < mNumThreads; thread++) {
	//	std::cout << "Thread " << thread << std::endl;
	//	for (auto& entry : mSchedules[thread]) {
	//		Task* t = entry.task.get();
	//		char *refpos = reinterpret_cast<char*>(reinterpret_cast<void*>(t)) + sizeof(Task);
	//		void *ref = *(reinterpret_cast<void**>(refpos));
	//		std::cout << entry.task->toString() << " " << ref << std::endl;
	//	}
		mSchedules[thread] = new ScheduleEntry[mTempSchedules[thread].size()];
		for (size_t i = 0; i < mTempSchedules[thread].size(); i++) {
			auto& task = mTempSchedules[thread][i];
			mSchedules[thread][i].task = task.get();
			counters[task] = &mSchedules[thread][i].endCounter;
		}
	}
	for (int thread = 0; thread < mNumThreads; thread++) {
		for (size_t i = 0; i < mTempSchedules[thread].size(); i++) {
			auto& task = mTempSchedules[thread][i];
			if (inEdges.find(task) != inEdges.end()) {
				for (auto req : inEdges.at(task)) {
					mSchedules[thread][i].reqCounters.push_back(counters[req]);
				}
			}
		}
	}
	for (int i = 1; i < mNumThreads; i++) {
		mThreads.emplace_back(threadFunction, this, i);
	}
}

void ThreadScheduler::step(Real time, Int timeStepCount) {
	mTime = time;
	mTimeStepCount = timeStepCount;
	mStartBarrier.wait();
	doStep(0);
	// since we don't have a final BarrierTask, wait for all threads to finish
	// their last task explicitly
	for (int thread = 1; thread < mNumThreads; thread++) {
		if (mTempSchedules[thread].size() != 0)
			mSchedules[thread][mTempSchedules[thread].size()-1].endCounter.wait(mTimeStepCount+1);
	}
}

void ThreadScheduler::stop() {
	if (!mThreads.empty()) {
		mJoining = true;
		mStartBarrier.wait();
		for (size_t thread = 0; thread < mThreads.size(); thread++) {
			mThreads[thread].join();
		}
	}
	if (!mOutMeasurementFile.empty()) {
		writeMeasurements(mOutMeasurementFile);
	}
}

void ThreadScheduler::threadFunction(ThreadScheduler* sched, Int idx) {
	while (true) {
		sched->mStartBarrier.wait();
		if (sched->mJoining)
			return;

		sched->doStep(idx);
	}
}

void ThreadScheduler::doStep(Int thread) {
	if (mOutMeasurementFile.empty()) {
		for (size_t i = 0; i != mTempSchedules[thread].size(); i++) {
			ScheduleEntry* entry = &mSchedules[thread][i];
			for (Counter* counter : entry->reqCounters)
				counter->wait(mTimeStepCount+1);
			entry->task->execute(mTime, mTimeStepCount);
			entry->endCounter.inc();
		}
	} else {
		for (size_t i = 0; i != mTempSchedules[thread].size(); i++) {
			ScheduleEntry* entry = &mSchedules[thread][i];
			for (Counter* counter : entry->reqCounters)
				counter->wait(mTimeStepCount+1);
			auto start = std::chrono::steady_clock::now();
			entry->task->execute(mTime, mTimeStepCount);
			auto end = std::chrono::steady_clock::now();
			updateMeasurement(entry->task, end-start);
			entry->endCounter.inc();
		}
	}
}
