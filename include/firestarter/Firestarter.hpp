#ifndef INCLUDE_FIRESTARTER_FIRESTARTER_HPP
#define INCLUDE_FIRESTARTER_FIRESTARTER_HPP

#include <firestarter/ThreadData.hpp>

#include <llvm/ADT/StringMap.h>
#include <llvm/Support/MemoryBuffer.h>

#include <list>
#include <string>

extern "C" {
#include <firestarter/Compat/util.h>

#include <hwloc.h>

#include <pthread.h>
}

namespace firestarter {
	
	class Firestarter {
		public:
			// Firestarter.cpp
			Firestarter(void);
			~Firestarter(void);

			// Environment.cpp
			int evaluateEnvironment(void);
			void printEnvironmentSummary(void);

			// CpuAffinity.cpp
			int setCpuAffinity(unsigned requestedNumThreads, std::string cpuBind);

			void run(void);

		private:
			// CpuClockrate.cpp
			std::unique_ptr<llvm::MemoryBuffer> getFileAsStream(std::string filePath, bool showError = true);
			std::unique_ptr<llvm::MemoryBuffer> getScalingGovernor(void);
			int getCpuClockrate(void);
			int genericGetCpuClockrate(void);

			// CpuAffinity.cpp
			int parse_cpulist(cpu_set_t *cpuset, const char *fsbind, unsigned *requestedNumThreads);

			// ThreadWorker.cpp
			static void *threadWorker(void *threadData);

			hwloc_topology_t topology;
			unsigned int numPackages;
			unsigned int numPhysicalCoresPerPackage;
			unsigned int numThreads;
			std::string architecture = std::string("");
			std::string vendor = std::string("");
			std::string processorName = std::string("");
			std::string model = std::string("");
			unsigned long long clockrate;
			llvm::StringMap<bool> cpuFeatures;

			unsigned requestedNumThreads;
			std::list<unsigned long long> cpuBind;
			pthread_t *threads;
			std::list<ThreadData *> threadData;
	};

}

#endif
