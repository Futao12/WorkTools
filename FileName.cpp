#include "FileName.h"

struct AutoProgramExe::Impl {
    std::string oldPtFilePath{};
    std::string matFilePath{};
    std::string newPtFilePath{};
    std::string particleInfosPath{};
    std::string pcgWorkPath{};
    std::string csFilePath{};
    char*       runTwoExeBatPath{};
    char*       reCompileBatPath{};
};

//AutoProgramExe::AutoProgramExe() : m_impl(std::make_unique<Impl>()) {};
//AutoProgramExe::~AutoProgramExe() = default;

AutoProgramExe::AutoProgramExe() : m_impl(std::make_unique<Impl>()) {};
AutoProgramExe::~AutoProgramExe() = default;

void AutoProgramExe::setOldPtFilePath(const std::string& oldPtFilePath) {
    m_impl->oldPtFilePath = oldPtFilePath;
}
void AutoProgramExe::setMatFilePath(const std::string& matFilePath) {
    m_impl->matFilePath = matFilePath;
}
void AutoProgramExe::setNewPtFilePath(const std::string& newPtFilePath) {
    m_impl->newPtFilePath = newPtFilePath;
}
void AutoProgramExe::setParticleInfoFilePath(const std::string& particleInfosPath) {
    m_impl->particleInfosPath = particleInfosPath;
}
void AutoProgramExe::setPcgExeWorkPath(const std::string& pcgWorkPath) {
    m_impl->pcgWorkPath = pcgWorkPath;
}
void AutoProgramExe::setCSfilePath(const std::string& csFilePath) {
    m_impl->csFilePath = csFilePath;
}
void AutoProgramExe::setRunTwoExeBatPath(char* runTwoExeBatPath) {
    m_impl->runTwoExeBatPath = runTwoExeBatPath;
}
void AutoProgramExe::setRecompileBatPath(char* reCompileBatPath) {
    m_impl->reCompileBatPath = reCompileBatPath;
}

const std::string& AutoProgramExe::getOldPtFilePath()        { return m_impl->oldPtFilePath; }
const std::string& AutoProgramExe::getMatFilePath()          { return m_impl->matFilePath; }
const std::string& AutoProgramExe::getNewPtFilePath()        { return m_impl->newPtFilePath; }
const std::string& AutoProgramExe::getParticleInfoFilePath() { return m_impl->particleInfosPath; }
const std::string& AutoProgramExe::getPcgExeWorkPath()       { return m_impl->pcgWorkPath; }
const std::string& AutoProgramExe::getCSfilePath()           { return m_impl->csFilePath; }
char*              AutoProgramExe::getRunTwoExeBatPath()     { return m_impl->runTwoExeBatPath;}
char*              AutoProgramExe::getRecompileBatPath()     { return m_impl->reCompileBatPath; }

void AutoProgramExe::resetPtfileSigs(const std::string& file, const std::string& newPtFilePath, const std::string& csvFilePath,
					int& csvParticleNums, std::vector<ParticleInfo_t>& particleInfosVec) {
	std::ifstream f(file.c_str(), std::ios::binary);
	if (!f.is_open()) {
		std::cout << "Open file: " << file << " failed." << std::endl;
		return;
	}
	f.seekg(0, std::ios::end);
	unsigned long long fileSize = f.tellg();
	f.seekg(0, std::ios::beg);

	char* buffer = new char[fileSize];
	f.read(buffer, fileSize);
	//f.close();
	long long* pPackageStartAddr = reinterpret_cast<long long*>(buffer);
	SWD_t* swdt = reinterpret_cast<SWD_t*>(pPackageStartAddr);
	int csvFileLines = 1048576;

	for (int i = 0; i < csvFileLines; i++)
	{
		swdt[i].m_signal += 0;
	}

	// 打开CSV文件
	std::ifstream csvfile(csvFilePath);
	std::vector<std::vector<int>> data; // 保存CSV数据的二维向量
	std::string line;
	int i = 0;
	while (std::getline(csvfile, line) && i < csvFileLines) {
		i++;
		//std::cout << line << std::endl;
		std::vector<int> row; // 保存每行数据的向量
		std::stringstream ss(line);
		std::string cell;

		while (std::getline(ss, cell, ',')) {
			row.push_back(std::stoi(cell));
		}

		data.push_back(row);
	}

	for (int i = 0; i < csvFileLines-1; ++i) {
		swdt[i].m_signal = data.at(i).at(0);
		if (data.at(i).at(1) > 0) {
			csvParticleNums++;
			particleInfosVec.push_back(ParticleInfo_t{i, data.at(i).at(1)});
		}
	}

	char* bufferrr = reinterpret_cast<char*> (swdt);
	std::ofstream outputFile(newPtFilePath, std::ios::binary);
	outputFile.write(bufferrr, fileSize);
	outputFile.close();

	delete[] buffer;
    return;
}

