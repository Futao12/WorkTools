#pragma once

#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <memory>
#include <cstdlib>
#include <filesystem>
#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <thread>
#include <regex>
#include <iomanip>
#include "spdlog/spdlog.h"

typedef
struct SWD final
{
	long long m_signal : 17;      // Detected raw data
	long long m_reserve : 1;      // Reserve for algo use
	long long m_xEncoder : 18;    // raw x-encoder value
	long long m_wEncoder : 18;    // raw w-encoder value
	long long m_gain : 4;         // raw lpm gain value
	long long m_afs : 4;          // raw afs gain value
	long long m_acc : 1;          // Inidicates if acc triggered
	long long m_trigger : 1;      // Is ACC triggered
}SWD_t;

typedef
struct particleInfos final
{
	int index;
	int size;
}ParticleInfo_t;

typedef
struct Param final
{
	float peakRatioDelta;
	float peakDistanceDelta;
}Param_t;

typedef
struct ParamKey final
{
	float peakRatioDelta;
	float peakDistanceDelta;

	bool operator == (const ParamKey& other) const {
		return (peakRatioDelta == other.peakRatioDelta) && (peakDistanceDelta == other.peakDistanceDelta);
	}
}ParamKey_t;

struct ParamKeyHash {
	std::size_t operator()(const ParamKey& p) const {
		std::size_t hashRatioDelta = std::hash<float>()(p.peakRatioDelta);
		std::size_t hashDistanceDelta = std::hash<float>()(p.peakDistanceDelta);
		return hashRatioDelta ^ (hashDistanceDelta << 1);
	}
};

typedef 
struct DeltaRes final
{
	float countPercent;
	float falseCountPercent;
	float missCountPercent;
}DeltaRes_t;

using ParamResMap = std::unordered_map<ParamKey_t, DeltaRes_t, ParamKeyHash>;

template <typename T>
class RangeValue {
public:
	RangeValue(T value, T range) : m_value(value), m_range(range){}
	T getMin()const { return m_value - m_range; }
	T getMax()const { return m_value + m_range; }

	T m_value;
	T m_range;
};

class AutoProgramExe final {
public:
	void setOldPtFilePath(const std::string& oldPtFilePath);
	void setMatFilePath(const std::string& matFilePath);
	void setNewPtFilePath(const std::string& newPtFilePath);
	void setParticleInfoFilePath(const std::string& particleInfosPath);
	void setPcgExeWorkPath(const std::string& pcgWorkPath);
	void setCSfilePath(const std::string& csFilePath);
	void setRunTwoExeBatPath(char* runTwoExeBatPath);
	void setRecompileBatPath(char* reCompileBatPath);

	const std::string& getOldPtFilePath();
	const std::string& getMatFilePath();
	const std::string& getNewPtFilePath();
	const std::string& getParticleInfoFilePath();
	const std::string& getPcgExeWorkPath();
	const std::string& getCSfilePath();
	char*              getRunTwoExeBatPath();
	char*              getRecompileBatPath();

	void resetPtfileSigs(const std::string& file, const std::string& newPtFilePath, const std::string& matFilePath,
						  int& csvParticleNums, std::vector<ParticleInfo_t>& particleInfosVec);
	void getParicleInfosCsv(const std::string& outputfilePath, const std::vector<ParticleInfo_t>& particleInfosVec);

	DWORD GetProcessIdByName(const TCHAR* processName);
	void profileReplace(const std::string& CSfilePath, Param_t deltaCombination);
	void runExeMultipleTimes(const std::string& CSfilePath, const std::vector<Param_t>& paramVec,
	                         char* runTwoExeBatPath, char* reCompileBatPath);

	void calcFalseAndMissCount(const std::string& diffDeltaResDirectory, const std::string& particleInfoFilePath, ParamResMap& paramResMap, int particleNums, int range);
	void parseCsv2Vector(const std::string& csvFilePath, std::vector<RangeValue<int>>& rangeValVec, bool IsParticleInfoCsv, int range);
	void getDeltaFromfname(const std::string& csvFileName, float& ratDelta, float& distanceDelta);

	AutoProgramExe();
	~AutoProgramExe();
	AutoProgramExe(const AutoProgramExe&) = delete;
	//AutoProgramExe(AutoProgramExe&&) = delete;
	AutoProgramExe& operator = (const AutoProgramExe&) = delete;
	//AutoProgramExe& operator = (AutoProgramExe&&) = delete;

	inline AutoProgramExe(AutoProgramExe&& other) noexcept
		: m_impl(std::move(other.m_impl)) {}

	inline AutoProgramExe& operator=(AutoProgramExe&& other) noexcept {
		if (this != &other) {
			m_impl = std::move(other.m_impl);
		}
		return *this;
	}

private:
	struct Impl;
	std::unique_ptr<Impl> m_impl;
};