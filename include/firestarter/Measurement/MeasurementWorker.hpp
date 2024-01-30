/******************************************************************************
 * FIRESTARTER - A Processor Stress Test Utility
 * Copyright (C) 2020-2023 TU Dresden, Center for Information Services and High
 * Performance Computing
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/\>.
 *
 * Contact: daniel.hackenberg@tu-dresden.de
 *****************************************************************************/

#pragma once

#include <firestarter/Logging/Log.hpp>
#include <firestarter/Measurement/Summary.hpp>
#include <firestarter/Measurement/TimeValue.hpp>

#include <chrono>
#include <map>
#include <mutex>

extern "C" {
#include <firestarter/Measurement/Metric/IPCEstimate.h>
#include <firestarter/Measurement/Metric/Perf.h>
#include <firestarter/Measurement/Metric/RAPL.h>
#include <firestarter/Measurement/MetricInterface.h>

#include <pthread.h>
}

void insertCallback(void *cls, const char *metricName, int64_t timeSinceEpoch,
                    double value);

namespace firestarter::measurement {

class MeasurementWorker {
private:
  pthread_t workerThread;
  pthread_t stdinThread;

  std::vector<metric_interface_t *> metrics = {
      &rapl_metric, &perf_ipc_metric, &perf_freq_metric, &ipc_estimate_metric};

  std::mutex values_mutex;
  std::map<std::string, std::vector<TimeValue>> values = {};

  static int *dataAcquisitionWorker(void *measurementWorker);

  static int *stdinDataAcquisitionWorker(void *measurementWorker);

  const metric_interface_t *findMetricByName(std::string metricName);

  std::chrono::milliseconds updateInterval;

  std::chrono::high_resolution_clock::time_point startTime;

  // some metric values have to be devided by this
  const unsigned long long numThreads;

  std::string availableMetricsString;

#ifndef FIRESTARTER_LINK_STATIC
  std::vector<void *> _metricDylibs = {};
#endif

  std::vector<std::string> _stdinMetrics = {};

public:
  // creates the worker thread
  MeasurementWorker(std::chrono::milliseconds updateInterval,
                    unsigned long long numThreads,
                    std::vector<std::string> const &metricDylibs,
                    std::vector<std::string> const &stdinMetrics);

  // stops the worker threads
  ~MeasurementWorker();

  std::string const &availableMetrics() const {
    return this->availableMetricsString;
  }

  std::vector<std::string> const &stdinMetrics() { return _stdinMetrics; }

  // returns a list of metrics
  std::vector<std::string> metricNames();

  // setup the selected metrics
  // returns a vector with the names of inialized metrics
  std::vector<std::string>
  initMetrics(std::vector<std::string> const &metricNames);

  // callback function for metrics
  void insertCallback(const char *metricName, int64_t timeSinceEpoch,
                      double value);

  // start the measurement
  void startMeasurement();

  // get the measurement values begining from measurement start until now.
  std::map<std::string, Summary> getValues(
      std::chrono::milliseconds startDelta = std::chrono::milliseconds::zero(),
      std::chrono::milliseconds stopDelta = std::chrono::milliseconds::zero());
};

} // namespace firestarter::measurement