void AutoProgramExe::getParicleInfosCsv(const std::string& outputfilePath, const std::vector<ParticleInfo_t>& particleInfosVec) {
	std::ofstream outputFile(outputfilePath);
	outputFile << "Index,Size\n";

	for (const auto& particleInfo : particleInfosVec) {
		outputFile << particleInfo.index << "," << particleInfo.size << "\n";
	}

	outputFile.close();
    return;
}

// 获取指定进程名的进程ID
DWORD AutoProgramExe::GetProcessIdByName(const TCHAR* processName) {
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (Process32First(hProcessSnap, &pe32)) {
        do {
            if (_tcscmp(pe32.szExeFile, processName) == 0) {
                CloseHandle(hProcessSnap);
                return pe32.th32ProcessID;
            }
        } while (Process32Next(hProcessSnap, &pe32));
    }

    CloseHandle(hProcessSnap);
    return 0;
}

void AutoProgramExe::profileReplace(const std::string& CSfilePath, Param_t deltaCombination) {
    std::regex ratioPattern(".\"PeakRatioDelta.\"\\s*:\\s*(\\d+\\.\\d+),");
    std::regex distancePattern(".\"PeakDistanceDelta.\"\\s*:\\s*(\\d+\\.\\d+)");

    // 读取配置文件内容
    std::ifstream inputFile(CSfilePath);
    std::string line;
    std::string fileContent;
    while (std::getline(inputFile, line)) {
        fileContent += line + '\n';
    }
    inputFile.close();

    float ratioDelta = deltaCombination.peakRatioDelta;
    float distanceDelta = deltaCombination.peakDistanceDelta;

    std::sregex_iterator ratioIter(fileContent.begin(), fileContent.end(), ratioPattern);
    std::smatch match1 = *ratioIter;
    std::ostringstream replacement1;
    replacement1 << std::fixed << std::setprecision(1) << ratioDelta;
    std::string replacedText1 = std::regex_replace(fileContent, ratioPattern, "\\\"PeakRatioDelta\\\": " + replacement1.str() + ",");

    std::sregex_iterator distanceIter(replacedText1.begin(), replacedText1.end(), distancePattern);
    std::smatch match2 = *distanceIter;
    std::ostringstream replacement2;
    replacement2 << std::fixed << std::setprecision(1) << distanceDelta;
    std::string replacedText2 = std::regex_replace(replacedText1, distancePattern, "\\\"PeakDistanceDelta\\\": " + replacement2.str());

    std::ofstream outputFile(CSfilePath);

    outputFile << replacedText2;
    outputFile.close();
    return;
}


void AutoProgramExe::runExeMultipleTimes(const std::string& CSfilePath, const std::vector<Param_t>& paramVec,
                                        char* runTwoExeBatPath, char* reCompileBatPath) {
    for (auto& param : paramVec) {
        AutoProgramExe::profileReplace(CSfilePath, param);

        std::filesystem::current_path("D:\\workRecord\\PCG_Algo");
        system(reCompileBatPath);
        std::this_thread::sleep_for(std::chrono::seconds(1));

        std::filesystem::current_path("D:\\workRecord\\PCG_Algo\\bin\\DataPC\\x64\\Debug");
        system(runTwoExeBatPath);
        DWORD processId1 = AutoProgramExe::GetProcessIdByName(_T("DataProcWcfHosting.exe"));
        DWORD processId2 = AutoProgramExe::GetProcessIdByName(_T("DataProcServiceProxy.exe"));

        // 使用TerminateProcess终止进程
        if (processId1 != 0) {
            HANDLE processHandle1 = OpenProcess(PROCESS_TERMINATE, FALSE, processId1);
            TerminateProcess(processHandle1, 0);
            CloseHandle(processHandle1);
        }

        if (processId2 != 0) {
            HANDLE processHandle2 = OpenProcess(PROCESS_TERMINATE, FALSE, processId2);
            TerminateProcess(processHandle2, 0);
            CloseHandle(processHandle2);
        }

        std::this_thread::sleep_for(std::chrono::seconds(6));
    }
    return;
}

void AutoProgramExe::getDeltaFromfname(const std::string& csvFileName, float& ratDelta, float& distanceDelta) {
    std::istringstream iss(csvFileName);
    std::string token;

    std::getline(iss, token, '_'); 
    std::getline(iss, token, '_'); 
    std::istringstream(token) >> ratDelta;

    std::getline(iss, token, '_'); 
    std::getline(iss, token, '_'); 
    std::istringstream(token) >> distanceDelta;

    return ;
}

void AutoProgramExe::parseCsv2Vector(const std::string& csvFilePath, std::vector<RangeValue<int>>& rangeValVec, bool IsParticleInfoCsv, int range) {
    int startLines, indexCols;
    if (IsParticleInfoCsv) {
        startLines = 1;
        indexCols  = 0;
    }
    else {
        startLines = 2;
        indexCols  = 5;
    }

    std::ifstream file(csvFilePath);
    std::string line;
    for (int i = 0; i < startLines; ++i) {
        std::getline(file, line);
    }

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string token;

        for (int j = 0; j < indexCols; ++j) {
            std::getline(iss, token, ',');
        }

        std::getline(iss, token, ',');
        int value;
        std::istringstream(token) >> value;

        rangeValVec.emplace_back(RangeValue(value, range));
    }

    return;
}

void AutoProgramExe::calcFalseAndMissCount(const std::string& diffDeltaResDirectory, const std::string& particleInfoFilePath, ParamResMap& paramResMap, int particleNums, int range) {
    std::vector<RangeValue<int>> particleInfoRangeVec;
    AutoProgramExe::parseCsv2Vector(particleInfoFilePath, particleInfoRangeVec, true, range);

    std::regex pattern("\\d+\\.\\d+_\\d+\\.\\d+_\\d+\\.\\d+_\\d+\\.\\d+_PeaksInfo\\.csv");
    for (const auto& entry : std::filesystem::directory_iterator("C:\\Users\\20231205\\Desktop\\evt_lpd")) {
        std::string csvFilePath = entry.path().string();
        if (std::regex_search(csvFilePath, pattern)) {
            std::string csvFileName = entry.path().filename().string();
            std::vector<RangeValue<int>> lpdRangeValVec;
            AutoProgramExe::parseCsv2Vector(csvFilePath, lpdRangeValVec, false, range);
            
            int count1 = 0, count2 = 0;
            for (auto& lpd : lpdRangeValVec) {
                for (auto& particle : particleInfoRangeVec) {
                    if (lpd.m_value <= particle.getMax() && lpd.m_value >= particle.getMin()) {
                        count1++;
                    }
                }
            }

            for (auto& particle : particleInfoRangeVec) {
                for (auto& lpd : lpdRangeValVec) {
                    if (particle.m_value <= lpd.getMax() && particle.m_value >= lpd.getMin()) {
                        count2++;
                    }
                }
            }

            float countPercent      = static_cast<float>(static_cast<float>(count1) / static_cast<float>(particleNums));
            float falseCountPercent = static_cast<float>((static_cast<float>(lpdRangeValVec.size()) - static_cast<float>(count1)) / static_cast<float>(lpdRangeValVec.size()));
            float missCountPercent  = static_cast<float>((static_cast<float>(particleNums) - static_cast<float>(count2)) / static_cast<float>(particleNums));

            float ratDelta, distanceDelta = -1;
            AutoProgramExe::getDeltaFromfname(csvFileName, ratDelta, distanceDelta);
            ParamKey_t mapKey{ ratDelta, distanceDelta };
            DeltaRes_t mapVal{ countPercent, falseCountPercent , missCountPercent };
            paramResMap[mapKey] = mapVal;
        }                                
    }

    return;
}

int main() {
    auto autoProgramExe = AutoProgramExe::AutoProgramExe();

    autoProgramExe.setOldPtFilePath("C:\\Users\\20231205\\Desktop\\matlabSimData1\\001_NoNoise_1_track_R=100mm_Np=200.csv\\proc_4.pt");
    autoProgramExe.setMatFilePath("C:\\Users\\20231205\\Desktop\\matlabSimData1\\001_NoNoise_1_track_R=100mm_Np=200.csv\\001.csv");
    autoProgramExe.setNewPtFilePath("D:\\workRecord\\PCG_Algo\\bin\\DataPC\\x64\\Debug\\sim\\hs\\001_1.pt");
    autoProgramExe.setParticleInfoFilePath("C:\\Users\\20231205\\Desktop\\matlabSimData1\\001_NoNoise_1_track_R=100mm_Np=200.csv\\001_particleInfos.csv");
    int csvParticleNums = 0;
    std::vector<ParticleInfo_t> particleInfosVec;
    autoProgramExe.resetPtfileSigs(autoProgramExe.getOldPtFilePath(), autoProgramExe.getNewPtFilePath(), autoProgramExe.getMatFilePath(), csvParticleNums, particleInfosVec);
    autoProgramExe.getParicleInfosCsv(autoProgramExe.getParticleInfoFilePath(), particleInfosVec);
    std::cout << "csvParticleNum is: " << csvParticleNums << std::endl;

    /*------------------------------------------------------------------------------------------------------------------------------*/
    /*------------------------------------------------------------------------------------------------------------------------------*/

    autoProgramExe.setCSfilePath("D:\\workRecord\\PCG_Algo\\Projects\\WCFService\\DataProcServiceProxy\\IMCTestMethodWrapper.cs");
    autoProgramExe.setRunTwoExeBatPath(const_cast<char*>("D:\\workRecord\\PCG_Algo\\bin\\DataPC\\x64\\Debug\\run_two_exe.bat"));
    autoProgramExe.setRecompileBatPath(const_cast<char*>("D:\\workRecord\\PCG_Algo\\autoCompile.bat"));
    std::vector<float> peakRatioDeltaVec    = {0.5, 1, 1.5, 2, 2.5, 3, 3.5};
    std::vector<float> peakDistanceDeltaVec = {0.5, 1, 1.5, 2, 2.5, 3, 3.5};
    /*std::vector<float> peakRatioDeltaVec    = {1.0, 2.5};
    std::vector<float> peakDistanceDeltaVec = {1.0, 2.0};*/
    std::vector<Param_t> paramVec;
    for (auto& ratDelta : peakRatioDeltaVec) {
        for (auto& disDelta : peakDistanceDeltaVec) {
            paramVec.emplace_back(Param_t{ratDelta, disDelta});
        }
    }
    autoProgramExe.runExeMultipleTimes(autoProgramExe.getCSfilePath(), paramVec, autoProgramExe.getRunTwoExeBatPath(), autoProgramExe.getRecompileBatPath());
	
    /*------------------------------------------------------------------------------------------------------------------------------*/
    /*------------------------------------------------------------------------------------------------------------------------------*/

    /*ParamResMap paramResMap;
    autoProgramExe.calcFalseAndMissCount("C:\\Users\\20231205\\Desktop\\evt_lpd", "C:\\Users\\20231205\\Desktop\\001_particleInfos.csv", paramResMap, csvParticleNums, 6); */

	return 0;
}